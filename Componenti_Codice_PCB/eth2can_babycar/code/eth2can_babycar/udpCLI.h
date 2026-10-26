#include <cstddef>
#include <stdint.h>
#include "IPAddress.h"
#ifndef UDP_CLI
#define UDP_CLI
#include <UIPEthernet.h>

#define B_FILTER(x) else if(strcmp(lBuffer,x)==0){
#define START_FILTER(x) else if(((strlen(x)+1)==sBuffer.length()) && (sBuffer.startsWith(x,0)==1)){        
#define START_FILTER_EXT(x) else if(sBuffer.startsWith(x,0)==1){        
#define END_F  }
                                                                                             
class udpCLI{
  private:
    IPAddress ip;
    IPAddress ldns;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress brodcastIp;
    IPAddress remoteIp;
    EthernetUDP udprx;
    EthernetUDP udptx;          
    CanManager *can_m;
  public:
    udpCLI(IPAddress* ip,IPAddress* dns,IPAddress* gateway,IPAddress* subnet,IPAddress* remoteIp,CanManager* can_m ){      
      this->ip        = *ip;
      this->ldns      = *dns;
      this->gateway   = *gateway;
      this->subnet    = *subnet;
      this->brodcastIp  = *remoteIp; // Destination IP          
      this->can_m = can_m;
      this->init();
    }
    ~udpCLI(){}
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
      this->heloInfo(1);  
      this->send("ready!\n>",1);
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
    void heloInfo(int brodcast=0){
      this->send("###################################################################\n",brodcast);
      this->send("#-----------------------------------------------------------------#\n",brodcast);
      this->send("#              Eth2Canbus - Gateway BabyCar CANBUS                #\n",brodcast);
      this->send("#-----------------------------------------------------------------#\n",brodcast);
      this->send("#         BabyCar CANBUS Team - Simone & Francesco De Lucia       #\n",brodcast); 
      this->send("###################################################################\n",brodcast);      
    }
    void infoSystem(){
      char str[20]={0};      
      this->send("# Configuration\n");
      this->send("# IP:......");             
      this->send((char*)this->ip.toString().c_str());
      this->send("\n# Subnet:..");
      this->send((char*)this->subnet.toString().c_str());
      this->send("\n# Gateway:.");
      this->send((char*)this->gateway.toString().c_str());
      this->send("\n# DNS:.....");
      this->send((char*)this->ldns.toString().c_str());                            
      this->send("\n##################### P O R T S - C O M M #########################\n");     
      this->send("# Brodcast ip:..........");
      this->send((char*)this->brodcastIp.toString().c_str());                      
      this->send("\n# Brodcast remote Port:.");
      itoa(REMOTE_PORT,str,10);
      this->send(str);                      
      this->send("\n# Local Port:...........");
      memset(&str,0,20);
      itoa(LOCAL_PORT,str,10);
      this->send(str);                      
      this->send("\n# CLI remote port:......");
      memset(&str,0,20);
      itoa(CLI_PORT,str,10);
      this->send(str);                      
      this->send("\n# CLI local port:.......");
      memset(&str,0,20);
      itoa(CLI_PORT+1,str,10);
      this->send(str);                      
      this->send("\n###################################################################\n>");           
    }
  public: 
    void receive(){
      int len_data = 0;
      char packetBuffer[BUFFER_SIZE] = {0};   
      if(this->udprx.begin(CLI_PORT)){                               
        len_data = this->udprx.parsePacket();        
        if (len_data) 
        {                      
          this->udprx.read(packetBuffer, len_data);  
          this->remoteIp = this->udprx.remoteIP();
          this->udprx.stop();          
          this->procCLI((char*)&packetBuffer, len_data);    
        }                 
      }
#ifdef ENABLE_DEBUG_CLI_LEVEL_1
      if (len_data){      
        Serial.printf("Len data: %i \r\n", len_data);                     
        Serial.printf("Received CLI command:  \r\n");             
        Serial.print("Data length: ");
        Serial.println(len_data); //Print the data length
        Serial.print("Data: ");
        for(int x = 0;x<len_data;x++){
           Serial.print(packetBuffer[x]); 
        }      
        Serial.print("\n"); 
      }
#endif               
    } 
    void getCanForCLIdata(CAN_DATA *tx_data){
      if(tx_data->data_avalible == 1){       
        this->getPongData(tx_data);
        this->getConfData(tx_data);
      }         
    }  
  private:   
    void getPongData(CAN_DATA *tx_data){
      if(tx_data->data_avalible == 1 && 
          tx_data->can_id == 0x00 && tx_data->can_data[0] == 0xaf &&
          tx_data->can_data[2] == 0x48 && tx_data->can_data[3] == 0x65 &&
          tx_data->can_data[4] == 0x6c && tx_data->can_data[5] == 0x6c &&
          tx_data->can_data[6] == 0x6f && tx_data->can_data[7] == 0x21)
        {          
          tx_data->data_avalible = -1;
          char rx_data[15];
          sprintf(rx_data, "Found device id DEC %i HEX 0x%02X\n", tx_data->can_data[1],tx_data->can_data[1]);
          this->send(rx_data);
          this->send(">");
        }   
    }
     void getConfData(CAN_DATA *tx_data){
      if( tx_data->data_avalible == 1 && 
          tx_data->can_data[0] ==  tx_data->can_id &&  tx_data->can_data[1] == 0xb2)          
        {      
          tx_data->data_avalible = -1;              
          char rx_data[15]={0};
          sprintf(&rx_data[0], "%c%c%c%c%c%c",
                         tx_data->can_data[2],
                         tx_data->can_data[3],
                         tx_data->can_data[4],
                         tx_data->can_data[5],
                         tx_data->can_data[6],
                         tx_data->can_data[7]);
          this->send(rx_data);          
        }   
    }
    void ShowHelp(){
      this->send("\nHelp - Comandi disponibili\n");
      this->send("- ");
      this->send(CMD_INFO);
      this->send("- restituisce le info della configurazione\n");
      this->send("- ");
      this->send(HELO);
      this->send("- restituisce le informazioni del dispositivo\n");
      this->send("- ");
      this->send(RESET);
      this->send("- riavvio del dispositivo\n");
      this->send("- ");
      this->send(DISCOVER);
      this->send("- fai il discover dei dispositivi sulla rete CAN \n");
      this->send("- ");   
      this->send(DEVINFO);
      this->send(" [DEC CAN ID] restituisce la conficurazione del device selezionato \n");
      this->send("- "); 
      this->send(SETCANID);      
      this->send(" [DEC CAN_ID SRC] [DEC CAN_ID DST] modifica il CAN_ID del device \n");
      this->send("- ");       
      this->send(REMOTEREBOOT);
      this->send(" [DEC CAN_ID device ] reboot del device remoto\n");
      this->send("- "); 
      this->send(WIFIMODULE);
      this->send(" [DEC CAN_ID device ] [0/1] abilita il modulo wifi del device\n");
      this->send("  ########################## ATTENZIONE ##########################\n");
      this->send("  questa modalita disattiva le funzionalita standard del device\n");
      this->send("  per permettere l'aggiornamento da remoto [OTA]\n");
      this->send("  Per rendere effettive le modifiche necessario riavvio [");
      this->send(REMOTEREBOOT);
      this->send("]\n");
      this->send("  ########################## ATTENZIONE ##########################\n");
      this->send("- "); 
      this->send(SETSSIDWIFI);
      this->send(" [DEC CAN_ID device ] [\"SSID\"] impotsa wifi SSID\n");
      this->send("- ");            
      this->send(SETPWDWIFI);
      this->send(" [DEC CAN_ID device ] [\"PASSWORD\"] imposta password wifi\n");
      this->send("- ");            
      this->send(BLINKLED);
      this->send(" [DEC CAN_ID device ] abilita lampeggio led sul device da identificare\n");
      this->send("- ");            
      this->send(HELP);
      this->send("- visualizza questo messaggio\n>");      
    }
    void procCLI(char *pBuffer,int lenBuffer){     
      String sBuffer(pBuffer);      
#ifdef  ENABLE_DEBUG_CLI_LEVEL_2              
      for(int i=0;i<lenBuffer;i++){     
        Serial.print(i,DEC);            
        Serial.print(pBuffer[i]);
        Serial.print(":");
        Serial.println(pBuffer[i],HEX);      
      }
#endif
      if(pBuffer[0]== '\n'){
        this->send(">");
      }                
      START_FILTER(CMD_INFO)      
        this->heloInfo();
        this->infoSystem();
      END_F
      START_FILTER(HELO)      
        this->send("Ciao io sono");
        this->heloInfo();
        this->send(">");
      END_F
      START_FILTER_EXT(WIFIMODULE)        
        this->setWifiModule(sBuffer);  
        this->send(">");   
      END_F       
      START_FILTER(RESET)            
        this->send("riavvio in corso......\n");
        ESP.restart();
      END_F      
      START_FILTER(DISCOVER)                         
        this->discover_device();
        this->send(">");        
      END_F
      START_FILTER_EXT(SETCANID)             
        this->setCanID(sBuffer);
        this->send(">");        
      END_F      
      START_FILTER_EXT(DEVINFO)        
        this->devInfo(sBuffer);  
        this->send(">");   
      END_F                        
      START_FILTER_EXT(REMOTEREBOOT)        
        this->remoteReboot(sBuffer);  
        this->send(">");   
      END_F   
      START_FILTER_EXT(SETSSIDWIFI)        
        //this->setWifiSSID(sBuffer);  
        this->setWifiData(sBuffer,0xB1);
        this->send(">");   
      END_F 
      START_FILTER_EXT(SETPWDWIFI)        
        //this->setWifiPassword(sBuffer);  
        this->setWifiData(sBuffer,0xB2);
        this->send(">");   
      END_F           
      START_FILTER_EXT(BLINKLED)        
        //this->setWifiPassword(sBuffer);  
        this->blinkLed(sBuffer);
        this->send(">");   
      END_F           
      START_FILTER(HELP)                        
        ShowHelp();
      END_F
      else{
        this->send(BAD_RES);
      }
    }  
    void setWifiData(String sBuffer,char can_function){
      char h[30]={0};      
      int startPos = sBuffer.indexOf("\"") + 1;
      int endPos = sBuffer.indexOf("\"", startPos);        
      String subString = sBuffer.substring(startPos, endPos);
      subString.toCharArray(&h[0],subString.length()+1); 
      uint8_t _len =strlen(h);           
      String buff[5];                      
      int n = this->splitString(sBuffer, buff,' ', 5);     
      if(buff[1].length()>0 && _len>0 && endPos > 0){
        char id[10];
        memset(&id,0,10);
        buff[1].toCharArray(&id[0],buff[1].length()+1);                  
        int device_id = atoi(&id[0]); 
        CAN_DATA devinfo_data;                  
        uint16_t _len_res = _len%5;
        uint8_t _len_for = (_len_res>0?_len + (5+_len_res):_len);        
        for(int a=0;a<_len_for;a=a+5){ 
          devinfo_data.can_len = 8;
          devinfo_data.can_id = device_id;
          devinfo_data.data_avalible = 1;                     
          devinfo_data.can_data[0] = can_function;
          devinfo_data.can_data[1] = a;// :a+1);//_len - a;                    
          devinfo_data.can_data[2] = a+4;//(_len - a)-5;                    
          devinfo_data.can_data[3] = (a>_len)?0:h[a]; 
          devinfo_data.can_data[4] = (a+1>_len)?0:h[a+1];
          devinfo_data.can_data[5] = (a+2>_len)?0:h[a+2];
          devinfo_data.can_data[6] = (a+3>_len)?0:h[a+3];
          devinfo_data.can_data[7] = (a+4>_len)?0:h[a+4];    
          devinfo_data.can_data[2] = (devinfo_data.can_data[7]==0?a+4:devinfo_data.can_data[2]);
          devinfo_data.can_data[2] = (devinfo_data.can_data[6]==0?a+3:devinfo_data.can_data[2]);
          devinfo_data.can_data[2] = (devinfo_data.can_data[5]==0?a+2:devinfo_data.can_data[2]);
          devinfo_data.can_data[2] = (devinfo_data.can_data[4]==0?a+1:devinfo_data.can_data[2]);
          //devinfo_data.can_data[2] = (devinfo_data.can_data[a]==0?0:0);
          //devinfo_data.can_data[2] = (devinfo_data.can_data[7]==0?0:devinfo_data.can_data[2]);
          if((devinfo_data.can_data[3] + 
            devinfo_data.can_data[4] + 
            devinfo_data.can_data[5] + 
            devinfo_data.can_data[6] +
            devinfo_data.can_data[7])>0 && devinfo_data.can_data[3]!=0){
            this->can_m->send(&devinfo_data);     
          }            
          memset(&(devinfo_data.can_data[0]),0,8);                         
          delay(100);                        
        }    
        memset(&(devinfo_data.can_data[0]),0,8);                         
        devinfo_data.can_len = 3;
        devinfo_data.can_id = device_id;
        devinfo_data.can_data[0] = can_function;
        devinfo_data.can_data[1] = 0xFF;                    
        devinfo_data.can_data[2] = 0xFF;                              
        this->can_m->send(&devinfo_data);                   
      }                         
    }          
    void remoteReboot(String sBuffer){
      String buff[5];                
      int n = this->splitString(sBuffer, buff,' ', 5);  
      //Serial.println(sBuffer);     
      if(buff[1].length()>0){
        char id[10];
        memset(&id,0,10);
        buff[1].toCharArray(&id[0],buff[1].length()+1);                  
        int device_id = atoi(&id[0]);        
        //Serial.println(id);        
        CAN_DATA devinfo_data;   
        devinfo_data.data_avalible = 1;                     
        devinfo_data.can_len = 2;
        devinfo_data.can_id = device_id;
        devinfo_data.can_data[0] = 0xA1; //API Remote Reboot
        devinfo_data.can_data[1] = 0x01; //API Remote Reboot
        this->can_m->send(&devinfo_data);
        delay(100);
        this->send("In fase di riavvio ");  
        this->send("Device ");              
        this->send(&id[0]);                      
        delay(100);                                           
      }                         
    }  
    void blinkLed(String sBuffer){
      String buff[5];                
      int n = this->splitString(sBuffer, buff,' ', 5);  
      Serial.println(sBuffer);     
      if(buff[1].length()>0){
        char id[10];
        memset(&id,0,10);
        buff[1].toCharArray(&id[0],buff[1].length()+1);                  
        int device_id = atoi(&id[0]);        
        Serial.println(id);        
        CAN_DATA devinfo_data;         
        devinfo_data.data_avalible = 1;                                    
        devinfo_data.can_len = 1;
        devinfo_data.can_id = device_id;
        devinfo_data.can_data[0] = 0xCC; //API LED blink        
        this->can_m->send(&devinfo_data);        
        this->send("Abilitata/Disabilitata lampeggio sulla scheda ");          
        this->send(&id[0]);                      
        delay(100);                                           
      }                         
    }        
    void  setWifiModule(String sBuffer){
      String buff[5];                
      int n = this->splitString(sBuffer, buff,' ', 5);     
      if(buff[1].length()>0 && buff[2].length()>0){
        char id[10];
        memset(&id,0,10);
        buff[1].toCharArray(&id[0],buff[1].length()+1);                  
        int device_id = atoi(&id[0]);
        memset(&id,0,10);        
        buff[2].toCharArray(&id[0],buff[2].length()+1);          
        int set_conf = atoi(&id[0]);
        CAN_DATA devinfo_data;
        if(set_conf == 1){
          devinfo_data.can_data[1] = 0x01; //API COMMAND ENABLE Wifi MODULE  
        }else{
          devinfo_data.can_data[1] = 0x00; //API COMMAND ENABLE Wifi MODULE
        }
        devinfo_data.data_avalible = 1;                     
        devinfo_data.can_len = 2;
        devinfo_data.can_id = device_id;
        devinfo_data.can_data[0] = 0xBB; //API COMMAND SET_RGB_MODULE        
        this->can_m->send(&devinfo_data);
        delay(1000);
        this->send("Module Wifi modificato per il device nuova configurazione\n");              
        this->send("Per rendere effettiva la configurazione necessario REBOOT\n");  
        delay(100);              
        this->devInfoDetails(device_id);                
      }                         
    }  
    void setCanID(String sBuffer){
      String buff[5];                
      int n = this->splitString(sBuffer, buff,' ', 5);           
      if(buff[1].length()>0 && buff[2].length()>0){
        char id[10];
        memset(&id,0,10);
        buff[1].toCharArray(&id[0],buff[1].length()+1);          
        //Serial.println(id);
        int device_id_source = atoi(&id[0]);
        memset(&id,0,10);        
        buff[2].toCharArray(&id[0],buff[2].length()+1);  
        //Serial.println(id);
        int device_id_dest = atoi(&id[0]);
        CAN_DATA devinfo_data;
        devinfo_data.data_avalible = 1;                     
        devinfo_data.can_len = 3;
        devinfo_data.can_id = device_id_source;
        devinfo_data.can_data[0] = 0xFF; //API COMMAND SET_ID_CAN
        devinfo_data.can_data[1] = device_id_dest; //API New CAN ID
        devinfo_data.can_data[2] = 0x01; //API reboot device
        this->can_m->send(&devinfo_data);                
        /*Serial.printf("device_id_source %i - device_id_dest %i\n",device_id_source,device_id_dest);
        Serial.printf("device_id_source %i - device_id_dest %i\n",buff[1],buff[2]);
        Serial.println(buff[1]);
        Serial.println(buff[2]);*/
        this->send("Comando di modifica CAN ID inviato - ");        
        this->send("avviare il comando di 'discover' per verificare i nuovi ID\n");                
      }                         
    }
    void devInfo(String sBuffer){
      String buff[5];                
      int n = this->splitString(sBuffer,  buff,' ', 5);     
      if(buff[1].length()>0){
        char idDev[10]={0};
        buff[1].toCharArray(idDev,buff[1].length());  
        int device_id = atoi(idDev);
        this->devInfoDetails(device_id);        
      }                         
    }
    void devInfoDetails(int device_id){      
      CAN_DATA devinfo_data;
      devinfo_data.data_avalible = 1;                     
      devinfo_data.can_len = 2;
      devinfo_data.can_id = device_id;
      devinfo_data.can_data[0] = 0xA2;
      devinfo_data.can_data[1] = 0x01;
      this->can_m->send(&devinfo_data);                               
    }
    int splitString(String input,  String output[], char separator,      int maxParts) {
      int count = 0;
      int startIndex = 0;
      for (int i = 0; i < input.length(); i++) {
          if (input.charAt(i) == separator) {
              if (count < maxParts) {
                  output[count++] = input.substring(startIndex, i);
              }
              startIndex = i + 1;
          }
      }
      // ultima parte
      if (count < maxParts) {
          output[count++] = input.substring(startIndex);
      }
      return count;
    }
    void discover_device(){
      CAN_DATA ping_data;
      ping_data.data_avalible = 1;
      ping_data.can_len = 2;
      ping_data.can_id = 0x00;
      ping_data.can_data[0] = 0xAA;
      ping_data.can_data[1] = 0x01;
      this->can_m->send(&ping_data);
    }
    void send(char* msg,int brodcast=0){
#ifdef ENABLE_DEBUG_CLI_LEVEL_1
      Serial.println(msg);
#endif      
      int msg_len = strlen(msg);
      char data_msg[BUFFER_SIZE] = {0};    
      memcpy(&data_msg,msg,strlen(msg));  

      for(int i=0;i<msg_len;i++){         
        data_msg[i]= data_msg[i];
      }                  
      if(brodcast==1){
        this->udptx.beginPacket(this->brodcastIp, CLI_PORT+1); // Destination IP and Port
      }else{
        this->udptx.beginPacket(this->remoteIp, CLI_PORT+1); // Destination IP and Port
      }
      this->udptx.write(data_msg,msg_len); 
      this->udptx.endPacket(); // Send the packet
    }
};


#endif