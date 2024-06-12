#include <systemc.h>
#include "axi_master_interface.h"

class Core : public sc_core::sc_module {

    private:
        int MEMORY_SIZE=16;

        void sensor_logic() {
            uint8_t data[5] = {16, 11, 81, 23, 54};

            while (true) {
                if(enable.read() == true) {
                    axi_interface->read(5, 9);
                    axi_interface->write(4, data, 3);
                }
                wait();
            }   
        };

    public:
        bool ready, comm_ongoing;
        uint8_t* register_memory;
        sc_port<Master_2_AXI_Port> axi_interface;

        //Input Port
        sc_core::sc_in <bool> enable;

        //Output Port
        sc_core::sc_out <bool> go;
            
        SC_CTOR(Core): 
            enable("Enable_signal"),
            go("Go")
        {
            register_memory = new uint8_t[MEMORY_SIZE];
            for (int i = 0; i < MEMORY_SIZE; i++)
                register_memory[i] = 0;

            SC_THREAD(sensor_logic);
            sensitive << enable;
        }

        Core(){}
        ~Core(){
            delete[]register_memory;
        }
};