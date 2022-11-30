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

#include <functions/movementSensor/movementSensor.h>
#include <functions/ambientLightSensor/ambientLight.h>
#include "functions/buzzer/buzzer.h"
#include <util/math.h>

#include <driverlib/aon_batmon.h>

#define STACKSIZE 1024
Char uartTaskStack[STACKSIZE];

static UART_Handle uart;
static char writeBuffer[90];
static uint8_t uartBuffer[80]; // Reception buffer

// Determines data states
enum dataState {
    READY, NOT_READY
};

enum dataState mpuDataState = NOT_READY;
enum dataState ambientDataState = NOT_READY;
enum dataState batteryDataState = NOT_READY;

static bool startsWith(const char *a, const char *b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

static void uartHandler(UART_Handle uart, void *rxBuf, size_t len) {

    //Checking if ID matches device
    if (startsWith((const char *) rxBuf, "2420")) {

        //if BEEP then beep2,
        if (strstr((const char *) uartBuffer, "BEEP") != NULL) {
            Buzzer_mustBeep(beep2());
        }
    }

    // K�sittelij�n viimeisen� asiana siirryt��n odottamaan uutta keskeytyst�..
    UART_read(uart, rxBuf, 80);
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
            for (i = 0; i < 100; ++i) {
                // Sends movement data to backend
                sendMessage(
                        "id:2420,time:%.0f,ax:%.4f,ay:%.4f,az:%.4f,gx:%.4f,gy:%.4f,gz:%.4f",
                        MovementSensor_sensorData[0][i] * 1000,
                        MovementSensor_sensorData[1][i],
                        MovementSensor_sensorData[2][i],
                        MovementSensor_sensorData[3][i],
                        MovementSensor_sensorData[4][i],
                        MovementSensor_sensorData[5][i],
                        MovementSensor_sensorData[6][i]);
            }
            sendMessage("id:2420,session:end");
        }
        
        if (ambientDataState == READY) {
            ambientDataState = NOT_READY;
            sendMessage("id:2420,session:start");
            // Sends lightness data to backend
            sendMessage("id:2420,light:%.4f,session:end", AmbientLight_amount);
        }

        if (batteryDataState == READY) {

        }

        uint32_t batt_reg = HWREG(AON_BATMON_BASE + AON_BATMON_O_BAT);
        int batt_int = (batt_reg & 896) >> 8;
        uint8_t batt_frac = (batt_reg & 127);
        double batt_value = batt_int + binFracToDec(batt_frac);

        // Sends battery state to backend
        sendMessage("battery: %f", batt_value);




        // Vastaanotetaan 1 merkki kerrallaan input-muuttujaan
//        UART_read(uart, &input, 29);
//        // L�hetet��n merkkijono takaisin
//        sprintf(echo_msg, "Received: %c\n", input);
//        System_printf(echo_msg);
//        System_flush();

        // JTKJ: Teht�v� 4. L�het� sama merkkijono UARTilla
        // JTKJ: Exercise 4. Send the same sensor data string with UART
//        sprintf(ping_msg, "id:420,ping");
//        UART_write(uart, ping_msg, strlen(ping_msg) + 1);
        // Just for sanity check for exercise, you can comment this out
//        System_printf("uartTask\n");
//        System_flush();

        // Once per second, you can modify this
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

void UART_notifyMpuDataReady() {
    mpuDataState = READY;
}

void UART_notifyAmbientDataReady() {
    ambientDataState = READY;
}

void UART_notifyBatteryDataReady() {
    ambientDataState = READY;
}
