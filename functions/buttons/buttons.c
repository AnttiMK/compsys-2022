/*
 * buttons.c
 *
 *  Created on: 16.11.2022
 *      Author: Antti
 */

#include <functions/buttons/buttons.h>

#include <stdbool.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "Board.h"
#include <functions/buzzer/buzzer.h>
#include <functions/movementSensor/movementSensor.h>
#include "functions/messaging/uart.h"

// Determines common stack size for buttons
#define STACKSIZE 512
Char auxBtnTaskStack[STACKSIZE];
Char pwrBtnTaskStack[STACKSIZE];

static PIN_Handle auxBtnHandle;
static PIN_State auxBtnState;
PIN_Config auxBtnConfig[] = {
Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES,
                              PIN_TERMINATE };

static PIN_Handle pwrBtnHandle;
static PIN_State pwrBtnState;
PIN_Config pwrBtnConfig[] = {
Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES,
                              PIN_TERMINATE };
PIN_Config pwrBtnWakeupConfig[] = {
Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
                                    PIN_TERMINATE };

// Determines Pressed state to calculate increment
enum state {
    OPEN = 1, PRESSED
};
enum state auxButtonState = OPEN;
enum state pwrButtonState = OPEN;

static PIN_Handle ledHandle;
static PIN_State ledState;
static int menuState = 0;

PIN_Config ledConfig[] = {
Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
                           PIN_TERMINATE };

static void auxButtonFxn(PIN_Handle handle, PIN_Id pinId) {
    if (PINCC26XX_getInputValue(pinId) == 0) {
        uint_t pinValue = PIN_getOutputValue( Board_LED1);
        pinValue = !pinValue;
        PIN_setOutputValue(ledHandle, Board_LED1, pinValue);
        auxButtonState = PRESSED;
    } else {
        auxButtonState = OPEN;
    }
}

static void pwrButtonFxn(PIN_Handle handle, PIN_Id pinId) {
    if (PINCC26XX_getInputValue(pinId) == 0) {
        pwrButtonState = PRESSED;
    } else {
        pwrButtonState = OPEN;
    }
}

/*
 static void auxButtonTask0(UArg arg0, UArg arg1) {
 int increment = 0;
 while (1) {
 if (auxButtonState == PRESSED) {
 increment++;
 } else {
 if (increment > 10) {
 playSong(nokia());
 }

 increment = 0;
 }
 Task_sleep(100000 / Clock_tickPeriod);
 }
 }
 */

/*

 auxButtonTask2 ja pwrButtonTask2 menu rakenne testausta varten. Esimerkiksi käynnistyessä
 power ja aux nappi, lyhyt painallus aux nappia = esim syönti funktio. Pitkä painallus aux nappia =
 menu value = 1, jolloin lyhyt painallus power nappia = soittaa tkn,
 lyhyt painallus aux nappia = soittaa mario, pitkä painallus aux nappia = menee pois menu tilasta

 */

// Handler function for auxButton

static void auxButtonTask(UArg arg0, UArg arg1) {
    int increment = 0;  // Adding increment value as long as buttons is pressed
    while (1) {
        // Checks if menu state is active
        if (menuState == 1) { 
            if (auxButtonState == PRESSED) { 
                increment++;
            } else {
                if (increment > 10) {
                    menuState = 0;
                } else if (increment > 0) {
                    Buzzer_playSong(mario());
                }
                increment = 0;
            }
        } else {
            if (auxButtonState == PRESSED) {
                increment++;
            } else {
                if (increment > 10) {
                    menuState = 1;
<<<<<<< Updated upstream
                }
                else if (increment > 0) {
                    sendMessage("id:2420,EAT:1");  // Sends EAT command to backend
=======
                } else if (increment > 0) {
                    sendMessage("id:2420,EAT:1");
>>>>>>> Stashed changes
                }
                increment = 0;
            }
        }

        Task_sleep(100000 / Clock_tickPeriod);
    }
}

// Handler function for pwrButton

static void pwrButtonTask(UArg arg0, UArg arg1) {
    int increment = 0;
    while (1) {
        if (pwrButtonState == PRESSED) {
            increment++;
        } else {
            if (increment > 1 && increment < 30) {
                increment = 0;
                MovementSensor_collectData();
            }

            if (increment >= 30) {
                increment = 0;
                Buzzer_playSong(nokia());
                PIN_close(pwrBtnHandle);
                PINCC26XX_setWakeup(pwrBtnWakeupConfig);
                Power_shutdown(NULL, 0);
            }

            if (menuState == 1 && increment > 0 && increment < 30) {
                increment = 0;
                Buzzer_playSong(tkn());
            }
        }

        Task_sleep(100000 / Clock_tickPeriod);
    }

}

/*
 static void pwrButtonTask0(UArg arg0, UArg arg1) {
 int increment = 0;
 while (1) {
 if (pwrButtonState == PRESSED) {
 increment++;

 if (increment > 30) {
 playSong(nokia());
 PIN_close(pwrBtnHandle);
 PINCC26XX_setWakeup(pwrBtnWakeupConfig);
 Power_shutdown(NULL,0);
 }
 }

 Task_sleep(100000 / Clock_tickPeriod);
 }
 }

 */


// Register buttons Tasks

static void registerTasks() {
    Task_Params auxBtnTaskParams; 
    Task_Params_init(&auxBtnTaskParams);
    auxBtnTaskParams.stackSize = STACKSIZE; // Determines stack size for auxButton
    auxBtnTaskParams.stack = &auxBtnTaskStack; 
    auxBtnTaskParams.priority = 2; // Determines task priority auxButton

    Task_Params pwrBtnTaskParams;
    Task_Params_init(&pwrBtnTaskParams);
    pwrBtnTaskParams.stackSize = STACKSIZE;
    pwrBtnTaskParams.stack = &pwrBtnTaskStack;
    pwrBtnTaskParams.priority = 2;

    // Creates Task

    Task_Handle auxBtnTaskHandle = Task_create(auxButtonTask, &auxBtnTaskParams,
    NULL);
    if (auxBtnTaskHandle == NULL) {
        System_abort("Aux button task create failed!");
    }

    Task_Handle pwrBtnTaskHandle = Task_create(pwrButtonTask, &pwrBtnTaskParams,
    NULL);
    if (pwrBtnTaskHandle == NULL) {
        System_abort("Power button task create failed!");
    }
}

// Setup all sub functions --> Called in main loop

void Buttons_registerTasks() {
    auxBtnHandle = PIN_open(&auxBtnState, auxBtnConfig);
    if (!auxBtnHandle) {
        System_abort("Error initializing auxiliary button pin\n");
    }

    pwrBtnHandle = PIN_open(&pwrBtnState, pwrBtnConfig);
    if (!pwrBtnHandle) {
        System_abort("Error initializing power button pin\n");
    }

    if (PIN_registerIntCb(auxBtnHandle, &auxButtonFxn) != 0) {
        System_abort("Error registering auxiliary button callback function");
    }

    if (PIN_registerIntCb(pwrBtnHandle, &pwrButtonFxn) != 0) {
        System_abort("Error registering power button callback function");
    }

    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) {
        System_abort("Error initializing LED pin\n");
    }

    registerTasks();
}
