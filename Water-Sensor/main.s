/*
 * main.s
 *
 *  Created on: Dec 8, 2024
 *      Author: Ahmed Amin
 *
 *  DESCRIPTION:
 *  Main Application Logic - PURE ASSEMBLY IMPLEMENTATION.
 */

@ ------------------------------------------------------------------------------
@ DEFINES & INCLUDES TOP
@ ------------------------------------------------------------------------------
#include "Board_Setup.h"

@ Threshold Values (0-4095)
@ Adjusted to Linear 3-Level Split:
@ - Level 1 (Safe): > 100
@ - Level 2 (Warning): > 2000
@ - Level 3 (Critical): > 3800
.equ THRESHOLD_SAFE,      100
.equ THRESHOLD_WARNING,   2000
.equ THRESHOLD_CRITICAL,  3800

@ ------------------------------------------------------------------------------
@ TEXT SECTION (Code)
@ ------------------------------------------------------------------------------
.section .text
.global main

main:
    @ --------------------------------------------------------------------------
    @ 1. SYSTEM INITIALIZATION
    @ --------------------------------------------------------------------------
    @ We call the init functions from the drivers included at the end.
    bl LEDS_INIT           @ Configure LED Pins as Output
    bl PIEZO_INIT          @ Configure Piezo Pin as Output
    bl SENSOR_INIT         @ Configure Sensor Pin as Analog Input & Enable ADC

    @ --------------------------------------------------------------------------
    @ 2. STARTUP DIAGNOSTIC FLASH (Visual Confirmation)
    @ --------------------------------------------------------------------------
    
    @ Turn ALL LEDs + Piezo ON
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    orr r1, r1, #(1 << LED1_PIN)   @ Green
    orr r1, r1, #(1 << LED2_PIN)   @ Yellow
    orr r1, r1, #(1 << LED3_PIN)   @ Red
    orr r1, r1, #(1 << PIEZO_PIN)  @ Piezo
    str r1, [r0]

    @ Large Delay Loop (Visible 1-2 seconds)
    ldr r2, =0x00F00000    @ Increased delay count
Startup_Wait:
    subs r2, r2, #1
    bne Startup_Wait

    @ Turn ALL OFF
    ldr r1, [r0]
    bic r1, r1, #(1 << LED1_PIN)
    bic r1, r1, #(1 << LED2_PIN)
    bic r1, r1, #(1 << LED3_PIN)
    bic r1, r1, #(1 << PIEZO_PIN)
    str r1, [r0]

loop:
    @ --------------------------------------------------------------------------
    @ 3. READ SENSOR DATA
    @ --------------------------------------------------------------------------
    bl READ_WATER_LEVEL    @ Returns: R0 = Current ADC Value (0-4095)

    @ --------------------------------------------------------------------------
    @ 4. DETERMINE STATE
    @ --------------------------------------------------------------------------

    @ A) Check if CRITICAL (> 3500)
    ldr r1, =THRESHOLD_CRITICAL
    cmp r0, r1
    bgt State_Critical

    @ B) Check if WARNING (> 2500)
    ldr r1, =THRESHOLD_WARNING
    cmp r0, r1
    bgt State_Warning

    @ C) Check if SAFE (> 1000)
    ldr r1, =THRESHOLD_SAFE
    cmp r0, r1
    bgt State_Safe

    @ D) Default: DRY (Level <= 1000)
    b State_Dry

    @ --------------------------------------------------------------------------
    @ 5. PURE ASSEMBLY STATE HANDLERS
    @ Direct Register Modification (Read-Modify-Write) on GPIOA_ODR
    @ --------------------------------------------------------------------------

State_Critical:
    @ TARGET: Red (PA8)=1, Yellow (PA9)=0, Green (PA6)=0, Piezo (PA5)=1
    
    ldr r0, =GPIOA_ODR     @ Load Address
    ldr r1, [r0]           @ Read
    
    @ Clear Others
    bic r1, r1, #(1 << LED1_PIN)
    bic r1, r1, #(1 << LED2_PIN)
    
    @ Set Targets
    orr r1, r1, #(1 << LED3_PIN)
    orr r1, r1, #(1 << PIEZO_PIN)
    
    str r1, [r0]           @ Write
    b loop

State_Warning:
    @ TARGET: Yellow (PA9)=1, Red (PA8)=0, Green (PA6)=0, Piezo (PA5)=0
    
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    
    @ Clear Others
    bic r1, r1, #(1 << LED3_PIN)
    bic r1, r1, #(1 << LED1_PIN)
    bic r1, r1, #(1 << PIEZO_PIN)
    
    @ Set Target
    orr r1, r1, #(1 << LED2_PIN)
    
    str r1, [r0]
    b loop

State_Safe:
    @ TARGET: Green (PA6)=1, Red (PA8)=0, Yellow (PA9)=0, Piezo (PA5)=0
    
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    
    @ Clear Others
    bic r1, r1, #(1 << LED3_PIN)
    bic r1, r1, #(1 << LED2_PIN)
    bic r1, r1, #(1 << PIEZO_PIN)
    
    @ Set Target
    orr r1, r1, #(1 << LED1_PIN)
    
    str r1, [r0]
    b loop

State_Dry:
    @ TARGET: ALL OFF
    
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    
    @ Clear All
    bic r1, r1, #(1 << LED3_PIN)
    bic r1, r1, #(1 << LED2_PIN)
    bic r1, r1, #(1 << LED1_PIN)
    bic r1, r1, #(1 << PIEZO_PIN)
    
    str r1, [r0]
    b loop

@ ------------------------------------------------------------------------------
@ INCLUDE DRIVER SUBROUTINES AT THE END
@ This ensures they are placed nicely in the .text section after main
@ ------------------------------------------------------------------------------
#include "Leds.h"
#include "piezzo.h"
#include "water_sensor.h"

