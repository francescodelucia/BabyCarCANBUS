#pragma once
#include <Arduino.h>
#include "driver/pulse_cnt.h"

class RotaryEncoderRTOS {
public:
    RotaryEncoderRTOS(int pinA, int pinB)
        : _pinA(pinA), _pinB(pinB),
          _pcntUnit(nullptr), _pcntChanA(nullptr), _pcntChanB(nullptr) {}

    ~RotaryEncoderRTOS() {
        if (_pcntUnit) {
            pcnt_unit_stop(_pcntUnit);
            pcnt_unit_disable(_pcntUnit);
            pcnt_del_unit(_pcntUnit);
        }
    }

    bool begin() {
        Serial.println("[Encoder] Inizializzazione PCNT...");

        // 1. Configurazione Unità PCNT
        pcnt_unit_config_t unit_config = {
            .low_limit = -32768,
            .high_limit = 32767
        };
        if (pcnt_new_unit(&unit_config, &_pcntUnit) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito pcnt_new_unit");
            return false;
        }

        // 2. Filtro Glitch Anti-Rimbalzo Hardware
        pcnt_glitch_filter_config_t filter_config = {
            .max_glitch_ns = 1000,
        };
        if (pcnt_unit_set_glitch_filter(_pcntUnit, &filter_config) != ESP_OK) {
            Serial.println("[Encoder] AVVISO: Impossibile impostare il filtro glitch (continuo comunque)");
        }

        // 3. Configurazione Canale A
        pcnt_chan_config_t chan_a_config = {
            .edge_gpio_num = _pinA,
            .level_gpio_num = _pinB,
        };
        if (pcnt_new_channel(_pcntUnit, &chan_a_config, &_pcntChanA) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito pcnt_new_channel per Canale A");
            return false;
        }

        // 4. Configurazione Canale B
        pcnt_chan_config_t chan_b_config = {
            .edge_gpio_num = _pinB,
            .level_gpio_num = _pinA,
        };
        if (pcnt_new_channel(_pcntUnit, &chan_b_config, &_pcntChanB) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito pcnt_new_channel per Canale B");
            return false;
        }

        if (pcnt_channel_set_edge_action(_pcntChanA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE) != ESP_OK ||
            pcnt_channel_set_level_action(_pcntChanA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito set azioni Canale A");
            return false;
        }

        if (pcnt_channel_set_edge_action(_pcntChanB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE) != ESP_OK ||
            pcnt_channel_set_level_action(_pcntChanB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito set azioni Canale B");
            return false;
        }

        // 5. Avvio contatore
        if (pcnt_unit_enable(_pcntUnit) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito pcnt_unit_enable");
            return false;
        }
        pcnt_unit_clear_count(_pcntUnit);
        if (pcnt_unit_start(_pcntUnit) != ESP_OK) {
            Serial.println("[Encoder] ERRORE: Fallito pcnt_unit_start");
            return false;
        }

        Serial.println("[Encoder] Inizializzazione completata con successo!");
        return true;
    }

    void update() {}

    int32_t getPosition(uint8_t stepsPerDetent = 4) const {
        if (!_pcntUnit) return 0;
        int pulse_count = 0;
        pcnt_unit_get_count(_pcntUnit, &pulse_count);
        return pulse_count / (stepsPerDetent ? stepsPerDetent : 4);
    }

    void setPosition(int32_t newPos, uint8_t stepsPerDetent = 4) {
        if (_pcntUnit) {
            pcnt_unit_clear_count(_pcntUnit);
        }
    }

private:
    int _pinA, _pinB;
    pcnt_unit_handle_t _pcntUnit;
    pcnt_channel_handle_t _pcntChanA;
    pcnt_channel_handle_t _pcntChanB;
};