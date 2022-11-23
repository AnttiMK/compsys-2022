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
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/UART.h>
#include "Board.h"
#include <functions/movementSensor/movementSensor.h>
#include "functions/buzzer/buzzer.h"

#define STACKSIZE 2048
Char uartTaskStack[STACKSIZE];

static UART_Handle uart;
static char writeBuffer[90];
static uint8_t uartBuffer[80]; // Vastaanottopuskuri

enum dataState {
    READY, NOT_READY
};

enum dataState mpuDataState = NOT_READY;
enum dataState ambientDataState = NOT_READY;

static void uartHandler(UART_Handle uart, void *rxBuf, size_t len) {
    System_printf(rxBuf);
    System_flush();

    // K‰sittelij‰n viimeisen‰ asiana siirryt‰‰n odottamaan uutta keskeytyst‰..
    UART_read(uart, rxBuf, 80);
}

void sendMessage(char msg[], ...) {
    va_list argptr;
    va_start(argptr, msg);
    vsprintf(writeBuffer, msg, argptr);
    va_end(argptr);
    UART_write(uart, writeBuffer, strlen(writeBuffer) + 1);
}

static void uartTask(UArg arg0, UArg arg1) {
    // JTKJ: Teht‰v‰ 4. Lis‰‰ UARTin alustus: 9600,8n1
    // JTKJ: Exercise 4. Setup here UART connection as 9600,8n1
    // UART-kirjaston asetukset

//    char input;
//    char ping_msg[20];
    UART_Params uartParams;

    // Alustetaan sarjaliikenne
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = &uartHandler;
    uartParams.baudRate = 9600; // nopeus 9600baud
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

        UART_read(uart, uartBuffer, 80);

        //if BEEP then beep2
        if (strcmp((const char*)uartBuffer, "id:2420,BEEP") == 0) {
            playSong(beep2());
        }

        // Vastaanotetaan 1 merkki kerrallaan input-muuttujaan
//        UART_read(uart, &input, 29);
//        // L‰hetet‰‰n merkkijono takaisin
//        sprintf(echo_msg, "Received: %c\n", input);
//        System_printf(echo_msg);
//        System_flush();

        // JTKJ: Teht‰v‰ 4. L‰het‰ sama merkkijono UARTilla
        // JTKJ: Exercise 4. Send the same sensor data string with UART
//        sprintf(ping_msg, "id:420,ping");
//        UART_write(uart, ping_msg, strlen(ping_msg) + 1);
        // Just for sanity check for exercise, you can comment this out
//        System_printf("uartTask\n");
//        System_flush();

        // Once per second, you can modify this
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

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
