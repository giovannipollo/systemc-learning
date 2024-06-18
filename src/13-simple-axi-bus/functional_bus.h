#include <systemc.h>
#include "axi/axi_master.h"
#include "axi/axi_slave.h"
#include "axi/axi_interface.h"
#define BUFFER_SIZE 100

class Functional_Bus : public sc_core::sc_module, public AXI_2_Bus_Port {
    
    private:
        bool resp_ready, comm_ongoing;
        int req_address, req_size, req_type;
        uint8_t *data_buffer;
        uint8_t *response_buffer; 
        
        sc_signal<bool> read_received;
        sc_signal<bool> write_received;

        void bus_logic() {
            int sensor_id;
            int address;

            while (true) {
                resp_ready = false;
                for (int i = 0; i < NUM_SENSORS; i++) 
                    sensor_enable[i].write(false);


                if (read_received.read() == true || write_received.read() == true) {
                    if (PRINT_AXI_BUS_DEBUG)
                        cout << "@" << sc_time_stamp() << ": [BUS INTERNAL]: Parsing request from master device to address " << (int) req_address << " and length " << req_size << endl;

                    // Select SLAVE device based on address
                    if (req_address >= CAMERA_BASE_ADDRESS && req_address <= CAMERA_BASE_ADDRESS + 15) {
                        sensor_id = 0;
                        address = req_address - CAMERA_BASE_ADDRESS;
                    }
                    else if (req_address >= MOTOR_BASE_ADDRESS && req_address <= MOTOR_BASE_ADDRESS + 15) {
                        sensor_id = 1;
                        address = req_address - MOTOR_BASE_ADDRESS;
                    }
                    else {
                        sensor_id = -1;
                    }

                    // Correct address received
                    if (sensor_id >= 0) {
                        // Activate enable signal
                        sensor_enable[sensor_id].write(true);

                        // Send request
                        if (read_received.read() == true) {
                            axi_interface_to_slave->read(address, req_size, data_buffer, response_buffer);
                        }
                        else if (write_received.read() == true) {
                            response_buffer[0] = axi_interface_to_slave->write(address, data_buffer, req_size);
                        }
                    }
                    // Response '11' has to be returned
                    else {
                        if (read_received.read() == true) {
                            for (int i = 0; i < req_size; i++) {
                                data_buffer[i] = 0;
                                response_buffer[i] = 3;
                            }
                        }
                        else if (write_received.read() == true) {
                            response_buffer[0] = 3;
                        }
                    }

                    resp_ready = true;
                }

                wait();
            }
        }

    public: 
        sc_port<Master_2_AXI_Port> axi_interface_to_slave;

        //Input Port
        sc_vector<sc_in <bool>> go_sensors{"go_sensors", NUM_SENSORS};

        //Output Port
        sc_vector<sc_out <bool>> sensor_enable{"sensor_enable", NUM_SENSORS};

        SC_CTOR(Functional_Bus) {
            data_buffer = new uint8_t[BUFFER_SIZE];
            response_buffer = new uint8_t[BUFFER_SIZE];

            SC_THREAD(bus_logic);
            sensitive << read_received << write_received;
        }
        
        Functional_Bus(){}
        ~Functional_Bus() {
            delete[] data_buffer;
            delete[] response_buffer;
        }

        /* The AXI module needs to know the status of the response from the sensor 
        * to decide when to resume communication with the master
        */
        bool is_response_ready(uint8_t *response, uint8_t *data) {
            if (resp_ready) {
                if (req_type == READ_REQUEST_BUS) {
                    for (int i = 0; i < req_size; i++) {
                        data[i] = data_buffer[i];
                        response[i] = response_buffer[i];
                    }
                }
                else if (req_type == WRITE_REQUEST_BUS) {
                    *response = response_buffer[0];
                }

                read_received.write(false);
                write_received.write(false);
            }

            return resp_ready;
        }

        /* Store data for the completion of a READ/WRITE request towards a slave device
        * Activates internal signal to indicate the need for the combinational logic to work
        */
        void send_request(int type, int address, int size) {
            req_address = address;
            req_size = size;
            req_type = type;

            if (type == READ_REQUEST_BUS)
                read_received.write(true);
            else if (type == WRITE_REQUEST_BUS)
                write_received.write(true);
        }

        /* Write data on internal buffer
        */
        void write(int address, uint8_t data) {
            data_buffer[address] = data;
        }
};