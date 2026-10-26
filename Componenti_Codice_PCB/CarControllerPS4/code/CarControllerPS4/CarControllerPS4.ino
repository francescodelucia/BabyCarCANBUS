#include "common.h"
#include <Adafruit_NeoPixel.h>
#include <driver/twai.h>
#include "CanManager.h"
#include "ControllerManager.h"




CanManager *can_m;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  Serial.begin(115200);
  delay(1000);
  can_m = new CanManager(&c_data);      
  gamepadMutex = xSemaphoreCreateMutex();
  // Task Pinned sul Core 0 per isolare la connessione Bluetooth
  xTaskCreatePinnedToCore(
    taskBluepad32,
    "BluepadTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );  
  xTaskCreatePinnedToCore(
    ledFade,
    "ledFade",
    1000,
    NULL,
    10,
    NULL,
    1
  );  
  xTaskCreatePinnedToCore(
    &canPooling, // task function
    "canPooling", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
    1); // out pointer to task handle
  xTaskCreatePinnedToCore(
    &dataParser, // task function
    "dataParser", // task name
    5000, // stack size in words
    NULL,
    5, // priority
    NULL,
    1); // out pointer to task handle
  Serial.println("Setup completato sul Core 1. In attesa di input dal gamepad...");
}

void ledFade(void *pvParameter)
{      
  while (1) {    
    if(LedEnable){
      pixels.begin();
      for(int i=0;i<64;i++){
        pixels.setPixelColor(0, pixels.Color(0,0,i));
        pixels.show();
        vTaskDelay(portTICK_PERIOD_MS*20);
      }
      for(int i=64;i>1;i--){
        pixels.setPixelColor(0, pixels.Color(0,0,i));
        pixels.show();
        vTaskDelay(portTICK_PERIOD_MS*20);
      }
    }   
    else{
      pixels.begin();
      for(int i=0;i<64;i++){
        if(isConnectionEnabled){
          pixels.setPixelColor(0, pixels.Color(0,i,0));
        }else{
          pixels.setPixelColor(0, pixels.Color(i,0,i));
        }        
        pixels.show();
        vTaskDelay(portTICK_PERIOD_MS*20);
      }
      for(int i=64;i>1;i--){
        if(isConnectionEnabled){
          pixels.setPixelColor(0, pixels.Color(0,i,0));
        }else{
          pixels.setPixelColor(0, pixels.Color(i,0,i));
        }           
        pixels.show();
        vTaskDelay(portTICK_PERIOD_MS*20);
      }
    }     
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void canPooling(void *pvParameter)
{      
  while (1) {    
    can_m->pooling();           
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void dataParser(void *pvParameter)
{    
  while (1) {    
    CAN_DATA can_data;
    uint8_t state=0xFF;
    can_m->getData(&can_data);        
    if(can_data.data_avalible==1){      
      if(can_data.can_id == CAN_ID_ECU_MANAGER){
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_REMOTE_PS4){
          state = can_data.can_data[2];
        }
      }      
    }
    switch(state){
      case ENABLE_CONTROLLER:
        isConnectionEnabled = true; // Di default la connessione è ABILITATA
        requestDisconnect = false;
      break;
      case DISABLE_CONTROLLER:
        isConnectionEnabled = false; // Di default la connessione è ABILITATA
        requestDisconnect = true;
      break;
    }     
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

// TASK FREERTOS (Core 0) - Gestione Gestore Bluetooth
void taskBluepad32(void *pvParameters) {
  //Serial.printf("Task Bluepad32 avviato sul Core %d\n", xPortGetCoreID());
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false); // Disabilita touchpad/mouse virtuale  
  
  while (true) {
    if (requestDisconnect) {
      disconnectAllControllers();
      requestDisconnect = false;
    }
    bool dataUpdated = BP32.update();        
    if (dataUpdated) {            
      processControllers();            
      sendTasti();
      sendDpad();
      sendDorsali();
      sendGrilletti();
      sendAxisL();
      sendAxisR();
      LedTest();
    }    
    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

void sendTasti(){
  int8_t state_tasti = sharedState.btnA + sharedState.btnB + sharedState.btnX +sharedState.btnY; 
  if(old_tasti!=state_tasti){
    old_tasti = state_tasti;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x01;
    s.can_data[1] = sharedState.btnA;       
    s.can_data[2] = sharedState.btnB;       
    s.can_data[3] = sharedState.btnX;       
    s.can_data[4] = sharedState.btnY;       
    s.data_avalible = 1;
    s.can_len = 5;
    can_m->sendCAN(&s);
  }
}
void sendDpad(){
  int8_t state_dpad = sharedState.dpadUp + sharedState.dpadDown + sharedState.dpadRight + sharedState.dpadLeft;       
  if(old_dpad!= state_dpad){
    old_dpad = state_dpad;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x02;
    s.can_data[1] = sharedState.dpadUp;       
    s.can_data[2] = sharedState.dpadDown;       
    s.can_data[3] = sharedState.dpadLeft;       
    s.can_data[4] = sharedState.dpadRight;       
    s.data_avalible = 1;
    s.can_len = 5;
    can_m->sendCAN(&s);
  }
}
void sendDorsali(){
  uint8_t state_dorsali = sharedState.btnL1 + sharedState.btnR1;
  if(old_dorsali!= state_dorsali){
    old_dorsali = state_dorsali;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x03;
    s.can_data[1] = sharedState.btnL1;       
    s.can_data[2] = sharedState.btnR1;         
    s.data_avalible = 1;
    s.can_len = 3;
    can_m->sendCAN(&s);
  }
}
void sendGrilletti(){
  uint32_t state_grilletti = sharedState.brakeL2 + sharedState.throttleR2;
  if(old_grilletti!=state_grilletti ){
    old_grilletti = state_grilletti;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x04;  
    s.can_data[1] = sharedState.brakeL2 & 0xFF;       
    s.can_data[2] = (sharedState.brakeL2 >> 8);       
    s.can_data[3] = sharedState.throttleR2 & 0xFF;         
    s.can_data[4] = (sharedState.throttleR2 >> 8);         
    s.data_avalible = 1;
    s.can_len = 5;
    can_m->sendCAN(&s);
  }
}
void sendAxisL(){
  uint32_t state_axisl = sharedState.axisX + sharedState.axisY;
  if(old_axisL != state_axisl){
    old_axisL = state_axisl;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x05;  
    s.can_data[1] = sharedState.axisX & 0xFF;       
    s.can_data[2] = (sharedState.axisX >> 8);       
    s.can_data[3] = sharedState.axisY & 0xFF;         
    s.can_data[4] = (sharedState.axisY >> 8);         
    s.data_avalible = 1;
    s.can_len = 5;
    can_m->sendCAN(&s);
  }
}
void sendAxisR(){
  uint32_t state_axisR = sharedState.axisRX + sharedState.axisRY;
  if(old_axisR != state_axisR){
    old_axisR = state_axisR;
    CAN_DATA s;
    s.can_id = 0x101;
    s.can_data[0] = 0x06;  
    s.can_data[1] = sharedState.axisRX & 0xFF;       
    s.can_data[2] = (sharedState.axisRX >> 8);       
    s.can_data[3] = sharedState.axisRY & 0xFF;         
    s.can_data[4] = (sharedState.axisRY >> 8);         
    s.data_avalible = 1;
    s.can_len = 5;
    can_m->sendCAN(&s);
  }
}
void  LedTest(){
  
}
void loop() {
  // Esempio di utilizzo dei dati registrati nel loop principale (Core 1)
  GamepadState currentInput;

  if (xSemaphoreTake(gamepadMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
    currentInput = sharedState;
    xSemaphoreGive(gamepadMutex);
  }
  // Qui puoi usare qualsiasi pulsante o levetta liberamente:
  // Esempio: se premi R1 oppure premi acceleratore L2 oltre la metà
  if (currentInput.btnR1 || currentInput.throttleR2 > 500) {
    // Esegui azione accelerazione/motore
  }
  vTaskDelay(pdMS_TO_TICKS(100));
}