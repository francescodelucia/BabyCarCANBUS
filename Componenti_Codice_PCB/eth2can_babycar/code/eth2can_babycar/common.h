#include <stdint.h>
//#include <ESP32-TWAI-CAN.hpp>
#include <ESP32_TWAI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#define ENABLE_DEBUG_CAN 1
//#define ENABLE_DEBUG_CLI_LEVEL_1 1
//#define ENABLE_DEBUG_CLI_LEVEL_2 1 
//#define ENABLE_DEBUG_IP 1
//#define ENABLE_DEBUG

#define SPI_MOSI 2
#define SPI_MISO 1
#define SPI_SCK  3
#define SPI_CS   4
#define ETH_RST  5

#define BUFFER_SIZE 150

#define CAN_TX   GPIO_NUM_13  // Connects to CTX
#define CAN_RX   GPIO_NUM_12  // Connects to CRX

#define MACADDR 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED

#define IP_DEVICE   169, 254, 100, 100
#define IP_DNS      169, 254, 0, 1
#define IP_GATEWAY  169, 254, 0, 1
#define SUBNET      255, 255, 0, 0
#define IP_REMOTE   0, 0, 0, 0


#define CMD_INFO        "sysinfo"
#define HELO            "helo"
#define RESET           "reset"
#define CMD_3           "-----"
#define DISCOVER        "discover"
#define DEVINFO         "devinfo"
#define SETCANID        "set_canid"
#define WIFIMODULE      "set_wifi"
#define SETSSIDWIFI     "set_ssid_wifi"
#define SETPWDWIFI      "set_pwd_wifi"
#define REMOTEREBOOT    "remote_reboot"
#define BLINKLED        "blink_led"
#define HELP            "help"

#define BAD_RES   "syntax error - comando non riconosciuto!\n>"

#define LOCAL_PORT  8888
#define REMOTE_PORT 8889
#define CLI_PORT 9080
#define CLI_LOCAL_PORT CLI_PORT+1

char banner_a_[]= "\
###################################################################\n\
#              Eth2Canbus - Gateway BabyCar CANBUS                #\n\
#         BabyCar CANBUS Team - Simone & Francesco De Lucia       #";

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

CAN_DATA c_data={-1,0,-1,{0}};
CAN_DATA u_data={-1,0,-1,{0}};
/*    
    how to open terminal for diagnostic
    socat -t 3 - UDP-DATAGRAM:[IP DEVICE]:[CLI_PORT],bind=:[CLI_LOCAL_PORT]
    es.
    socat -t 3 - UDP-DATAGRAM:169.254.100.100:9080,bind=:9081
*/

