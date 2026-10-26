#include "common.h"
#include "CanManager.h"
#ifndef BODY_CAR
  #include "rgb_routines.h"
#endif  
#include "ThrottleManager.h"
#include "myRotaryEncoder.h"

ThrottleManager *thMangr;
CanManager *can_m;
RotaryEncoderRTOS *encoder;
#ifndef BODY_CAR
  LedController *ledManager;
#endif  


int throttle_data;
int old_throttle_data=-1;
void setup() {
#ifdef BODY_CAR  
  pinMode(ZERO_CROSS_PIN, INPUT);
#endif  
  Serial.begin(115200);
  thMangr = new ThrottleManager(&throttle_data);   
  can_m = new CanManager(&c_data); 
  encoder = new RotaryEncoderRTOS(ENCODER_PIN_A, ENCODER_PIN_B,ENCODER_BTN);
#ifndef BODY_CAR 
  ledManager= new LedController(NUM_LEDS, LED_PIN);  
  ledManager->begin();
  ledManager->setBrightness(30);
  ledManager->clear();
#endif  
  if (!encoder->begin()) {
        Serial.println("Errore nell'inizializzazione dell'hardware PCNT!");
        return;
  } 

  xTaskCreatePinnedToCore(
    &taskThrottle,
    "taskThrottle",
    4096,
    NULL,
    0,
    NULL,
    0
  );
  xTaskCreatePinnedToCore(
    &systemPooling,
    "systemPooling",
    4096,
    NULL,
    5,
    NULL,
    1
  );
  xTaskCreatePinnedToCore(
    &canRx, // task function
    "canRx", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
    1); // out pointer to task handle
  xTaskCreatePinnedToCore(
    &canTx, // task function
    "canTx", // task name
    5000, // stack size in words
    NULL,
    5, // priority
    NULL,
    1); // out pointer to task handle*/   
  xTaskCreatePinnedToCore(
    &encoderPooling, // task function
    "encoderPooling", // task name
    5000, // stack size in words
    NULL,
    15, // priority
    NULL,
    1); // out pointer to task handle
    

    
}
void encoderProc(int32_t *lastPos){
  encoder->update();
  // Verifica rotazione
  int32_t currentPos = encoder->getPosition(); 
    if (currentPos != *lastPos) {      
      if(currentPos>15){
        currentPos=15;
        encoder->setPosition(15);
      }
      else if(currentPos<-15){
        currentPos=-15;
        encoder->setPosition(-15);
      }
      Serial.println(currentPos);           
      CAN_DATA s;
      s.can_id = CAN_ID_BOARD_STEERING;
      s.can_data[0] = currentPos;      
      s.data_avalible = 1;
      s.can_len = 1;
      can_m->send(&s);      
      *lastPos = currentPos;
    }    
}
void dataParser()
{          
  CAN_DATA can_data;
  can_m->getData(&can_data);
  if(can_data.data_avalible==1){       
    if(can_data.can_id == CAN_ID_ECU_MANAGER){
      if(can_data.can_data[0]==CAN_OPTIONS && 
        can_data.can_data[1]==CAN_OPTIONS_LIGHT_CAR){
        car_data_system.light_car = can_data.can_data[2];
        car_data_system.light_car_r = can_data.can_data[3];
        car_data_system.light_car_g = can_data.can_data[4];
        car_data_system.light_car_b = can_data.can_data[5];
      }
      if(can_data.can_data[0]==CAN_OPTIONS && 
        can_data.can_data[1]==CAN_OPTIONS_STOP_CAR){        
        car_data_system.stop_car = can_data.can_data[2];        
      }
      can_data.data_avalible=0;        
    }    
  }           
}
CAR_DATA_SYSTEM old_car_data_system;
void system_proc(){
  
#ifdef BODY_CAR  
  if(digitalRead(ZERO_CROSS_PIN)==0){
    encoder->setPosition(0);
  }  
#endif  
  if(memcmp(&car_data_system, &old_car_data_system, sizeof(CAR_DATA_SYSTEM))!=0){  
    if(car_data_system.light_car!=old_car_data_system.light_car){
      if(car_data_system.light_car==1){      
        ledManager->fill(car_data_system.light_car_r,
          car_data_system.light_car_g,
          car_data_system.light_car_b);      
      }
      else{
        ledManager->fill(0,0,0);      
      }      
    }
    if(car_data_system.stop_car!=old_car_data_system.stop_car){
      if(car_data_system.stop_car==1){              
        for(int i=0;i<5;i++){
          ledManager->fill(0xFF,0,0);      
          vTaskDelay(portTICK_PERIOD_MS*100);
          if(car_data_system.light_car==1){      
            ledManager->fill(car_data_system.light_car_r,
            car_data_system.light_car_g,
            car_data_system.light_car_b);                
          }else{
            ledManager->fill(0,0,0);      
          }
          vTaskDelay(portTICK_PERIOD_MS*100);  
        }        
      }      
    }
    memcpy(&old_car_data_system,&car_data_system,sizeof(CAR_DATA_SYSTEM));                 
  }
}

void encoderPooling(void *pvParameter)
{      
  int32_t lastPos = 0;
  while (1) {            
    encoderProc(&lastPos);    
    vTaskDelay(portTICK_PERIOD_MS*20);
  }
  vTaskDelete( NULL );
}
void taskThrottle(void *pvParameter){
  while (1) 
  {  
    thMangr->pooling();
    if(old_throttle_data!= throttle_data){
#ifdef ENABLE_DEBUG
      //Serial.println(throttle_data);
#endif
      old_throttle_data = throttle_data;
      CAN_DATA s;
      s.can_id = CAN_ID_BOARD_THROTTLE;
      s.can_data[0] = throttle_data;      
      s.data_avalible = 1;
      s.can_len = 1;
      can_m->send(&s);
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void systemPooling(void *pvParameter){
  while (1) 
  {              
    dataParser();
    system_proc();              
    vTaskDelay(portTICK_PERIOD_MS*10);
  }
  vTaskDelete( NULL );
}
void canTx(void *pvParameter)
{        
  while (1) {
    can_m->processTx();        
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void canRx(void *pvParameter)
{      
  while (1) {    
    can_m->processRx();      
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void loop() {}