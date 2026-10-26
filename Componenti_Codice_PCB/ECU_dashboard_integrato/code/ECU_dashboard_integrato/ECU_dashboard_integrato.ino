#include <Arduino.h>
#include "common.h"
#include "DisplayManager.h"
#include "DrawManager.h"
#include "CanManager.h"
#include "myRotaryEncoder.h"
#include "ControllerIcon.h"
#include "ToneManager.h"
#include "NavigationManager.h"
#include <freertos/queue.h>

RotaryEncoderRTOS *encoder;
DisplayManager *display_mngr;
CanManager *can_m;
DrawManager *draw_manager;
ToneManager *buzzer;

// Tutte le modifiche alla navigazione e tutti i disegni TFT avvengono nel task DisplayRender.
enum class UIEvent : uint8_t { LEFT, RIGHT, CLICK, BACK };
QueueHandle_t uiEvents = nullptr;
NavigationManager* navigation = nullptr;
int brightnessSetting = 80; // Solo parametro UI: nessun GPIO retroilluminazione configurato
int canBitrateSetting = 500; // Solo parametro UI: CAN rimane a 500 kbps
int effeetto_luci;
int r,g,b;
const NavigationManager::Item setController[] = {
  {"Abilita", NavigationManager::VALUE, nullptr, 0, &(car_data_system.remote_ps4), 0,1,1, nullptr, 0},  
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};
const NavigationManager::Item setLuciRainbow[] = {  
  {"Effetto", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_rainbow), 0,10,1, nullptr, 0},  
  {"R", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_rainbow_r), 0,255,5, nullptr, 0},  
  {"G", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_rainbow_g), 0,255,5, nullptr, 0},  
  {"B", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_rainbow_b), 0,255,5, nullptr, 0},  
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};
const NavigationManager::Item setLuci[] = {
  {"Accendi", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_car), 0,1,1, nullptr, 0}, 
  {"R", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_car_r), 0,255,5, nullptr, 0},  
  {"G", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_car_g), 0,255,5, nullptr, 0},  
  {"B", NavigationManager::VALUE, nullptr, 0, &(car_data_system.light_car_b), 0,255,5, nullptr, 0},   
  {"Rainbow", NavigationManager::SUBMENU, setLuciRainbow, 5, nullptr, 0,0,0, nullptr, 0},  
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};
const NavigationManager::Item setLidar[] = {
  {"Distanza Lidar cm", NavigationManager::VALUE, nullptr, 0,(int*)&(car_data_engine.lidar_action), 0,1000,20, nullptr, 0},  
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};

const NavigationManager::Item setTorque[] = {
  {"% sterzo", NavigationManager::VALUE, nullptr, 0, &(car_data_engine.torque_val), 0,50,10, nullptr, 0},  
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};

const NavigationManager::Item dashboardItems[] = {  
  {"Luci", NavigationManager::SUBMENU, setLuci, 6, nullptr, 0,0,0, nullptr, 0},
  {"Controller", NavigationManager::SUBMENU, setController, 2, nullptr, 0,0,0, nullptr, 0},
  {"LIDAR", NavigationManager::SUBMENU, setLidar, 2, nullptr, 0,0,0, nullptr, 0},
  {"Torque", NavigationManager::SUBMENU, setTorque, 2, nullptr, 0,0,0, nullptr, 0},
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};

const NavigationManager::Item settingsItems[] = {
  {"Luminosita", NavigationManager::VALUE, nullptr, 0, &brightnessSetting, 0,100,5, nullptr, 0},
  {"CAN kbps", NavigationManager::VALUE, nullptr, 0, &canBitrateSetting, 125,1000,125, nullptr, 0},
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};
const NavigationManager::Item rootItems[] = {
  {"Dashboard", NavigationManager::SUBMENU, dashboardItems, 5, nullptr, 0,0,0, nullptr, 0},
  {"Impostazioni", NavigationManager::SUBMENU, settingsItems, 3, nullptr, 0,0,0, nullptr, 0},
  {"Exit", NavigationManager::SCREEN, nullptr, 0, nullptr, 0,0,0, nullptr, DASH_MAIN}
};

void sendUIEvent(UIEvent event) {
  if (uiEvents) xQueueSend(uiEvents, &event, 0);
}
void onLongPress() { sendUIEvent(UIEvent::BACK); }

void initButtons(){   
#ifdef CAR_BODY  
   pinMode(OPTION_R, INPUT);   
   pinMode(OPTION_L, INPUT);   
#endif   
   pinMode(GEAR_DOWN, INPUT);   
   pinMode(GEAR_UP, INPUT); 
   pinMode(AUDIO_BEEP, OUTPUT);    
   if (!encoder->begin()) {
        Serial.println("Errore nell'inizializzazione dell'hardware PCNT!");
        return;
  } 
  encoder->setLongPressThreshold(1000, onLongPress); 
}
void setup() {  
  //Serial1.setTxTimeoutMs(0);  
  Serial.begin(115200);
  delay(300); 
  Serial.println("System initialized!");  
   
  display_mngr = new DisplayManager(); 
  draw_manager = new DrawManager(display_mngr->getDisplay(),&car_data_engine,&car_data_system,&dash_data);  
  can_m = new CanManager(&c_data);    
  encoder = new RotaryEncoderRTOS(ENCODER_PIN_A, ENCODER_PIN_B, ENC_BUT);   
  buzzer = new ToneManager(AUDIO_BEEP, 0);
  buzzer->begin();
  uiEvents = xQueueCreate(32, sizeof(UIEvent));
  navigation = new NavigationManager(display_mngr->getDisplay(), rootItems,
                                    sizeof(rootItems)/sizeof(rootItems[0]));
  initButtons(); 
  
  for(int x=0;x<3;x++){
    buzzer->beep();
    delay(100);    
  }

  xTaskCreatePinnedToCore(
    &DisplayRender, // task function
    "DisplayRender", // task name
    5000, // stack size in words
    (void*)draw_manager,
    0, // priority
    NULL,
    0); // out pointer to task handle
  xTaskCreatePinnedToCore(
    &system_analize, // task function
    "system_analize", // task name
    5000, // stack size in words
    NULL,
    4, // priority
    NULL,
    0); // out pointer to task handle   
  xTaskCreatePinnedToCore(
    &canPooling, // task function
    "canPooling", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
    1); // out pointer to task handle*/   
  xTaskCreatePinnedToCore(
    &canTx, // task function
    "canTx", // task name
    5000, // stack size in words
    NULL,
    3, // priority
    NULL,
    1); // out pointer to task handle*/   
  xTaskCreatePinnedToCore(
    &dataParser, // task function
    "dataParser", // task name
    5000, // stack size in words
    NULL,
    5, // priority
    NULL,
    1); // out pointer to task handle

  xTaskCreatePinnedToCore(
    &system_proc, // task function
    "system_proc", // task name
    5000, // stack size in words
    (void*)draw_manager,
    7, // priority
    NULL,
    1); // out pointer to task handle

}

void dashTone(){
  buzzer->tone(TONE_BUTTON); // Tono a 1kHz
  vTaskDelay(portTICK_PERIOD_MS*TONE_LONG);
  buzzer->noTone();
}
void button_proc(){
  // Pulsanti laterali: eventi, non modificano direttamente la dashboard.
    static bool lastRight = HIGH, lastLeft = HIGH;
#ifdef CAR_BODY    
    bool right = digitalRead(OPTION_R);
    bool left = digitalRead(OPTION_L);
    if (lastRight == HIGH && right == LOW) {sendUIEvent(UIEvent::RIGHT);dashTone();vTaskDelay(portTICK_PERIOD_MS*200);}
    if (lastLeft == HIGH && left == LOW) {sendUIEvent(UIEvent::LEFT);dashTone();vTaskDelay(portTICK_PERIOD_MS*200);}
    lastRight = right;
    lastLeft = left;
#endif    
    if(digitalRead(GEAR_DOWN)== 0){
      car_data_engine.gear--;
      dashTone();
      vTaskDelay(portTICK_PERIOD_MS*200);
    }
    if(digitalRead(GEAR_UP)== 0){
      car_data_engine.gear++;
      dashTone();
      vTaskDelay(portTICK_PERIOD_MS*200);
    }     
    
}
CAR_DATA_SYSTEM old_car_data_system;
void system_variable_proc(){    
  if(memcmp(&car_data_system, &old_car_data_system, sizeof(CAR_DATA_SYSTEM))!=0){       
    if(car_data_system.light_car!=old_car_data_system.light_car ||
      car_data_system.light_car_r!=old_car_data_system.light_car_r ||
      car_data_system.light_car_g!=old_car_data_system.light_car_g||
      car_data_system.light_car_b!=old_car_data_system.light_car_b){            
      CAN_DATA can_data;
      can_data.can_id = CAN_ID_ECU_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 6;      
      can_data.can_data[0]= CAN_OPTIONS;
      can_data.can_data[1]= CAN_OPTIONS_LIGHT_CAR;
      can_data.can_data[2]= car_data_system.light_car;
      can_data.can_data[3]= car_data_system.light_car_r;
      can_data.can_data[4]= car_data_system.light_car_g;
      can_data.can_data[5]= car_data_system.light_car_b;
      can_m->send(&can_data);
    }    
    if(car_data_system.light_rainbow!=old_car_data_system.light_rainbow||
      car_data_system.light_rainbow_r!=old_car_data_system.light_rainbow_r ||
      car_data_system.light_rainbow_g!=old_car_data_system.light_rainbow_g||
      car_data_system.light_rainbow_b!=old_car_data_system.light_rainbow_b){       
        CAN_DATA can_data;
        can_data.can_id = CAN_ID_ECU_MANAGER;
        can_data.data_avalible = 1;
        can_data.can_len = 6;
        can_data.can_data[0]= CAN_OPTIONS;
        can_data.can_data[1]= CAN_OPTIONS_LIGHT_RAINBOW;
        can_data.can_data[2]= car_data_system.light_rainbow;
        can_data.can_data[3]= car_data_system.light_rainbow_r;
        can_data.can_data[4]= car_data_system.light_rainbow_g;
        can_data.can_data[5]= car_data_system.light_rainbow_b;
        can_m->send(&can_data);
    }    
    if(car_data_system.remote_ps4!=old_car_data_system.remote_ps4){      
      CAN_DATA can_data;
      can_data.can_id = CAN_ID_ECU_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 3;
      can_data.can_data[0]=CAN_OPTIONS;
      can_data.can_data[1]=CAN_OPTIONS_REMOTE_PS4;
      can_data.can_data[2]=car_data_system.remote_ps4;
      can_m->send(&can_data);
    }    
    if(car_data_system.steering_servo!=old_car_data_system.steering_servo){            
      CAN_DATA can_data;
      can_data.can_id = CAN_ID_ECU_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 3;
      can_data.can_data[0]=CAN_OPTIONS;
      can_data.can_data[1]=CAN_OPTIONS_STEERING_SERVO;      
      can_data.can_data[2]= car_data_system.steering_servo;      
      can_m->send(&can_data);
    }                      
    if(car_data_system.stop_car!=old_car_data_system.stop_car){      
      for(int i=0;i<3;i++){
        CAN_DATA can_data;
        can_data.can_id = CAN_ID_ECU_MANAGER;
        can_data.data_avalible = 1;
        can_data.can_len = 3;
        can_data.can_data[0]= CAN_OPTIONS;
        can_data.can_data[1]= CAN_OPTIONS_STOP_CAR;
        can_data.can_data[2]= car_data_system.stop_car;
        can_m->send(&can_data);      
        vTaskDelay(portTICK_PERIOD_MS*10);
      }
    }
    if(car_data_system.gear!=old_car_data_system.gear){
      CAN_DATA can_data;
      can_data.can_id = CAN_ID_ECU_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 3;
      can_data.can_data[0]= CAN_OPTIONS;
      can_data.can_data[1]= CAN_OPTIONS_GEAR_DATA;
      can_data.can_data[2]= car_data_system.gear;
      can_m->send(&can_data);      
    }          
    if(car_data_system.motor_1!=old_car_data_system.motor_1 ||
      car_data_system.motor_2!=old_car_data_system.motor_2){      
      CAN_DATA can_data;
      can_data.can_id = CAN_ID_BOARD_MOTOR_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 4;
      can_data.can_data[0]= MOTOR_1;
      can_data.can_data[1]= (car_data_engine.gear>0?FORWARD_DIRECTION:BACKWARD_DIRECTION);
      can_data.can_data[2]= car_data_system.motor_1;
      can_data.can_data[3]= 10;
      can_m->send(&can_data);            
      can_data.can_id = CAN_ID_BOARD_MOTOR_MANAGER;
      can_data.data_avalible = 1;
      can_data.can_len = 4;
      can_data.can_data[0]= MOTOR_2;
      can_data.can_data[1]= (car_data_engine.gear>0?FORWARD_DIRECTION:BACKWARD_DIRECTION);
      can_data.can_data[2]= car_data_system.motor_2;
      can_data.can_data[3]= 10;
      can_m->send(&can_data);      
    }
    memcpy(&old_car_data_system,&car_data_system,sizeof(CAR_DATA_SYSTEM));             
  }  
}
void lidar_analize(){  
  if(car_data_engine.lidar_distance<car_data_engine.lidar_action){
    car_data_system.stop_car = 1;
    car_data_engine.gear = 0;
    buzzer->tone(1000); // Tono a 1kHz
    vTaskDelay(portTICK_PERIOD_MS*(TONE_LONG*2));
    buzzer->noTone();
  }else{
    car_data_system.stop_car = 0;
  }
}
void engine_analize(){  
  car_data_system.gear = constrain((int)car_data_engine.gear ,-1,2);   
  car_data_engine.gear = car_data_system.gear;  
  if(car_data_engine.gear>=1){  
    uint8_t out_power =(uint8_t)((float)(car_data_engine.gear*0.5)*(float)car_data_engine.throttle); 
    int p = map(out_power,0,100,0,255);
    //car_data_engine.steering_position *   
    uint8_t right;
    uint8_t left;
    if(car_data_engine.steering_position>0){
      right = car_data_engine.steering_position;
    }else{
      left = car_data_engine.steering_position*-1;
    }

    long torque_current_left = map(left,0,15,0,car_data_engine.torque_val);
    long torque_current_right = map(right,0,15,0,car_data_engine.torque_val);
    int pwmLeft = map(100 - torque_current_left, 0, 100, 0, p);
    int pwmRight = map(100 - torque_current_right, 0, 100, 0, p);

    car_data_system.motor_1= pwmLeft;
    car_data_system.motor_2= pwmRight;    
  }  else if(car_data_engine.gear<0){  
    int p = map(car_data_engine.throttle,0,100,0,180);
    car_data_system.motor_1= p;
    car_data_system.motor_2= p;    
  }
}
GamepadState old_ps4_controller;
void use_controller(){
  if(car_data_system.remote_ps4==1){
    car_data_engine.torque_val = 30;
    car_data_engine.lidar_action = 200;
    if(memcmp(&ps4_controller, &old_ps4_controller, sizeof(GamepadState))!=0){  
        if(ps4_controller.btnA!=old_ps4_controller.btnA){
#ifdef ENABLE_DEBUG          
          Serial.print("btnA:");
          Serial.println(ps4_controller.btnA);
#endif          
        }
        if(ps4_controller.btnB!=old_ps4_controller.btnB){
#ifdef ENABLE_DEBUG                    
          Serial.print("btnB:");
          Serial.println(ps4_controller.btnB);
#endif                    
        }
        if(ps4_controller.btnX!=old_ps4_controller.btnX){
#ifdef ENABLE_DEBUG                    
          Serial.print("btnX:");
          Serial.println(ps4_controller.btnX);
#endif                    
        }
        if(ps4_controller.btnY!=old_ps4_controller.btnY){
#ifdef ENABLE_DEBUG                    
          Serial.print("btnY:");
          Serial.println(ps4_controller.btnY);
#endif                    
        }    
        if(ps4_controller.axisX!=old_ps4_controller.axisX){
          car_data_engine.steering_position = map(ps4_controller.axisX,-512,512,-15,15);
#ifdef ENABLE_DEBUG                    
          Serial.print("axisX:");
          Serial.println(ps4_controller.axisX);
#endif                    
        }                 
        if(ps4_controller.axisY!=old_ps4_controller.axisY){
#ifdef ENABLE_DEBUG                    
          Serial.print("axisY:");
          Serial.println(ps4_controller.axisY);
#endif                    
        }                 
        if(ps4_controller.axisRY!=old_ps4_controller.axisRY){          
#ifdef ENABLE_DEBUG                    
          Serial.print("axisRY:");
          Serial.println(ps4_controller.axisRY);
#endif                    
        }                 
        if(ps4_controller.axisRX!=old_ps4_controller.axisRX){
          car_data_engine.steering_position = map(ps4_controller.axisRX,-512,512,-15,15);
#ifdef ENABLE_DEBUG                    
          Serial.print("axisRX:");
          Serial.println(ps4_controller.axisRX);
#endif                    
        }                 
        if(ps4_controller.btnR1!=old_ps4_controller.btnR1){
          if(ps4_controller.btnR1==1){
            car_data_engine.gear++;
            dashTone();
          }
#ifdef ENABLE_DEBUG                    
          Serial.print("btnR1:");
          Serial.println(ps4_controller.btnR1);
#endif                    
        }                 
        if(ps4_controller.btnL1!=old_ps4_controller.btnL1){
          if(ps4_controller.btnL1==1){
            car_data_engine.gear--;
            dashTone();
          }
#ifdef ENABLE_DEBUG                    
          Serial.print("btnL1:");
          Serial.println(ps4_controller.btnL1);
#endif                    
        }                 
        if(ps4_controller.dpadDown!=old_ps4_controller.dpadDown){
          if(ps4_controller.dpadDown==1){  
            buzzer->tone(1000); // Tono a 1kHz
            vTaskDelay(portTICK_PERIOD_MS*TONE_LONG*2);
            buzzer->noTone();
          }
#ifdef ENABLE_DEBUG                    
          Serial.print("dpadDown:");
          Serial.println(ps4_controller.dpadDown);
#endif                    
        }                 
        if(ps4_controller.dpadUp!=old_ps4_controller.dpadUp){
          if(ps4_controller.dpadUp==1){
            car_data_system.light_car = !car_data_system.light_car;
          }
#ifdef ENABLE_DEBUG                    
          Serial.print("dpadUp:");
          Serial.println(ps4_controller.dpadUp);
#endif                    
        }                 
        if(ps4_controller.dpadLeft!=old_ps4_controller.dpadLeft){
#ifdef ENABLE_DEBUG                    
          Serial.print("dpadLeft:");
          Serial.println(ps4_controller.dpadLeft);
#endif                    
        }                 
        if(ps4_controller.dpadRight!=old_ps4_controller.dpadRight){
#ifdef ENABLE_DEBUG                    
          Serial.print("dpadRight:");
          Serial.println(ps4_controller.dpadRight);
#endif                    
        }                 
        if(ps4_controller.brakeL2!=old_ps4_controller.brakeL2){
#ifdef ENABLE_DEBUG                    
          Serial.print("brakeL2:");
          Serial.println(ps4_controller.brakeL2);
#endif                    
        }                 
        if(ps4_controller.throttleR2!=old_ps4_controller.throttleR2){
          car_data_engine.throttle = map(ps4_controller.throttleR2,0,1020,0,100);
#ifdef ENABLE_DEBUG                    
          Serial.print("throttleR2:");
          Serial.println(ps4_controller.throttleR2);
#endif                    
        }  
        memcpy(&old_ps4_controller,&ps4_controller,sizeof(GamepadState));
    }                      
  } 
}
void system_analize(void *pvParameter){  
  while (1) {            
    lidar_analize();
    engine_analize(); 
    use_controller();
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void DisplayRender(void *pvParameter)
{
  DrawManager *dw_mgr = (DrawManager*)pvParameter;
  int lastScreen = -1;
  bool wasMenu = false;
  while (1) {
    UIEvent event;
    while (xQueueReceive(uiEvents, &event, 0) == pdTRUE) {
      switch (event) {
        case UIEvent::LEFT: navigation->rotate(-1); break;
        case UIEvent::RIGHT: navigation->rotate(1); break;
        case UIEvent::CLICK: navigation->click(); break;
        case UIEvent::BACK: navigation->back(); break;
      }
    }
    if (navigation->inMenu()) {
      navigation->render();
      wasMenu = true;
    } else {
      int current = navigation->screen();
      if (wasMenu || lastScreen != current) {
        dash_data.current_dash = current;
        dw_mgr->invalidate();
        // Le schermate ancora prive di grafica vengono pulite al cambio.
        /*if (current == DASH_LIDAR || current == DASH_TORQUE || current == DASH_LIGHT){display_mngr->getDisplay()->fillScreen(ILI9341_BLACK);}*/
      }
      dash_data.current_dash = current;
      dw_mgr->updateGUI();
      lastScreen = current;
      wasMenu = false;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void canTx(void *pvParameter)
{        
  while (1) {
    can_m->processTx();        
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void encoderProc(int32_t *lastPos){
  encoder->update();
  // Verifica rotazione
  int32_t currentPos = encoder->getPosition();
    if (currentPos != *lastPos) {
      int32_t delta = currentPos - *lastPos;
      // Limite per evitare di saturare la coda in caso di movimenti molto rapidi.
      delta = constrain(delta, -16L, 16L);
      for (int32_t i=0; i<abs(delta); ++i){
        sendUIEvent(delta > 0 ? UIEvent::LEFT : UIEvent::RIGHT);
      }
      *lastPos = currentPos;
    }
    if (encoder->isClicked()){sendUIEvent(UIEvent::CLICK);}
}
void dataParser(void *pvParameter)
{      
  while (1) {      
    //uint8_t data[17]={0};      
    CAN_DATA can_data;
    can_m->getData(&can_data);
    if(can_data.data_avalible==1){
      if(can_data.can_id == CAN_ID_BOARD_THROTTLE){
        if(car_data_system.remote_ps4!=1){
          car_data_engine.throttle = can_data.can_data[0];
        }
      }
      if(can_data.can_id == CAN_ID_BOARD_BATTERY){car_data_engine.battery = (can_data.can_data[0] << 8) | can_data.can_data[1];}
      if(can_data.can_id == CAN_ID_DEBUG_GEAR){ 
        if(car_data_system.remote_ps4!=1){
          if(can_data.can_data[0]==CAN_GEAR_UP){car_data_engine.gear++;} 
          else if(can_data.can_data[0]==CAN_GEAR_DOWN){car_data_engine.gear--;}}
        }
      if(can_data.can_id == CAN_ID_BOARD_LIDAR){car_data_engine.lidar_distance = (can_data.can_data[1] << 8) | can_data.can_data[0];}            
      if(can_data.can_id == CAN_ID_BOARD_STEERING){car_data_engine.steering_position = can_data.can_data[0];}
      if(can_data.can_id == CAN_ID_BOARD_GENERAL){
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_LIGHT_CAR){car_data_system.light_car = can_data.can_data[2];}
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_LIGHT_RAINBOW){car_data_system.light_rainbow = can_data.can_data[2];}
        //if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_REMOTE_PS4){car_data_system.remote_ps4 = can_data.can_data[2];}
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_LIDAR){car_data_engine.lidar_distance = (can_data.can_data[2] << 8) | can_data.can_data[3];}        
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_STEERING_POSITION){car_data_engine.steering_position = can_data.can_data[2];}
        if(can_data.can_data[0]==CAN_OPTIONS && can_data.can_data[1]==CAN_OPTIONS_STEERING_SERVO){
          if(car_data_system.remote_ps4!=1){
            car_data_system.steering_servo = can_data.can_data[2];
          }else{
            car_data_system.steering_servo = 0;
          }
        }
      }
      if(car_data_system.remote_ps4==1){
        if(can_data.can_id == CAN_ID_BOARD_PS4){
          if(can_data.can_data[0]==PS4_BUTTONS){
            ps4_controller.btnA=can_data.can_data[1];
            ps4_controller.btnB=can_data.can_data[2];
            ps4_controller.btnX=can_data.can_data[3];
            ps4_controller.btnY=can_data.can_data[4];
          }
          if(can_data.can_data[0]==PS4_AXIS_L){
            ps4_controller.axisX =  (can_data.can_data[2] << 8) | can_data.can_data[1]; 
            ps4_controller.axisY =  (can_data.can_data[4] << 8) | can_data.can_data[3]; 
          }
          if(can_data.can_data[0]==PS4_AXIS_R){
            ps4_controller.axisRX =  (can_data.can_data[2] << 8) | can_data.can_data[1];
            ps4_controller.axisRY =  (can_data.can_data[4] << 8) | can_data.can_data[3]; 
          }
          if(can_data.can_data[0]==PS4_DORSALI){
            ps4_controller.btnR1 = can_data.can_data[2];
            ps4_controller.btnL1 = can_data.can_data[1];
          }
          if(can_data.can_data[0]==PS4_DPADS){
            ps4_controller.dpadDown = can_data.can_data[2];
            ps4_controller.dpadUp = can_data.can_data[1];
            ps4_controller.dpadLeft = can_data.can_data[3];
            ps4_controller.dpadRight = can_data.can_data[4];
          }
          if(can_data.can_data[0]==PS4_GRILLETTI){
            ps4_controller.brakeL2 = (can_data.can_data[2] << 8) | can_data.can_data[1]; 
            ps4_controller.throttleR2 = (can_data.can_data[4] << 8) | can_data.can_data[3]; 
          }
        }
      }
    }       
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void canPooling(void *pvParameter)
{      
  int32_t lastPos = 0;
  while (1) {    
    can_m->processRx();       
    encoderProc(&lastPos);
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void system_proc(void *pvParameter){
  DrawManager *dw_mgr =(DrawManager*)pvParameter; 
  while (1) {        
    button_proc();
    system_variable_proc(); 
    
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void loop(void) {}
