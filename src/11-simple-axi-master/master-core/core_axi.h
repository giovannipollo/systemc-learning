#include <systemc.h>
#include "core.h"
#include "axi_master.h"

SC_MODULE(Core_M_Axi) {

    // Port
    sc_in<bool> enable;
    sc_out<bool> go;
    AXI_M_Interface axi_if;

    // Modules
    Core core{"core"};
    AxiMaster master{"AxiMaster"};

    SC_CTOR(Core_M_Axi) {
        core.enable(enable);
        core.go(go);
        core.axi_interface(master);

        master.enable(enable);
        master.axi_interface.clk(axi_if.clk);
        master.axi_interface.AW_address(axi_if.AW_address);
        master.axi_interface.AW_valid(axi_if.AW_valid);
        master.axi_interface.AW_ready(axi_if.AW_ready);
        master.axi_interface.W_data(axi_if.W_data);
        master.axi_interface.W_valid(axi_if.W_valid);
        master.axi_interface.W_ready(axi_if.W_ready);
        master.axi_interface.W_last(axi_if.W_last);
        master.axi_interface.B_ready(axi_if.B_ready);
        master.axi_interface.B_valid(axi_if.B_valid);
        master.axi_interface.B_resp(axi_if.B_resp);
        master.axi_interface.AR_address(axi_if.AR_address);
        master.axi_interface.AR_valid(axi_if.AR_valid);
        master.axi_interface.AR_length(axi_if.AR_length);
        master.axi_interface.AR_ready(axi_if.AR_ready);
        master.axi_interface.R_data(axi_if.R_data);
        master.axi_interface.R_valid(axi_if.R_valid);
        master.axi_interface.R_ready(axi_if.R_ready);
        master.axi_interface.R_last(axi_if.R_last);
        master.axi_interface.R_resp(axi_if.R_resp);
    }
};

