#include <systemc.h>
#include "axi_interface.h"

class Sensor : public sc_core::sc_module, public AXI_2_Slave_Port {

    private:
        int MEMORY_SIZE=16;

        void sensor_logic() {
            while (true) {
                if(enable.read() == true) {
                    ready = true;
                    if (PRINT_AXI_DEBUG)
                        cout << "@" << sc_time_stamp() << ": [CAMERA INTERNAL]: Checking AXI status... " << endl;

                    if (comm_ongoing == true)
                        wait(can_work.posedge_event());

                    ready = false;
                    if (PRINT_AXI_DEBUG)
                        cout << "@" << sc_time_stamp() << ": [CAMERA INTERNAL]: Starting internal work; Cannot communicate... " << endl;

                    /* -------------- 
                     * DO OTHER STUFF
                     * --------------
                     */
                    wait(10, SC_NS);

                    ready = true;
                    if (PRINT_AXI_DEBUG)
                        cout << "@" << sc_time_stamp() << ": [CAMERA INTERNAL]: Finished work... " << endl;
                }
                wait();
            }   
        };

    public:
        bool ready, comm_ongoing;
        uint8_t* register_memory;
        sc_signal<bool> can_work;

        //Input Port
        sc_core::sc_in <bool> enable;

        //Output Port
        sc_core::sc_out <bool> go;
            
        SC_CTOR(Sensor): 
            enable("Enable_signal"),
            ready("Ready"),
            go("Go")
        {
            register_memory = new uint8_t[MEMORY_SIZE];

            SC_THREAD(sensor_logic);
            sensitive << enable;
            sensitive << can_work;
        }

        Sensor(){}
        ~Sensor(){
            delete[]register_memory;
        }

        /* The AXI module needs to know the status of the sensor to decide if it can start a communication
         */
        bool is_ready() {
            return ready;
        }

        /* Indicates that a communication was started with the bus
         * During this phase, the sensor must not perform any other action
         */
        void start_transaction() {
            comm_ongoing = true;
            can_work = false;
        }

        /* Indicates that a communication was completed with the bus
         * The sensor is free to perform other actions until another communication is started
         */
        void close_transaction() {
            comm_ongoing = false;
            can_work = true;
        }

        Read_Response read(int address) {
            Read_Response res;

            if (address < 0 || address > MEMORY_SIZE) {
                res.r_err = true;
                res.data = 0;
            }
            else {
                res.r_err = false;
                res.data = register_memory[address];
            }

            return res;
        }

        int write(int address, uint8_t data) {
            if (address < 0 || address > MEMORY_SIZE) 
                return 1;

            register_memory[address] = data;
            return 0;
        }
};