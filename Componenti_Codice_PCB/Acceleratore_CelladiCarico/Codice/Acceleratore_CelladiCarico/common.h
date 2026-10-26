#include <sys/_stdint.h>
#pragma once
#include <Arduino.h>
// Fix di compatibilità per core basati su ESP-IDF v4.x (Bluepad32 board)

#define ENABLE_DEBUG
//#define BODY_CAR

#define CAN_TX   GPIO_NUM_13  // Connects to CTX
#define CAN_RX   GPIO_NUM_12  // Connects to CRX

#ifndef BODY_CAR
#define LOADCELL_DOUT_PIN  GPIO_NUM_7
#define LOADCELL_SCK_PIN   GPIO_NUM_6
#define ENCODER_PIN_A   GPIO_NUM_10
#define ENCODER_PIN_B   GPIO_NUM_9
#define ENCODER_BTN     GPIO_NUM_8
#else
#define LOADCELL_DOUT_PIN   GPIO_NUM_8
#define LOADCELL_SCK_PIN    GPIO_NUM_9
#define ENCODER_PIN_A       GPIO_NUM_11
#define ENCODER_PIN_B       GPIO_NUM_7
#define ZERO_CROSS_PIN      GPIO_NUM_6
#endif

#define CAN_ID_ECU_MANAGER          0x0AA
#define CAN_ID_BOARD_THROTTLE       0x0A1 
#define CAN_ID_BOARD_BATTERY        0x0A2
#define CAN_ID_DEBUG_GEAR           0x0A3
#define CAN_ID_BOARD_GENERAL        0x0A4 
#define CAN_ID_BOARD_REMOTE_CTRL    0x0A5
#define CAN_ID_BOARD_LUCI           0x0A5
#define CAN_ID_BOARD_STEERING       0x0A6
#define CAN_ID_BOARD_LIDAR          0x0B0

#define CAN_GEAR_UP      0xF0
#define CAN_GEAR_DOWN    0xF1
#define CAN_OPTION_R     0xF2
#define CAN_OPTION_L     0xF3
#define CAN_ENC_BUT      0xF4
#define CAN_ENC_LONG_BUT 0xF5

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
#define CAN_OPTIONS_STOP_CAR           0xA9    
#define CAN_OPTIONS_LIDAR              0xB0



// Configurazione hardware
#define LED_PIN     GPIO_NUM_5
#define NUM_LEDS    12

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

typedef struct CAR_DATA_SYSTEM{        
    int light_car;
    int light_car_r;
    int light_car_g;
    int light_car_b;
    int light_rainbow;
    int light_rainbow_r;
    int light_rainbow_g;
    int light_rainbow_b;
    int remote_ps4;    
    int steering_servo;  //comanda la funzione servo per la centralina motore      
    uint8_t stop_car;    
};
CAR_DATA_SYSTEM car_data_system={0,0,0,0,0,0,0,0,0,0,0};
CAN_DATA c_data={-1,0,-1,{0}};