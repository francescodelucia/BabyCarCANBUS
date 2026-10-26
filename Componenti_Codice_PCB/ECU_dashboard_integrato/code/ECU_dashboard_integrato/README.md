# ECU Dashboard - NavigationManager integrato

La navigazione ora e realmente collegata al file .ino.

- Encoder GPIO 2/1 e pulsante GPIO 42 (common.h).
- Click apre menu, entra nel sottomenu, seleziona dashboard o modifica valori.
- Rotazione scorre le voci o modifica il parametro.
- Pressione lunga (1000 ms) torna indietro; dal menu principale ritorna alla dashboard.
- Pulsanti OPTION_L e OPTION_R inviano eventi LEFT/RIGHT.
- La coda FreeRTOS separa acquisizione encoder e disegno: NavigationManager e TFT sono usati solo nel task DisplayRender.
- Le dashboard grafiche originali rimangono in DrawManager. LIDAR, Torque e Luci sono ancora segnaposto.
- Luminosita e CAN kbps sono parametri dimostrativi: non comandano hardware e non vengono salvati in NVS.

## Dipendenze
Arduino ESP32 core compatibile con driver/pulse_cnt.h; Adafruit GFX; Adafruit ILI9341; ESP32_TWAI compatibile con la versione del core installata.

## Avvertenze
Non e stata effettuata compilazione con Arduino CLI o prova su scheda. La ricezione CAN usa ancora il buffer singolo originale e car_data e condiviso tra task senza lock. La lettura dei pulsanti laterali usa la logica active-low originale; verificare i pull-up elettrici. Il display mantiene la configurazione SPI originale, inclusi 80 MHz e reset.
