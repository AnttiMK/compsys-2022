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
#include <functions/ambientLightSensor/ambientLight.h>
#include <functions/movementSensor/movementSensor.h>
#include <functions/messaging/uart.h>
#include <functions/messaging/wireless.h>


Int main(void) {
    // Task variables

    // Initialize board
    Board_initGeneral();

    /* Wireless kills the board so we're not using it :-) */
    // Wireless_init();

    Board_initI2C();
    Board_initUART();
    UART_registerTask();
    MovementSensor_registerTask();

    AmbientLight_registerTask();

    // Rekister�id��n painonapit ja niiden taskit
    Buttons_registerTasks();

    // Rekister�id��n summeri
    //Buzzer_register();


    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
