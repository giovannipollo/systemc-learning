#include <systemc.h>
#include "core_axi.h"
#include "functional_bus_axi.h"
#include "sensor_axi.h"
#define PRINT_TB_DEBUG 0

SC_MODULE(TB) {
    // TB signals
    sc_trace_file* tf;
    Core_M_Axi core{"core"};
    Functional_Bus_Axi fbus{"fbus"};
    Sensor_S_Axi camera{"camera"};
    Sensor_S_Axi motor{"motor"};

    // NON-AXI signals
    sc_signal<bool> enable, clk_CORE, clk_SENSOR;
    sc_signal<bool> enable_SENSOR[NUM_SENSORS];
    sc_signal<bool> go_core, go_SENSOR[NUM_SENSORS];

    /* CORE-BUS AXI signals
     */
    // Write address channel
    sc_signal<int> AW_address_CORE;
    sc_signal<bool> AW_valid_CORE, AW_ready_CORE, aw_ready;

    // Write data channel
    sc_signal<uint8_t> W_data_CORE;
    sc_signal<bool> W_valid_CORE, W_ready_CORE, W_last_CORE, w_ready;

    // Write response channel
    sc_signal<bool> B_ready_CORE, B_valid_CORE, b_valid;
    sc_signal<sc_uint<2>> B_resp_CORE, b_resp;

    // Read address channel
    sc_signal<int> AR_address_CORE;
    sc_signal<uint8_t> AR_len_CORE;
    sc_signal<bool> AR_valid_CORE, AR_ready_CORE, ar_ready;

    // Read data channel
    sc_signal<uint8_t> R_data_CORE, r_data;
    sc_signal<sc_uint<2>> R_resp_CORE, r_resp;
    sc_signal<bool> R_valid_CORE, r_valid, R_ready_CORE, R_last_CORE, r_last;

    /* SENSOR-BUS AXI signals
     */
    sc_signal<int> AW_address_SENSOR;
    sc_signal<bool> AW_valid_SENSOR, AW_ready_SENSOR[NUM_SENSORS];
    
    // Write data channel
    sc_signal<uint8_t> W_data_SENSOR;
    sc_signal<bool> W_valid_SENSOR, W_ready_SENSOR[NUM_SENSORS], W_last_SENSOR;
    
    // Write response channel
    sc_signal<bool> B_ready_SENSOR, B_valid_SENSOR[NUM_SENSORS];
    sc_signal<sc_uint<2>> B_resp_SENSOR[NUM_SENSORS];
    
    // Read address channel
    sc_signal<int> AR_address_SENSOR;
    sc_signal<uint8_t> AR_len_SENSOR;
    sc_signal<bool> AR_valid_SENSOR, AR_ready_SENSOR[NUM_SENSORS];
    
    // Read data channel
    sc_signal<uint8_t> R_data_SENSOR[NUM_SENSORS];
    sc_signal<sc_uint<2>> R_resp_SENSOR[NUM_SENSORS];
    sc_signal<bool> R_valid_SENSOR[NUM_SENSORS], R_ready_SENSOR, R_last_SENSOR[NUM_SENSORS];


    SC_CTOR(TB) {
        SC_THREAD(run);
        SC_THREAD(multiplex_sensors_out);
        for (int i = 0; i < NUM_SENSORS; i++) {
            sensitive << enable_SENSOR[i];
            sensitive << AW_ready_SENSOR[i];
            sensitive << W_ready_SENSOR[i];
            sensitive << B_valid_SENSOR[i];
            sensitive << B_resp_SENSOR[i];
            sensitive << AR_ready_SENSOR[i];
            sensitive << R_data_SENSOR[i];
            sensitive << R_resp_SENSOR[i];
            sensitive << R_valid_SENSOR[i];
            sensitive << R_last_SENSOR[i];
        }

        // Enable
        core.enable(enable);
        camera.enable(enable);
        motor.enable(enable);

        fbus.sensor_enable[0](enable_SENSOR[0]);
        camera.axi_enable(enable_SENSOR[0]);
        fbus.sensor_enable[1](enable_SENSOR[1]);
        motor.axi_enable(enable_SENSOR[1]);

        // Go
        core.go(go_core);
        fbus.go_sensors[0](go_SENSOR[0]);
        camera.go(go_SENSOR[0]);
        fbus.go_sensors[1](go_SENSOR[1]);
        motor.go(go_SENSOR[1]);

        // Clock
        core.axi_if.clk(clk_CORE);
        fbus.axi_if_S.clk(clk_CORE);
        fbus.axi_if_M.clk(clk_SENSOR);
        camera.axi_if.clk(clk_SENSOR);
        motor.axi_if.clk(clk_SENSOR);

        // CORE-BUS signals
        core.axi_if.AW_address(AW_address_CORE);
        core.axi_if.AW_valid(AW_valid_CORE);
        core.axi_if.AW_ready(AW_ready_CORE);
        core.axi_if.W_data(W_data_CORE);
        core.axi_if.W_valid(W_valid_CORE);
        core.axi_if.W_ready(W_ready_CORE);
        core.axi_if.W_last(W_last_CORE);
        core.axi_if.B_ready(B_ready_CORE);
        core.axi_if.B_valid(B_valid_CORE);
        core.axi_if.B_resp(B_resp_CORE);
        core.axi_if.AR_address(AR_address_CORE);
        core.axi_if.AR_valid(AR_valid_CORE);
        core.axi_if.AR_length(AR_len_CORE);
        core.axi_if.AR_ready(AR_ready_CORE);
        core.axi_if.R_data(R_data_CORE);
        core.axi_if.R_valid(R_valid_CORE);
        core.axi_if.R_ready(R_ready_CORE);
        core.axi_if.R_last(R_last_CORE);
        core.axi_if.R_resp(R_resp_CORE);

        fbus.axi_if_S.AW_address(AW_address_CORE);
        fbus.axi_if_S.AW_valid(AW_valid_CORE);
        fbus.axi_if_S.AW_ready(AW_ready_CORE);        
        fbus.axi_if_S.W_data(W_data_CORE);
        fbus.axi_if_S.W_valid(W_valid_CORE);
        fbus.axi_if_S.W_ready(W_ready_CORE);
        fbus.axi_if_S.W_last(W_last_CORE);
        fbus.axi_if_S.B_ready(B_ready_CORE);
        fbus.axi_if_S.B_valid(B_valid_CORE);
        fbus.axi_if_S.B_resp(B_resp_CORE);
        fbus.axi_if_S.AR_address(AR_address_CORE);
        fbus.axi_if_S.AR_valid(AR_valid_CORE);
        fbus.axi_if_S.AR_length(AR_len_CORE);
        fbus.axi_if_S.AR_ready(AR_ready_CORE);
        fbus.axi_if_S.R_data(R_data_CORE);
        fbus.axi_if_S.R_valid(R_valid_CORE);
        fbus.axi_if_S.R_ready(R_ready_CORE);
        fbus.axi_if_S.R_last(R_last_CORE);
        fbus.axi_if_S.R_resp(R_resp_CORE);

        // SENSORS-BUS signals
        fbus.axi_if_M.AW_address(AW_address_SENSOR);
        fbus.axi_if_M.AW_valid(AW_valid_SENSOR);
        fbus.axi_if_M.AW_ready(aw_ready);        
        fbus.axi_if_M.W_data(W_data_SENSOR);
        fbus.axi_if_M.W_valid(W_valid_SENSOR);
        fbus.axi_if_M.W_ready(w_ready);
        fbus.axi_if_M.W_last(W_last_SENSOR);
        fbus.axi_if_M.B_ready(B_ready_SENSOR);
        fbus.axi_if_M.B_valid(b_valid);
        fbus.axi_if_M.B_resp(b_resp);
        fbus.axi_if_M.AR_address(AR_address_SENSOR);
        fbus.axi_if_M.AR_valid(AR_valid_SENSOR);
        fbus.axi_if_M.AR_length(AR_len_SENSOR);
        fbus.axi_if_M.AR_ready(ar_ready);
        fbus.axi_if_M.R_data(r_data);
        fbus.axi_if_M.R_valid(r_valid);
        fbus.axi_if_M.R_ready(R_ready_SENSOR);
        fbus.axi_if_M.R_last(r_last);
        fbus.axi_if_M.R_resp(r_resp);

        camera.axi_if.AW_address(AW_address_SENSOR);
        camera.axi_if.AW_valid(AW_valid_SENSOR);
        camera.axi_if.AW_ready(AW_ready_SENSOR[0]);        
        camera.axi_if.W_data(W_data_SENSOR);
        camera.axi_if.W_valid(W_valid_SENSOR);
        camera.axi_if.W_ready(W_ready_SENSOR[0]);
        camera.axi_if.W_last(W_last_SENSOR);
        camera.axi_if.B_ready(B_ready_SENSOR);
        camera.axi_if.B_valid(B_valid_SENSOR[0]);
        camera.axi_if.B_resp(B_resp_SENSOR[0]);
        camera.axi_if.AR_address(AR_address_SENSOR);
        camera.axi_if.AR_valid(AR_valid_SENSOR);
        camera.axi_if.AR_length(AR_len_SENSOR);
        camera.axi_if.AR_ready(AR_ready_SENSOR[0]);
        camera.axi_if.R_data(R_data_SENSOR[0]);
        camera.axi_if.R_valid(R_valid_SENSOR[0]);
        camera.axi_if.R_ready(R_ready_SENSOR);
        camera.axi_if.R_last(R_last_SENSOR[0]);
        camera.axi_if.R_resp(R_resp_SENSOR[0]);

        motor.axi_if.AW_address(AW_address_SENSOR);
        motor.axi_if.AW_valid(AW_valid_SENSOR);
        motor.axi_if.AW_ready(AW_ready_SENSOR[1]);        
        motor.axi_if.W_data(W_data_SENSOR);
        motor.axi_if.W_valid(W_valid_SENSOR);
        motor.axi_if.W_ready(W_ready_SENSOR[1]);
        motor.axi_if.W_last(W_last_SENSOR);
        motor.axi_if.B_ready(B_ready_SENSOR);
        motor.axi_if.B_valid(B_valid_SENSOR[1]);
        motor.axi_if.B_resp(B_resp_SENSOR[1]);
        motor.axi_if.AR_address(AR_address_SENSOR);
        motor.axi_if.AR_valid(AR_valid_SENSOR);
        motor.axi_if.AR_length(AR_len_SENSOR);
        motor.axi_if.AR_ready(AR_ready_SENSOR[1]);
        motor.axi_if.R_data(R_data_SENSOR[1]);
        motor.axi_if.R_valid(R_valid_SENSOR[1]);
        motor.axi_if.R_ready(R_ready_SENSOR);
        motor.axi_if.R_last(R_last_SENSOR[1]);
        motor.axi_if.R_resp(R_resp_SENSOR[1]);
    }

    /* When multiple sensors are considered, their outputs must be multiplexed towards the BUS
     * This is done by checking the enable_SENSOR signal, which, hopefully, is HIGH for one sensor at a time
     * When no sensor is enabled, every signal is resetted to LOW
     */
    void multiplex_sensors_out() {
        while (true) {
            aw_ready = false;
            ar_ready = false;
            w_ready = false;
            b_resp = false;
            b_valid = false;
            r_data = false;
            r_resp = false;
            r_valid = false;
            r_last = false;

            for (int i = 0; i < NUM_SENSORS; i++) {
                if (enable_SENSOR[i].read() == true) {
                    if (PRINT_TB_DEBUG)
                        cout << "Forwarding AXI output to sensor: " << i << endl;
                        
                    aw_ready = AW_ready_SENSOR[i];
                    ar_ready = AR_ready_SENSOR[i];
                    w_ready = W_ready_SENSOR[i];

                    b_resp = B_resp_SENSOR[i];
                    b_valid = B_valid_SENSOR[i];

                    r_data = R_data_SENSOR[i];
                    r_resp = R_resp_SENSOR[i];
                    r_valid = R_valid_SENSOR[i];
                    r_last = R_last_SENSOR[i];
                }
            }

            wait();
        }
    }

    void run() {
        // Trace file to generate a waveform
        tf = sc_create_vcd_trace_file("trace");
        sc_trace(tf, clk_CORE, "clk_core");
        sc_trace(tf, clk_SENSOR, "clk_bus");
        sc_trace(tf, enable, "enable");
        sc_trace(tf, camera.axi_enable, "ENABLE_CAMERA");
        sc_trace(tf, motor.axi_enable, "ENABLE_MOTOR");


        sc_trace(tf, AW_address_CORE, "MASTER_AW_address");
        sc_trace(tf, AW_valid_CORE, "MASTER_AW_valid");
        sc_trace(tf, AW_ready_CORE, "MASTER_AW_ready");
        sc_trace(tf, W_data_CORE, "MASTER_W_data");
        sc_trace(tf, W_valid_CORE, "MASTER_W_valid");
        sc_trace(tf, W_ready_CORE, "MASTER_W_ready");
        sc_trace(tf, W_last_CORE, "MASTER_W_last");
        sc_trace(tf, B_ready_CORE, "MASTER_B_ready");
        sc_trace(tf, B_valid_CORE, "MASTER_B_valid");
        sc_trace(tf, B_resp_CORE, "MASTER_B_resp");
        sc_trace(tf, AR_address_CORE, "MASTER_AR_address");
        sc_trace(tf, AR_len_CORE, "MASTER_AR_length");
        sc_trace(tf, AR_valid_CORE, "MASTER_AR_valid");
        sc_trace(tf, AR_ready_CORE, "MASTER_AR_ready");
        sc_trace(tf, R_data_CORE, "MASTER_R_data");
        sc_trace(tf, R_resp_CORE, "MASTER_R_resp");
        sc_trace(tf, R_valid_CORE, "MASTER_R_valid");
        sc_trace(tf, R_ready_CORE, "MASTER_R_ready");
        sc_trace(tf, R_last_CORE, "MASTER_R_last");
        
        sc_trace(tf, AW_address_SENSOR, "SLAVE_AW_address");
        sc_trace(tf, AW_valid_SENSOR, "SLAVE_AW_valid");
        sc_trace(tf, aw_ready, "SLAVE_AW_ready");
        sc_trace(tf, W_data_SENSOR, "SLAVE_W_data");
        sc_trace(tf, W_valid_SENSOR, "SLAVE_W_valid");
        sc_trace(tf, w_ready, "SLAVE_W_ready");
        sc_trace(tf, W_last_SENSOR, "SLAVE_W_last");
        sc_trace(tf, B_ready_SENSOR, "SLAVE_B_ready");
        sc_trace(tf, b_valid, "SLAVE_B_valid");
        sc_trace(tf, b_resp, "SLAVE_B_resp");
        sc_trace(tf, AR_address_SENSOR, "SLAVE_AR_address");
        sc_trace(tf, AR_len_SENSOR, "SLAVE_AR_length");
        sc_trace(tf, AR_valid_SENSOR, "SLAVE_AR_valid");
        sc_trace(tf, ar_ready, "SLAVE_AR_ready");
        sc_trace(tf, r_data, "SLAVE_R_data");
        sc_trace(tf, r_resp, "SLAVE_R_resp");
        sc_trace(tf, r_valid, "SLAVE_R_valid");
        sc_trace(tf, R_ready_SENSOR, "SLAVE_R_ready");
        sc_trace(tf, r_last, "SLAVE_R_last");

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

        sc_trace(tf, motor.camera.register_memory[0], "MOTOR_RF0");
        sc_trace(tf, motor.camera.register_memory[1], "MOTOR_RF1");
        sc_trace(tf, motor.camera.register_memory[2], "MOTOR_RF2");
        sc_trace(tf, motor.camera.register_memory[3], "MOTOR_RF3");
        sc_trace(tf, motor.camera.register_memory[4], "MOTOR_RF4");
        sc_trace(tf, motor.camera.register_memory[5], "MOTOR_RF5");
        sc_trace(tf, motor.camera.register_memory[6], "MOTOR_RF6");
        sc_trace(tf, motor.camera.register_memory[7], "MOTOR_RF7");
        sc_trace(tf, motor.camera.register_memory[8], "MOTOR_RF8");
        sc_trace(tf, motor.camera.register_memory[9], "MOTOR_RF9");
        sc_trace(tf, motor.camera.register_memory[10], "MOTOR_RF10");
        sc_trace(tf, motor.camera.register_memory[11], "MOTOR_RF11");
        sc_trace(tf, motor.camera.register_memory[12], "MOTOR_RF12");
        sc_trace(tf, motor.camera.register_memory[13], "MOTOR_RF13");
        sc_trace(tf, motor.camera.register_memory[14], "MOTOR_RF14");
        sc_trace(tf, motor.camera.register_memory[15], "MOTOR_RF15");   

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

        wait(clk_CORE.posedge_event());
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
