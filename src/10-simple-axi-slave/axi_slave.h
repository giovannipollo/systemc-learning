#include <systemc.h>
#include "axi_interface.h"
#include "port.h"

SC_MODULE(AxiSlave) {

    sc_in<bool> enable;
    AXI_Module mod_interface;           // From peripheral
    AXI_Interface axi_interface;        // To bus

    sc_port<AXI_2_Slave_Port> mod{"temp"};

    sc_signal<int> address;
    sc_signal<uint8_t> data;

    SC_CTOR(AxiSlave) {
        SC_THREAD(handshake_process);
        sensitive << mod_interface.ready;
        //sensitive << mod_interface.data_in;
        
        sensitive << axi_interface.clk.pos();
        sensitive << axi_interface.AW_address;
        sensitive << axi_interface.AW_valid;
        sensitive << axi_interface.W_data;
        sensitive << axi_interface.W_last;
        sensitive << axi_interface.W_valid;
        sensitive << enable;

        SC_THREAD(update_sensor_signals);
        sensitive << mod_interface.ready;
    }

    void handshake_process() {
        int w_err = 0;

        while(true) {
            if (enable.read() == true) {
                wait(axi_interface.AW_valid.posedge_event());

                /* // READ: address received
                if (axi_interface.AR_valid.read() == true) {
                    //cout << "@" << sc_time_stamp() << ": READ received" << endl;
                    // wait(axi_interface.clk.posedge_event());

                    // // Raise AR_READY and retrieve ADDRESS at next posedge
                    // axi_interface.AR_ready.write(true);
                    // wait(axi_interface.clk.posedge_event());
                    // address = axi_interface.AW_address.read();
                    // wait(axi_interface.clk.posedge_event());
                    //cout << "@" << sc_time_stamp() << ": MODULE received " <<  address << " address" << endl;
                } */
                // WRITE: address received 
                if (axi_interface.AW_valid.read() == true) {
                    cout << "@" << sc_time_stamp() << ": 'Camera' : AW_VALID received" << endl;

                    // Wait for the sensor to be ready for accepting the communication
                    if (mod_interface.ready.read() == false) {
                        cout << "@" << sc_time_stamp() << ": waiting for sensor READY" << endl;
                        wait(mod_interface.ready.posedge_event());
                    }

                    address = axi_interface.AW_address.read();
                    wait(axi_interface.clk.posedge_event());

                    // In the clock cycle after both VALID and READY are high, handshake is completed
                    cout << "@" << sc_time_stamp() << " 'Camera' : handshake complete; Starting comms..." << endl;
                    w_err = 0;

                    while(true) {
                        if (axi_interface.W_valid.read() == true && mod_interface.ready.read() == true) {
                            mod->write(address, axi_interface.W_data.read());
                            address = address + 1;          
                            cout << "@" << sc_time_stamp() << " 'Camera' : packet received with: " << (int) axi_interface.W_data.read() << endl;
                        }

                        if (axi_interface.W_last.read() == true) 
                            break;

                        wait(axi_interface.clk.posedge_event());       
                    }

                    cout << "@" << sc_time_stamp() << " 'Camera' : last packet received" << endl;
                    wait(axi_interface.clk.posedge_event());
                }
            }
            wait();
        }
    }

    void update_sensor_signals() {
        while(true) {
            axi_interface.AW_ready.write(mod_interface.ready.read());
            axi_interface.W_ready.write(mod_interface.ready.read());
            wait();
        }
    }
};

