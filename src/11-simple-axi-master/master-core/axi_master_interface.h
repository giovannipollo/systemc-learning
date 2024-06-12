#ifndef AXI_MASTER_INTERFACE
#define AXI_MASTER_INTERFACE

#include <systemc.h>
#define PRINT_AXI_MASTER_DEBUG 1

struct AXI_M_Interface {
          
    // Clock 
    sc_out<bool> clk{"Clock"};

    // Read address channel
    sc_in<bool> AR_ready{"AR_ready"};
    sc_out<int> AR_address{"AR_Address"};
    sc_out<uint8_t> AR_length{"AR_length"};
    sc_out<bool> AR_valid{"AR_valid"};

    // Read data channel
    sc_in<bool> R_last{"R_Last"};
    sc_in<bool> R_valid{"R_Valid"};
    sc_in<sc_uint<2>> R_resp{"R_resp"};
    sc_in<uint8_t> R_data{"R_Data"};
    sc_out<bool> R_ready{"R_Ready"};

    // Write address channel
    sc_in<bool> AW_ready{"AW_ready"};
    sc_out<int> AW_address{"AW_Address"};
    sc_out<bool> AW_valid{"AW_valid"};

    // Write data channel
    sc_in<bool> W_ready{"W_Ready"};
    sc_out<uint8_t> W_data{"W_Data"};
    sc_out<bool> W_last{"W_Last"};
    sc_out<bool> W_valid{"W_Valid"};

    // Write response channel
    sc_in<bool> B_valid{"B_valid"};
    sc_in<sc_uint<2>> B_resp{"B_resp"};
    sc_out<bool> B_ready{"B_ready"};
};

struct Read_Response_M {
    uint8_t* data;
    uint8_t* req_status;
};

class Master_2_AXI_Port : virtual public sc_interface {

    public: 
        virtual Read_Response_M read(int address, int length)=0;
        virtual int write(int address, uint8_t* data, int length)=0;
};
#endif