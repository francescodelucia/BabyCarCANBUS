<p align="center">
  <!-- LOGO DEL PROGETTO (LATO SINISTRO) -->
  <img src="https://raw.githubusercontent.com/francescodelucia/BabyCarCANBUS/main/assets/logo.png" alt="BabyCar CANBUS Logo" width="130" align="left"/>

  <!-- PARTNER & SPONSOR (LATO DESTRO) -->
  <a href="https://makerfairerome.eu">
    <img src="https://raw.githubusercontent.com/francescodelucia/BabyCarCANBUS/main/assets/maker_faire_logo.png" alt="Maker Faire Rome 2026" height="42" align="right"/>
  </a>
  <a href="https://jlcpcb.com">
    <img src="https://raw.githubusercontent.com/francescodelucia/BabyCarCANBUS/main/assets/jlcpcb_logo.png" alt="JLCPCB Logo" height="42" align="right"/>
  </a>
</p>

<!-- TITOLO E TITOLETTI AL CENTRO -->
# &nbsp; BabyCar CANBUS
### &nbsp; Automotive CAN-bus Architecture, ADAS & Torque Vectoring for Toys

<!-- BADGE DINAMICI ACCATTIVANTI -->
<p align="left">
  <a href="#-italiano"><img src="https://img.shields.io/badge/Lingua-Italiano-blue?style=for-the-badge&logo=google-translate" alt="Italiano"></a>
  <a href="#-english"><img src="https://img.shields.io/badge/Language-English-green?style=for-the-badge&logo=google-translate" alt="English"></a>
  <a href="https://github.com/francescodelucia/BabyCarCANBUS"><img src="https://img.shields.io/badge/STEM-Father_%26_Son-ff69b4?style=for-the-badge&logo=heart" alt="STEM Project"></a>
  <a href="https://jlcpcb.com"><img src="https://img.shields.io/badge/Sponsored_by-JLCPCB-0072c6?style=for-the-badge&logo=circuit-board" alt="JLCPCB"></a>
  <a href="https://makerfairerome.eu"><img src="https://img.shields.io/badge/Selected_for-Maker_Faire_Rome_2026-red?style=for-the-badge" alt="Maker Faire Rome"></a>
</p>

<br clear="all">

---

<a name="-italiano"></a>
# 🏎️ BabyCar CANBUS: L'Evoluzione Automotive e ADAS nei Giocattoli

Cosa succede se unisci l'elettronica di un'auto vera a un progetto STEM di famiglia? Nasce **BabyCar CANBUS**: una macchinina elettrica riprogettata da zero con rete CAN-bus, sistemi ADAS avanzati ed ecosistema ESP32, realizzata insieme a un giovane maker di 12 anni.

---

## 📝 Descrizione del Progetto

**BabyCar CANBUS** è un progetto di totale retrofitting elettronico e meccatronico applicato a una comune vettura giocattolo cavalcabile per bambini. L'impianto elettrico di fabbrica (rudimentale e basato su scattosi interruttori ON/OFF) è stato completamente rimosso per fare spazio a un'architettura a intelligenza distribuita, basata sul protocollo industriale CAN-bus e sui microcontrollori della famiglia ESP32.

Il progetto ha una forte valenza educativa: **è stato interamente sviluppato e assemblato a quattro mani, coinvolgendo mio figlio di 12 anni** in tutte le fasi, dalla saldatura dei componenti alla programmazione degli algoritmi di controllo.

Grazie a questa trasformazione, il veicolo adotta gli standard tecnologici delle vere automobili di ultima generazione. **BabyCar CANBUS** integra:
* **Sistemi ADAS e Sicurezza:** Frenata automatica anti-collisione tramite sensore **LiDAR** e sistema di **assistenza alla sterzata**.
* **Dinamica di Guida Avanzata:** Controllo di trazione con **ripartizione dinamica della potenza (Torque Vectoring)** sull'asse posteriore per ottimizzare l'inserimento in curva.
* **Controllo di Potenza Fluido:** Acceleratore elettronico a parzializzazione millimetrica e gestione motori in PWM (zero scatti in partenza).
* **Telemetria e Interfaccia:** Monitoraggio costante della salute della batteria e cruscotto digitale interattivo in tempo reale.

---

## 🛠️ Architettura Tecnica e Hardware

Il cuore di BabyCar CANBUS è la suddivisione dei compiti tra diverse centraline elettroniche (ECU) collegate tra loro su un bus a due fili, esattamente come nei veicoli moderni:

* **Unità Centrale, Dashboard & ADAS (Master - ESP32-S3):** Sfrutta la potenza di calcolo e le capacità grafiche dell'ESP32-S3 per coordinare la rete CAN-bus, gestire il cruscotto digitale e processare i dati del sensore **LiDAR anti-collisione** per la frenata d'emergenza.
* **Centralina Potenza & Torque Vectoring (Slave - ESP32-C3):** Gestisce l'acceleratore elettronico e controlla indipendentemente i motori dell'asse posteriore tramite segnali PWM, calcolando la ripartizione ottimale della coppia in curva e integrando l'**assistenza alla sterzata**.
* **Modulo Gestione Energia (Slave - ESP32-C3):** Un nodo dedicato al monitoraggio continuo di tensione, corrente assorbita e temperatura del pacco batteria per prevenire sovraccarichi o scariche profonde.

---

## ✨ Innovazioni e Punti di Forza (Maker Faire Roma 2026)

* **Il Valore di un Progetto STEM in Famiglia:** Dimostra come concetti complessi di ingegneria automobilistica (dalla robotica LiDAR alla dinamica del veicolo) possano essere compresi e sviluppati con entusiasmo da un ragazzo di 12 anni.
* **Sicurezza Attiva e ADAS:** Il sistema LiDAR interviene direttamente sulla rete CAN-bus arrestando il veicolo in caso di ostacoli improvvisi, affiancato dal controllo PWM per partenze dolci.
* **Dinamica da Supercar (Torque Vectoring):** La ripartizione attiva della potenza sulle ruote posteriori migliora drasticamente la manovrabilità e la sterzata, riducendo il raggio di curvatura.
* **Cablaggio Semplificato ed Estendibile:** L'architettura CAN-bus permette di integrare nuovi sensori o attuatori con soli due cavi di segnale, azzerando la complessità dei cablaggi tradizionali.
* **Predisposizione IoT:** L'ecosistema ESP32 consente il monitoraggio della telemetria in tempo reale e il controllo parentale wireless via Wi-Fi/Bluetooth.

---

## ⚙️ Scheda Tecnica in Sintesi

| Componente | Tecnologia Utilizzata | Funzione Principale in BabyCar CANBUS |
| :--- | :--- | :--- |
| **Protocollo di Rete** | CAN-bus (Controller Area Network) | Comunicazione di bordo ad alta velocità ed immunità ai disturbi |
| **Core Grafico & ADAS** | ESP32-S3 Dual-Core | Coordinamento rete, Cruscotto Digitale e Gestione LiDAR |
| **Sensore Anti-Collisione**| LiDAR | Rilevamento ostacoli e frenata automatica d'emergenza (AEB) |
| **Controllo Dinamica** | ESP32-C3 + Doppio PWM | Torque Vectoring posteriore e Assistenza alla Sterzata |
| **Input Guida** | Acceleratore Elettronico | Erogazione progressiva e parzializzata della potenza |
| **Sicurezza Batteria** | Telemetria Real-Time | Monitoraggio costante di tensione, corrente e temperatura |

---

<br>

<a name="-english"></a>
# 🏎️ BabyCar CANBUS: Bringing Automotive ADAS and Evolution to Toys

What happens when you combine real-world automotive electronics with a family STEM project? **BabyCar CANBUS** is born: an electric toy car re-engineered from scratch using a CAN-bus network, advanced ADAS features, and the ESP32 ecosystem, co-developed alongside a talented 12-year-old maker.

---

## 📝 Project Overview

**BabyCar CANBUS** is a complete electronic and mechatronic retrofitting project applied to a standard ride-on toy car for children. The stock factory electrical system—rudimentary and reliant on jerky ON/OFF switches—was entirely removed to make way for a distributed intelligence architecture based on the industrial CAN-bus protocol and the ESP32 microcontroller family.

This project holds significant educational value: **it was fully co-developed and assembled from the ground up, involving my 12-year-old son** in every phase, from soldering components to programming control algorithms.

Thanks to this transformation, the toy vehicle adopts technology standards found in modern high-end automobiles. **BabyCar CANBUS** integrates:
* **ADAS & Safety Systems:** Automatic emergency braking via a **LiDAR** collision avoidance sensor and **steering assist**.
* **Advanced Vehicle Dynamics:** Active traction control with **rear-axle Torque Vectoring** to optimize cornering and steering response.
* **Smooth Power Delivery:** Millimeter-precise electronic throttle and PWM motor control (zero startup jerks).
* **Telemetry & Interface:** Real-time battery health monitoring and an interactive digital dashboard.

---

## 🛠️ Technical Architecture & Hardware

The core of BabyCar CANBUS lies in distributing specific tasks across different Electronic Control Units (ECUs) interconnected via a two-wire bus, exactly like modern commercial vehicles:

* **Central Unit, Dashboard & ADAS (Master - ESP32-S3):** Leverages the processing power of the ESP32-S3 to orchestrate the CAN-bus network, drive the digital UI, and process **LiDAR** data for emergency collision avoidance.
* **Power & Torque Vectoring ECU (Slave - ESP32-C3):** Reads the electronic throttle and independently drives the rear-axle motors via dual PWM signals, calculating real-time torque distribution during turns and handling **steering assistance**.
* **Energy Management Module (Slave - ESP32-C3):** Dedicated node for real-time tracking of voltage, current draw, and battery pack temperature.

---

## ✨ Key Innovations & Features (Maker Faire Rome 2026)

* **The Power of a Family STEM Project:** Demonstrates how complex automotive engineering concepts (from LiDAR robotics to vehicle dynamics) can be enthusiastically learned and implemented by a 12-year-old.
* **Active Safety & ADAS:** The LiDAR system interrupts drive commands via the CAN-bus if obstacles are detected, working alongside PWM soft-starts.
* **Supercar Dynamics (Torque Vectoring):** Active rear-wheel torque allocation dramatically improves turning agility and reduces the steering radius.
* **Simplified & Expandable Wiring:** The CAN-bus architecture allows seamlessly adding new sensors using just two data wires.
* **IoT Readiness:** The ESP32 ecosystem enables real-time telemetry streaming and wireless parental controls via Wi-Fi/Bluetooth.

---

## ⚙️ Technical Specifications Summary

| Component | Technology Used | Main Function in BabyCar CANBUS |
| :--- | :--- | :--- |
| **Network Protocol** | CAN-bus (Controller Area Network) | High-speed onboard communication and noise immunity |
| **Graphics Core & ADAS**| ESP32-S3 Dual-Core | Network coordination, Digital Dashboard UI & LiDAR processing |
| **Anti-Collision Sensor**| LiDAR | Obstacle detection and Automatic Emergency Braking (AEB) |
| **Dynamics Controller** | ESP32-C3 + Dual PWM | Rear Torque Vectoring and Steering Assist |
| **Driving Input** | Electronic Throttle | Linear and progressive power delivery |
| **Battery Safety** | Real-Time Telemetry | Constant tracking of voltage, current, and thermals |
