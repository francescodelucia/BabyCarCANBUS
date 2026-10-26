#include <stdint.h>
class ThrottleManager{
  private:
    int16_t old_throttle=-100;
    Adafruit_ILI9341 *tft;
    CAR_DATA_ENGINE *_car_data;
  public:
    ThrottleManager( Adafruit_ILI9341 *tft,CAR_DATA_ENGINE *car_data){
      this->tft = tft; 
      this->_car_data = car_data;
      this->init();
    }
    ~ThrottleManager(){}
  private:
    void init(){}      
  public:
    void updThrottle(int force_update=0){
      uint16_t tAttuale = this->_car_data->throttle;
      if(old_throttle != tAttuale || force_update!=0){
        old_throttle = tAttuale;
        this->tft->fillRect(264, 55, 45, 20, ILI9341_BLACK); // Pulisce interno
        this->tft->fillRect(260, 155, 22, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(262, 146, 23, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(264, 136, 25, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(266, 126, 25, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(268, 116, 25, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(266, 106, 29, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(266, 97, 29, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(264, 87, 29, 7, ILI9341_DARKGREY); // Pulisce interno
        this->tft->fillRect(264, 77, 27, 7, ILI9341_DARKGREY); // Pulisce interno        
        if(tAttuale > 0){  this->tft->fillRect(260, 155, 22, 7, ILI9341_ORANGE); }
        if(tAttuale > 10){ this->tft->fillRect(260, 155, 22, 7, ILI9341_ORANGE); }
        if(tAttuale > 20){ this->tft->fillRect(262, 146, 23, 7, ILI9341_ORANGE); }
        if(tAttuale > 30){ this->tft->fillRect(264, 136, 25, 7, ILI9341_ORANGE); }
        if(tAttuale > 40){ this->tft->fillRect(266, 126, 25, 7, ILI9341_GREEN); }
        if(tAttuale > 50){ this->tft->fillRect(268, 116, 25, 7, ILI9341_GREEN); }
        if(tAttuale > 60){ this->tft->fillRect(266, 106, 29, 7, ILI9341_GREEN); }
        if(tAttuale > 70){ this->tft->fillRect(266, 97, 29, 7, ILI9341_RED); }
        if(tAttuale > 80){ this->tft->fillRect(264, 87, 29, 7, ILI9341_RED); }            
        if(tAttuale > 90){ this->tft->fillRect(264, 77, 27, 7, ILI9341_RED); }            
        this->tft->setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
        this->tft->setTextSize(2);
        this->tft->setCursor(250, 60);        
        if (tAttuale < 100) this->tft->print(" ");
        if (tAttuale < 10)  this->tft->print(" ");
        this->tft->print(tAttuale);
        this->tft->setTextSize(1);
        this->tft->print("%");
      }
    }  
};