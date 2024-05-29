#include <systemc.h>

struct AXI_Interface : public sc_interface {
    sc_in<bool> clk;   // Clock input
    sc_in<bool> valid; // Valid input
    sc_out<bool> ready; // Ready output
};