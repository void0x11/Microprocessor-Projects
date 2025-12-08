
/*
 * Board_Setup.h
 *
 *  Created on: Dec 8, 2024
 *      Author: Ahmed Amin
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

@ RCC (Reset and Clock Control)
.equ RCC_BASE,         (0x40023800)
.equ RCC_AHB1ENR,      (RCC_BASE + 0x30)
.equ RCC_APB2ENR,      (RCC_BASE + 0x44) @ For ADC1

@ RCC Enable Bits
.equ GPIOA_EN,         (1 << 0)
.equ ADC1_EN,          (1 << 8)

@ GPIO PORT A Registers
.equ GPIOA_BASE,       (0x40020000)
.equ GPIOA_MODER,      (GPIOA_BASE + 0x00)
.equ GPIOA_OTYPER,     (GPIOA_BASE + 0x04)
.equ GPIOA_OSPEEDR,    (GPIOA_BASE + 0x08)
.equ GPIOA_PUPDR,      (GPIOA_BASE + 0x0C)
.equ GPIOA_IDR,        (GPIOA_BASE + 0x10)
.equ GPIOA_ODR,        (GPIOA_BASE + 0x14)
.equ GPIOA_BSRR,       (GPIOA_BASE + 0x18)

@ PIN DEFINITIONS (User Mapping)
@ PA5 -> D13 -> PIEZO
.equ PIEZO_PIN,        5

@ PA6 -> D12 -> LED1 (Safe/Low)
.equ LED1_PIN,         6

@ ------------------------------------------------------------------------------
@ PA7 -> D11 -> WATER SENSOR (Input/ADC)
@ ------------------------------------------------------------------------------
@ WHY ADC (Analog-to-Digital Converter) and NOT PWM?
@
@ 1. The Water Sensor is an ANALOG device.
@    As water touches the sensor traces, the resistance decreases, causing the
@    voltage output to increase smoothly (e.g., 0V, 0.5V, 1.2V, 2.5V...).
@
@ 2. ADC is required to MEASURE voltage.
@    We use the ADC to convert this continuous analog voltage into a digital
@    number (0 to 4095) that our code can understand and compare.
@
@ 3. PWM is for OUTPUT or digital pulse timing.
@    PWM (Pulse Width Modulation) is typically used to *drive* devices (like
@    dimming an LED) or to measure the width of digital pulses. It cannot
@    measure the magnitude of a standing DC voltage like the sensor provides.
@ ------------------------------------------------------------------------------

.equ SENSOR_PIN,       7

@ PA9 -> D8 -> LED2 (Warning)
.equ LED2_PIN,         9

@ PA8 -> D7 -> LED3 (Critical)
.equ LED3_PIN,         8

@ GPIO MODER VALUES (2 bits per pin)
@ 00: Input, 01: Output, 10: Alternate Function, 11: Analog
@ Shift amount is PinNumber * 2

.equ MODE_INPUT,       0
.equ MODE_OUTPUT,      1
.equ MODE_ALT,         2
.equ MODE_ANALOG,      3
