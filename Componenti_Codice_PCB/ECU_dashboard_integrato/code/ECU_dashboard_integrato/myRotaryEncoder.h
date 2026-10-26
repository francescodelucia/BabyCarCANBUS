#pragma once
#include <Arduino.h>
#include "driver/pulse_cnt.h"

class RotaryEncoderRTOS {
public:
    typedef void (*ButtonCallback)();

    RotaryEncoderRTOS(int pinA, int pinB, int pinBtn)
        : _pinA(pinA), _pinB(pinB), _pinBtn(pinBtn),
          _pcntUnit(nullptr), _pcntChanA(nullptr), _pcntChanB(nullptr),
          _btnPressed(false), _btnPressTime(0), _lastBtnChange(0),
          _longPressThreshold(1000), _longPressTriggered(false),
          _lastBtnStateHandled(false), _onLongPressCb(nullptr) {}

    ~RotaryEncoderRTOS() {
        if (_pcntUnit) {
            pcnt_unit_stop(_pcntUnit);
            pcnt_unit_disable(_pcntUnit);
            pcnt_del_unit(_pcntUnit);
        }
    }

    // Inizializza l'unità PCNT dell'ESP32-S3 e il pin del pulsante
    bool begin() {
        pinMode(_pinBtn, INPUT_PULLUP);

        // 1. Configurazione Unità PCNT
        pcnt_unit_config_t unit_config = {
            .low_limit = -32768,
            .high_limit = 32767,
            .flags = { .accum_count = 1 } // Permette il rollover senza perdere il conteggio
        };
        if (pcnt_new_unit(&unit_config, &_pcntUnit) != ESP_OK) return false;

        // 2. Configurazione Filtro Glitch Anti-Rimbalzo Hardware (in nanosecondi)
        pcnt_glitch_filter_config_t filter_config = {
            .max_glitch_ns = 1000, // Filtra impulsi spurii sotto 1 µs
        };
        pcnt_unit_set_glitch_filter(_pcntUnit, &filter_config);

        // 3. Configurazione Canale A (Pin A = Segnale, Pin B = Controllo direzione)
        pcnt_chan_config_t chan_a_config = {
            .edge_gpio_num = _pinA,
            .level_gpio_num = _pinB,
        };
        if (pcnt_new_channel(_pcntUnit, &chan_a_config, &_pcntChanA) != ESP_OK) return false;

        // 4. Configurazione Canale B (Pin B = Segnale, Pin A = Controllo direzione)
        pcnt_chan_config_t chan_b_config = {
            .edge_gpio_num = _pinB,
            .level_gpio_num = _pinA,
        };
        if (pcnt_new_channel(_pcntUnit, &chan_b_config, &_pcntChanB) != ESP_OK) return false;

        pcnt_channel_set_edge_action(_pcntChanA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
        pcnt_channel_set_level_action(_pcntChanA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

        pcnt_channel_set_edge_action(_pcntChanB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
        pcnt_channel_set_level_action(_pcntChanB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

        // 6. Abilita e avvia il contatore hardware
        pcnt_unit_enable(_pcntUnit);
        pcnt_unit_clear_count(_pcntUnit);
        pcnt_unit_start(_pcntUnit);

        return true;
    }

    // Da chiamare periodicamente nel Task FreeRTOS dedicato
    void update() {
        bool currentState = (digitalRead(_pinBtn) == LOW); // LOW = Premuto
        TickType_t nowTicks = xTaskGetTickCount();
        uint32_t nowMs = pdTICKS_TO_MS(nowTicks);

        // Anti-rimbalzo pulsante via FreeRTOS ticks
        if (currentState && !_btnPressed && (nowMs - _lastBtnChange > 50)) {
            _btnPressed = true;
            _btnPressTime = nowMs;
            _lastBtnChange = nowMs;
            _longPressTriggered = false;
        } 
        else if (!currentState && _btnPressed && (nowMs - _lastBtnChange > 50)) {
            _btnPressed = false;
            _lastBtnChange = nowMs;
        }

        // Controllo Pressione Prolungata
        if (_btnPressed && !_longPressTriggered && (nowMs - _btnPressTime >= _longPressThreshold)) {
            _longPressTriggered = true;
            if (_onLongPressCb != nullptr) {
                _onLongPressCb();
            }
        }
    }

    // Legge la posizione hardware corrente senza interrompere la CPU
    int32_t getPosition(uint8_t stepsPerDetent = 4) const {
        int pulse_count = 0;
        pcnt_unit_get_count(_pcntUnit, &pulse_count);
        return pulse_count / stepsPerDetent;
    }

    // Reset della posizione hardware
    void setPosition(int32_t newPos, uint8_t stepsPerDetent = 4) {
        pcnt_unit_clear_count(_pcntUnit);
        // Se si vuole impostare un offset specifico lo si può gestire via software
    }

    // Rileva il click singolo (rilascio)
    bool isClicked() {
        if (!_btnPressed && _lastBtnStateHandled && !_longPressTriggered) {
            _lastBtnStateHandled = false;
            return true;
        }
        if (_btnPressed) {
            _lastBtnStateHandled = true;
        }
        return false;
    }

    bool isPressed() const {
        return _btnPressed;
    }

    void setLongPressThreshold(uint32_t ms, ButtonCallback callback = nullptr) {
        _longPressThreshold = ms;
        _onLongPressCb = callback;
    }

    bool isLongPressed() const {
        return _longPressTriggered;
    }

private:
    int _pinA, _pinB, _pinBtn;

    pcnt_unit_handle_t _pcntUnit;
    pcnt_channel_handle_t _pcntChanA;
    pcnt_channel_handle_t _pcntChanB;

    bool _btnPressed;
    bool _lastBtnStateHandled;
    uint32_t _btnPressTime;
    uint32_t _lastBtnChange;
    uint32_t _longPressThreshold;
    bool _longPressTriggered;

    ButtonCallback _onLongPressCb;
};