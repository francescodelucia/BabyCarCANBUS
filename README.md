# 🏎️ BabyCar CANBUS: L'Evoluzione Automotive nei Giocattoli

Cosa succede se unisci l'elettronica di un'auto vera a un progetto STEM di famiglia? Nasce **BabyCar CANBUS**: una macchinina elettrica riprogettata da zero con rete CAN-bus ed ecosistema ESP32, realizzata insieme a un giovane maker di 12 anni.

---

## 📝 Descrizione del Progetto

**BabyCar CANBUS** è un progetto di totale retrofitting elettronico applicato a una comune vettura giocattolo cavalcabile per bambini. L'impianto elettrico di fabbrica (notoriamente rudimentale e basato su scattosi interruttori ON/OFF) è stato completamente rimosso per fare spazio a un'architettura a intelligenza distribuita, basata sul protocollo industriale CAN-bus e sui microcontrollori della famiglia ESP32.

Il progetto ha una forte valenza educativa: **è stato interamente sviluppato e assemblato a quattro mani, coinvolgendo mio figlio di 12 anni** in tutte le fases, dalla saldatura dei componenti alla comprensione delle logiche di programmazione.

Grazie a questa trasformazione, il veicolo giocattolo adotta gli standard di comunicazione delle vere automobili. **BabyCar CANBUS** integra:
* Un acceleratore elettronico a parzializzazione millimetrica.
* Un controllo motori fluido tramite moduli PWM (che elimina i pericolosi scatti in partenza).
* Un sistema di telemetria costante per la salute della batteria.
* Un cruscotto digitale interattivo per il monitoraggio di tutte le funzioni di bordo.

---

## 🛠️ Architettura Tecnica e Hardware

Il cuore di BabyCar CANBUS è la suddivisione dei compiti tra diverse centraline elettroniche (ECU) collegate tra loro su un bus a due fili, esattamente come nei veicoli commerciali moderni:

* **Unità Centrale e Dashboard (Master - ESP32-S3):** Sfrutta la potenza di calcolo e le capacità grafiche dell'ESP32-S3 per gestire la logica di coordinamento dell'intera rete CAN-bus e per pilotare il cruscotto digitale. Mostra in tempo reale velocità, stato della batteria e diagnostica di sistema.
* **Centralina di Potenza e Guida (Slave - ESP32-C3):** Basata sulla snella ed efficiente architettura RISC-V, questa unità acquisisce i dati dell'acceleratore elettronico (potenziometrico) e genera i segnali PWM (Pulse-Width Modulation) per i motori, garantendo accelerazioni e frenate progressive e sicure.
* **Modulo Gestione Energia (Slave - ESP32-C3):** Un nodo dedicato esclusivamente al monitoraggio continuo di tensione, corrente assorbita e temperatura del pacco batteria, per prevenire sovraccarichi o scariche profonde.

---

## ✨ Innovazioni e Punti di Forza (Maker Faire Roma 2026)

* **Il Valore di un Progetto STEM in Famiglia:** Questo prototipo dimostra come concetti complessi di ingegneria elettronica (come il protocollo CAN-bus o la programmazione firmware) possano essere appresi e applicati con entusiasmo anche da un ragazzo di 12 anni, stimolando la prossima generazione di maker.
* **Sicurezza Attiva per i Bambini:** Il controllo PWM impedisce il "colpo di frusta" nelle ripartenze da fermo e permette di programmare via software limiti di velocità differenziati.
* **Cablaggio Semplificato ed Estendibile:** Grazie al protocollo CAN-bus, l'aggiunta di futuri moduli (come sensori di parcheggio a ultrasuoni per la frenata automatica o fari LED intelligenti) richiede il collegamento di soli due fili di dati, azzerando la complessità del cablaggio.
* **Predisposizione IoT:** L'utilizzo della famiglia ESP32 rende la macchinina nativamente pronta per la telemetria wireless su smartphone tramite Wi-Fi o Bluetooth, aprendo la strada a un controllo parentale avanzato da remoto.

---

## ⚙️ Scheda Tecnica in Sintesi

| Componente | Tecnologia Utilizzata | Funzione Principale in BabyCar CANBUS |
| :--- | :--- | :--- |
| **Protocollo di Rete** | CAN-bus (Controller Area Network) | Comunicazione di bordo ed eliminazione interferenze |
| **Core Grafico/Master** | ESP32-S3 Dual-Core | Gestione rete e Cruscotto Digitale |
| **Nodi Periferici** | ESP32-C3 RISC-V Single-Core | Controllo Motori (PWM) e Lettura Sensori |
| **Input Guida** | Acceleratore Elettronico | Erogazione progressiva e lineare della potenza |
| **Sicurezza Batteria** | Telemetria Real-Time | Monitoraggio costante di tensione, corrente e termica |

# 🏎️ BabyCar CANBUS: Bringing Automotive Evolution to Toys

What happens when you combine real-world automotive electronics with a family STEM project? **BabyCar CANBUS** is born: an electric toy car re-engineered from scratch using a CAN-bus network and the ESP32 ecosystem, co-developed alongside a talented 12-year-old maker.

---

## 📝 Project Overview

**BabyCar CANBUS** is a complete electronic retrofitting project applied to a standard ride-on toy car for children. The stock factory electrical system—notoriously rudimentary and reliant on jerky ON/OFF switches—was entirely stripped out to make way for a distributed intelligence architecture based on the industrial CAN-bus protocol and the ESP32 microcontroller family.

This project holds significant educational value: **it was fully co-developed and assembled from the ground up, involving my 12-year-old son** in every single phase, from component soldering to understanding core firmware programming logic.

Thanks to this transformation, the toy vehicle now adopts the communication standards of real modern automobiles. **BabyCar CANBUS** integrates:
* An electronic throttle for millimeter-precise acceleration control.
* Smooth motor control via PWM modules (eliminating dangerous jerks upon starting).
* A continuous telemetry system monitoring battery health.
* An interactive digital dashboard for real-time monitoring of all onboard functionalities.

---

## 🛠️ Technical Architecture & Hardware

The core of BabyCar CANBUS lies in distributing specific tasks across different Electronic Control Units (ECUs) interconnected via a two-wire bus, exactly like modern commercial vehicles:

* **Central Unit & Dashboard (Master - ESP32-S3):** Leverages the high computing power and graphic processing capabilities of the ESP32-S3 to orchestrate the entire CAN-bus network and drive the digital dashboard. It displays speed, battery status, and system diagnostics in real time.
* **Power & Drive Control Unit (Slave - ESP32-C3):** Based on the lean and highly efficient RISC-V architecture, this unit reads data from the electronic potentiometer throttle and generates PWM (Pulse-Width Modulation) signals for the motors, ensuring smooth and safe acceleration and braking.
* **Energy Management Module (Slave - ESP32-C3):** A dedicated node solely focused on the continuous tracking of voltage, current draw, and battery pack temperature to prevent deep discharges or overloading.

---

## ✨ Key Innovations & Features (Maker Faire Rome 2026)

* **The Power of a Family STEM Project:** This prototype proves that complex electronic engineering concepts (such as the CAN-bus protocol or firmware programming) can be learned and applied with enthusiasm even by a 12-year-old, inspiring the next generation of makers.
* **Active Safety for Children:** PWM control prevents any "whiplash" effect when starting from a standstill and allows customizable speed limits to be programmed via software.
* **Simplified & Expandable Wiring:** Thanks to the CAN-bus protocol, adding future modules (such as ultrasonic parking sensors for automatic emergency braking or smart LED headlights) requires connecting just two data wires, completely eliminating wiring clutter.
* **IoT Readiness:** Utilizing the ESP32 family makes the toy car natively ready for wireless telemetry on smartphones via Wi-Fi or Bluetooth, paving the way for advanced remote parental control features.

---

## ⚙️ Technical Specifications Summary

| Component | Technology Used | Main Function in BabyCar CANBUS |
| :--- | :--- | :--- |
| **Network Protocol** | CAN-bus (Controller Area Network) | Onboard communication and noise/interference immunity |
| **Graphics Core/Master** | ESP32-S3 Dual-Core | Network coordination and Digital Dashboard UI |
| **Peripheral Nodes** | ESP32-C3 RISC-V Single-Core | Motor Control (PWM) and Sensor Data Acquisition |
| **Driving Input** | Electronic Throttle | Linear and progressive power delivery |
| **Battery Safety** | Real-Time Telemetry | Constant monitoring of voltage, current, and thermals |
