/*
 * piezzo.h
 *
 *  Created on: Dec 8, 2024
 *      Author: Ahmed Amin
 *
 *  DESCRIPTION:
 *  Driver logic for the Piezo Buzzer (PA5).
 *  Refactored to use subroutines for educational clarity.
 */

@ PIEZO = PA5

@ ------------------------------------------------------------------------------
@ Subroutine: PIEZO_ON
@ Description: Turns on the Piezo Buzzer (PA5).
@ Logic: Sets Bit 5 in GPIOA_ODR.
@ ------------------------------------------------------------------------------
PIEZO_ON:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR     @ Load Address of ODR
    ldr r1, [r0]           @ Read current value
    orr r1, r1, #(1 << PIEZO_PIN) @ Set Bit 5 (High)
    str r1, [r0]           @ Write back
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: PIEZO_OFF
@ Description: Turns off the Piezo Buzzer (PA5).
@ Logic: Clears Bit 5 in GPIOA_ODR.
@ ------------------------------------------------------------------------------
PIEZO_OFF:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    bic r1, r1, #(1 << PIEZO_PIN) @ Clear Bit 5 (Low)
    str r1, [r0]
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: PIEZO_INIT
@ Description: Configures PA5 as Output for the Piezo.
@ Steps:
@   1. Enable GPIOA Clock (RCC_AHB1ENR)
@   2. Configure PA5 Mode:
@      - Clear bits 10,11 (Mode Mask)
@      - Set to "01" (Output Mode)
@ ------------------------------------------------------------------------------
PIEZO_INIT:
    push {r0, r1, lr}

    @ 1. Enable GPIOA Clock (Safe to redo even if Leds.h did it)
    ldr r0, =RCC_AHB1ENR
    ldr r1, [r0]
    orr r1, r1, #GPIOA_EN
    str r1, [r0]

    @ 2. Configure PA5 as Output
    ldr r0, =GPIOA_MODER
    ldr r1, [r0]

    @ Clear PA5 bits (10, 11)
    bic r1, r1, #(3 << (PIEZO_PIN * 2))
    @ Set PA5 to Output (01)
    orr r1, r1, #(MODE_OUTPUT << (PIEZO_PIN * 2))

    str r1, [r0]

    pop {r0, r1, pc}
