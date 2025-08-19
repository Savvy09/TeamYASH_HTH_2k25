# TeamYASH_HTH_2k25

# Smart Fan & Light Controller (HTH-06)

## 📌 Problem Statement

Current classroom/room automation systems often lack integrated security and environment control. While IoT-based smart locks are common, the ability to **automatically manage occupancy-based lighting and temperature control** is missing. This results in unnecessary power consumption and inefficient usage of appliances.

## 🎯 Solution Approach

We extend the **Smart Door Lock** project into a **multi-mode automation system** that integrates:

* **IoT-based Smart Door Lock** - secured by keypad input and IoT mobile app.
* **MOSFET-controlled Light & Lock** - switches ON/OFF automatically when the door unlocks/locks, ensuring energy-efficient lighting.
* **Servo-based Door Control** - automatically opens/closes the door upon lock/unlock for hands-free convenience.
* **Temperature-based Smart Fan** - using a temperature sensor, the fan turns ON only when the room is occupied **and** the temperature exceeds a defined threshold.

This combined approach ensures **security, comfort, and energy efficiency** in classrooms and similar environments.

---

## ⚙️ Required Components

* **ESP32** (Wi-Fi enabled microcontroller)
* **4x3 Keypad** (PIN entry for unlocking)
* **PIR Sensor** (occupancy detection)
* **12V Solenoid Lock + MOSFET** (for secure locking mechanism)
* **Servo Motor** (for physical door control)
* **MOSFET + 5V LED Light** (for automated lighting)
* **Temperature Sensor** (for fan control)
* **5V DC Fan + MOSFET** (temperature-controlled ventilation)
* **Blynk IoT App** (remote lock/unlock and monitoring)
* **Power supply & supporting circuitry** (buck converters, resistors, flyback diodes, etc.)

---

## 🏗️ System Structure

1. **Door Locking & Unlocking**

   * Locked by default.
   * Unlock via correct keypad PIN or Blynk app.
   * Servo motor physically opens/closes the door.

2. **Smart Lighting**

   * Light automatically turns ON when the door unlocks.
   * Turns OFF when the door is locked/vacant.

3. **Smart Fan Control**

   * Room must be **occupied** (detected via PIR).
   * Fan turns ON only if **temperature > threshold**.
   * Automatically turns OFF when room is vacant or temperature is low.

4. **IoT Integration**

   * Real-time control & monitoring via Blynk.
   * Override options for lock, light, and fan.

---

## 📂 Documentation

* **Research Basis:** Extended from *“A Multi-Mode Smart Door Lock for Classroom Security: Vacancy Detection and IoT Based Access”*.
* **Modifications:** Added MOSFET-controlled light & fan, servo door control, and temperature-based automation.
* **Use Case:** Energy-efficient classroom security & comfort automation.
