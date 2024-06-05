#include <systemc.h>
#include "sensor.h"
#include "axi_slave.h"
#define PRINT_TB_DEBUG 1

SC_MODULE(TB) {
    // TB signals
    sc_trace_file* tf;
    sc_clock clk{"clock", 2, SC_NS}; 
    Sensor camera{"camera"};
    AxiSlave slave{"AxiSlave"};

    // Camera
    sc_signal<bool> enable, go;

    // Write address channel
    sc_signal<int> AW_address;
    sc_signal<bool> AW_valid, AW_ready;

    // Write data channel
    sc_signal<uint8_t> W_data;
    sc_signal<bool> W_valid, W_ready, W_last;

    // Write response channel
    sc_signal<bool> B_ready, B_valid;
    sc_signal<sc_uint<2>> B_resp, resp;

    // Read address channel
    sc_signal<int> AR_address;
    sc_signal<uint8_t> AR_len;
    sc_signal<bool> AR_valid, AR_ready;

    // Read data channel
    sc_signal<uint8_t> R_data;
    sc_signal<sc_uint<2>> R_resp, read_resp;
    sc_signal<bool> R_valid, R_ready, R_last;

    SC_CTOR(TB) {
        SC_THREAD(run);

        // Signal of the sensor
        camera.enable(enable);
        camera.go(go);

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
        slave.axi_interface.B_ready(B_ready);
        slave.axi_interface.B_valid(B_valid);
        slave.axi_interface.B_resp(B_resp);
        slave.axi_interface.AR_address(AR_address);
        slave.axi_interface.AR_valid(AR_valid);
        slave.axi_interface.AR_length(AR_len);
        slave.axi_interface.AR_ready(AR_ready);
        slave.axi_interface.R_data(R_data);
        slave.axi_interface.R_valid(R_valid);
        slave.axi_interface.R_ready(R_ready);
        slave.axi_interface.R_last(R_last);
        slave.axi_interface.R_resp(R_resp);
        slave.mod_interface(camera);
    }

    void run() {
        // Trace file to generate a waveform
        tf = sc_create_vcd_trace_file("trace");
        sc_trace(tf, clk, "clk");
        sc_trace(tf, enable, "Sensor_Enable");
        sc_trace(tf, AW_address, "AW_ADDRESS");
        sc_trace(tf, AW_valid, "AW_VALID");
        sc_trace(tf, AW_ready, "AW_READY");
        sc_trace(tf, W_data, "W_DATA");
        sc_trace(tf, W_valid, "W_VALID");
        sc_trace(tf, W_last, "W_LAST");
        sc_trace(tf, W_ready, "W_READY");
        sc_trace(tf, B_ready, "B_READY");
        sc_trace(tf, B_valid, "B_VALID");
        sc_trace(tf, B_resp, "B_RESP");
        sc_trace(tf, AR_address, "AR_ADDRESS");
        sc_trace(tf, AR_valid, "AR_VALID");
        sc_trace(tf, AR_len, "AR_LEN");
        sc_trace(tf, AR_ready, "AR_READY");
        sc_trace(tf, R_ready, "R_READY");
        sc_trace(tf, R_data, "R_DATA");
        sc_trace(tf, R_valid, "R_VALID");
        sc_trace(tf, R_resp, "R_RESP");
        sc_trace(tf, R_last, "R_LAST");
        
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
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising ENABLE" << endl;
        enable = 1;
        wait(1, SC_NS);

        // Test a WRITE operation
        write_tb();

        // Test a READ operation
        read_tb();

        sc_stop();
        sc_close_vcd_trace_file(tf);
    }

    void read_tb() {
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising AR_VALID and sending AR_ADDRESS" << endl;

        AR_valid = 1;
        AR_address = 5;
        AR_len = 3;
        wait(clk.posedge_event());

        if (AR_ready.read() == false)
            wait(AR_ready.posedge_event());

        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": handshake complete; Starting read ops..." << endl;

        wait(clk.posedge_event());
        wait(1.5, SC_NS);
        AR_valid = 0;
        AR_address = 0;
        AR_len = 0;
        R_ready = 1;
        wait(clk.posedge_event());

        for (int i = 0; i < 3; i++)
            wait(clk.posedge_event());

        R_ready = 0;
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": finished read..." << endl;
        wait(30, SC_NS);
    }

    void write_tb() {
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising AW_VALID and sending AW_ADDRESS" << endl;
        AW_valid = 1;
        AW_address = 5;

        if (AW_ready.read() == false)
            wait(AW_ready.posedge_event());

        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": handshake complete; Starting comms..." << endl;
        wait(clk.posedge_event());
        wait(1.5, SC_NS);
        
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": sending data..." << endl;
        AW_valid = 0;
        W_data = 100;
        wait(2, SC_NS);

        W_valid = 1;
        if (W_ready.read() == false)
            wait(W_ready.posedge_event());
        wait(clk.posedge_event());

        W_data = 255;
        wait(clk.posedge_event());

        W_data = 35;
        wait(clk.posedge_event());

        W_data = 74;
        wait(clk.posedge_event());

        W_data = 10;
        W_last = 1;
        wait(clk.posedge_event());
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": finished data..." << endl;
        wait(0.5, SC_NS);

        W_valid = 0;
        W_data = 0;
        W_last = 0;
        wait(clk.posedge_event());

        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": waiting response status..." << endl;
        B_ready = 1;
        resp = B_resp.read();
        if (B_valid.read() == false)
            wait(B_valid.posedge_event());

        wait(clk.posedge_event());
        B_ready = 0;
        if (resp.read() > 0)
            cout << "ERROR RECEIVED" << endl;

        wait(clk.posedge_event());
    }
};

int sc_main(int argc, char* argv[]) {
    TB testbench("tb");

    // Run the simulation
    sc_start();
    return 0;
}
