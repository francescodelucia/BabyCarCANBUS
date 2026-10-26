#include "common.h"
#include <Bluepad32.h>

const String ALLOWED_MAC_ADDRESS = "3c:5f:f8:6a:90:f4";



ControllerPtr myControllers[BP32_MAX_GAMEPADS];
void onConnectedController(ControllerPtr ctl) {
  if (!isConnectionEnabled) {
    Serial.println("--> ACCESSO NEGATO: Le connessioni Bluetooth sono attualmente DISABILITATE da software.");
    ctl->disconnect();
    return;
  }
  LedEnable = true;
  ControllerProperties properties = ctl->getProperties();
  // Estrazione dell'indirizzo MAC del dispositivo che tenta la connessione
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
           properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
  Serial.printf("Tentativo di connessione da MAC: %s\n", macStr);
  if (String(macStr).equalsIgnoreCase(ALLOWED_MAC_ADDRESS)) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
      if (myControllers[i] == nullptr) {
        Serial.printf("CALLBACK: Controller connesso sul Core %d, indice=%d\n", xPortGetCoreID(), i);
        ControllerProperties properties = ctl->getProperties();
        Serial.printf("Modello: %s, VID=0x%04x, PID=0x%04x\n", 
                      ctl->getModelName().c_str(), 
                      properties.vendor_id, 
                      properties.product_id);
        myControllers[i] = ctl;
        foundEmptySlot = true;
        break;
      }
    }
    if (!foundEmptySlot) {
      Serial.println("CALLBACK: Nessun slot libero per il nuovo controller.");
    }
  }else {
    // SE IL MAC NON CORRISPONDE, RIFIUTA E DISCONNETTI IMMEDIATAMENTE
    Serial.printf("--> ACCESSO NEGATO: Il dispositivo %s non è autorizzato!\n", macStr);
    ctl->disconnect(); 
  }

  
}

void onDisconnectedController(ControllerPtr ctl) {
  //bool foundController = false;
  LedEnable = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnesso dall'indice=%d\n", i);
      myControllers[i] = nullptr;
      //foundController = true;
      break;
    }
  }
  /*if (!foundController) {
    Serial.println("CALLBACK: Controller disconnesso non trovato.");
  }*/
}

void disconnectAllControllers() {
  bool anyDisconnected = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] != nullptr && myControllers[i]->isConnected()) {
      Serial.printf("Disconnessione forzata slot %d...\n", i);
      myControllers[i]->disconnect();
      myControllers[i] = nullptr;
      anyDisconnected = true;
    }
  }
  if (!anyDisconnected) {
    Serial.println("Nessun controller connesso al momento.");
  }
}
// Stampa sul Serial Monitor di TUTTI i controlli attivi
void dumpGamepad(ControllerPtr ctl) {
  //uint8_t dpad = ctl->dpad();
  /*Serial.printf(
    "Idx:%d | Tasti:[A:%d B:%d X:%d Y:%d] | D-Pad:[U:%d D:%d L:%d R:%d] | Dorsali:[L1:%d R1:%d] | Grilletti:[L2:%4d R2:%4d] | AxisL:[%4d,%4d] | AxisR:[%4d,%4d]\n",
    ctl->index(),
    ctl->a(), ctl->b(), ctl->x(), ctl->y(),
    (dpad & DPAD_UP) ? 1 : 0, (dpad & DPAD_DOWN) ? 1 : 0, (dpad & DPAD_LEFT) ? 1 : 0, (dpad & DPAD_RIGHT) ? 1 : 0,
    ctl->l1(), ctl->r1(),
    ctl->brake(), ctl->throttle(),
    ctl->axisX(), ctl->axisY(),
    ctl->axisRX(), ctl->axisRY()
  );*/
}

void processGamepad(ControllerPtr ctl) {
  // Aggiornamento dello stato condiviso (Thread-Safe)
  if (xSemaphoreTake(gamepadMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
    // Tasti Destra
    sharedState.btnA = ctl->a();
    sharedState.btnB = ctl->b();
    sharedState.btnX = ctl->x();
    sharedState.btnY = ctl->y();
    
    // D-Pad / Frecce
    uint8_t dpadVal = ctl->dpad();
    sharedState.dpadUp    = (dpadVal & DPAD_UP) != 0;
    sharedState.dpadDown  = (dpadVal & DPAD_DOWN) != 0;
    sharedState.dpadLeft  = (dpadVal & DPAD_LEFT) != 0;
    sharedState.dpadRight = (dpadVal & DPAD_RIGHT) != 0;

    // Dorsali e Grilletti
    sharedState.btnL1      = ctl->l1();
    sharedState.btnR1      = ctl->r1();
    sharedState.brakeL2    = ctl->brake();    // Grilletto L2
    sharedState.throttleR2 = ctl->throttle(); // Grilletto R2

    // Click Levette e Tasti Speciali
    sharedState.btnThumbL     = ctl->thumbL();
    sharedState.btnThumbR     = ctl->thumbR();
    sharedState.btnMiscSelect = ctl->miscSelect();
    sharedState.btnMiscStart  = ctl->miscStart();

    // Levette Analogiche (Sx e Dx)
    sharedState.axisX  = ctl->axisX();
    sharedState.axisY  = ctl->axisY();
    sharedState.axisRX = ctl->axisRX();
    sharedState.axisRY = ctl->axisRY();
    
    xSemaphoreGive(gamepadMutex);
  }

  // Stampa istantanea di tutti i valori a video
  dumpGamepad(ctl);
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      }
    }
  }
  /*for (auto myController : myControllers) {
    if (myController && myController->isConnected()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      }
    }
  }*/
}

