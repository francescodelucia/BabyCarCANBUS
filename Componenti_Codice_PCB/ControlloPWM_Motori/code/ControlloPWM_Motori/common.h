#include <sys/_stdint.h>
#pragma once
#include <Arduino.h>
// Fix di compatibilità per core basati su ESP-IDF v4.x (Bluepad32 board)

#define ENABLE_DEBUG

#define CAN_TX   GPIO_NUM_13  // Connects to CTX
#define CAN_RX   GPIO_NUM_12  // Connects to CRX

#define MOTOR1_IN1   GPIO_NUM_1  // Connects to CRX
#define MOTOR1_IN2   GPIO_NUM_2  // Connects to CRX

#define MOTOR2_IN1   GPIO_NUM_3  // Connects to CRX
#define MOTOR2_IN2   GPIO_NUM_4  // Connects to CRX

#define CAN_ID_ECU_MANAGER          0x0AA
#define CAN_ID_BOARD_THROTTLE       0x0A1 
#define CAN_ID_BOARD_BATTERY        0x0A2
#define CAN_ID_DEBUG_GEAR           0x0A3
#define CAN_ID_BOARD_GENERAL        0x0A4 
#define CAN_ID_BOARD_REMOTE_CTRL    0x0A5
#define CAN_ID_BOARD_LUCI           0x0A5
#define CAN_ID_BOARD_STEERING       0x0A6
#define CAN_ID_BOARD_MOTOR_MANAGER  0x0AB
#define CAN_ID_BOARD_LIDAR          0x0B0

#define ENABLE_CONTROLLER   0x01
#define DISABLE_CONTROLLER  0x00

#define SERVO_POWER       100
#define STERRING_POWER    150

#define SERVO_MODE          0x01
#define STERRING_MODE       0x02
#define MOTOR_1             0x01
#define MOTOR_2             0x02
#define MOTOR_ALL           0x03
#define FORWARD_DIRECTION   0x01
#define BACKWARD_DIRECTION  0x02
#define STOP_DIRECTION      0x00

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

// --- STRUTTURA PER I COMANDI MOTORE ---
enum Direction {
  DIR_STOP = 0,
  DIR_FORWARD,
  DIR_BACKWARD
};

struct MotorCommand {
  Direction dir;        // Direzione del movimento
  uint8_t targetSpeed;  // Velocità target (0 - 255)
  uint16_t rampTimeMs;  // Tempo totale per la rampa
};




CAN_DATA c_data={-1,0,-1,{0}};
