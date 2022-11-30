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

static PIN_Handle led1Handle;
static PIN_Handle led2Handle;
static PIN_State led1State;
static PIN_State led2State;
static int menuState = 0;

PIN_Config led1Config[] = {
Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
                            PIN_TERMINATE };
PIN_Config led2Config[] = {
Board_LED2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
                            PIN_TERMINATE };

static void auxButtonFxn(PIN_Handle handle, PIN_Id pinId) {
    if (PINCC26XX_getInputValue(pinId) == 0) {
        uint_t pinValue = PINCC26XX_getOutputValue( Board_LED1);
        pinValue = !pinValue;
        PIN_setOutputValue(led1Handle, Board_LED1, pinValue);
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
                    PINCC26XX_setOutputValue(Board_LED2, 0);
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
                    PINCC26XX_setOutputValue(Board_LED2, 1);
                } else if (increment > 0) {
                    UART_notifyEat(); // Send EAT command to backend
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
            if (menuState == 1) {
                if (increment > 1 && increment < 30) {
                    increment = 0;
                    Buzzer_playSong(nokia());
                }
            } else {
                if (increment > 1 && increment < 30) {
                    increment = 0;
                    uint_t pinValue = PINCC26XX_getOutputValue( Board_LED2);
                    pinValue = !pinValue;
                    PIN_setOutputValue(led1Handle, Board_LED2, pinValue);
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

    led1Handle = PIN_open(&led1State, led1Config);
    if (!led1Handle) {
        System_abort("Error initializing LED 1 pin\n");
    }

    led2Handle = PIN_open(&led2State, led2Config);
    if (!led2Handle) {
        System_abort("Error initializing LED 2 pin\n");
    }

    registerTasks();
}
