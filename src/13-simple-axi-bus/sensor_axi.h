#include <systemc.h>
#include "sensor.h"
#include "axi/axi_slave.h"
#include "axi/axi_interface.h"

SC_MODULE(Sensor_S_Axi) {

    // Port
    sc_in<bool> enable;
    sc_out<bool> go;
    AXI_S_Interface axi_if;

    // Modules
    Sensor camera{"camera"};
    AxiSlave slave{"AxiSlave"};

    SC_CTOR(Sensor_S_Axi) {
        camera.enable(enable);
        camera.go(go);

        slave.enable(enable);
        slave.axi_interface.clk(axi_if.clk);
        slave.axi_interface.AW_address(axi_if.AW_address);
        slave.axi_interface.AW_valid(axi_if.AW_valid);
        slave.axi_interface.AW_ready(axi_if.AW_ready);
        slave.axi_interface.W_data(axi_if.W_data);
        slave.axi_interface.W_valid(axi_if.W_valid);
        slave.axi_interface.W_ready(axi_if.W_ready);
        slave.axi_interface.W_last(axi_if.W_last);
        slave.axi_interface.B_ready(axi_if.B_ready);
        slave.axi_interface.B_valid(axi_if.B_valid);
        slave.axi_interface.B_resp(axi_if.B_resp);
        slave.axi_interface.AR_address(axi_if.AR_address);
        slave.axi_interface.AR_valid(axi_if.AR_valid);
        slave.axi_interface.AR_length(axi_if.AR_length);
        slave.axi_interface.AR_ready(axi_if.AR_ready);
        slave.axi_interface.R_data(axi_if.R_data);
        slave.axi_interface.R_valid(axi_if.R_valid);
        slave.axi_interface.R_ready(axi_if.R_ready);
        slave.axi_interface.R_last(axi_if.R_last);
        slave.axi_interface.R_resp(axi_if.R_resp);

        slave.mod_interface(camera);
    }
};

