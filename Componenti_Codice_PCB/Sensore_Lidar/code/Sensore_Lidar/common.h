#include <sys/_stdint.h>
#pragma once
#include <Arduino.h>
// Fix di compatibilità per core basati su ESP-IDF v4.x (Bluepad32 board)

//#define ENABLE_DEBUG
#define ENABLE_BATTERY_SIMULATION

#define CAN_TX    GPIO_NUM_13  // Connects to CTX
#define CAN_RX    GPIO_NUM_12  // Connects to CRX

#define SDA_PIN   GPIO_NUM_8
#define SCL_PIN   GPIO_NUM_9

#define CAN_ID_ECU_MANAGER          0x0AA
#define CAN_ID_BOARD_THROTTLE       0x0A1 
#define CAN_ID_BOARD_BATTERY        0x0A2
#define CAN_ID_DEBUG_GEAR           0x0A3
#define CAN_ID_BOARD_GENERAL        0x0A4 
#define CAN_ID_BOARD_REMOTE_CTRL    0x0A5
#define CAN_ID_BOARD_LUCI           0x0A5
#define CAN_ID_BOARD_STEERING       0x0A6
#define CAN_ID_BOARD_LIDAR          0x0B0
#define CAN_ID_BOARD_MOTOR_MANAGER  0x0AB


#define CAN_OPTIONS                    0xAA        
#define CAN_OPTIONS_SPEED_DATA         0xA0        
#define CAN_OPTIONS_BATTERY_DATA       0xA1  
#define CAN_OPTIONS_THROTTLE_DATA      0xA2
#define CAN_OPTIONS_LIGHT_CAR          0xA3
#define CAN_OPTIONS_LIGHT_RAINBOW      0xA4
#define CAN_OPTIONS_TORQUE_VAL         0xA5
#define CAN_OPTIONS_STEERING_POSITION  0xA6
#define CAN_OPTIONS_REMOTE_PS4         0xA7
#define CAN_OPTIONS_STEERING_SERVO     0xA8    
#define CAN_OPTIONS_GEAR_DATA          0xB1    
#define CAN_OPTIONS_STOP_CAR           0xA9    
#define CAN_OPTIONS_LIDAR              0xB0

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

uint16_t distanza;
uint16_t old_distanza;

CAN_DATA c_data={-1,0,-1,{0}};

/*static uint8_t speed = 0;
static uint16_t battery = 0;
static uint8_t throttle = 0;*/