//#include <ESP32-TWAI-CAN.hpp>

#include "common.h"
#include "eth_routines.h"
#include "can_routines.h"
#include "udpCAN.h"
#include "udpCLI.h"


EthManager *eth_m;
CanManager *can_m;
udpCAN *udp_CAN;
udpCLI *udp_CLI;

#ifdef ENABLE_DEBUG              
void init_serial(){ 
  Serial.begin(115200);
  Serial.println(banner_a_);  
}
#endif
void setup() {  
  delay(2000);
#ifdef ENABLE_DEBUG                
  init_serial();  
#endif  
  // Set up serial for debugging  
  pinMode(ETH_RST, OUTPUT);
  digitalWrite(ETH_RST, LOW);
  delay(1000);
  digitalWrite(ETH_RST, HIGH);
  
  delay(1000);
  eth_m = new EthManager();  
  delay(100);
  can_m = new CanManager(&c_data);  
  delay(100);
  IPAddress ip; 
  IPAddress ldns;
  IPAddress gateway;
  IPAddress subnet; 
  IPAddress remoteIp;

  if(eth_m->GetNetConf(&ip,&ldns, &gateway, &subnet,&remoteIp)!=-1){    
    udp_CAN = new udpCAN(&u_data,&ip,&ldns,&gateway,&subnet,&remoteIp);
    udp_CLI = new udpCLI(&ip,&ldns,&gateway,&subnet,&remoteIp,can_m);
  }

  xTaskCreatePinnedToCore(
    &proc, // task function
    "proc", // task name
    5000, // stack size in words
    NULL,
    0, // priority
    NULL,
    0); // out pointer to task handle*/          
  xTaskCreatePinnedToCore(
    &proc_pooling_can, // task function
    "proc_pooling_can", // task name
    5000, // stack size in words
    NULL,
    1, // priority
    NULL,
    1); // out pointer to task handle*/    
}


void proc(void *pvParameter)
{    
  while (1) {        
    udp_CAN->pooling();      
    udp_CLI->receive();      
    udp_CLI->getCanForCLIdata(&c_data);
    udp_CAN->send(&c_data);        
    can_m->send(&u_data);         
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}
void proc_pooling_can(void *pvParameter)
{    
  while (1) {    
    can_m->pooling();     
    vTaskDelay(portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}

void loop() {}
