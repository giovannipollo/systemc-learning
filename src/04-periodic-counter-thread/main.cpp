#include <iostream>
#include <systemc.h>

using namespace std;
SC_MODULE(counter) {
    int cnt = 0;
    int cnt_no_next_trigger = 0;

    SC_CTOR(counter) {
        cout << "Counter initialized" << endl;
        SC_THREAD(count_thread);
    }

    void count_thread() {
        while (true) {
            cout << "Counter value: " << cnt << endl;
            cnt++;
            wait(10, SC_NS);
        }
    }
};

int sc_main(int, char *[]) {
    counter c("counter"); // Counter module instantiation
    sc_start(100, SC_NS); // Start the simulation for 100 ns
    return 0;
}