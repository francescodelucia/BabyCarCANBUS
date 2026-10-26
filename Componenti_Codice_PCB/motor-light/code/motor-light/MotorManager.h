#pragma once
#include "common.h"

class MotorManager {
  private:
    int PIN_IN1;
    int PIN_IN2;
    const int PWM_FREQ = 5000;
    const int PWM_RES  = 8;

    // STATO PERSISTENTE DEL MOTORE
    int currentSpeed = 0;
    Direction currentDirection = DIR_STOP;

    void init() {
      ledcAttach(this->PIN_IN1, this->PWM_FREQ, this->PWM_RES);
      ledcAttach(this->PIN_IN2, this->PWM_FREQ, this->PWM_RES);
    }

    void setMotorOutputSafe(int pinActive, int pinInactive, int duty) {
      ledcWrite(pinInactive, 0);
      ledcWrite(pinActive, duty);
    }

    void stopMotorSafe() {
      ledcWrite(this->PIN_IN1, 0);
      ledcWrite(this->PIN_IN2, 0);
      this->currentSpeed = 0;
      this->currentDirection = DIR_STOP;
    }

  public:
    MotorManager(int PIN_IN1, int PIN_IN2) {
      this->PIN_IN1 = PIN_IN1;
      this->PIN_IN2 = PIN_IN2;
      this->init();
      this->stopMotorSafe();
    }

    void directDrive(MotorCommand *currentCmd) {
      // 1. GESTIONE STOP
      if (currentCmd->dir == DIR_STOP) {
        // Decelerazione morbida verso lo STOP se il motore stava girando
        if (this->currentSpeed > 0 && currentCmd->rampTimeMs > 0) {
          uint32_t delayPerStep = currentCmd->rampTimeMs / this->currentSpeed;
          int activePin   = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN1 : this->PIN_IN2;
          int inactivePin = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN2 : this->PIN_IN1;

          for (int spd = this->currentSpeed; spd >= 0; spd--) {
            setMotorOutputSafe(activePin, inactivePin, spd);
            if (delayPerStep > 0) vTaskDelay(pdMS_TO_TICKS(delayPerStep));
          }
        }
        stopMotorSafe();
        return;
      }

      // 2. CAMBIO DI MARCIA (INVERSIONE MORBIDA)
      // Se il motore sta andando in una direzione e gli chiediamo di andare nell'altra:
      if (this->currentDirection != DIR_STOP && this->currentDirection != currentCmd->dir) {
        
        uint32_t rampMs = (currentCmd->rampTimeMs > 0) ? currentCmd->rampTimeMs : 500; // Tempo rampa di default
        
        // FASE A: Decelera fino a 0 nella direzione attuale
        if (this->currentSpeed > 0) {
          uint32_t delayDown = (rampMs / 2) / this->currentSpeed; // Metà tempo per scendere
          int activePin   = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN1 : this->PIN_IN2;
          int inactivePin = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN2 : this->PIN_IN1;

          for (int spd = this->currentSpeed; spd >= 0; spd--) {
            setMotorOutputSafe(activePin, inactivePin, spd);
            if (delayDown > 0) vTaskDelay(pdMS_TO_TICKS(delayDown));
          }
        }

        stopMotorSafe(); // Breve azzeramento hardware
        vTaskDelay(pdMS_TO_TICKS(100)); // Pausa di sicurezza a 0V per salvare il driver TA6586
      }

      // Imposta la nuova direzione
      this->currentDirection = currentCmd->dir;

      // 3. ACCELERAZIONE VERSO LA NUOVA VELOCITÀ
      int speedDelta = currentCmd->targetSpeed - this->currentSpeed;
      int steps = abs(speedDelta);

      if (steps > 0) {
        // Se si tratta di un'inversione, usiamo metà del rampTimeMs per la salita
        uint32_t totalRampTime = (currentCmd->rampTimeMs > 0) ? currentCmd->rampTimeMs : 500;
        uint32_t delayPerStep = totalRampTime / steps;
        
        int stepIncrement = (speedDelta > 0) ? 1 : -1;
        int activePin   = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN1 : this->PIN_IN2;
        int inactivePin = (this->currentDirection == DIR_FORWARD) ? this->PIN_IN2 : this->PIN_IN1;

        for (int i = 0; i < steps; i++) {
          this->currentSpeed += stepIncrement;
          setMotorOutputSafe(activePin, inactivePin, this->currentSpeed);
          if (delayPerStep > 0) {
            vTaskDelay(pdMS_TO_TICKS(delayPerStep));
          }
        }
      }
    }
};