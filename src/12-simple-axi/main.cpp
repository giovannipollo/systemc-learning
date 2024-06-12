#include <systemc.h>
#include "core_axi.h"
#include "sensor_axi.h"
#define PRINT_TB_DEBUG 0

SC_MODULE(TB) {
    // TB signals
    sc_trace_file* tf;
    Core_M_Axi core{"core"};
    Sensor_S_Axi camera{"camera"};

    // Core
    sc_signal<bool> enable, clk;
    sc_signal<bool> go_core, go_camera;

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
        core.enable(enable);
        core.go(go_core);
        camera.enable(enable);
        camera.go(go_camera);

        core.axi_if.clk(clk);
        camera.axi_if.clk(clk);

        core.axi_if.AW_address(AW_address);
        core.axi_if.AW_valid(AW_valid);
        core.axi_if.AW_ready(AW_ready);
        core.axi_if.W_data(W_data);
        core.axi_if.W_valid(W_valid);
        core.axi_if.W_ready(W_ready);
        core.axi_if.W_last(W_last);
        core.axi_if.B_ready(B_ready);
        core.axi_if.B_valid(B_valid);
        core.axi_if.B_resp(B_resp);
        core.axi_if.AR_address(AR_address);
        core.axi_if.AR_valid(AR_valid);
        core.axi_if.AR_length(AR_len);
        core.axi_if.AR_ready(AR_ready);
        core.axi_if.R_data(R_data);
        core.axi_if.R_valid(R_valid);
        core.axi_if.R_ready(R_ready);
        core.axi_if.R_last(R_last);
        core.axi_if.R_resp(R_resp);

        camera.axi_if.AW_address(AW_address);
        camera.axi_if.AW_valid(AW_valid);
        camera.axi_if.AW_ready(AW_ready);
        camera.axi_if.W_data(W_data);
        camera.axi_if.W_valid(W_valid);
        camera.axi_if.W_ready(W_ready);
        camera.axi_if.W_last(W_last);
        camera.axi_if.B_ready(B_ready);
        camera.axi_if.B_valid(B_valid);
        camera.axi_if.B_resp(B_resp);
        camera.axi_if.AR_address(AR_address);
        camera.axi_if.AR_valid(AR_valid);
        camera.axi_if.AR_length(AR_len);
        camera.axi_if.AR_ready(AR_ready);
        camera.axi_if.R_data(R_data);
        camera.axi_if.R_valid(R_valid);
        camera.axi_if.R_ready(R_ready);
        camera.axi_if.R_last(R_last);
        camera.axi_if.R_resp(R_resp);
    }

    void run() {
        // Trace file to generate a waveform
        tf = sc_create_vcd_trace_file("trace");
        sc_trace(tf, clk, "clk");
        sc_trace(tf, enable, "enable");

        sc_trace(tf, core.core.register_memory[0], "CORE_RF0");
        sc_trace(tf, core.core.register_memory[1], "CORE_RF1");
        sc_trace(tf, core.core.register_memory[2], "CORE_RF2");
        sc_trace(tf, core.core.register_memory[3], "CORE_RF3");
        sc_trace(tf, core.core.register_memory[4], "CORE_RF4");
        sc_trace(tf, core.core.register_memory[5], "CORE_RF5");
        sc_trace(tf, core.core.register_memory[6], "CORE_RF6");
        sc_trace(tf, core.core.register_memory[7], "CORE_RF7");
        sc_trace(tf, core.core.register_memory[8], "CORE_RF8");
        sc_trace(tf, core.core.register_memory[9], "CORE_RF9");
        sc_trace(tf, core.core.register_memory[10], "CORE_RF10");
        sc_trace(tf, core.core.register_memory[11], "CORE_RF11");
        sc_trace(tf, core.core.register_memory[12], "CORE_RF12");
        sc_trace(tf, core.core.register_memory[13], "CORE_RF13");
        sc_trace(tf, core.core.register_memory[14], "CORE_RF14");
        sc_trace(tf, core.core.register_memory[15], "CORE_RF15");    

        sc_trace(tf, camera.camera.register_memory[0], "CAMERA_RF0");
        sc_trace(tf, camera.camera.register_memory[1], "CAMERA_RF1");
        sc_trace(tf, camera.camera.register_memory[2], "CAMERA_RF2");
        sc_trace(tf, camera.camera.register_memory[3], "CAMERA_RF3");
        sc_trace(tf, camera.camera.register_memory[4], "CAMERA_RF4");
        sc_trace(tf, camera.camera.register_memory[5], "CAMERA_RF5");
        sc_trace(tf, camera.camera.register_memory[6], "CAMERA_RF6");
        sc_trace(tf, camera.camera.register_memory[7], "CAMERA_RF7");
        sc_trace(tf, camera.camera.register_memory[8], "CAMERA_RF8");
        sc_trace(tf, camera.camera.register_memory[9], "CAMERA_RF9");
        sc_trace(tf, camera.camera.register_memory[10], "CAMERA_RF10");
        sc_trace(tf, camera.camera.register_memory[11], "CAMERA_RF11");
        sc_trace(tf, camera.camera.register_memory[12], "CAMERA_RF12");
        sc_trace(tf, camera.camera.register_memory[13], "CAMERA_RF13");
        sc_trace(tf, camera.camera.register_memory[14], "CAMERA_RF14");
        sc_trace(tf, camera.camera.register_memory[15], "CAMERA_RF15");

        wait(clk.posedge_event());
        wait(5, SC_NS);

        // Enable sensor and AXI module
        enable = 1;
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising ENABLE" << endl;
            
        wait(500, SC_NS);

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
