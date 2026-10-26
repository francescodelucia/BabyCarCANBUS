#include <sys/_stdint.h>
#pragma once
#include <Arduino.h>
// Fix di compatibilità per core basati su ESP-IDF v4.x (Bluepad32 board)

#define ENABLE_DEBUG

#define CAN_TX   GPIO_NUM_13  // Connects to CTX
#define CAN_RX   GPIO_NUM_12  // Connects to CRX

#define PIN            GPIO_NUM_14
#define NUMPIXELS      1


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
#define CAN_ID_BOARD_PS4            0x101

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

/*
Tasti:[A:0 B:0 X:0 Y:0]     0x01  
D-Pad:[U:0 D:0 L:0 R:0]     0x02
Dorsali:[L1:0 R1:0]         0x03
Grilletti:[L2:   0 R2:   0] 0x04
AxisL:[   4,   4] |         0x05
AxisR:[   4,   4]           0x06
*/

#define PS4_BUTTONS                    0x01
#define PS4_DPADS                      0x02
#define PS4_DORSALI                    0x03
#define PS4_GRILLETTI                  0x04
#define PS4_AXIS_L                     0x05
#define PS4_AXIS_R                     0x06

#define SPEED_DATA          0x0A0        
#define BATTERY_DATA        0x0A1  
#define THROTTLE_DATA       0x0A2
#define ENABLE_CONTROLLER   0x01
#define DISABLE_CONTROLLER  0x00
// Definiamo i parametri
#define RGB_LED GPIO_NUM_14  // Pin GPIO
const int freq = 5000;     // 5 kHz
const int resolution = 8;  // Risoluzione 8 bit (0-255)

SemaphoreHandle_t gamepadMutex;

bool LedEnable = false;

volatile bool isConnectionEnabled = false; // Di default la connessione è ABILITATA
volatile bool requestDisconnect = true;   // Flag per forzare la disconnessione

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

int8_t old_tasti=0;
int8_t old_dpad=0;
int8_t old_dorsali=0;
int32_t old_grilletti=0;
int32_t old_axisL=0;
int32_t old_axisR=0;

// Struttura completa con TUTTI i controlli del Gamepad (senza Gyro/Touchpad)
struct GamepadState {
  // Pulsanti Frontali Destra (A, B, X, Y / Croce, Cerchio, Quadrato, Triangolo)
  bool btnA = false;
  bool btnB = false;
  bool btnX = false;
  bool btnY = false;
  
  // D-Pad / Frecce Direzionali Sinistra
  bool dpadUp = false;
  bool dpadDown = false;
  bool dpadLeft = false;
  bool dpadRight = false;

  // Dorsali Superiori (Bumper)
  bool btnL1 = false;
  bool btnR1 = false;

  // Grilletti Analogici Inferiori (Trigger / Brake & Throttle)
  uint16_t brakeL2 = 0;    // Valore da 0 a 1023
  uint16_t throttleR2 = 0; // Valore da 0 a 1023

  // Pulsanti di Pressione Levette (Thumbsticks)
  bool btnThumbL = false;
  bool btnThumbR = false;

  // Pulsanti di Sistema / Misc (Select, Start, System/Home)
  bool btnMiscSelect = false;
  bool btnMiscStart = false;

  // Levetta Analogica Sinistra
  int16_t axisX = 0;  // Da -511 a 512
  int16_t axisY = 0;  // Da -511 a 512

  // Levetta Analogica Destra
  int16_t axisRX = 0; // Da -511 a 512
  int16_t axisRY = 0; // Da -511 a 512
} sharedState;

CAN_DATA c_data={-1,0,-1,{0}};

/*static uint8_t speed = 0;
static uint16_t battery = 0;
static uint8_t throttle = 0;*/