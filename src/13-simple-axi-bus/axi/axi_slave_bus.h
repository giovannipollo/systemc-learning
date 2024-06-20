#ifndef AXI_SLAVE_BUS
#define AXI_SLAVE_BUS

#include <systemc.h>
#include "axi_interface.h"

class AxiSlave_Bus : public sc_core::sc_module {

    public: 
        // Interface
        AXI_S_Interface axi_interface;
        sc_port<AXI_2_Bus_Port> mod_interface{"Module_Interface"};

        // Internal signals
        sc_signal<int> address;
        sc_signal<uint8_t> length, data;

        SC_CTOR(AxiSlave_Bus) {
            SC_THREAD(handshake_process);
            sensitive << axi_interface.clk.pos();

            sensitive << axi_interface.AW_address;
            sensitive << axi_interface.AW_valid;
            sensitive << axi_interface.W_data;
            sensitive << axi_interface.W_last;
            sensitive << axi_interface.W_valid;
            sensitive << axi_interface.B_ready;

            sensitive << axi_interface.AR_address;
            sensitive << axi_interface.AR_valid;
            sensitive << axi_interface.R_ready;
        }

        void handshake_process() {
            int counter, r_err, w_err;
            int buffer_write_address;
            uint8_t *read_response, *read_data;
            uint8_t write_response;

            while(true) {
                axi_interface.AW_ready = false;
                axi_interface.AR_ready = false;
                axi_interface.W_ready = false;
                axi_interface.B_valid = false;
                axi_interface.R_valid = false;
                axi_interface.R_last = false;
                axi_interface.R_data = 0;
                axi_interface.B_resp = 0;

                if (PRINT_AXI_SLAVE_DEBUG)
                    cout << "@" << sc_time_stamp() << " [SENSOR INFO] : waiting for a read or write request..." << endl;

                if (axi_interface.AW_valid.read() == false && axi_interface.AR_valid.read() == false)
                    wait(axi_interface.AW_valid.posedge_event() | axi_interface.AR_valid.posedge_event());

                // READ: address received
                if (axi_interface.AR_valid.read() == true) {
                    // In the clock cycle after both VALID and READY are high, handshake is completed
                    axi_interface.AR_ready = true;
                    address = axi_interface.AR_address.read();
                    length = axi_interface.AR_length.read();
                    wait(axi_interface.clk.posedge_event());

                    axi_interface.AR_ready = false;
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR READ] : handshake complete; Starting communications..." << endl;

                    // Wait for the manager to be ready to READ
                    if (axi_interface.R_ready.read() == false)
                        wait(axi_interface.R_ready.posedge_event());

                    counter = 0;
                    r_err = 0;
                    wait(axi_interface.clk.posedge_event());

                    // Wait for the bus to be ready to forward the slave response
                    read_data = new uint8_t[length];
                    read_response = new uint8_t[length];
                    mod_interface->send_request(READ_REQUEST_BUS, address, length);
                    while (mod_interface->is_response_ready(read_response, read_data) == false)
                        wait(axi_interface.clk.posedge_event());

                    // Read data from RF
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR READ] : starting reading operations..." << endl;

                    while(counter < length) {
                        if (counter == length - 1) {
                            if (PRINT_AXI_SLAVE_DEBUG)
                                cout << "@" << sc_time_stamp() << " [SENSOR READ] : last element reached..." << endl;

                            axi_interface.R_last = true;
                        }

                        axi_interface.R_resp = read_response[counter];
                        axi_interface.R_data = read_data[counter];
                        axi_interface.R_valid = true;
                        counter++;
                        wait(axi_interface.clk.posedge_event());
                    }
            
                    axi_interface.R_data = 0;
                    axi_interface.R_resp = 0;
                    axi_interface.R_valid = false;
                    axi_interface.R_last = false;
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR READ] : ending read exchange..." << endl;

                    wait(axi_interface.clk.posedge_event());
                    delete[] read_data;
                    delete[] read_response;
                }
                // WRITE: address received 
                else if (axi_interface.AW_valid.read() == true) {
                    // In the clock cycle after both VALID and READY are high, handshake is completed
                    axi_interface.AW_ready = true;
                    address = axi_interface.AW_address.read();
                    wait(axi_interface.clk.posedge_event());

                    w_err = 0;
                    buffer_write_address = 0;
                    axi_interface.AW_ready = false;
                    axi_interface.W_ready = true;
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR WRITE] : handshake complete; Starting communications..." << endl;

                    while(true) {
                        if (axi_interface.W_valid.read() == true) {
                            mod_interface->write(buffer_write_address, axi_interface.W_data.read());
                            buffer_write_address = buffer_write_address + 1;
                        }

                        if (axi_interface.W_last.read() == true) {
                            if (PRINT_AXI_SLAVE_DEBUG)
                                cout << "@" << sc_time_stamp() << " [SENSOR WRITE] : last packet received" << endl;

                            break;
                        }

                        wait(axi_interface.clk.posedge_event());       
                    }

                    axi_interface.W_ready = false;
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR WRITE] : preparing response" << endl;

                    // Wait for the bus to be ready to forward the slave response
                    mod_interface->send_request(WRITE_REQUEST_BUS, address, buffer_write_address);
                    while (mod_interface->is_response_ready(&write_response, NULL) == false)
                        wait(axi_interface.clk.posedge_event());

                    /* B_RESP (as with R_RESP) can assume four values:
                    * - 00: "Normal access success or exclusive access failure"
                    * - 01: "Only a portion of an exclusive access completed successfully"
                    * - 10: "Subordinate error"
                    * - 11: "Decode error: indicate that there is no subordinate at the transaction address"
                    * 
                    * For our purposes, only "10" is managed during a port-to-port communication
                    * When a bus is included for a one-to-many exchange, "11" has to be considered
                    */
                    axi_interface.B_valid = true;
                    axi_interface.B_resp = write_response;
                    
                    if (axi_interface.B_ready.read() == false)
                        wait(axi_interface.B_ready.posedge_event());
                        
                    wait(axi_interface.clk.posedge_event());
                    if (PRINT_AXI_SLAVE_DEBUG)
                        cout << "@" << sc_time_stamp() << " [SENSOR WRITE] : ending write exchange..." << endl;

                    axi_interface.B_valid = false;
                    axi_interface.B_resp = 0;
                }
            
                wait();
            }
        }
};
#endif