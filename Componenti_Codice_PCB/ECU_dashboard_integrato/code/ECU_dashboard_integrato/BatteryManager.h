#include <stdint.h>
class BatteryManger{
  private:
    int old_battery=-100;
    Adafruit_ILI9341 *tft;
    CAR_DATA_ENGINE *_car_data;
  public:
    BatteryManger( Adafruit_ILI9341 *tft,CAR_DATA_ENGINE *car_data){
      this->tft = tft; 
      this->_car_data = car_data;
      this->init();
    }
    ~BatteryManger(){}
  private:
    void init(){}      
  public:
      void updataBattery(int force_update=0){//int p/*data mappato*/,int r /*dato reale*/){
        int p = map(this->_car_data->battery,0,4095,0,100);
        int r = this->_car_data->battery;
        if(p!= this->old_battery || force_update!=0){  
          this->old_battery = p;
          int color;
          if (p > 50) color = ILI9341_GREEN;
          else if (p > 20) color = ILI9341_ORANGE;
          else color = ILI9341_RED;    
          //Riempimento interno dinamico
          this->tft->fillRect(12, 85, 58, 22, ILI9341_LIGHTGREY); // Pulisce interno
          int width = map(p, 0, 100, 0, 58);
          this->tft->fillRect(12, 85, width, 22, color);
          // Testo percentuale
          this->tft->fillRect(15, 60, 58, 22, ILI9341_BLACK); // Pulisce interno
          this->tft->setTextSize(2);
          this->tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          this->tft->setCursor(15, 60);
          if (p < 100) this->tft->print(" "); 
          this->tft->print(p);    
          this->tft->print("%");    
          float v = 15.0;
          float d = v/4095;
          float vAttuale = r*d; 
          this->tft->setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
          this->tft->setTextSize(1);
          this->tft->setCursor(10, 12);          
          this->tft->print(vAttuale);
          this->tft->setTextSize(1);
          this->tft->print(" V");
      }
    }
};