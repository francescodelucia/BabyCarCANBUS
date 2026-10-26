#include <Arduino.h>
#include "common.h"
#include "MotorManager.h"
#include "CanManager.h"

MotorManager *mot1;
MotorManager *mot2;
CanManager *can_m;

void setup() {
  Serial.begin(115200);
  
  mot1= new MotorManager(MOTOR1_IN1,MOTOR1_IN2);  
  mot2= new MotorManager(MOTOR2_IN1,MOTOR2_IN2);  
  can_m = new CanManager(&c_data);

  xTaskCreatePinnedToCore(
    &canPooling, // task function
    "canPooling", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
  0); // out pointer to task handle

  xTaskCreatePinnedToCore(
    &dataParser, // task function
    "dataParser", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
  1); // out pointer to task handle
  
}

void loop() {/*vTaskDelay(pdMS_TO_TICKS(1000));*/}


void canPooling(void *pvParameter)
{      
  while (1) {    
    can_m->processRx();           
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void dataParser(void *pvParameter) {    
  while (1) {     
    CAN_DATA can_data;    
    MotorCommand cmd;
    can_m->getData(&can_data);             
    if (can_data.data_avalible==1) {
      Serial.println("Data available");
      
      if (can_data.can_id == CAN_ID_BOARD_MOTOR_MANAGER) {
        
        // Calcolo della rampa: moltiplichiamo data[4] per 10 (es. val 100 = 1000ms = 1 sec)
        // Oppure se usate un moltiplicatore diverso, impostate ad esempio 10 o 20.
        uint16_t calculatedRampMs = (uint16_t)data[4] * 10; 

        switch (data[2]) {
          case FORWARD_DIRECTION:           
            cmd.dir = DIR_FORWARD;
            cmd.targetSpeed = data[3];
            cmd.rampTimeMs  = calculatedRampMs;              
            break;
          case BACKWARD_DIRECTION:            
            cmd.dir = DIR_BACKWARD;
            cmd.targetSpeed = data[3];
            cmd.rampTimeMs  = calculatedRampMs;              
            break;
          case STOP_DIRECTION:
            cmd.dir = DIR_STOP;
            cmd.targetSpeed = 0;
            cmd.rampTimeMs  = calculatedRampMs;
            break;
        }
        // Inoltro del comando ai motori
        if (data[1] == MOTOR_1) {
          mot1->directDrive(&cmd);
        } else if (data[1] == MOTOR_2) {
          mot2->directDrive(&cmd);
        } else if (data[1] == MOTOR_ALL) {
          mot1->directDrive(&cmd);
          mot2->directDrive(&cmd);
        }
      }
    }
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}