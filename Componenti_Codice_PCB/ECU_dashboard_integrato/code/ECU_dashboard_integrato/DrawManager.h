#include "common.h"
#include <sys/_stdint.h>
#include <stdint.h>
#ifndef _DRAW_MANAGER_
#define _DRAW_MANAGER_
#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include "BatteryManager.h"
#include "ThrottleManager.h"
#include "GearManager.h"
#include "dash_img.h"
#include "logo_boot.h"
#include "ControllerIcon.h"


#define COLOR_BG          0x0842  // Sfondo scuro principale (Dark Slate)
#define COLOR_HEADER      0x18E3  // Header superiore (Navy Blue scuro)
#define COLOR_CARD_BG     0x1906  // Sfondo schede/card
#define COLOR_CARD_BORDER 0x31AC  // Bordo schede/card
#define COLOR_ACCENT      0x05BF  // Colore di accento/evidenziazione (Cyan/Teal)
#define COLOR_TEXT_MAIN   0xFFFF  // Testo principale (Bianco)
#define COLOR_TEXT_MUTED  0x9CD3  // Testo secondario (Grigio chiaro)
#define COLOR_GRID        0x10A4  // Colore griglia sottile

// Dimensioni Standard TFT ILI9341 (320x240)
#define TFT_WIDTH  320
#define TFT_HEIGHT 240

class DrawManager{
  private:
    Adafruit_ILI9341 *tft;
    BatteryManger *battManager;
    ThrottleManager *thrManager;
    GearManager *grManager; 
    int velocitaPrecedente = -1;
    const int barraLarghezzaMax = 108; // Larghezza massima della barra (128 - 20)
    const int barraAltezza = 15;
    const int barraX = 10;
    const int barraY = 80;
    // Configurazione della rampa a barre verticali crescenti
    // (Puoi variare questi parametri per adattare la rampa agli spazi liberi nel display)    
    // Ricalibrato per display ruotato (Larghezza 160px, Altezza 128px)
    const int totaleBarre = 15;         // Aumentate a 15 per coprire meglio lo schermo
    const int barraXInizio = 7;         // Centra meglio la rampa (lascia ~7px a sinistra e destra)
    const int barraYBase = 124;         // Spostata quasi a fondo schermo (altezza max schermo = 128)
    const int larghezzaBarra = 7;       // Barre leggermente più larghe per miglior visibilità
    const int spaziaturaBarre = 3;      // Spazio tra le barre
    const int altezzaMinima = 6;        // Altezza della prima barra
    const int incrementoAltezza = 3;    // Crescita più dolce (la barra più alta sarà di ~48px)
    int16_t old_status_dash=-100;
    int old_thtrottle =-100;
    int old_gear=-100;
    CAR_DATA_ENGINE *_car_data_engine;
    CAR_DATA_SYSTEM *_car_data_system;
    DASH_DATA *dash_data;
  public:
    DrawManager(Adafruit_ILI9341 *tft,CAR_DATA_ENGINE *car_data_engine,CAR_DATA_SYSTEM *car_data_system,DASH_DATA *dash_data){            
      this->tft = tft;
      this->_car_data_engine = car_data_engine;      
      this->_car_data_system = car_data_system;      
      this->dash_data = dash_data;
      this->battManager = new BatteryManger(this->tft,car_data_engine);
      this->thrManager = new ThrottleManager(this->tft,car_data_engine);
      this->grManager = new GearManager(this->tft,car_data_engine);
      this->init();
    }
    ~DrawManager(){}
  private:
    void init(){        
      this->boot_logo();
    }
    void MainDash(){
      this->tft->setTextColor(ILI9341_WHITE);
      this->tft->setTextSize(1);
      this->tft->setCursor(10, 10);    
      this->tft->drawFastHLine(0, 25, 160, ILI9341_WHITE); // Linea superiore
      this->tft->setCursor(120, 140);
      this->tft->print("km/h");      
      this->show_dash();
    }
  public:
     /**
    * Disegna una porzione (sub-rettangolo) di un'immagine RGB565 memorizzata in memoria
    * 
    * @param xPos  Coordinate X sullo schermo dove disegnare
    * @param yPos  Coordinate Y sullo schermo dove disegnare
    * @param bitmap  Puntatore all'array dell'immagine originale
    * @param imgW  Larghezza totale dell'immagine originale in pixel
    * @param imgH  Altezza totale dell'immagine originale in pixel
    * @param cropX Posizione X di partenza del ritaglio nell'immagine originale
    * @param cropY Posizione Y di partenza del ritaglio nell'immagine originale
    * @param cropW Larghezza del ritaglio
    * @param cropH Altezza del ritaglio
    */
    void drawBitmapRegion(int16_t xPos, int16_t yPos, const uint16_t *bitmap, 
                          int16_t imgW, int16_t imgH, 
                          int16_t cropX, int16_t cropY, 
                          int16_t cropW, int16_t cropH) {      
      // Scansione riga per riga la porzione interessata
      for (int16_t y = 0; y < cropH; y++) {
        // Calcola l'indice del primo pixel della riga all'interno dell'array originale
        uint32_t offset = (uint32_t)(cropY + y) * imgW + cropX;        
        // Disegna la singola riga di larghezza cropW
        this->tft->drawRGBBitmap(xPos, yPos + y, &bitmap[offset], cropW, 1);
      }
    }
    void GUI_lights_dash(){
      if(this->_car_data_system->remote_ps4==1){
        this->tft->drawRGBBitmap(15, 190 ,(uint16_t*)controller_ico, CONTROLLER_WIDTH, CONTROLLER_HEIGHT);
      }else{
        this->tft->fillRect(15, 190, CONTROLLER_WIDTH, CONTROLLER_HEIGHT, ILI9341_BLACK);
      }      
      if(this->_car_data_system->stop_car==1){
        this->tft->drawRGBBitmap(90, 190 ,(uint16_t*)freno_on, FRENO_WIDTH, FRENO_HEIGHT);
      }else{
        this->tft->fillRect(90, 190, FRENO_WIDTH, FRENO_HEIGHT, ILI9341_BLACK);
      }
      if(this->_car_data_system->light_car!=0){
        this->tft->drawRGBBitmap(170, 190 ,(uint16_t*)luci_on, LUCI_WIDTH, LUCI_HEIGHT);        
      }else{
        this->tft->fillRect(170, 190, LUCI_WIDTH, LUCI_HEIGHT, ILI9341_BLACK);
      }
      if(this->_car_data_engine->battery<2560){
        this->tft->drawRGBBitmap(245, 190 ,(uint16_t*)spia_carica, CARICA_WIDTH, CARICA_HEIGHT);
      }else{
        this->tft->fillRect(245, 190, CARICA_WIDTH, CARICA_HEIGHT, ILI9341_BLACK);
      }
    }
    void GUI_main_dash(){
        if(this->dash_data->current_dash!=old_status_dash){
          old_status_dash = this->dash_data->current_dash;
          this->MainDash();              
          this->battManager->updataBattery(1);      
          this->thrManager->updThrottle(1);
          this->grManager->updtateGear(1);                 
        }          
        this->battManager->updataBattery();      
        this->thrManager->updThrottle();
        this->grManager->updtateGear();
        this->GUI_lights_dash();  

        //this->drawBitmapRegion(10, 190,(uint16_t*)controller_ico, CONTROLLER_WIDTH, CONTROLLER_HEIGHT, 0,0, 0,0);
    }
    void GUI_green_point(){
      this->tft->fillCircle(5,32+(1*30), 4, 0x0000); // Verde OK
      this->tft->fillCircle(5,32+(2*30), 4, 0x0000); // Verde OK
      this->tft->fillCircle(5,32+(3*30), 4, 0x0000); // Verde OK
      this->tft->fillCircle(5,32+(4*30), 4, 0x0000); // Verde OK
      this->tft->fillCircle(5,32+(this->dash_data->current_option*30), 4, 0x07E0); // Verde OK
    }
    void GUI_option_dash(){
        if(this->dash_data->current_dash!=old_status_dash){
          old_status_dash = this->dash_data->current_dash;          
          this->drawDashOption("Opzioni");
          this->drawTextOption("Remote Controller",1);
          this->drawTextOption("LIDAR Action",2);
          this->drawTextOption("Torque Vectoring",3);
          this->drawTextOption("Rainbow Light",4);
          this->drawTextOption("Exit",5);
        }       
        this->GUI_green_point();           
    }
    void GUI_renote_control(){
      if(this->dash_data->current_dash==old_status_dash)return;
      old_status_dash=this->dash_data->current_dash;
      this->drawDashOption("Remote Control");
      this->drawTextOption("Abilita",1);
      this->drawTextOption("Disabilita",2);
      this->drawTextOption("Exit",3);
      this->GUI_green_point();                 
    }
    void invalidate(){old_status_dash=-100;}
    void updateGUI(int battery=0,int16_t throttle=0,int8_t gear=0){
      if(this->dash_data->current_dash == DASH_MAIN){ 
        this->GUI_main_dash();
      }/*else if(this->dash_data->current_dash == DASH_OPTION){
        this->GUI_option_dash();              
      }else if(this->dash_data->current_dash == DASH_LIDAR){
      }else if(this->dash_data->current_dash == DASH_CONTROLLER){
        this->GUI_renote_control();
      }else if(this->dash_data->current_dash == DASH_TORQUE){
      }else if(this->dash_data->current_dash == DASH_LIGHT){
      }*/    
    }
   
  void updButton(int botton){
    this->tft->fillRect(10, 30, 100, 200, ILI9341_ORANGE); // Pulisce interno
    this->tft->setTextSize(1);
    this->tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    this->tft->setCursor(10, 30);    
    this->tft->print(botton);     
  }
 /*void show_battery(){
      this->tft->setRotation(1);
      this->tft->fillScreen(ILI9341_BLACK);              
      this->drawBitmapRegion(0,0,(const uint16_t *)battery_,BATTERY_WIDTH,BATTERY_HEIGHT,5,0,30,40);      
  }*/
  void boot_logo(){
      this->tft->setRotation(1);
      this->tft->fillScreen(ILI9341_BLACK);      
      //this->drawDashboard();
      this->tft->drawRGBBitmap(
          0,//random(-DASHBOARD_WIDTH , tft->width()),
          0,//random(-DASHBOARD_HEIGHT, tft->height()),
          (uint16_t *)logo_bmp,LOGO_WIDTH,LOGO_HEIGHT);      
      vTaskDelay(portTICK_PERIOD_MS*5000);
  } 
  void show_dash(){
      this->tft->setRotation(1);
      this->tft->fillScreen(ILI9341_BLACK);      
      //this->drawDashboard();
      this->tft->drawRGBBitmap(
          0,//random(-DASHBOARD_WIDTH , tft->width()),
          0,//random(-DASHBOARD_HEIGHT, tft->height()),
          (uint16_t *)dash_img_bmp,DASH_IMG_WIDTH,DASH_IMG_HEIGHT);      
  } 
   void drawDashOption(const char* headerTitle = "ESP32-S3 DASHBOARD") {
        // 1. Sfondo generale
        this->tft->fillScreen(COLOR_BG);
        // 2. Griglia di sfondo estetica (opzionale)
        for (int16_t x = 0; x < TFT_WIDTH; x += 20) {
            this->tft->drawFastVLine(x, 0, TFT_HEIGHT, COLOR_GRID);
        }
        for (int16_t y = 0; y < TFT_HEIGHT; y += 20) {
            this->tft->drawFastHLine(0, y, TFT_WIDTH, COLOR_GRID);
        }
        // 3. Header superiore
        this->tft->fillRect(0, 0, TFT_WIDTH, 30, COLOR_HEADER);
        this->tft->drawFastHLine(0, 30, TFT_WIDTH, COLOR_ACCENT);
        // Titolo Header
        this->tft->setTextColor(COLOR_TEXT_MAIN);
        this->tft->setTextSize(2);
        this->tft->setCursor(10, 7);
        this->tft->print(headerTitle);
        
    }
    void drawTextOption(const char* title = nullptr,int line=0){
      this->tft->setTextColor(COLOR_TEXT_MAIN);
      this->tft->setTextSize(2);
      this->tft->setCursor(15,30+(line*30));
      this->tft->print(title);
    }
    // Disegna una scheda/riquadro con stile moderno
    void drawCard(int16_t x, int16_t y, int16_t w, int16_t h, const char* title = nullptr) {
        // Sfondo card
        this->tft->fillRect(x, y, w, h, COLOR_CARD_BG);
        
        // Bordo card
        this->tft->drawRect(x, y, w, h, COLOR_CARD_BORDER);

        // Intestazione card (se specificata)
        if (title != nullptr) {
            this->tft->fillRect(x + 1, y + 1, w - 2, 22, COLOR_HEADER);
            this->tft->drawFastHLine(x + 1, y + 23, w - 2, COLOR_CARD_BORDER);
            
            this->tft->setTextColor(COLOR_TEXT_MAIN);
            this->tft->setTextSize(1);
            this->tft->setCursor(x + 8, y + 7);
            this->tft->print(title);
        }
    }

};

#endif