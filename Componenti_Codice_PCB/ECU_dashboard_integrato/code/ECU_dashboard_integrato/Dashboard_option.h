#ifndef DASHBOARD_BACKGROUND_H
#define DASHBOARD_BACKGROUND_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ============================================================================
// DEFINIZIONE COLORI (RGB565 per ILI9341)
// ============================================================================
#define COLOR_BG          0x0842  // Sfondo scuro principale (Dark Slate)
#define COLOR_HEADER      0x18E3  // Header superiore (Navy Blue scuro)
#define COLOR_CARD_BG     0x1906  // Sfondo schede/card
#define COLOR_CARD_BORDER 0x31AC  // Bordo schede/card
#define COLOR_ACCENT      0x05BF  // Colore di accento/evidenziazione (Cyan/Teal)
#define COLOR_TEXT_MAIN   0xFFFF  // Testo principale (Bianco)
#define COLOR_TEXT_MUTED  0x9CD3  // Testo secondario (Grigio chiaro)
#define COLOR_GRID        0x10A4  // Colore griglia sottile

// Dimensioni Standard TFT ILI9341 (320x240)
#define TFT_WIDTH  320
#define TFT_HEIGHT 240

// ============================================================================
// CLASSE PER LA GESTIONE DELLO SFONDO
// ============================================================================
class DashboardBackground {
  private:
  Adafruit_ILI9341* _tft;
  uint16_t* _frameBuffer; // Opzionale per ESP32-S3 se si usa la PSRAM
  bool _usePSRAM;
  public:
    DashboardBackground(Adafruit_ILI9341* tft) : _tft(tft), _frameBuffer(nullptr), _usePSRAM(false) {}
    ~DashboardBackground() {
      if (_frameBuffer != nullptr) {
        free(_frameBuffer);
      }
    }
    // Inizializza l'allocazione della memoria (utilizza la PSRAM dell'ESP32-S3 se disponibile)
    bool begin(bool enablePSRAM = false) {
        _usePSRAM = enablePSRAM;
        
        if (_usePSRAM) {
            #if defined(BOARD_HAS_PSRAM) || defined(CONFIG_SPIRAM_SUPPORT)
            // Allocazione di un frame buffer completo 320x240 in PSRAM (153.6 KB)
            _frameBuffer = (uint16_t*) ps_malloc(TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t));
            if (_frameBuffer != nullptr) {
                memset(_frameBuffer, 0, TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t));
                return true;
            }
            #endif
            _usePSRAM = false; // Fallback se la PSRAM fallisce o non è presente
        }
        return true;
    }

    // Disegna una scheda/riquadro con stile moderno
    void drawCard(int16_t x, int16_t y, int16_t w, int16_t h, const char* title = nullptr) {
        // Sfondo card
        _tft->fillRect(x, y, w, h, COLOR_CARD_BG);
        
        // Bordo card
        _tft->drawRect(x, y, w, h, COLOR_CARD_BORDER);

        // Intestazione card (se specificata)
        if (title != nullptr) {
            _tft->fillRect(x + 1, y + 1, w - 2, 22, COLOR_HEADER);
            _tft->drawFastHLine(x + 1, y + 23, w - 2, COLOR_CARD_BORDER);
            
            _tft->setTextColor(COLOR_TEXT_MAIN);
            _tft->setTextSize(1);
            _tft->setCursor(x + 8, y + 7);
            _tft->print(title);
        }
    }

    // Disegna l'intera struttura di sfondo della Dashboard
    void drawDashboard(const char* headerTitle = "ESP32-S3 DASHBOARD") {
        // 1. Sfondo generale
        _tft->fillScreen(COLOR_BG);

        // 2. Griglia di sfondo estetica (opzionale)
        for (int16_t x = 0; x < TFT_WIDTH; x += 20) {
            _tft->drawFastVLine(x, 0, TFT_HEIGHT, COLOR_GRID);
        }
        for (int16_t y = 0; y < TFT_HEIGHT; y += 20) {
            _tft->drawFastHLine(0, y, TFT_WIDTH, COLOR_GRID);
        }

        // 3. Header superiore
        _tft->fillRect(0, 0, TFT_WIDTH, 30, COLOR_HEADER);
        _tft->drawFastHLine(0, 30, TFT_WIDTH, COLOR_ACCENT);

        // Titolo Header
        _tft->setTextColor(COLOR_TEXT_MAIN);
        _tft->setTextSize(2);
        _tft->setCursor(10, 7);
        _tft->print(headerTitle);

        // Indicatore di stato/Connessione (Pallino verde di esempio)
        _tft->fillCircle(TFT_WIDTH - 15, 15, 4, 0x07E0); // Verde OK
    }

    // Effettua il Push del Frame Buffer (se allocato in PSRAM)
    void updateScreen() {
        if (_usePSRAM && _frameBuffer != nullptr) {
            _tft->drawRGBBitmap(0, 0, _frameBuffer, TFT_WIDTH, TFT_HEIGHT);
        }
    }

    // Ritorna il puntatore al buffer in PSRAM per manipulazione diretta
    uint16_t* getFrameBuffer() {
        return _frameBuffer;
    }


};

#endif // DASHBOARD_BACKGROUND_H