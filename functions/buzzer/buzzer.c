/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       buzzer.c
 *
 *  @brief      PWM-based buzzer interface.
 *  ============================================================================
 */

/* -----------------------------------------------------------------------------
*  Includes
* ------------------------------------------------------------------------------
*/

#include <functions/buzzer/buzzer.h>

// TI RTOS drivers
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// Temporary PWM solution directly on DriverLib
// (until a Timer RTOS driver is in place)
#include <ti/drivers/pin/PINCC26XX.h>
#include <driverlib/timer.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "Board.h"


/* -----------------------------------------------------------------------------
*  Local variables
* ------------------------------------------------------------------------------
*/
static PIN_Handle buzzerHandle;
static PIN_State buzzerState;
static PIN_Config buzzerConfig[] = {
        Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
                | PIN_DRVSTR_MAX,
        PIN_TERMINATE };

/* -----------------------------------------------------------------------------
*  Public Functions
* ------------------------------------------------------------------------------
*/

/*******************************************************************************
 * @fn          buzzerOpen
 *
 * @brief       Initialize the Buzzer
 *
 * @descr       Initializes pin and PWM
 *
 * @return      -
 */
void buzzerOpen()
{
    // Turn on PERIPH power domain and clock for GPT0 and GPIO
    Power_setDependency(PowerCC26XX_PERIPH_GPT0);
    Power_setConstraint(PowerCC26XX_SB_DISALLOW);

    // Assign GPT0
    TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);

    // Configure pin for PWM output
    PINCC26XX_setMux(buzzerHandle, Board_BUZZER, IOC_PORT_MCU_PORT_EVENT0);
}


/*******************************************************************************
 * @fn          buzzerSetFrequency
 *
 * @brief       Set the frequency (3Hz - 8 KHz)
 *
 * @return      return true if the requency is within range
 */
bool buzzerSetFrequency(uint16_t freq)
{
    uint32_t ticks;
    uint32_t loadLow;
    uint32_t loadHigh;
    uint32_t matchLow;
    uint32_t matchHigh;

    if (freq < BUZZER_FREQ_MIN && freq > BUZZER_FREQ_MAX)
    {
        return false;
    }

    // Stop timer during reconfiguration
    TimerDisable(GPT0_BASE, TIMER_A);

    // Calculate timer load and match values
    ticks = 48000000 / freq;
    loadLow = ticks & 0x0000FFFF;
    loadHigh = (ticks & 0x00FF0000) >> 16;
    matchLow = (ticks / 2) & 0x0000FFFF;
    matchHigh = ((ticks / 2) & 0x00FF0000) >> 16;

    // Set timer load
    TimerLoadSet(GPT0_BASE, TIMER_A, loadLow);
    TimerPrescaleSet(GPT0_BASE, TIMER_A, loadHigh);

    // Set timer match
    TimerMatchSet(GPT0_BASE, TIMER_BOTH, matchLow);
    TimerPrescaleMatchSet(GPT0_BASE, TIMER_A, matchHigh);

    // Start timer
    TimerEnable(GPT0_BASE, TIMER_A);

    return true;
}

/*******************************************************************************
 * @fn          buzzerClose
 *
 * @brief       Closes the buzzer interface
 *
 * @return      -
 */
void buzzerClose(void)
{
    // Configure pin as GPIO
    PINCC26XX_setMux(buzzerHandle, Board_BUZZER, IOC_PORT_GPIO);

    // Turn off PERIPH power domain and clock for GPT0
    Power_releaseDependency(PowerCC26XX_PERIPH_GPT0);
    Power_releaseConstraint(PowerCC26XX_SB_DISALLOW);
}

static void buzzerTask(UArg arg0, UArg arg1) {
    while (1) {
        Task_sleep(100000 / Clock_tickPeriod);
    };
}

/*
 * Gracefully borrowed from https://github.com/robsoncouto/arduino-songs/
 * and adapted to SensorTag
 */
void playSong(Song *song) {
    int wholenote = (60000 * 4) / song->tempo;
    int divider = 0, noteDuration = 0;

    int thisNote = 0;
    do {
      // calculates the duration of each note
      divider = song->melody[thisNote + 1];
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }

      buzzerOpen();
      buzzerSetFrequency(song->melody[thisNote]);
      Task_sleep(noteDuration * 1000 / Clock_tickPeriod);
      buzzerClose();

      thisNote += 2;
    } while (song->melody[thisNote] != -1);
}

static void registerTask() {
    Task_Params buzzerTaskParams;
    Char buzzerTaskStack[1024];

    Task_Params_init(&buzzerTaskParams);
    buzzerTaskParams.stackSize = 1024;
    buzzerTaskParams.stack = &buzzerTaskStack;
    buzzerTaskParams.priority = 2;

    Task_Handle buzzerTaskHandle = Task_create(buzzerTask, &buzzerTaskParams, NULL);
    if (buzzerTaskHandle == NULL) {
        System_abort("Buzzer task create failed!");
    }
}

void Buzzer_register() {
    buzzerHandle = PIN_open(&buzzerState, buzzerConfig);
    if (buzzerHandle == NULL) {
      System_abort("Pin open failed!");
    }

    registerTask();
}


