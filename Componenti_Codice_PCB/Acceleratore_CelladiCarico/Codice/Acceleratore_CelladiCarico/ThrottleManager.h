#pragma once
#include "common.h"
#include "HX711.h"

class ThrottleManager{
  private:
    HX711 scale;
    // Valori grezzi da tarare con la tua pressione del piede
#ifdef BODY_CAR    
    long RAW_MIN = 1080000;       // Lettura a pedale RIPOSATO (sarà impostata con la tare)
    long RAW_MAX = 1020000; // Lettura a pedale PREMUTO A FONDO (da regolare)
#else
    long RAW_MIN = 7586110;       // Lettura a pedale RIPOSATO (sarà impostata con la tare)
    long RAW_MAX = 7481252; // Lettura a pedale PREMUTO A FONDO (da regolare)
#endif    
    int *throttle_pct; 
  public:
    ThrottleManager(int *throttle_pct){
      this->throttle_pct = throttle_pct;  
      this->init();
    }
    ~ThrottleManager(){}
  private:
    void init(){
      this->scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
      // Attendi stabilità all'avvio
      //delay(1000);  
       vTaskDelay(portTICK_PERIOD_MS*1000);
      // Imposta lo zero all'avvio (pedale non premuto)
      if (this->scale.is_ready()) {
        this->RAW_MIN = this->scale.read_average(10);
#ifdef ENABLE_DEBUG           
        Serial.print("Zero pedale impostato a: ");
        Serial.println(this->RAW_MIN);
#endif        
      }
    }
  public:
    void pooling(){
      if (scale.is_ready()) {
        // Leggi il valore grezzo attuale (senza fare troppe medie per non creare ritardo)
        long raw = this->scale.read();
        // Converte il valore grezzo in percentuale da 0 a 100
        *this->throttle_pct = map(raw, this->RAW_MIN, this->RAW_MAX, 0, 100);
        // Limita il valore tra 0% e 100% per sicurezza
        *throttle_pct = constrain(*this->throttle_pct, 0, 100);
#ifdef ENABLE_DEBUG      
        /*Serial.print("Grezzo: ");
        Serial.print(raw);
        Serial.print(" | Acceleratore: ");
        Serial.print(*this->throttle_pct);
        Serial.println("%");*/
#endif        
      }
      // Piccolissimo ritardo per non saturare la CPU, ma sufficiente per una risposta rapida      
    }
};