#include <sys/_stdint.h>
#pragma once 
#include "common.h"
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

class LidarManager{
  private:  
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    VL53L0X_RangingMeasurementData_t measure;
    uint16_t *distanza;
  public:
    LidarManager(uint16_t *distanza){ 
      this->distanza = distanza;
      this->init();   
    }
    ~LidarManager(){}
  private:
    void init(){
        // Inizializza il bus I2C specificando i pin GPIO 8 e GPIO 9
        Wire.begin(SDA_PIN, SCL_PIN);

      // Inizializza il sensore specificando l'indirizzo predefinito e la porta I2C (Wire)
      if (!this->lox.begin(VL53L0X_I2C_ADDR, false, &Wire)) {
        Serial.println(F("Errore: Impossibile trovare il sensore VL53L0X sui pin 8 e 9."));
        Serial.println(F("Verifica i collegamenti e che il sensore sia alimentato correttamente."));
        while (1);
      }
      Serial.println(F("Sensore VL53L0X pronto!"));
    }
  public: 
    void pooling(){      
      // Richiedi una misurazione
      this->lox.rangingTest(&this->measure, false); 
      // Se la misurazione è valida (status != 4 significa misurazione riuscita)
      if (this->measure.RangeStatus != 4) {  
        //Serial.print("Distanza: "); 
        //Serial.print(this->measure.RangeMilliMeter);
        *this->distanza = this->measure.RangeMilliMeter;
        //Serial.println(" mm");
      }
      else {
        *this->distanza =8191;
#ifdef ENABLE_DEBUG                
        Serial.println(" Fuori portata / Errore ");
#endif                      
      }

    } 
};
