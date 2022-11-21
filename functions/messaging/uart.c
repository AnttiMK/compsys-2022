/*
 * uart.c
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#include "uart.h"

#define STACKSIZE 2048
Char uartTaskStack[STACKSIZE];

uint8_t uartBuffer[80]; // Vastaanottopuskuri

static void uartHandler(UART_Handle handle, void *rxBuf, size_t len) {
    //char echo_msg[50];
    //sprintf(echo_msg, "Received: %c\n", rxBuf);
    //System_printf(rxBuf);
    //System_flush();

    // K‰sittelij‰n viimeisen‰ asiana siirryt‰‰n odottamaan uutta keskeytyst‰..
    UART_read(handle, rxBuf, 80);
}

static void uartTask(UArg arg0, UArg arg1) {
    // JTKJ: Teht‰v‰ 4. Lis‰‰ UARTin alustus: 9600,8n1
    // JTKJ: Exercise 4. Setup here UART connection as 9600,8n1
    // UART-kirjaston asetukset

    char input;
    char echo_msg[30];
    char ping_msg[20];
    UART_Handle uart;
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

    while (1) {
        //Task_sleep(10000 / Clock_tickPeriod);

        // Vastaanotetaan 1 merkki kerrallaan input-muuttujaan
//        UART_read(uart, &input, 29);
//        // L‰hetet‰‰n merkkijono takaisin
//        sprintf(echo_msg, "Received: %c\n", input);
//        System_printf(echo_msg);
//        System_flush();

        // JTKJ: Teht‰v‰ 3. Kun tila on oikea, tulosta sensoridata merkkijonossa debug-ikkunaan
        //       Muista tilamuutos
        // JTKJ: Exercise 3. Print out sensor data as string to debug window if the state is correct
        //       Remember to modify state

//        if (programState == DATA_READY) {
//            sprintf(echo_msg, "%f\n", ambientLight);
//            programState = WAITING;
//        }

        // JTKJ: Teht‰v‰ 4. L‰het‰ sama merkkijono UARTilla
        // JTKJ: Exercise 4. Send the same sensor data string with UART
        sprintf(ping_msg, "id:420,ping");
        UART_write(uart, ping_msg, strlen(ping_msg) + 1);
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
