#include "common.h"
#include "LidarManager.h"
#include "CanManager.h"

LidarManager *lidaMan;
CanManager *can_m;

void setup() {
  Serial.begin(115200);  
  lidaMan = new LidarManager(&distanza);  
  can_m = new CanManager(&c_data); 
  xTaskCreatePinnedToCore(
    taskLidar,
    "taskLidar",
    5000,
    NULL,
    1,
    NULL,
    0
  );  
   xTaskCreatePinnedToCore(
    &canPooling, // task function
    "canPooling", // task name
    2000, // stack size in words
    NULL,
    1, // priority
    NULL,
    1); // out pointer to task handle  
}
void taskLidar(void *pvParameter){
   while (1) {       
      lidaMan->pooling();
      if(distanza!=old_distanza){
        old_distanza = distanza;
#ifdef ENABLE_DEBUG       
        Serial.print("Distanza: "); 
        Serial.print(distanza);    
        Serial.println(" mm");  
#endif      
        CAN_DATA s;
        s.can_id = CAN_ID_BOARD_LIDAR;      
        s.can_data[0] = distanza & 0xFF;       
        s.can_data[1] = (distanza >> 8);             
        s.data_avalible = 1;
        s.can_len = 2;
        can_m->sendCAN(&s);
#ifdef ENABLE_BATTERY_SIMULATION
        s.can_id = CAN_ID_BOARD_BATTERY;      
        s.can_data[0] = 0x09;       
        s.can_data[1] = 0xFF;             
        s.data_avalible = 1;
        s.can_len = 2;
        can_m->sendCAN(&s);
#endif        
      }
      vTaskDelay(portTICK_PERIOD_MS*150);
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


void loop() {  }