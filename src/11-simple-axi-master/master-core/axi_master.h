#include <systemc.h>
#include "axi_master_interface.h"

class AxiMaster : public sc_core::sc_module, public Master_2_AXI_Port {

    public: 
        // Interface
        sc_in<bool> enable;
        AXI_M_Interface axi_interface;

        SC_CTOR(AxiMaster) {
            SC_THREAD(generate_clock);
        }

        void generate_clock() {
            while (true) {
                axi_interface.clk.write(false);
                wait(1, SC_NS);
                axi_interface.clk.write(true);
                wait(1, SC_NS);
            }
        }

        Read_Response_M read(int address, int length) {
            int i = 0;
            Read_Response_M res;
            res.data = new uint8_t[length];
            res.req_status = new uint8_t[length];

            // Check if the module was enabled
            if (enable.read() == false) {
                for (int i = 0; i < length; i++) 
                    res.req_status[i] = 2;

                return res;
            }

            // Start the operation by synchronizing its events to the clock positive edge
            wait(axi_interface.clk.posedge_event());

            // Raise VALID, ADDRESS and LENGTH to the slave device
            axi_interface.AR_valid = 1;
            axi_interface.AR_address = address;
            axi_interface.AR_length = length;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER READ]: raising address, valid and length on the AR channel " << endl;

            if (axi_interface.AR_ready.read() == false)
                wait(axi_interface.AR_ready.posedge_event());

            wait(axi_interface.clk.posedge_event());
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER READ]: handshake completed; Started read operations... " << endl;

            // Once the READY has been raised, activate the READY on the R channel to receive data
            axi_interface.AR_valid = 0;
            axi_interface.AR_address = 0;
            axi_interface.AR_length = 0;
            axi_interface.R_ready = 1;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER READ]: started reading data burst " << endl;
            
            while (true) {
                if (axi_interface.R_valid.read() == true) {
                    res.data[i] = axi_interface.R_data.read();
                    res.req_status[i] = axi_interface.R_resp.read();
                    i++;

                    if (axi_interface.R_last.read() == true)
                        break;
                }

                wait(axi_interface.clk.posedge_event());
            }

            axi_interface.R_ready = 0;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER READ]: finished reading data burst " << endl;

            return res;
        }

        int write(int address, uint8_t* data, int length) {
            int res; 

            // Check if the module was enabled
            if (enable.read() == false)
                return 1;

            // Start the operation by synchronizing its events to the clock positive edge
            wait(axi_interface.clk.posedge_event());

            // Raise VALID, ADDRESS and LENGTH to the slave device
            axi_interface.AW_valid = 1;
            axi_interface.AW_address = address;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: raising address and valid on the AW channel " << endl;

            if (axi_interface.AW_ready.read() == false)
                wait(axi_interface.AW_ready.posedge_event());
            
            wait(axi_interface.clk.posedge_event());
            axi_interface.AW_valid = 0;
            axi_interface.AW_address = 0;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: handshake completed; Started write operations... " << endl;

            // Wait for the W_READY signal before raising W_VALID
            // Wait the rising edge of the clock after W_READY is raised to account for signal instability on the channel
            if (axi_interface.W_ready.read() == false)
                wait(axi_interface.W_ready.posedge_event());

            wait(axi_interface.clk.posedge_event());
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: start sending data " << endl;

            axi_interface.W_valid = 1;
            axi_interface.W_last = 0;
            for (int i = 0; i < length; i++) {
                axi_interface.W_data = data[i];
                if (i == length - 1) 
                    axi_interface.W_last = 1;

                wait(axi_interface.clk.posedge_event());
            }

            axi_interface.W_valid = 0;
            axi_interface.W_data = 0;
            axi_interface.W_last = 0;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: finished sending data " << endl;

            // Last step: wait for the response from the slave device
            if (axi_interface.B_valid.read() == false)
                wait(axi_interface.B_valid.posedge_event());

            wait(axi_interface.clk.posedge_event());
            axi_interface.B_ready = 1;
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: preparing to receive response " << endl;
                
            res = axi_interface.B_resp.read();
            wait(axi_interface.clk.posedge_event());
            if (PRINT_AXI_MASTER_DEBUG)
                cout << "@" << sc_time_stamp() << " [MASTER WRITE]: response received! " << endl;

            axi_interface.B_ready = 0;
            return res;
        }
};

