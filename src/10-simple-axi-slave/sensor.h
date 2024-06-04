#include <systemc.h>
#include "port.h"

class Sensor : public sc_core::sc_module, public AXI_2_Slave_Port {

    private:
        int MEMORY_SIZE=16;

    public:
        //Input Port
        sc_core::sc_in <bool> enable;
        //sc_core::sc_in <int> address;
        //sc_core::sc_in <uint8_t> data_in;
        //sc_core::sc_in <bool> flag_wr;
        //sc_core::sc_in <bool> valid;

        //Output Port
        sc_core::sc_out <bool> go;
        sc_core::sc_out <uint8_t>  data_out;
        sc_core::sc_out <bool> ready;

        uint8_t* register_memory;
            
        SC_CTOR(Sensor): 
            enable("Enable_signal"), 
            //address("Address"), 
            //data_in("Data_in"), 
            //flag_wr("Flag"), 
            //valid("Valid"), 
            go("Go"), 
            ready("Ready"), 
            data_out("Data_out")
        {
            register_memory = new uint8_t[MEMORY_SIZE];

            SC_THREAD(sensor_logic);
            sensitive << enable;
            //sensitive << address;
            //sensitive << data_in;
            //sensitive << flag_wr;
            //sensitive << valid;
        }

        Sensor(){}
        ~Sensor(){
            delete[]register_memory;
        }

        void sensor_logic() {
            while (true) {
                if(enable.read() == true) {
                    ready.write(true);

                    //if (valid.read() == true) {
                    //    // WRITE Operation
                    //    if (flag_wr.read() == true) {
                    //        register_memory[address.read()] = data_in;
                    //    }
                    //    // READ Operation
                    //    else {
//
                    //    }
                    //}
                } 
                wait();
            }   
        };

        int write(int address, uint8_t data) {
            // Write data inside register memory
            if (address < 0 || address > MEMORY_SIZE) 
                return 1;

            register_memory[address] = data;
        }
};