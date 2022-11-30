/*
 * uart.c
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#include "uart.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/UART.h>
#include "Board.h"

#include <functions/sensors/sensors.h>
#include <functions/buzzer/buzzer.h>
#include <util/math.h>

#include <driverlib/aon_batmon.h>

#define STACKSIZE 1024
Char uartTaskStack[STACKSIZE];

static UART_Handle uart;
static char writeBuffer[90];
static uint8_t uartBuffer[80]; // Reception buffer
static int mpuIndex = 0;

// Determines data states
enum dataState {
    READY, NOT_READY
};

enum dataState mpuDataState = NOT_READY;
enum dataState ambientDataState = NOT_READY;
enum dataState batteryDataState = NOT_READY;
enum dataState pressureDataState = NOT_READY;
enum dataState tempDataState = NOT_READY;
static bool moveRecognized = false;
static Move recognizedMove;
static bool shouldEat = false;

static bool startsWith(const char *a, const char *b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

static void uartHandler(UART_Handle uart, void *rxBuf, size_t len) {
    //Checking if ID matches device
    if (startsWith((const char*) rxBuf, "2420")) {

        //if BEEP then beep2,
        if (strstr((const char*) uartBuffer, "BEEP") != NULL) {
            Buzzer_mustBeep(beep2());
        }

        if (strstr((const char*) uartBuffer, "Too late") != NULL) {
                    Buzzer_mustPlaySong(sleep());
                }
    }
    memset(uartBuffer, 0, 80);
    // K�sittelij�n viimeisen� asiana siirryt��n odottamaan uutta keskeytyst�..
    UART_read(uart, uartBuffer, 80);
}

// Sends messages to backend
void sendMessage(char msg[], ...) {
    va_list argptr;
    va_start(argptr, msg);
    vsprintf(writeBuffer, msg, argptr);
    va_end(argptr);
    UART_write(uart, writeBuffer, strlen(writeBuffer) + 1);
}

// Creates UART task 
static void uartTask(UArg arg0, UArg arg1) {
    // JTKJ: Teht�v� 4. Lis�� UARTin alustus: 9600,8n1
    // JTKJ: Exercise 4. Setup here UART connection as 9600,8n1
    // UART-kirjaston asetukset

//    char input;
//    char ping_msg[20];
    UART_Params uartParams;

    // Initializing serial traffic
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = &uartHandler;
    uartParams.baudRate = 9600; // Speed 9600baud
    uartParams.dataLength = UART_LEN_8; // 8
    uartParams.parityType = UART_PAR_NONE; // n
    uartParams.stopBits = UART_STOP_ONE; // 1

    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_read(uart, uartBuffer, 80);

    sendMessage("id:420,ping");

    while (1) {
        if (mpuDataState == READY) {
            mpuDataState = NOT_READY;
            sendMessage("id:2420,session:start");

            int i;
            for (i = 0; i < mpuIndex; ++i) {
                // Sends movement data to backend
                sendMessage(
                        "id:2420,time:%.0f,ax:%.4f,ay:%.4f,az:%.4f,gx:%.4f,gy:%.4f,gz:%.4f",
                        Sensors_mpuData[0][i] * 1000, Sensors_mpuData[1][i],
                        Sensors_mpuData[2][i], Sensors_mpuData[3][i],
                        Sensors_mpuData[4][i], Sensors_mpuData[5][i],
                        Sensors_mpuData[6][i]);
            }
            sendMessage("id:2420,session:end");
        }

        /* Sends ambient light amount to backend when ready */
        if (ambientDataState == READY) {
            ambientDataState = NOT_READY;
            sendMessage("id:2420,session:start");
            sendMessage("id:2420,light:%.4f,session:end", Sensors_ambientLight);
        }

        /* Calculate battery data and send to msgbox 2 along with pressure data */
        if (pressureDataState == READY) {
            pressureDataState = NOT_READY;
            uint32_t batt_reg = HWREG(AON_BATMON_BASE + AON_BATMON_O_BAT);
            int batt_int = (batt_reg & 896) >> 8;
            uint8_t batt_frac = (batt_reg & 127);
            double batt_value = batt_int + binFracToDec(batt_frac);
            sendMessage("id:2420,MSG2:Battery: %.2fV - Air pressure: %.2fhPa",
                        batt_value, Sensors_airPressure);
            sendMessage("id:2420,session:start");
            sendMessage("id:2420,press:%.4f,session:end", Sensors_ambientLight);
        }

        if (moveRecognized == true) {
            moveRecognized = false;
            if (recognizedMove == LIFT) {
                sendMessage("id:2420,EXERCISE:1,MSG1:Exercise move recognized");
            } else if (recognizedMove == SLIDE) {
                sendMessage("id:2420,PET:2,MSG1:Pet move recognized");
            } else if (recognizedMove == JUMP) {
                sendMessage("id:2420,EXERCISE:2,MSG1:Exercise move recognized");
            } else if (recognizedMove == STAIRS) {
                sendMessage("id:2420,EXERCISE:3,MSG1:Exercise move recognized");
            } else {
                sendMessage("id:2420,MSG1:Movement not recognized");
            }
        }

        if (shouldEat == true) {
            shouldEat = false;
            sendMessage("id:2420,EAT:1");
        }

        Task_sleep(250000 / Clock_tickPeriod);
    }
}

// Setup all sub functions --> Called in main loop

void UART_registerTask() {
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskParams.priority = 2;
    uartTaskHandle = Task_create(uartTask, &uartTaskParams, NULL);
    if (uartTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}

void UART_notifyMpuDataReady(int index) {
    mpuDataState = READY;
    mpuIndex = index;
}

void UART_notifyLightDataReady() {
    ambientDataState = READY;
}

void UART_notifyTempDataReady() {
    tempDataState = READY;
}

void UART_notifyPresDataReady() {
    pressureDataState = READY;
}

void UART_notifyBatteryDataReady() {
    ambientDataState = READY;
}

void UART_notifyMoveRecognized(Move move) {
    moveRecognized = true;
    recognizedMove = move;
}

void UART_notifyEat() {
    shouldEat = true;
}
