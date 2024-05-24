#include <systemc.h>

// Define the counter module
SC_MODULE(Counter) {
    // Ports
    sc_in<bool> clk;       // Clock input
    sc_in<bool> reset;     // Reset input
    sc_out<int> count_out; // Counter output

    // Internal variable to store the count value
    int count;

    // Constructor
    SC_CTOR(Counter) : count(0) {
        // Register the process method to the clock's positive edge
        SC_METHOD(count_process);
        sensitive << clk.pos();
    }

    // Process method
    void count_process() {
        if (reset.read() == true) {
            count = 0; // Reset the count to 0
        } else {
            count++;   // Increment the count
        }
        count_out.write(count); // Write the count to the output port
    }
};