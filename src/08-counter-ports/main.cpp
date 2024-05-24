#include <systemc.h>
#include "counter.h"


int sc_main(int argc, char* argv[]) {
    // Signals
    sc_signal<bool> clk;
    sc_signal<bool> reset;
    sc_signal<int> count_out;

    // Instantiate the counter module
    Counter counter("Counter");

    // Bind the signals to the counter's ports
    counter.clk(clk);
    counter.reset(reset);
    counter.count_out(count_out);

    // Trace file to generate a waveform
    sc_trace_file* tf = sc_create_vcd_trace_file("counter_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, reset, "reset");
    sc_trace(tf, count_out, "count_out");

    // Simulation setup
    reset = true;  // Apply reset
    sc_start(1, SC_NS);
    reset = false; // Release reset

    // Run the simulation for a few clock cycles
    for (int i = 0; i < 5; ++i) {
        clk = true;
        sc_start(1, SC_NS);
        clk = false;
        sc_start(1, SC_NS);
    }

    reset = true;
    sc_start(1, SC_NS);
    clk = true;
    reset = false;
    sc_start(1, SC_NS);
    reset = false; // Release reset
    clk = false;

    // Run the simulation for a few clock cycles
    for (int i = 0; i < 5; ++i) {
        sc_start(1, SC_NS);
        clk = true;
        sc_start(1, SC_NS);
        clk = false;
    }

    // Close the trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}
