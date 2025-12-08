# Water Level Monitor - ARM Assembly Project

## 1. Project Overview
This project simulates a **Water Level Monitor** system using an STM32 microcontroller. 
It reads an analog signal from a Water Sensor and provides real-time feedback using three LEDs (Green, Yellow, Red) and a Piezo Buzzer.

**Educational Goal**: To understand "Bare Metal" programming, Register Manipulation (Read-Modify-Write), and checking Analog-to-Digital (ADC) values using Assembly Language.

---

## 2. Hardware Setup (Pin Mapping)

**IMPORTANT**: Ensure your wires are connected exactly as follows.
(See `Board_Setup.h` for code definitions)

| Component           | STM32 Pin | Arduino Header | Type   | Description              |
|---------------------|-----------|----------------|--------|--------------------------|
| **Water Sensor**    | **PA7**   | D11            | Input  | Analog Voltage (ADC1 Ch7)|
| **Green LED**       | **PA6**   | D12            | Output | Safe Level Indicator     |
| **Yellow LED**      | **PA9**   | D8             | Output | Warning Level Indicator  |
| **Red LED**         | **PA8**   | D7             | Output | Critical Level Indicator |
| **Piezo Buzzer**    | **PA5**   | D13            | Output | Audio Alarm (Critical)   |

---

## 3. How It Works

### The Logic (See `main.s`)
1.  **System Initialization**:
    - We verify the board is working by **Flashing ALL LEDs ON** for a second at startup.
    - We enable the Hardware Clocks (RCC) and configure Pins as Input/Output/Analog.
    
2.  **The Infinite Loop**:
    - **Step 1: Read Sensor**: We tell the ADC (Analog-to-Digital Converter) to measure the voltage on PA7. It gives us a number between `0` (Dry/0V) and `4095` (Full/3.3V).
    - **Step 2: Check Thresholds**: We compare this number against our calibrated levels (e.g., 500, 2000, 3800).
    - **Step 3: Update State**: Based on the level, we jump to a specific state (Safe, Warning, Critical) to turn on the correct LED using **Pure Assembly**.

### Why ADC and not PWM?
- **ADC (Analog-to-Digital Converter)** measures the *magnitude* of voltage. The water sensor outputs a higher voltage the deeper it is submerged. We need ADC to read "How much water?"
- **PWM (Pulse Width Modulation)** is for digital signals (duty cycles). It cannot measure a static DC voltage level like a water sensor produces.

---

## 4. Code Structure

- **`main.s`**: The Brain. Contains the main loop, threshold logic, and the direct assembly instructions (`LDR`, `ORR`, `STR`) to control the pins.
- **`Board_Setup.h`**: The Map. Defines which Pin Number goes to which LED.
- **`water_sensor.h`**: The Driver. Handles the complex ADC registers (`CR2`, `SQR3`, `DR`) to get a reading.
- **`Leds.h` & `piezzo.h`**: Helper files containing initialization routines.

---

## 5. Key Assembly Concepts Used

### Read-Modify-Write (RMW)
To change *just one pin* without messing up the others, we follow this pattern:
1.  **READ**: `LDR r1, [r0]` -> Load the current state of the whole port.
2.  **MODIFY**: 
    - `BIC r1, r1, #(1 << PIN)` -> Clear specific bits (Turn OFF).
    - `ORR r1, r1, #(1 << PIN)` -> Set specific bits (Turn ON).
3.  **WRITE**: `STR r1, [r0]` -> Store the modified value back.

### Branching
- `CMP r0, r1`: Compare sensor value (r0) with threshold (r1).
- `BGT Label`: Branch (Jump) if Greater Than.

---

## 6. How to Run & Verify

1.  **Build**: Run `make -j32 all`.
2.  **Debug/Run**: Flash the code to the board.
3.  **Startup Test**: Watch the board immediately after reset.
    - **PASS**: All 3 LEDs and Buzzer turn ON for ~1 second, then OFF.
    - **FAIL**: If they don't flash, check your wiring!
4.  **Water Test**: 
    - Dip sensor slightly -> **Green LED**
    - Dip deeper -> **Yellow LED**
    - Full Submersion -> **Red LED + Buzzer**
