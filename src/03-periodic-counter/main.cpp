#include <iostream>
#include <systemc.h>

using namespace std;
SC_MODULE(counter) {
    int cnt = 0;
    int cnt_no_next_trigger = 0;
    
    SC_CTOR(counter){
        cout << "Counter initialized" << endl;
        SC_METHOD(count_method);
        SC_METHOD(count_no_next_trigger_method);
    }

    void count_method() {
        cout << "Counter value: " << cnt << endl;
        cnt++;
        next_trigger(sc_time(10, SC_NS));
    }

    void count_no_next_trigger_method(){
        cout << "Counter no next trigger value: " << cnt_no_next_trigger << endl;
        cnt_no_next_trigger++;
    
    }
};

int sc_main(int, char *[]) {
    counter c("counter"); // Counter module instantiation
    sc_start(100, SC_NS); // Start the simulation for 100 ns
    return 0;
}