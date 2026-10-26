#include "ESP32_TWAI.h"
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
      init();
    }
    ~CanManager(){}

  private:
    void init(){
     CAN.begin(CanBitRate::BR_500k,CAN_TX,CAN_RX);
    }
  public:    
    CAN_DATA *c_data;// ={-1,0,-1,{0}};    
  public:
    void send(CAN_DATA *tx_data){//int id, void* data,int len) {   
      if( tx_data->data_avalible == 1){        
        tx_data->data_avalible = -1;
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
        unsigned char canMsg[16] = {0x00};      
        for(int i=0; i< tx_data->can_len; i++)
        {        
          canMsg[i] = tx_data->can_data[i];
        }
        CanMsg const msgCAN(CanStandardId(tx_data->can_id), tx_data->can_len,canMsg);            
        CAN.write(msgCAN);         
      }
    }
    void pooling() {
      if (CAN.available()) { 
        CanMsg data = CAN.read();
        for(int k=0;k<data.data_length;k++){
          this->c_data->can_data[k] = data.data[k];  
        }             
        this->c_data->can_id = data.id;   
        this->c_data->can_len = data.data_length;                        
        this->c_data->data_avalible = 1;
      }            
    }
    void getData(){
      if (this->c_data->data_avalible !=-1) {         
#ifdef ENABLE_DEBUG        
        Serial.printf("Received frame: %03X \r\n", this->c_data->can_id);             
        Serial.print("Data length: ");
        Serial.println(this->c_data->can_len); //Print the data length
        Serial.print("Data: ");
        for(int k = 0;k< this->c_data->can_len;k++){
          Serial.print(this->c_data->can_data[k], HEX); 
        }        
        Serial.print("\n"); // Transmit value from the frame    
#endif                    
        this->c_data->data_avalible = -1;        
      }else{
        this->c_data->can_len = -1;        
      }
      //return this->c_data;               
    }
};

#endif
