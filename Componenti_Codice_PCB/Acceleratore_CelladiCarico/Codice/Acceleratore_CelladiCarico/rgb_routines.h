#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Adafruit_NeoPixel.h>

class LedController {
  private:
    Adafruit_NeoPixel strip;
    uint16_t numPixels;
    uint8_t pin;
    unsigned long previousMillis;
    bool blinkState;

  public:
    // Costruttore: numero di LED, pin di controllo e tipo di strip
    LedController(uint16_t pixels, uint8_t ledPin, neoPixelType type = NEO_GRB + NEO_KHZ800) 
      : numPixels(pixels), pin(ledPin), strip(pixels, ledPin, type), previousMillis(0), blinkState(false) {}

    // Inizializzazione della strip
    void begin() {
      strip.begin();
      strip.show(); // Inizializza tutti i LED a spento
      strip.setBrightness(50); // Luminosità predefinita (0-255)
    }

    // Imposta la luminosità globale
    void setBrightness(uint8_t brightness) {
      strip.setBrightness(brightness);
    }

    // Accende un LED specifico con un colore RGB
    void setPixelColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
      if (index < numPixels) {
        strip.setPixelColor(index, strip.Color(r, g, b));
        strip.show();
      }
    }

    // Riempie l'intera strip con un colore solido
    void fill(uint8_t r, uint8_t g, uint8_t b) {
      strip.fill(strip.Color(r, g, b));
      strip.show();
    }

    // Spegne tutti i LED
    void clear() {
      strip.clear();
      strip.show();
    }

    // Effetto arcobaleno ciclico (da chiamare periodicamente nel loop)
    void rainbowCycle(uint8_t wait, unsigned long currentMillis) {
      static unsigned long lastUpdate = 0;
      static uint16_t j = 0;

      if (currentMillis - lastUpdate > wait) {
        lastUpdate = currentMillis;
        for (uint16_t i = 0; i < numPixels; i++) {
          strip.setPixelColor(i, wheel(((i * 256 / numPixels) + j) & 255));
        }
        strip.show();
        j = (j + 1) & 255;
      }
    }

  private:
    // Funzione di supporto per l'arcobaleno (genera colori lungo la ruota dei 360°)
    uint32_t wheel(byte WheelPos) {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }
      if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      WheelPos -= 170;
      return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
};

#endif