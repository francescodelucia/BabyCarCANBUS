#pragma once
//#define ENABLE_DEBUG

//#define CAR_BODY


#define CAN_TX   GPIO_NUM_8  // Connects to CTX
#define CAN_RX   GPIO_NUM_3  // Connects to CRX

#ifdef CAR_BODY
    #define ENCODER_PIN_A   GPIO_NUM_2
    #define ENCODER_PIN_B   GPIO_NUM_1
    #define ENC_BUT         GPIO_NUM_42
    #define OPTION_R    GPIO_NUM_41
    #define OPTION_L    GPIO_NUM_40
    #define GEAR_DOWN   GPIO_NUM_39
    #define GEAR_UP     GPIO_NUM_18
#else
    #define ENCODER_PIN_A   GPIO_NUM_2
    #define ENCODER_PIN_B   GPIO_NUM_41
    #define ENC_BUT         GPIO_NUM_39
    #define GEAR_UP    GPIO_NUM_40
    #define GEAR_DOWN   GPIO_NUM_42
#endif

#define AUDIO_BEEP  GPIO_NUM_35

#define TONE_BUTTON 2000
#define TONE_LONG   200


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
#define CAN_OPTIONS_GEAR_DATA          0xB1    
#define CAN_OPTIONS_STOP_CAR           0xA9    
#define CAN_OPTIONS_LIDAR              0xB0

#define PS4_BUTTONS                    0x01
#define PS4_DPADS                      0x02
#define PS4_DORSALI                    0x03
#define PS4_GRILLETTI                  0x04
#define PS4_AXIS_L                     0x05
#define PS4_AXIS_R                     0x06
/*
Tasti:[A:0 B:0 X:0 Y:0] 0x01  
D-Pad:[U:0 D:0 L:0 R:0] 0x02
Dorsali:[L1:0 R1:0] 0x03
Grilletti:[L2:   0 R2:   0] 0x4
AxisL:[   4,   4] | 0x05
AxisR:[   4,   4] 0x06
*/

#define SERVO_MODE          0x01
#define STERRING_MODE       0x02
#define MOTOR_1             0x01
#define MOTOR_2             0x02
#define MOTOR_ALL           0x03
#define FORWARD_DIRECTION   0x01
#define BACKWARD_DIRECTION  0x02
#define STOP_DIRECTION      0x00

#define DASH_COMMAND       0x0F
#define DASH_MAIN          0x01

int16_t status_dash=DASH_MAIN;

TaskHandle_t xHandleButtons = NULL;
BaseType_t xReturnedButtons;
TaskHandle_t xHandleDisplay = NULL;
BaseType_t xReturnedDisplay;

typedef struct CAN_DATA{
    int8_t data_avalible;
    uint32_t can_id;
    int16_t can_len;    
    uint8_t can_data[16];          
};

typedef struct DASH_DATA{
    int8_t current_dash;
    int8_t current_option;
};

typedef struct CAR_DATA_ENGINE{
    int8_t gear;
    uint16_t battery;
    uint8_t throttle;
    int8_t steering_position;    
    int lidar_distance;
    int torque_val;    
    int lidar_action;       
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
    uint8_t motor_1;     
    uint8_t motor_2;    
    uint8_t gear;    
};

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
} ps4_controller;

DASH_DATA dash_data={DASH_MAIN,1};
CAR_DATA_SYSTEM car_data_system={0,255,255,255,0,255,255,255,0,0,0,0,0,0};
CAR_DATA_ENGINE car_data_engine={0,0,0,0,0,0,0};
CAN_DATA c_data={-1,0,-1,{0}};