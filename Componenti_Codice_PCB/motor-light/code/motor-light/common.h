#include <sys/_stdint.h>
#pragma once
#include <Arduino.h>
#include <FastLED.h>
// Fix di compatibilità per core basati su ESP-IDF v4.x (Bluepad32 board)

#define ENABLE_DEBUG

#define CAN_TX   GPIO_NUM_13  // Connects to CTX
#define CAN_RX   GPIO_NUM_12  // Connects to CRX

#define LOADCELL_DOUT_PIN  GPIO_NUM_7
#define LOADCELL_SCK_PIN   GPIO_NUM_6

#define ENCODER_PIN_A   GPIO_NUM_10
#define ENCODER_PIN_B   GPIO_NUM_9
#define ENCODER_BTN     GPIO_NUM_8

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
#define CAN_ID_BOARD_MOTOR_MANAGER  0x0AB    
#define CAN_OPTIONS_LIDAR              0xB0

#define SERVO_MODE          0x01
#define STERRING_MODE       0x02
#define MOTOR_1             0x01
#define MOTOR_2             0x02
#define MOTOR_ALL           0x03
#define FORWARD_DIRECTION   0x01
#define BACKWARD_DIRECTION  0x02
#define STOP_DIRECTION      0x00


#define LED_PIN      5    
#define TOTAL_LEDS   40   
#define BRIGHTNESS   30   

#define INPUT_PIN_1   6   
#define ZONE1_START   0  
#define ZONE1_LEDS   20   

#define INPUT_PIN_2   7  
#define ZONE2_START  40-  
#define ZONE2_LEDS   20

#define NUM_LEDS 1
#define DATA_PIN 48


typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

typedef struct CAR_DATA_MOTOR{        
    uint8_t motor_1;   
    uint8_t motor_2;      
    uint8_t rampTimeMs;
    uint8_t stop_car;    
};
//CAR_DATA_SYSTEM car_data_system={0,0,0,0,0,0,0,0,0,0,0};
CAR_DATA_MOTOR car_data_motor={0,0,0,0};
CAN_DATA c_data={-1,0,-1,{0}};