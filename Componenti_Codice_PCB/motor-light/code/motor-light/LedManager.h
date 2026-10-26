#pragma once
#include "common.h"
class LedManager{
  private:
    CRGB leds[TOTAL_LEDS];
    CRGB singleLed[NUM_LEDS]; // Array separato per il singolo LED sul pin 48

    float fluidiZona1 = 0.0;
    float fluidiZona2 = 0.0;

    // Variabili per gestire il fade non bloccante del pin 48
    unsigned long ultimoAggiornamentoFade = 0;
    int luminositaLed48 = 0;
    int versoFade = 8; // Velocità e direzione del fade
  public:
    LedManager(){
      this->init();
    }
    ~LedManager(){
     
    }
  private:
    void init(){
      FastLED.addLeds<WS2813, LED_PIN, GRB>(leds, TOTAL_LEDS);
      FastLED.setBrightness(BRIGHTNESS);

      // Inizializziamo la striscia/LED singolo sul pin 48 con un array dedicato
      FastLED.addLeds<WS2812B, DATA_PIN, GRB>(singleLed, NUM_LEDS);            

      fill_solid(leds, TOTAL_LEDS, CRGB::Green);
      fill_solid(singleLed, NUM_LEDS, CRGB::Blue);
      FastLED.show();
    }
  public:
    void fadeLed(){
      // --- 1. GESTIONE FADE NON-BLOCCANTE PIN 48 ---
      if (millis() - ultimoAggiornamentoFade >= 15) { // Ogni 15ms aggiorna il fade
        ultimoAggiornamentoFade = millis();
        luminositaLed48 += versoFade;
        
        // Inverti la direzione quando arrivi ai limiti (0 o 255)
        if (luminositaLed48 <= 0 || luminositaLed48 >= 255) {
          versoFade = -versoFade;
        }
      }
    }
   
    void updateLeds(){
      // --- 4. AGGIORNAMENTO PRINCIPALE LED (40 LED) ---
      fill_solid(leds, TOTAL_LEDS, CRGB::Green);

      uint8_t led_bar_1 = map(car_data_motor.motor_1,0,255,0,20);
      for(int i = 0; i < led_bar_1; i++) {          
          leds[i] = CRGB::Blue;         
      }      
      uint8_t led_bar_2 = map(car_data_motor.motor_2, 0, 255, 0, 20);                  
      for(int i = 0; i < led_bar_2; i++) {                    
          leds[39 - i] = CRGB::Blue;                   
      }      
    }
    void pooling(){
      this->fadeLed();      
      this->updateLeds();            
      fill_solid(singleLed, NUM_LEDS, CRGB(0, 0, luminositaLed48)); // Fade sul canale Blu
      FastLED.show();
      vTaskDelay(portTICK_PERIOD_MS);
    }

};