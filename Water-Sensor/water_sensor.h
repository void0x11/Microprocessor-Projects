/*
 * water_sensor.h
 *
 *  Created on: Dec 8, 2024
 *      Author: Ahmed Amin
 *
 *  DESCRIPTION:
 *  Driver to initialize ADC1 and read the analog value from the Water Sensor (PA7).
 *  This explains the ADC (Analog-to-Digital Converter) Registers.
 */

@ ADC1 Base Address (The start of ADC1 registers in memory)
.equ ADC1_BASE,        0x40012000

@ ADC Registers Offsets
.equ ADC_SR,           (ADC1_BASE + 0x00) @ Status Register (Checks if conversion is done)
.equ ADC_CR2,          (ADC1_BASE + 0x08) @ Control Register 2 (Starts ADC, Turns it ON)
.equ ADC_SQR3,         (ADC1_BASE + 0x34) @ Regular Sequence Register 3 (Selects Channel)
.equ ADC_DR,           (ADC1_BASE + 0x4C) @ Data Register (Holds the result: 0-4095)

@ Bit Definitions
.equ ADC_ADON,         (1 << 0)   @ Enable ADC Bit
.equ ADC_SWSTART,      (1 << 30)  @ Start Conversion Bit
.equ ADC_EOC,          (1 << 1)   @ End Of Conversion Flag Bit
.equ ADC_CH7,          7          @ The sensor is connected to PA7, which is ADC Channel 7

@ ------------------------------------------------------------------------------
@ Subroutine: SENSOR_INIT
@ Description: Configures PA7 as Analog and Enables ADC1.
@ Steps:
@   1. Enable Clocks:
@      - GPIOA (for the pin itself)
@      - ADC1 (for the peripheral logic)
@   2. Configure PA7 Mode:
@      - MUST be set to "11" (Analog Mode).
@   3. Configure ADC Sequence:
@      - Tell ADC to look at Channel 7 first.
@   4. Turn ON ADC:
@      - Set ADON bit in CR2.
@ ------------------------------------------------------------------------------
SENSOR_INIT:
    push {r0, r1, lr}

    @ 1. Enable GPIOA and ADC1 Clocks
    ldr r0, =RCC_AHB1ENR   @ GPIO is on AHB1 Bus
    ldr r1, [r0]
    orr r1, r1, #GPIOA_EN
    str r1, [r0]

    ldr r0, =RCC_APB2ENR   @ ADC1 is on APB2 Bus
    ldr r1, [r0]
    orr r1, r1, #ADC1_EN
    str r1, [r0]

    @ 2. Configure PA7 as Analog (11)
    ldr r0, =GPIOA_MODER
    ldr r1, [r0]
    orr r1, r1, #(MODE_ANALOG << (SENSOR_PIN * 2)) @ Set mode to 11
    str r1, [r0]

    @ 3. Configure ADC1 Sequence (Select Channel 7)
    ldr r0, =ADC_SQR3      @ Sequence Register
    mov r1, #ADC_CH7       @ Load Channel Number (7)
    str r1, [r0]           @ Determine that the 1st conversion is Ch7

    @ 4. Enable ADC (ADON)
    ldr r0, =ADC_CR2
    ldr r1, [r0]
    orr r1, r1, #ADC_ADON  @ Set ADON bit
    str r1, [r0]

    @ 5. Stabilization Delay (tSTAB ~ 10us)
    @ Run a small loop to waste cycles waiting for ADC to power up
    mov r0, #1000
Init_Delay_Loop:
    subs r0, r0, #1
    bne Init_Delay_Loop

    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: READ_WATER_LEVEL
@ Description: Reads current voltage value from Water Sensor.
@ Steps:
@   1. Start Conversion: Set SWSTART bit.
@   2. Wait Loop: Poll the Status Register (SR) until EOC (End of Conversion) is 1.
@   3. Read Data: Load value from Data Register (DR).
@ Returns: R0 (12-bit ADC value: 0 - 4095)
@ ------------------------------------------------------------------------------
READ_WATER_LEVEL:
    push {r1, r2, lr}      @ Save scratch registers, return address

    @ 1. Start Conversion (SWSTART)
    ldr r2, =ADC_CR2
    ldr r1, [r2]
    orr r1, r1, #ADC_SWSTART
    str r1, [r2]

    @ 2. Wait for End of Conversion (EOC)
    ldr r2, =ADC_SR        @ Address of Status Register
Wait_EOC:
    ldr r1, [r2]           @ Read Status
    tst r1, #ADC_EOC       @ Test Logic AND with EOC bit
    beq Wait_EOC           @ If result is 0 (not done), branch back

    @ 3. Read Data Register
    ldr r2, =ADC_DR
    ldr r0, [r2]           @ Read Result into R0

    pop {r1, r2, pc}       @ Restore registers, return with Result in R0
