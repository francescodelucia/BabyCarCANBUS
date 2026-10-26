#include <stdint.h>
class GearManager{
  private:
    int8_t old_gear=-100;
    Adafruit_ILI9341 *tft;
    CAR_DATA_ENGINE *_car_data;
  public:
    GearManager( Adafruit_ILI9341 *tft,CAR_DATA_ENGINE *car_data){
      this->tft = tft; 
      this->_car_data= car_data;
      this->init();
    }
    ~GearManager(){}
  private:
    void init(){}      
  public:    
    void updtateGear(int force_update=0){      
      if(this->_car_data->gear!=old_gear || force_update!=0){
        old_gear = this->_car_data->gear;
        this->tft->fillRect(145, 95, 35, 35, ILI9341_BLACK); // Pulisce interno
        this->tft->setTextSize(4);
        this->tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        this->tft->setCursor(150, 100);
        
        int displayGear = constrain((int)this->_car_data->gear,-1,2);        
        

        if(displayGear == -1){ this->tft->print("R"); }
        else if( displayGear == 0){this->tft->print("N"); } 
        else { this->tft->print(displayGear); }
      }
    }  
};