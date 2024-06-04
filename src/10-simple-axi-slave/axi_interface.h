#ifndef AXI_INTERFACE
#define AXI_INTERFACE
#include <systemc.h>

struct AXI_Interface {
          
    // Clock 
    sc_in<bool> clk{"Clock"};

    // Write address channel
    sc_in<int> AW_address{"AW_Address"};
    sc_in<bool> AW_valid{"AW_valid"};
    sc_out<bool> AW_ready{"AW_ready"};

    // Write data channel
    sc_in<uint8_t> W_data{"W_Data"};
    sc_in<bool> W_last{"W_Last"};
    sc_in<bool> W_valid{"W_Valid"};
    sc_out<bool> W_ready{"W_Ready"};
};

struct AXI_Module {

    sc_in<bool> ready{"Ready"};        // Module ready to receive a transmission
    //sc_in<uint8_t> data_in{"Data_In"};     // Data from MASTER
    //sc_out<bool> type{"Type"};         // R' / W
    //sc_out<bool> valid{"Valid"}; 
    //sc_out<int> address{"Address"};
    sc_out<uint8_t> data_out{"Data_Out"};  // Data from SLAVE
};
#endif