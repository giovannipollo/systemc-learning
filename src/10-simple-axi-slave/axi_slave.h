#include <systemc.h>
#include "axi_slave_interface.h"

SC_MODULE(AxiSlave) {

    // Instantiate the AXI slave interface
    sc_port<AXI_Interface> axi_slave_interface_inst;

    SC_CTOR(AxiSlave) {
        SC_THREAD(clock_process);
        sensitive << axi_slave_interface_inst->clk.pos();
    }

    void clock_process() {
        if (axi_slave_interface_inst->valid.read() == true) {
            axi_slave_interface_inst->ready.write(true);
            wait(1, SC_NS);
            axi_slave_interface_inst->ready.write(false);
            cout << "Data received at time: " << sc_time_stamp() << endl;
        }
    }
};

