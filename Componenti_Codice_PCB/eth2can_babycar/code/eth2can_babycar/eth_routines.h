#include <cstddef>
#include <stdint.h>
#include "IPAddress.h"
#ifndef ETH_ROUTINE
#define ETH_ROUTINE
#include <UIPEthernet.h>
#include <SPI.h>
                                                                                             
class EthManager{
  private:
    byte mac[6] = { MACADDR};
    int8_t busy = -1;
    IPAddress *ip;
    IPAddress *ldns;
    IPAddress *gateway;
    IPAddress *subnet;
    IPAddress *remoteIp;
    int8_t DHCP_on=-1;          
  public:
    EthManager(){
      this->ip        = new IPAddress(IP_DEVICE);
      this->ldns      = new IPAddress(IP_DNS);
      this->gateway   = new IPAddress(IP_GATEWAY);
      this->subnet    = new IPAddress(SUBNET);
      this->remoteIp  = new IPAddress(IP_REMOTE); // Destination IP    
      this->init();
    }
    ~EthManager(){} 
  public:
    int GetNetConf(IPAddress* ip,IPAddress* dns,IPAddress* gateway,IPAddress* subnet,IPAddress* remoteIp){
      if(this->ip!=NULL && this->ldns!=NULL && this->gateway!=NULL && this->subnet!=NULL && this->remoteIp!=NULL){
        *ip = *this->ip;
        *dns = *this->ldns;
        *gateway = *this->gateway;
        *subnet = *this->subnet;
        *remoteIp = *this->remoteIp;        
        return 1;        
      }
      return -1;
    }  
  private:
    void init(){
      SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);      
      Ethernet.init(SPI_CS);             
      delay(100); 
      if(Ethernet.begin(mac)==0){        
        Ethernet.begin(mac, *(this->ip), *(this->ldns), *(this->gateway), *(this->subnet));
      }else{
        DHCP_on = 1;
      }
      if(this->DHCP_on!=-1){

        this->ip = new IPAddress(Ethernet.localIP());
        this->gateway = new IPAddress(Ethernet.gatewayIP());
        this->subnet = new IPAddress(Ethernet.subnetMask());        
        this->ldns = new IPAddress(Ethernet.dnsServerIP());                
#ifdef ENABLE_DEBUG              
        Serial.print("######################### D H C P #################################\n");
#endif        
      } else{
#ifdef ENABLE_DEBUG                      
        Serial.print("##################### S T A T I C - I P  ##########################\n");
#endif        
      }
      for (int i = 0; i < 4; i++) {
          this->remoteIp[i] = this->ip[i] | ~this->subnet[i];          
      }      
#ifdef ENABLE_DEBUG      
      Serial.print("# Configuration\n");
      Serial.print("# IP:......");             
      Serial.println(this->ip->toString());
      Serial.print("# Subnet:..");
      Serial.println(this->subnet->toString());
      Serial.print("# Gateway:.");
      Serial.println(this->gateway->toString());
      Serial.print("# DNS:.....");
      Serial.println(this->ldns->toString());                            
      Serial.print("##################### P O R T S - C O M M #########################\n");     
      Serial.print("# Brodcast ip:..........");
      Serial.println(this->remoteIp->toString());                      
      Serial.print("# Brodcast remote Port:.");
      Serial.println(REMOTE_PORT);                      
      Serial.print("# Local Port:...........");
      Serial.println(LOCAL_PORT);                      
      Serial.print("# CLI remote Port:......");
      Serial.println(CLI_PORT);                      
      Serial.print("# CLI local Port:.......");
      Serial.println(CLI_LOCAL_PORT);                      
      Serial.print("###################################################################\n");    
#endif       
      delay(1000);            
    }  
};


#endif