#include <systemc.h>
#include "axi_slave.h"

int sc_main(int argc, char* argv[]) {
    // Define the interface signals
    sc_signal<bool> clk;
    sc_signal<bool> valid;
    sc_signal<bool> ready;

    // Instantiate the slave module
    AxiSlave slave("AxiSlave");

    // Pass the interface signals to the slave module
    slave.axi_slave_interface_inst->clk(clk);
    slave.axi_slave_interface_inst->valid(valid);
    slave.axi_slave_interface_inst->ready(ready);

    // Trace file to generate a waveform
    sc_trace_file* tf = sc_create_vcd_trace_file("axi_slave_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, valid, "valid");
    sc_trace(tf, ready, "ready");

    // Run the simulation
    clk = 0;
    valid = 0;
    sc_start(1, SC_NS);

    clk = 1;
    valid = 0;
    sc_start(1, SC_NS);

    clk = 0;
    valid = 0;
    sc_start(1, SC_NS);

    // Close the trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}
