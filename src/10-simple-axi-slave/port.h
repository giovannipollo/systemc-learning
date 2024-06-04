#ifndef PORT_INTERFACE
#define PORT_INTERFACE
#include <systemc.h>

class AXI_2_Slave_Port : virtual public sc_interface {
    
    public: 
        virtual int write(int address, uint8_t data)=0;
};
#endif