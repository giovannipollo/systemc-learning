#include <iostream>
#include <systemc.h>

using namespace sc_core;
using namespace std;

// 4-bit counter module
SC_MODULE(counter) {
    SC_CTOR(counter) {
        cout << "Counter module constructor" << endl;
    }
};

int sc_main(int, char *[]) {
    counter c("counter");           // Counter module instantiation
    sc_start(100, SC_NS);           // Start the simulation for 100 ns
    return 0;
}