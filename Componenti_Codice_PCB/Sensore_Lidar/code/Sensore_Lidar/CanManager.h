//#include "ESP32_TWAI.h"
#include <driver/twai.h>
#include <cstddef>
#include <stdint.h>
#ifndef CAN_ROUTINES
  #define CAN_ROUTINES

class CanManager{  
  public:
    int8_t data_avalible = -1;
  public:
    CanManager(CAN_DATA *c_data){
      this->c_data = c_data;
      this->initCAN();
    }
    ~CanManager(){}
  private:    
    bool initCAN() {
      twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX,CAN_RX,TWAI_MODE_NORMAL);
      twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
      twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
      if (twai_driver_install( &g_config,&t_config,&f_config) != ESP_OK) {  
          Serial.println("Errore installazione TWAI");
          return false;
      }
      if (twai_start() != ESP_OK) {
          Serial.println("Errore avvio TWAI");
          return false;
      }
      Serial.println("CAN TWAI avviato");
      return true;
    }    
  public:    
    CAN_DATA *c_data;// ={-1,0,-1,{0}};    
  public:
    //void sendCAN(uint32_t id, uint8_t* data, uint8_t len) 
    void sendCAN(CAN_DATA *tx_data) 
    {
      if( tx_data->data_avalible == 1){    
        tx_data->data_avalible = -1;        
        
        twai_message_t message = {};

        message.identifier = tx_data->can_id;
        message.extd = 0;
        message.rtr = 0;
        message.data_length_code = tx_data->can_len;
        for(int i=0; i< tx_data->can_len; i++)
        {        
          //canMsg[i] = tx_data->can_data[i];
          message.data[i] = tx_data->can_data[i];
        }
#ifdef ENABLE_DEBUG        
        Serial.printf("Received frame: %03X \r\n", tx_data->can_id);             
        Serial.print("Data length: ");
        Serial.println(tx_data->can_len); //Print the data length
        Serial.print("Data: ");
        for(int k = 0;k< tx_data->can_len;k++){
          Serial.print(tx_data->can_data[k], HEX); 
        }        
        Serial.print("\n"); // Transmit value from the frame    
#endif        
        //memcpy(message.data, data, len);

        esp_err_t result =
            twai_transmit(&message, pdMS_TO_TICKS(10));

        if (result != ESP_OK) {
            Serial.println("Errore TX CAN");
        }
      }
    }
    
    void pooling() {
      twai_message_t message;
      if (twai_receive(&message, 0) == ESP_OK) {
        for(int k=0;k<message.data_length_code;k++){
          this->c_data->can_data[k] = message.data[k];  
        }             
        this->c_data->can_id = message.identifier;   
        this->c_data->can_len = message.data_length_code;                        
        this->c_data->data_avalible = 1;
      }
    }
     void getData(CAN_DATA *_data){          
      if (this->c_data->data_avalible == 1) {   
        _data->data_avalible = 1;     
        _data->can_len = this->c_data->can_len;        
        _data->can_id = this->c_data->can_id;                                
        for(int k = 0; k < this->c_data->can_len; k++){          
          _data->can_data[k] = this->c_data->can_data[k];                              
        }        
        this->c_data->data_avalible = -1;   
      } else {
        this->c_data->can_len = -1;        
        _data->can_len = -1;        
        _data->data_avalible = -1;                
      }               
    }    
};

#endif
