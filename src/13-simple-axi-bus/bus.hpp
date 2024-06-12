#include <systemc.h>
#include "axi/axi_master.h"
#include "axi/axi_slave.h"
#include "axi/axi_interface.h"

SC_MODULE(Functional_bus){

    //Input Port
    sc_core::sc_in <unsigned int>  request_address;
    sc_core::sc_in <uint8_t*>  request_data;
    sc_core::sc_in <bool> flag_from_core;
    sc_core::sc_in <bool> request_ready;
    sc_core::sc_in <unsigned int> request_size;
    sc_core::sc_in <uint8_t*>  data_input_sensor[NUM_SENSORS];
    sc_core::sc_in <bool> go_sensors[NUM_SENSORS];
    //Output Port
    sc_core::sc_out <uint8_t*>  request_value;
    sc_core::sc_out <bool> request_go;
    sc_core::sc_out <int> idx_sensor;
    sc_core::sc_out <unsigned int>  address_out_sensor[NUM_SENSORS];
    sc_core::sc_out <uint8_t*>  data_out_sensor[NUM_SENSORS];
    sc_core::sc_out <unsigned int>  size_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> flag_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> ready_sensor[NUM_SENSORS];

    SC_CTOR(Functional_bus):
        request_address("Address_from_Master_to_Bus"),
        request_data("Data_from_Master_to_Bus"),
        flag_from_core("Flag_from_Master_to_Bus"),
        request_ready("Ready_from_Master_to_Bus"),
        request_value("Data_from_Bus_to_Master"),
        request_go("Go_from_Bus_to_Master"),
        idx_sensor("selected_sensor_from_request")
    {
        SC_THREAD(processing_data);
        sensitive << request_ready;
        for (int i = 0; i < NUM_SENSORS; i++){
            sensitive << go_sensors[i];
        }      
        
    }

    void processing_data(){
        unsigned int add_s;
        uint8_t* data_s;
        unsigned int reg_s;
        unsigned int size_s;
        bool flag_s;
        
        while (true){
            if (request_ready.read() == true) {
                add_s = request_address.read();
                flag_s = flag_from_core.read();
                size_s = request_size.read();
                
                if
                (camera_BASE_ADDRESS <= add_s && add_s < (camera_BASE_ADDRESS + 102402)) 
                {
                    if(add_s+size_s > (camera_BASE_ADDRESS + 102402)){
                        fprintf(stderr,"\n[MESSY]\tRead/Write at %d of %d bytes exceeds camera memory\n",add_s,size_s);
                        selected_sensor=-1;
                        request_value.write(0x0);
                        request_go.write(true);
                    }
                    else{
                        reg_s = add_s - camera_BASE_ADDRESS;
                        data_s = request_data.read();
                        size_out_sensor[0].write(size_s);
                        address_out_sensor[0].write(reg_s);
                        data_out_sensor[0].write(data_s);
                        flag_out_sensor[0].write(flag_s);
                        selected_sensor = 0;
                        ready_sensor[selected_sensor].write(true);
                    }
                }
                else if
                (stm32_wrapper_BASE_ADDRESS <= add_s && add_s < (stm32_wrapper_BASE_ADDRESS + 34)) 
                {
                    if(add_s+size_s > (stm32_wrapper_BASE_ADDRESS + 34)){
                        fprintf(stderr,"\n[MESSY]\tRead/Write at %d of %d bytes exceeds stm32_wrapper memory\n",add_s,size_s);
                        selected_sensor=-1;
                        request_value.write(0x0);
                        request_go.write(true);
                    }
                    else{
                        reg_s = add_s - stm32_wrapper_BASE_ADDRESS;
                        data_s = request_data.read();
                        size_out_sensor[1].write(size_s);
                        address_out_sensor[1].write(reg_s);
                        data_out_sensor[1].write(data_s);
                        flag_out_sensor[1].write(flag_s);
                        selected_sensor = 1;
                        ready_sensor[selected_sensor].write(true);
                    }
                }
                else{
                    fprintf(stderr,"\n[MESSY]\tAddress %d not found by functional bus\n", add_s);
                    selected_sensor=-1;
                    request_value.write(0x0);
                    request_go.write(true);
                }
                idx_sensor.write(selected_sensor);
            }
            
            wait();

            if(selected_sensor>=0){

                response();

                while (go_sensors[selected_sensor].read() != false && request_ready.read() != false) {
                    wait();
                }
                
                request_go.write(false);
            }
    
        }
    }
    
    void response(){
        if (go_sensors[selected_sensor].read() == true) {
            request_value.write(data_input_sensor[selected_sensor].read());
            request_go.write(true);
            ready_sensor[selected_sensor].write(false);
        }
    }
    
    private:
        int selected_sensor = 0;

Functional_bus(){}
};