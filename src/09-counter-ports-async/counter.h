#include <systemc.h>

// Define the counter module
SC_MODULE(Counter) {
    // Ports
    sc_in<bool> clk;       // Clock input
    sc_in<bool> reset;     // Asynchronous reset input
    sc_out<int> count_out; // Counter output

    // Internal variable to store the count value
    int count;

    // Constructor
    SC_CTOR(Counter) : count(0) {
        // Register the clock process method to the clock's positive edge
        SC_METHOD(clock_process);
        sensitive << clk.pos();
        sensitive << reset.pos();
    }

    // Clock process method
    void clock_process() {
        if (reset.read() == false) { // Only increment when reset is not active
            count++;
            count_out.write(count); // Write the count to the output port
        } else {
            count = 0;
            count_out.write(0);
        }
    }
};