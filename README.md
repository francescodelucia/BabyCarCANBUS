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
