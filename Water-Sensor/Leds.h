/*
 * Leds.h
 *
 *  Created on: Dec 8, 2024
 *      Author: Ahmed Amin
 *
 *  DESCRIPTION:
 *  This file contains the driver logic for controlling the 3 LEDs on the board.
 *  It replaces simple Macros with proper Assembly Subroutines so students
 *  can see exactly how "Read-Modify-Write" operations work on registers.
 */

@ ------------------------------------------------------------------------------
@ LED1 (Green/Safe)     = PA6
@ LED2 (Yellow/Warning) = PA9
@ LED3 (Red/Critical)   = PA8
@ ------------------------------------------------------------------------------


@ ------------------------------------------------------------------------------
@ Subroutine: LED_GREEN_ON
@ Description: Turns on LED1 (PA6) by setting the corresponding bit in ODR.
@ Logic:
@   1. Load Address of ODR (Output Data Register)
@   2. Load current value from that address
@   3. ORR (Set) the bit for PA6
@   4. Store the modified value back to ODR
@ ------------------------------------------------------------------------------
LED_GREEN_ON:
    push {r0, r1, lr}      @ Save registers used
    ldr r0, =GPIOA_ODR     @ r0 = Address of GPIOA Output Data Register
    ldr r1, [r0]           @ r1 = Current Value of ODR
    orr r1, r1, #(1 << LED1_PIN) @ Set Bit 6 (PA6) to 1 (High)
    str r1, [r0]           @ Write back to ODR
    pop {r0, r1, pc}       @ Restore registers and return

@ ------------------------------------------------------------------------------
@ Subroutine: LED_GREEN_OFF
@ Description: Turns off LED1 (PA6) by clearing the bit in ODR.
@ Logic:
@   1. Load Address of ODR
@   2. Load current value
@   3. BIC (Bit Clear) the bit for PA6
@   4. Store back
@ ------------------------------------------------------------------------------
LED_GREEN_OFF:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    bic r1, r1, #(1 << LED1_PIN) @ Clear Bit 6 (PA6) to 0 (Low)
    str r1, [r0]
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: LED_YELLOW_ON
@ Description: Turns on LED2 (PA9)
@ ------------------------------------------------------------------------------
LED_YELLOW_ON:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    orr r1, r1, #(1 << LED2_PIN) @ Set Bit 9 (PA9)
    str r1, [r0]
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: LED_YELLOW_OFF
@ Description: Turns off LED2 (PA9)
@ ------------------------------------------------------------------------------
LED_YELLOW_OFF:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    bic r1, r1, #(1 << LED2_PIN) @ Clear Bit 9 (PA9)
    str r1, [r0]
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: LED_RED_ON
@ Description: Turns on LED3 (PA8)
@ ------------------------------------------------------------------------------
LED_RED_ON:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    orr r1, r1, #(1 << LED3_PIN) @ Set Bit 8 (PA8)
    str r1, [r0]
    pop {r0, r1, pc}

@ ------------------------------------------------------------------------------
@ Subroutine: LED_RED_OFF
@ Description: Turns off LED3 (PA8)
@ ------------------------------------------------------------------------------
LED_RED_OFF:
    push {r0, r1, lr}
    ldr r0, =GPIOA_ODR
    ldr r1, [r0]
    bic r1, r1, #(1 << LED3_PIN) @ Clear Bit 8 (PA8)
    str r1, [r0]
    pop {r0, r1, pc}


@ ------------------------------------------------------------------------------
@ Subroutine: LEDS_INIT
@ Description: Configures PA6, PA9, PA8 as Outputs
@ Steps:
@   1. Enable GPIOA Clock (RCC_AHB1ENR)
@   2. Configure Mode Register (MODER) for each pin:
@      - Clear 2 bits (safety first!)
@      - Set to "01" (Output Mode)
@ ------------------------------------------------------------------------------
LEDS_INIT:
    push {r0, r1, lr}

    @ 1. Enable GPIOA Clock
    ldr r0, =RCC_AHB1ENR   @ Address of Clock Control Register
    ldr r1, [r0]           @ Read current value
    orr r1, r1, #GPIOA_EN  @ Turn on GPIOA Enable Bit
    str r1, [r0]           @ Write back

    @ 2. Configure Pin Modes
    ldr r0, =GPIOA_MODER   @ Address of Mode Register
    ldr r1, [r0]           @ Read current modes

    @ -- PA6 (LED1) --
    bic r1, r1, #(3 << (LED1_PIN * 2))     @ Clear bits 12,13
    orr r1, r1, #(MODE_OUTPUT << (LED1_PIN * 2)) @ Set to 01 (Output)

    @ -- PA9 (LED2) --
    bic r1, r1, #(3 << (LED2_PIN * 2))     @ Clear bits 18,19
    orr r1, r1, #(MODE_OUTPUT << (LED2_PIN * 2)) @ Set to 01 (Output)

    @ -- PA8 (LED3) --
    bic r1, r1, #(3 << (LED3_PIN * 2))     @ Clear bits 16,17
    orr r1, r1, #(MODE_OUTPUT << (LED3_PIN * 2)) @ Set to 01 (Output)

    str r1, [r0]           @ Write ALL changes to MODER at once

    pop {r0, r1, pc}
