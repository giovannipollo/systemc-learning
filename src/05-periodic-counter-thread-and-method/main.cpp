#include <iostream>
#include <systemc.h>

using namespace std;
SC_MODULE(counter) {
    int cnt = 0;

    SC_CTOR(counter) {
        cout << "Counter initialized" << endl;
        SC_METHOD(count_method);
        SC_THREAD(count_thread);

    }

    void count_thread() {
        while (true) {
            cout << "TIME @" << sc_time_stamp() <<" THREAD: Counter value: " << cnt << endl;
            cnt++;
            cout << "Incrementing counter inside THREAD" << endl;
            wait(10, SC_NS);
        }
    }

    void count_method() {
        cout << "TIME@ " << sc_time_stamp() << " METHOD: Counter value: " << cnt << endl;
        cnt++;
        cout << "Incrementing counter inside METHOD" << endl;
        next_trigger(sc_time(20, SC_NS));
        wait(10, SC_NS);
    }

};

int sc_main(int, char *[]) {
    counter c("counter"); // Counter module instantiation
    sc_start(50, SC_NS); // Start the simulation for 100 ns
    cout << "Simulation ending @" << sc_time_stamp() << endl;
    return 0;
}