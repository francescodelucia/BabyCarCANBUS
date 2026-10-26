/*#include "ESP32_TWAI.h"
#include <cstddef>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifndef CAN_ROUTINES
  #define CAN_ROUTINES

// Struttura dati interna sicura per la coda FreeRTOS (array fissi, zero puntatori)
struct CanTxMessage {
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
};

class CanManager {  
  public:
    int8_t data_avalible = -1;
    
  public:
    CanManager(CAN_DATA *c_data, uint32_t txQueueSize = 20){
      this->c_data = c_data;
      // La coda gestisce la struct sicura CanTxMessage
      _txQueue = xQueueCreate(txQueueSize, sizeof(CanTxMessage));
      init();
    }
    
    ~CanManager(){
      if (_txQueue != NULL) {
        vQueueDelete(_txQueue);
      }
    }

  private:
    void init(){
      CAN.begin(CanBitRate::BR_500k, CAN_TX, CAN_RX);
    }
    
  public:    
    CAN_DATA *c_data;

  private:
    QueueHandle_t _txQueue;

  public:    
    // -------------------------------------------------------------
    // 1. TRASMISSIONE DIRETTA (ID, Dati, Lunghezza)
    // -------------------------------------------------------------
    bool send(uint32_t id, const uint8_t *data, uint8_t len, TickType_t timeout = 0) {
      if (len > 8) len = 8;
      
      CanTxMessage msg;
      msg.id = id;
      msg.len = len;
      for (uint8_t i = 0; i < len; i++) {
        msg.data[i] = data[i];
      }
      
      return (xQueueSend(_txQueue, &msg, timeout) == pdPASS);
    }

    // -------------------------------------------------------------
    // 2. TRASMISSIONE TRAMITE STRUTTURA (CAN_DATA *)
    // -------------------------------------------------------------
    bool send(CAN_DATA *tx_data, TickType_t timeout = 0) {   
      if (tx_data != nullptr) {        
        CanTxMessage msg;
        msg.id = tx_data->can_id;
        msg.len = (tx_data->can_len > 8) ? 8 : tx_data->can_len;
        
        for (uint8_t i = 0; i < msg.len; i++) {
          msg.data[i] = tx_data->can_data[i];
        }
        
        return (xQueueSend(_txQueue, &msg, timeout) == pdPASS);
      }
      return false;
    }

    // -------------------------------------------------------------
    // TASK DI TRASMISSIONE SEPARATO
    // -------------------------------------------------------------
    void processTx() {
      CanTxMessage msg;
      
      // Si mette in attesa bloccante finché non c'è un messaggio (0% CPU)
      if (xQueueReceive(_txQueue, &msg, portMAX_DELAY) == pdPASS) {
        
        unsigned char canMsg[16] = {0x00};
        for (uint8_t i = 0; i < msg.len; i++) {
          canMsg[i] = msg.data[i];
        }

#ifdef ENABLE_DEBUG        
        Serial.printf("[TX] Frame: %03X, Len: %d\r\n", msg.id, msg.len);
#endif

        CanMsg const msgCAN(CanStandardId(msg.id), msg.len, canMsg);
        CAN.write(msgCAN);
      }
    }

    // -------------------------------------------------------------
    // TASK DI RICEZIONE SEPARATO
    // -------------------------------------------------------------
    void processRx() {
      if (CAN.available()) {             
        CanMsg data = CAN.read();
        for(int k = 0; k < data.data_length; k++){
          this->c_data->can_data[k] = data.data[k];  
        }             
        (this->c_data->can_id) = data.id;   
        (this->c_data->can_len) = data.data_length;                        
        (this->c_data->data_avalible) = 1;
      } else {
        vTaskDelay(pdMS_TO_TICKS(2)); 
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
*/
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
    void getData2(uint8_t *data,uint8_t *data_len){
      *data_len =0;
      if (this->c_data->data_avalible !=-1) {        
        *data_len = this->c_data->can_len+1;
        uint8_t _data[*data_len] ={0};              
        _data[0] = this->c_data->can_id;    
#ifdef ENABLE_DEBUG        
        Serial.printf("Received frame: %03X \r\n", this->c_data->can_id);             
        Serial.print("Data length: ");
        Serial.println(this->c_data->can_len); //Print the data length
        Serial.print("Data: ");
#endif                            
        for(int k = 1;k< this->c_data->can_len+1;k++){          
          _data[k] = this->c_data->can_data[k-1];
#ifdef ENABLE_DEBUG                 
          Serial.print(this->c_data->can_data[k-1], HEX); 
#endif                              
        }        
#ifdef ENABLE_DEBUG                         
        Serial.print("\n"); // Transmit value from the frame    
#endif                                    
        memcpy(data,&_data,*data_len);  
        this->c_data->data_avalible = -1;        
      }else{
        this->c_data->can_len = -1;        
      }
      //return this->c_data;               
    }
};

#endif