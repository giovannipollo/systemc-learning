#include <systemc.h>
#include "master-core/core_axi.h"
#define PRINT_TB_DEBUG 1

SC_MODULE(TB) {
    // TB signals
    sc_trace_file* tf;
    Core_M_Axi core{"core"};

    // Core
    sc_signal<bool> enable, clk, go;

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
        core.axi_if.clk(clk);
        core.go(go);

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
        
        sc_trace(tf, core.core.register_memory[0], "RF0");
        sc_trace(tf, core.core.register_memory[1], "RF1");
        sc_trace(tf, core.core.register_memory[2], "RF2");
        sc_trace(tf, core.core.register_memory[3], "RF3");
        sc_trace(tf, core.core.register_memory[4], "RF4");
        sc_trace(tf, core.core.register_memory[5], "RF5");
        sc_trace(tf, core.core.register_memory[6], "RF6");
        sc_trace(tf, core.core.register_memory[7], "RF7");
        sc_trace(tf, core.core.register_memory[8], "RF8");
        sc_trace(tf, core.core.register_memory[9], "RF9");
        sc_trace(tf, core.core.register_memory[10], "RF10");
        sc_trace(tf, core.core.register_memory[11], "RF11");
        sc_trace(tf, core.core.register_memory[12], "RF12");
        sc_trace(tf, core.core.register_memory[13], "RF13");
        sc_trace(tf, core.core.register_memory[14], "RF14");
        sc_trace(tf, core.core.register_memory[15], "RF15");

        wait(clk.posedge_event());
        wait(0.5, SC_NS);

        // Enable sensor and AXI module
        enable = 1;
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising ENABLE" << endl;
            
        wait(1, SC_NS);

        // // Test a WRITE operation
        // int address = 10;
        // int data[5] = {27, 89, 10, 12, 90};
        // write_tb(address, data, 5);

        // Test a READ operation
        int data[9] = {3, 4, 6, 23, 12, 67, 89, 12, 76};
        int data_rp[9] = {0, 1, 2, 2, 1, 0, 1, 0, 2};
        read_tb(data, 3, data_rp);
        wait(clk.posedge_event());
        wait(clk.posedge_event());

        // Test a WRITE operation
        write_tb();
        wait(clk.posedge_event());
        wait(clk.posedge_event());

        sc_stop();
        sc_close_vcd_trace_file(tf);
    }

    void read_tb(int* data, int length, int* resp) {
        int address, length_rec;
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": waiting AR_valid" << endl;

        if (AR_valid.read() == false)
            wait(AR_valid.posedge_event());

        address = AR_address.read();
        length_rec = AR_len.read();
        wait(0.5, SC_NS);

        AR_ready = true;
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": raising AR_ready" << endl;
        
        wait(clk.posedge_event());
        AR_ready = false;
        if (R_ready.read() == false)
            wait(R_ready.posedge_event());

        wait(clk.posedge_event());
        R_valid = 1;
        for (int i = 0; i < length_rec; i++) {
            R_data = data[i];
            R_resp = resp[i];
            
            if (i == length_rec - 1)
                R_last = 1;
            
            wait(clk.posedge_event());
        }

        R_data = 0;
        R_resp = 0;
        R_last = 0;
    }

    void write_tb() {
        int address; 
        uint8_t data;

        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": waiting AW_valid" << endl;

        if (AW_valid.read() == false)
            wait(AW_valid.posedge_event());
        
        wait(clk.posedge_event());
        AW_ready = true;
        address = AW_address.read();
        if (PRINT_TB_DEBUG)
            cout << "@" << sc_time_stamp() << ": received AW_address " << address << endl;

        wait(clk.posedge_event());
        
        AW_ready = false;
        wait(clk.posedge_event());

        W_ready = true;
        while(true) {
            if (W_valid.read() == true) 
                if (PRINT_TB_DEBUG)
                    cout << "@" << sc_time_stamp() << ": received data " << (int) W_data.read() << endl;

            if (W_last.read() == true) {
                if (PRINT_TB_DEBUG)
                    cout << "@" << sc_time_stamp() << " last packet received" << endl;

                break;
            }

            wait(clk.posedge_event());       
        }

        W_ready = false;
        wait(clk.posedge_event());          

        B_valid = true;
        B_resp = 2; 
        if (B_ready.read() == false)
            wait(B_ready.posedge_event());

        wait(clk.posedge_event());
        B_valid = false;
        B_resp = 0;
        wait(clk.posedge_event());
    }
};

int sc_main(int argc, char* argv[]) {
    TB testbench("tb");

    // Run the simulation
    sc_start();
    return 0;
}
