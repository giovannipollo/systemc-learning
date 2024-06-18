#ifndef AXI_INTERFACE
#define AXI_INTERFACE

#include <systemc.h>
#define READ_REQUEST_BUS 1
#define WRITE_REQUEST_BUS 2
#define CLK_PERIOD 2
#define CLK_TIME_UNIT SC_NS
#define PRINT_AXI_MASTER_DEBUG 0
#define PRINT_AXI_SLAVE_DEBUG 0
#define PRINT_AXI_BUS_DEBUG 0

#define NUM_SENSORS 2
#define CAMERA_BASE_ADDRESS 0
#define MOTOR_BASE_ADDRESS 16

struct AXI_M_Interface {
          
    // Clock 
    sc_out<bool> clk{"M_Clock"};

    // Read address channel
    sc_in<bool> AR_ready{"M_AR_ready"};
    sc_out<int> AR_address{"M_AR_Address"};
    sc_out<uint8_t> AR_length{"M_AR_length"};
    sc_out<bool> AR_valid{"M_AR_valid"};

    // Read data channel
    sc_in<bool> R_last{"M_R_Last"};
    sc_in<bool> R_valid{"M_R_Valid"};
    sc_in<sc_uint<2>> R_resp{"M_R_resp"};
    sc_in<uint8_t> R_data{"M_R_Data"};
    sc_out<bool> R_ready{"M_R_Ready"};

    // Write address channel
    sc_in<bool> AW_ready{"M_AW_ready"};
    sc_out<int> AW_address{"M_AW_Address"};
    sc_out<bool> AW_valid{"M_AW_valid"};

    // Write data channel
    sc_in<bool> W_ready{"M_W_Ready"};
    sc_out<uint8_t> W_data{"M_W_Data"};
    sc_out<bool> W_last{"M_W_Last"};
    sc_out<bool> W_valid{"M_W_Valid"};

    // Write response channel
    sc_in<bool> B_valid{"M_B_valid"};
    sc_in<sc_uint<2>> B_resp{"M_B_resp"};
    sc_out<bool> B_ready{"M_B_ready"};
};

struct AXI_S_Interface {
          
    // Clock 
    sc_in<bool> clk{"S_Clock"};

    // Read address channel
    sc_in<int> AR_address{"S_AR_Address"};
    sc_in<uint8_t> AR_length{"S_AR_length"};
    sc_in<bool> AR_valid{"S_AR_valid"};
    sc_out<bool> AR_ready{"S_AR_ready"};

    // Read data channel
    sc_in<bool> R_ready{"S_R_Ready"};
    sc_out<bool> R_last{"S_R_Last"};
    sc_out<bool> R_valid{"S_R_Valid"};
    sc_out<sc_uint<2>> R_resp{"S_R_resp"};
    sc_out<uint8_t> R_data{"S_R_Data"};

    // Write address channel
    sc_in<int> AW_address{"S_AW_Address"};
    sc_in<bool> AW_valid{"S_AW_valid"};
    sc_out<bool> AW_ready{"S_AW_ready"};

    // Write data channel
    sc_in<uint8_t> W_data{"S_W_Data"};
    sc_in<bool> W_last{"S_W_Last"};
    sc_in<bool> W_valid{"S_W_Valid"};
    sc_out<bool> W_ready{"S_W_Ready"};

    // Write response channel
    sc_in<bool> B_ready{"S_B_ready"};
    sc_out<bool> B_valid{"S_B_valid"};
    sc_out<sc_uint<2>> B_resp{"S_B_resp"};
};

class Master_2_AXI_Port : virtual public sc_interface {

    public: 
        virtual void read(int address, int length, uint8_t* data, uint8_t* response)=0;
        virtual int write(int address, uint8_t* data, int length)=0;
};

class AXI_2_Slave_Port : virtual public sc_interface {

    public: 
        virtual bool is_ready()=0;
        virtual void start_transaction()=0;
        virtual void close_transaction()=0;

        virtual bool read(int address, uint8_t* data)=0;
        virtual int write(int address, uint8_t data)=0;
};

class AXI_2_Bus_Port : virtual public sc_interface {

    public:
        virtual bool is_response_ready(uint8_t *response, uint8_t *data)=0;
        virtual void send_request(int type, int address, int size)=0;
        virtual void write(int address, uint8_t data)=0;
};
#endif