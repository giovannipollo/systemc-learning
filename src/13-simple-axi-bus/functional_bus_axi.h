#include <systemc.h>
#include "functional_bus.h"
#include "axi/axi_slave_bus.h"
#include "axi/axi_master_bus.h"
#include "axi/axi_interface.h"

SC_MODULE(Functional_Bus_Axi) {

    // Port
    sc_vector<sc_in <bool>> go_sensors{"go_sensors", NUM_SENSORS};
    sc_vector<sc_out <bool>> sensor_enable{"sensor_enable", NUM_SENSORS};
    AXI_S_Interface axi_if_S;
    AXI_M_Interface axi_if_M;

    // Modules
    Functional_Bus fbus{"fbus_internal"};
    AxiSlave_Bus slave{"AxiSlave_B"};
    AxiMaster_Bus master{"AxiMaster_B"};

    SC_CTOR(Functional_Bus_Axi) {
        for (int i = 0; i < NUM_SENSORS; i++) {
            fbus.go_sensors[i](go_sensors[i]);
            fbus.sensor_enable[i](sensor_enable[i]);
        }

        slave.mod_interface(fbus);
        fbus.axi_interface_to_slave(master);

        slave.axi_interface.clk(axi_if_S.clk);
        slave.axi_interface.AW_address(axi_if_S.AW_address);
        slave.axi_interface.AW_valid(axi_if_S.AW_valid);
        slave.axi_interface.AW_ready(axi_if_S.AW_ready);
        slave.axi_interface.W_data(axi_if_S.W_data);
        slave.axi_interface.W_valid(axi_if_S.W_valid);
        slave.axi_interface.W_ready(axi_if_S.W_ready);
        slave.axi_interface.W_last(axi_if_S.W_last);
        slave.axi_interface.B_ready(axi_if_S.B_ready);
        slave.axi_interface.B_valid(axi_if_S.B_valid);
        slave.axi_interface.B_resp(axi_if_S.B_resp);
        slave.axi_interface.AR_address(axi_if_S.AR_address);
        slave.axi_interface.AR_valid(axi_if_S.AR_valid);
        slave.axi_interface.AR_length(axi_if_S.AR_length);
        slave.axi_interface.AR_ready(axi_if_S.AR_ready);
        slave.axi_interface.R_data(axi_if_S.R_data);
        slave.axi_interface.R_valid(axi_if_S.R_valid);
        slave.axi_interface.R_ready(axi_if_S.R_ready);
        slave.axi_interface.R_last(axi_if_S.R_last);
        slave.axi_interface.R_resp(axi_if_S.R_resp);

        master.axi_interface.clk(axi_if_M.clk);
        master.axi_interface.AW_address(axi_if_M.AW_address);
        master.axi_interface.AW_valid(axi_if_M.AW_valid);
        master.axi_interface.AW_ready(axi_if_M.AW_ready);
        master.axi_interface.W_data(axi_if_M.W_data);
        master.axi_interface.W_valid(axi_if_M.W_valid);
        master.axi_interface.W_ready(axi_if_M.W_ready);
        master.axi_interface.W_last(axi_if_M.W_last);
        master.axi_interface.B_ready(axi_if_M.B_ready);
        master.axi_interface.B_valid(axi_if_M.B_valid);
        master.axi_interface.B_resp(axi_if_M.B_resp);
        master.axi_interface.AR_address(axi_if_M.AR_address);
        master.axi_interface.AR_valid(axi_if_M.AR_valid);
        master.axi_interface.AR_length(axi_if_M.AR_length);
        master.axi_interface.AR_ready(axi_if_M.AR_ready);
        master.axi_interface.R_data(axi_if_M.R_data);
        master.axi_interface.R_valid(axi_if_M.R_valid);
        master.axi_interface.R_ready(axi_if_M.R_ready);
        master.axi_interface.R_last(axi_if_M.R_last);
        master.axi_interface.R_resp(axi_if_M.R_resp);
    }
};

