#ifndef TONE_MANAGER_H
#define TONE_MANAGER_H
#include <Arduino.h>
class ToneManager {
private:
    uint8_t  _pin;
    uint8_t  _channel;
    uint16_t _resolutionBits;
    bool     _isPlaying;

public:
    /**
     * @param pin Pin GPIO collegato al Gate del MOSFET.
     * @param channel Canale LEDC da usare (usato solo su ESP32 Core v2.x, su v3.x è automatico).
     * @param resolutionBits Risoluzione PWM (default: 8 bit, valori 0-255).
     */
    ToneManager(uint8_t pin, uint8_t channel = 0, uint16_t resolutionBits = 8) 
        : _pin(pin), _channel(channel), _resolutionBits(resolutionBits), _isPlaying(false) {}

    /**
     * Inizializza il pin di uscita.
     */
    void begin() {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
        
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        // ESP32 Arduino Core v3.x: non serve ledcSetup/ledcAttachPin
        // Il canale viene assegnato automaticamente dal core
#else
        // ESP32 Arduino Core v2.x (Retrocompatibilità)
        ledcSetup(_channel, 2000, _resolutionBits);
        ledcAttachPin(_pin, _channel);
#endif

        noTone();
    }

    /**
     * Genera un tono alla frequenza specificata (in Hz).
     * @param frequency Frequenza in Hz (es. 440 per la nota LA4).
     * @param dutyCycle Duty cycle (default 128 = 50% di un'onda quadra a 8 bit).
     */
    void tone(uint32_t frequency, uint32_t dutyCycle = 128) {
        if (frequency == 0) {
            noTone();
            return;
        }

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        // API Core v3.x
        ledcAttach(_pin, frequency, _resolutionBits);
        ledcWriteTone(_pin, frequency);
        ledcWrite(_pin, dutyCycle);
#else
        // API Core v2.x
        ledcWriteTone(_channel, frequency);
        ledcWrite(_channel, dutyCycle);
#endif

        _isPlaying = true;
    }

    /**
     * Genera un tono per una durata specifica in millisecondi (bloccante).
     */
    void toneFor(uint32_t frequency, uint32_t durationMs) {
        tone(frequency);
        vTaskDelay(portTICK_PERIOD_MS*durationMs);
        noTone();
    }

    /**
     * Spegne il segnale PWM e forza il pin a LOW per spegnere il MOSFET.
     */
    void noTone() {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcDetach(_pin);
#else
        ledcWrite(_channel, 0);
#endif
        digitalWrite(_pin, LOW);
        _isPlaying = false;
    }

    /**
     * Genera un bip rapido di cortesia.
     */
    void beep(uint32_t frequency = 2700, uint32_t durationMs = 100) {
        toneFor(frequency, durationMs);
    }

    /**
     * Verifica se il buzzer sta riproducendo un suono.
     */
    bool isPlaying() const {
        return _isPlaying;
    }
};

#endif