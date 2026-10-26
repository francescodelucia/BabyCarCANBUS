#include <cstddef>
#include <stdint.h>
#include "IPAddress.h"
#ifndef UDP_CAN
#define UDP_CAN
#include <UIPEthernet.h>
                                                                                            
class udpCAN{
  private:    
    IPAddress ip;
    IPAddress ldns;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress remoteIp;
    EthernetUDP udprx;
    EthernetUDP udptx;        
  public:    
    CAN_DATA *c_data; //={-1,0,-1,{0}}; 
  public:
    udpCAN(CAN_DATA *c_data,IPAddress* ip,IPAddress* dns,IPAddress* gateway,IPAddress* subnet,IPAddress* remoteIp){      
      this->c_data    = c_data;
      this->ip        = *ip;
      this->ldns      = *dns;
      this->gateway   = *gateway;
      this->subnet    = *subnet;
      this->remoteIp  = *remoteIp; // Destination IP          
      this->init();
    }
    ~udpCAN(){}
  private:        
    int oct[4]={0};          
  private:
    void init(){   
      char *argv[8];  
      int argc;
      split(argv, &argc,(char*)this->ip.toString().c_str(), '.', 0);
      for(int i = 0; i < argc; i++){
        this->oct[i]=atoi(argv[i]);        
      }   
    }      
    char **split(char **argv, int *argc, char *string, const char delimiter, int allowempty)
    {
        *argc = 0;
        do
        {
            if(*string && (*string != delimiter || allowempty))
            {
                argv[(*argc)++] = string;
            }
            while(*string && *string != delimiter) string++;
            if(*string) *string++ = 0;
            if(!allowempty) 
                while(*string && *string == delimiter) string++;
        }while(*string);
        return argv;
    } 
  public: 
    void receive(){      
       int len_data =0;
       char packetBuffer[BUFFER_SIZE] = {0};               
      if(this->udprx.begin(LOCAL_PORT)){ 
        
        len_data = this->udprx.parsePacket();
        if (len_data) 
        {            
          this->udprx.read(packetBuffer, len_data);         
          this->c_data->data_avalible = 1;           
        }       
        this->udprx.stop();
      }
      uint32_t _id_ = ( packetBuffer[0] << 8 ) + packetBuffer[1];     // big endian or little endian ???????  
      if(_id_ == 0 ){
        this->c_data->data_avalible = -1;           
        this->c_data->can_len = -1;           
      }else{
        this->c_data->can_id = _id_;
        this->c_data->can_len = packetBuffer[2]; 
        for(int k=0;k<this->c_data->can_len;k++){
          this->c_data->can_data[k]= packetBuffer[k+3];
        } 
      }
#ifdef ENABLE_DEBUG_CAN   
      if (len_data){      
        Serial.printf("Len data: %i \r\n", len_data);             
        if(this->c_data->data_avalible==1){
          Serial.printf("Received frame EthManager: %03X \r\n", this->c_data->can_id);             
          Serial.print("Data length: ");
          Serial.println(this->c_data->can_len); //Print the data length
          Serial.print("Data: ");
          for(int x = 0;x<this->c_data->can_len;x++){
            Serial.print(this->c_data->can_data[x], HEX); 
          }      
          Serial.print("\n");  
        }
      }
#endif               
    }      
    void send(CAN_DATA *tx_data){//void* data,int len){         
      if(tx_data->data_avalible == 1){
        tx_data->data_avalible = -1;
#ifdef ENABLE_DEBUG_IP              
        Serial.printf("IP: %i %i %i %i\n" , this->oct[0],this->oct[1],this->oct[2],this->oct[3]); 
#endif    
        char dataraw[tx_data->can_len+7] = {0};         
        dataraw[0] = oct[0];
        dataraw[1] = oct[1];        
        dataraw[2] = oct[2];
        dataraw[3] = oct[3];
        dataraw[4] = (tx_data->can_id >> 8); 
        dataraw[5] = tx_data->can_id & 0xff;       
        dataraw[6] = tx_data->can_len;
        for(int i=0;i<tx_data->can_len+1;i++){         
          dataraw[7+i]= tx_data->can_data[i];
        }                
        this->udptx.beginPacket(this->remoteIp, REMOTE_PORT); // Destination IP and Port
        this->udptx.write(dataraw,tx_data->can_len+7); 
        this->udptx.endPacket(); // Send the packet
      }         
    }
    void pooling() {      
      this->receive();    
    } 
    /*CAN_DATA  getData(){         
      this->receive();    
      if(this->c_data.data_avalible!=-1){
        this->c_data.data_avalible= -1;        
      }else{
        this->c_data.can_len= -1;        
      }
      return this->c_data;;
    }*/
};


#endif