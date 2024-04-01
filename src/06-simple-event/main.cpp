#include <iostream>
#include <systemc.h>

using namespace std;
SC_MODULE(counter) {
    int cnt = 0;
    sc_event increment_cnt;

    SC_CTOR(counter) {
        cout << "Counter initialized" << endl;
        SC_THREAD(count_thread);
        SC_METHOD(trigger_increment_cnt);

    }

    void trigger_increment_cnt() {
        increment_cnt.notify();
        next_trigger(10, SC_NS);
    }

    void count_thread() {
        while (true) {
            cout << "TIME @" << sc_time_stamp() <<" THREAD: Counter value: " << cnt << endl;
            cnt++;
            cout << "Incrementing counter inside THREAD" << endl;
            wait(increment_cnt);
        }
    }
};

int sc_main(int, char *[]) {
    counter c("counter"); // Counter module instantiation
    sc_start(50, SC_NS); // Start the simulation for 100 ns
    cout << "Simulation ending @" << sc_time_stamp() << endl;
    return 0;
}