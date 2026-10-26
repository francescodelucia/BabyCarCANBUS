#include "esp32-hal-gpio.h"
#ifndef  _DISPLAY_MANAGER_
#define  _DISPLAY_MANAGER_

#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include <SPI.h>
//#include "dashboard_img.h"
//#include "Battery_img.h"
#include "common.h"
#define TFT_DC 46
#define TFT_CS 10
//#define TFT_RESET 9
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_RST 9
#define TFT_MISO 13

class DisplayManager{
  private:
    Adafruit_ILI9341 *tft;
  public:

  public:
    DisplayManager(){
      
      //this->tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);   
      this->reset_display();  
      this->tft = new Adafruit_ILI9341(TFT_CS, TFT_DC,TFT_RST);       
      SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
      this->init();
    }; 
    ~DisplayManager(){}; 
  private:
    void reset_display(){
      pinMode(TFT_RST, OUTPUT);    
      for(int x=0;x<10;x++)
      {    
        digitalWrite(TFT_RST, LOW);  // change state of the LED by setting the pin to the HIGH voltage level
        delay(100);                      // wait for a second
        digitalWrite(TFT_RST, HIGH);   // change state of the LED by setting the pin to the LOW voltage level
        delay(100);                      // wait for a second
      }
      digitalWrite(TFT_RST, LOW);  // change state of the LED by setting the pin to the HIGH voltage level
      delay(200);                      // wait for a second
    }
    void init(){            
      this->tft->begin(80000000);      
    }
   
  public:    
    Adafruit_ILI9341 *getDisplay(){
      return this->tft;
    }

};
#endif