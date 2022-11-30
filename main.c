/*
 * Project Tamagotchi
 *
 * -- Authors --
 * Antti Koponen, akoponen21@student.oulu.fi
 * Joonas Tapaninaho, jtapanin21@student.oulu.fi
 * Matias Paavilainen, mpaavila21@student.oulu.fi
 */

/* C Standard library */
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"
#include <functions/buttons/buttons.h>
#include <functions/buzzer/buzzer.h>
#include <functions/sensors/sensors.h>
#include <functions/messaging/uart.h>
#include <functions/messaging/wireless.h>

int main(void) {
    // Task variables

    // Initialize board
    Board_initGeneral();

    /* Wireless kills the board so we're not using it :-) */
    //Wireless_init();

    Board_initI2C();
    Board_initUART();

    // Registers Uart and its Tasks
    UART_registerTask();

    // testing 123
    Sensors_registerTask();

    // Registers buttons and their Tasks
    Buttons_registerTasks();

    // Registers Buzzer and its Tasks
    Buzzer_register();
    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
