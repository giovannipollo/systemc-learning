#include <systemc.h>
#include "sensor.h"
#include "axi_slave.h"

SC_MODULE(TB) {
    // TB signals
    sc_trace_file* tf;
    sc_clock clk{"clock", 2, SC_NS}; 
    Sensor camera{"camera"};
    AxiSlave slave{"AxiSlave"};

    // Camera
    sc_signal<bool> enable, flag_wr, valid, ready;
    sc_signal<int> address;
    sc_signal<uint8_t> data_axi_to_sens, data_sens_to_axi;
    sc_signal<bool> go;


    // Write address channel
    sc_signal<int> AW_address;
    sc_signal<bool> AW_valid, AW_ready;

    // Write data channel
    sc_signal<uint8_t> W_data;
    sc_signal<bool> W_valid, W_ready, W_last;

    SC_CTOR(TB) {
        SC_THREAD(run);

        // Signal of the sensor
        camera.enable(enable);
        //camera.address(address);
        //camera.data_in(data_axi_to_sens);
        //camera.flag_wr(flag_wr);
        //camera.valid(valid);
        camera.data_out(data_sens_to_axi);
        camera.ready(ready);
        camera.go(go);

        // Signal of the AXI slave
        //slave.mod_interface.type(flag_wr);
        slave.mod_interface.ready(ready);
        //slave.mod_interface.valid(valid);
        //slave.mod_interface.data_in(data_sens_to_axi);
        slave.mod_interface.data_out(data_axi_to_sens);
        //slave.mod_interface.address(address);

        // TB
        slave.enable(enable);
        slave.axi_interface.clk(clk);
        slave.axi_interface.AW_address(AW_address);
        slave.axi_interface.AW_valid(AW_valid);
        slave.axi_interface.AW_ready(AW_ready);
        slave.axi_interface.W_data(W_data);
        slave.axi_interface.W_valid(W_valid);
        slave.axi_interface.W_ready(W_ready);
        slave.axi_interface.W_last(W_last);
    }

    void run() {
        // Trace file to generate a waveform
        tf = sc_create_vcd_trace_file("trace");
        sc_trace(tf, clk, "clk");
        sc_trace(tf, AW_ready, "READY");
        sc_trace(tf, enable, "Sensor_Enable");
        sc_trace(tf, AW_address, "AW_ADDRESS");
        sc_trace(tf, AW_valid, "AW_VALID");
        sc_trace(tf, W_data, "W_DATA");
        sc_trace(tf, W_valid, "W_VALID");
        sc_trace(tf, W_last, "W_LAST");
        
        sc_trace(tf, camera.register_memory[0], "RF0");
        sc_trace(tf, camera.register_memory[1], "RF1");
        sc_trace(tf, camera.register_memory[2], "RF2");
        sc_trace(tf, camera.register_memory[3], "RF3");
        sc_trace(tf, camera.register_memory[4], "RF4");
        sc_trace(tf, camera.register_memory[5], "RF5");
        sc_trace(tf, camera.register_memory[6], "RF6");
        sc_trace(tf, camera.register_memory[7], "RF7");
        sc_trace(tf, camera.register_memory[8], "RF8");
        sc_trace(tf, camera.register_memory[9], "RF9");
        sc_trace(tf, camera.register_memory[10], "RF10");
        sc_trace(tf, camera.register_memory[11], "RF11");
        sc_trace(tf, camera.register_memory[12], "RF12");
        sc_trace(tf, camera.register_memory[13], "RF13");
        sc_trace(tf, camera.register_memory[14], "RF14");
        sc_trace(tf, camera.register_memory[15], "RF15");

        wait(clk.posedge_event());
        wait(0.5, SC_NS);

        // Enable sensor and AXI module
        cout << "@" << sc_time_stamp() << ": raising ENABLE" << endl;
        enable = 1;
        wait(1, SC_NS);

        cout << "@" << sc_time_stamp() << ": raising AW_VALID and sending AW_ADDRESS" << endl;
        AW_valid = 1;
        AW_address = 5;

        if (AW_ready.read() == false)
            wait(AW_ready.posedge_event());

        wait(clk.posedge_event());
        wait(1.5, SC_NS);

        AW_valid = 0;
        W_data = 100;
        wait(2, SC_NS);

        W_valid = 1;
        wait(clk.posedge_event());

        W_data = 255;
        wait(clk.posedge_event());

        W_data = 9850;
        wait(clk.posedge_event());

        W_data = 10;
        W_last = 1;
        wait(clk.posedge_event());
        wait(0.5, SC_NS);

        W_valid = 0;
        W_data = 0;
        W_last = 0;

        wait(30, SC_NS);
        sc_stop();
        sc_close_vcd_trace_file(tf);
    }

};

int sc_main(int argc, char* argv[]) {
    TB testbench("tb");

    // Run the simulation
    sc_start();
    return 0;
}
