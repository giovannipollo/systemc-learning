#include <systemc.h>
#include "axi/axi_interface.h"

class Core : public sc_core::sc_module {

    private:
        int MEMORY_SIZE=16;

        void sensor_logic() {
            uint8_t data_1[5] = {16, 11, 81, 23, 54};
            uint8_t data_2[5] = {32, 1, 5, 176, 43};
            uint8_t data_3[5] = {212, 254, 32, 134, 54};
            uint8_t data_4[5] = {9, 98, 78, 145, 178};
            uint8_t data_5[5] = {16, 11, 81, 23, 54};

            // To store the received data
            uint8_t resp;
            uint8_t rec_data[10];
            uint8_t rec_resp[10];
            for (int i = 0; i < 10; i++) {
                rec_data[i] = 0; 
                rec_resp[i] = 0;
            }

            while (true) {
                if(enable.read() == true) {
                    resp = axi_interface->write(CAMERA_BASE_ADDRESS + 4, data_1, 5);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                
                    resp = axi_interface->write(MOTOR_BASE_ADDRESS + 6, data_3, 2);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                
                    resp = axi_interface->write(MOTOR_BASE_ADDRESS + 10, data_5, 5);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                    cout << endl;

                    axi_interface->read(CAMERA_BASE_ADDRESS + 2, 2, rec_data, rec_resp);
                    for (int i = 0; i < 2; i++)
                        cout << "READ PERFORMED. DATA: " << (int) rec_data[i] << "\tSTATUS: " << (int) rec_resp[i] << endl;
                    
                    cout << endl;
                
                    resp = axi_interface->write(CAMERA_BASE_ADDRESS + 13, data_2, 5);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                    cout << endl;

                    axi_interface->read(47, 6, rec_data, rec_resp);
                    for (int i = 0; i < 6; i++)
                        cout << "READ PERFORMED. DATA: " << (int) rec_data[i] << " \tSTATUS: " << (int) rec_resp[i] << endl;

                    cout << endl;

                    axi_interface->read(CAMERA_BASE_ADDRESS + 13, 6, rec_data, rec_resp);
                    for (int i = 0; i < 6; i++)
                        cout << "READ PERFORMED. DATA: " << (int) rec_data[i] << " \tSTATUS: " << (int) rec_resp[i] << endl;

                    cout << endl;
                
                    resp = axi_interface->write(MOTOR_BASE_ADDRESS + 5, data_2, 5);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                    resp = axi_interface->write(MOTOR_BASE_ADDRESS + 9, data_1, 5);
                    cout << "WRITE PERFORMED. RESPONSE RECEIVED: " << (int) resp << endl;
                    cout << endl;

                    axi_interface->read(MOTOR_BASE_ADDRESS + 6, 3, rec_data, rec_resp);
                    for (int i = 0; i < 3; i++)
                        cout << "READ PERFORMED. DATA: " << (int) rec_data[i] << " \tSTATUS: " << (int) rec_resp[i] << endl;

                    cout << endl;
                }
                wait();
            }   
        };

    public:
        bool ready, comm_ongoing;
        uint8_t* register_memory;
        sc_port<Master_2_AXI_Port> axi_interface;

        //Input Port
        sc_core::sc_in <bool> enable;

        //Output Port
        sc_core::sc_out <bool> go;
            
        SC_CTOR(Core): 
            enable("Enable_signal"),
            go("Go")
        {
            register_memory = new uint8_t[MEMORY_SIZE];
            for (int i = 0; i < MEMORY_SIZE; i++)
                register_memory[i] = 0;

            SC_THREAD(sensor_logic);
            sensitive << enable;
        }

        Core(){}
        ~Core(){
            delete[]register_memory;
        }
};