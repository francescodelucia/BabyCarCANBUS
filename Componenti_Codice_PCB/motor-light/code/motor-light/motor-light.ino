#include "common.h"
#include "CanManager.h"
#include "LedManager.h"

LedManager *ledMgr;
CanManager *can_m;


//MotorManager *mot1;
//MotorManager *mot2;
   
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("System Init!");
  ledMgr = new LedManager();
  can_m = new CanManager(&c_data);  
  //mot1= new MotorManager(MOTOR1_IN1,MOTOR1_IN2);  
  //mot2= new MotorManager(MOTOR2_IN1,MOTOR2_IN2);  
  xTaskCreatePinnedToCore(
    &canPooling, // task function
    "canPooling", // task name
    5000, // stack size in words
    NULL,
    0, // priority
    NULL,
    0); // out pointer to task handle*/     
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
    NULL,
    1, // priority
    NULL,
  1); // out pointer to task handle
}

void dataParser(void *pvParameter)
{      
  while (1) {          
    if (c_data.can_id == CAN_ID_BOARD_MOTOR_MANAGER) {                
        uint16_t calculatedRampMs = (uint16_t)c_data.can_data[3] * 10; 
        if (c_data.can_data[0] == MOTOR_1) {
          car_data_motor.motor_1 =  c_data.can_data[2];                               
        } else if (c_data.can_data[0] == MOTOR_2) {
          car_data_motor.motor_2 =  c_data.can_data[2];                               
        } else if (c_data.can_data[0] == MOTOR_ALL) {
          car_data_motor.motor_1 =  c_data.can_data[2];                               
          car_data_motor.motor_2 =  c_data.can_data[2];                                         
        }
        car_data_motor.rampTimeMs = c_data.can_data[3];        
    }
    if (c_data.can_id == CAN_ID_ECU_MANAGER) {                
        if ((c_data.can_data[0] == CAN_OPTIONS) && 
            (c_data.can_data[1] == CAN_OPTIONS_STOP_CAR)){
          car_data_motor.motor_1 =  0;                               
          car_data_motor.motor_2 =  0; 
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
    //can_m->processTx();    
    can_m->processRx();       
    //encoderProc(&lastPos);
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void system_proc(void *pvParameter){  
  while (1) {      
    ledMgr->pooling();     
    //button_proc();
    //system_variable_proc(); 
    
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void loop() {}
