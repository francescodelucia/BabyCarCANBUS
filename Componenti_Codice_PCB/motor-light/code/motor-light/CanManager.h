#include "ESP32_TWAI.h"
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
      if (tx_data->data_avalible!=0) {        
        CanTxMessage msg;
        msg.id = tx_data->can_id;
        msg.len = (tx_data->can_len > 8) ? 8 : tx_data->can_len;        
        for (uint8_t i = 0; i < msg.len; i++) {
          msg.data[i] = tx_data->can_data[i];
        }
        tx_data->data_avalible=0;
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