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

void clockTask(UArg arg0, UArg arg1) {

}

Int main(void) {
    // Task variables

    // Initialize board
    Board_initGeneral();

    /* Wireless kills the board so we're not using it :-) */
    //Wireless_init();

    Board_initI2C();
    Board_initUART();

    // Registers Uart and its Tasks
    UART_registerTask();

    // Registers Movment sensor and its Tasks
    MovementSensor_registerTask();

    // Registers Light sensor and its Tasks
    AmbientLight_registerTask();

    // Registers buttons and their Tasks
    Buttons_registerTasks();

    // Registers Buzzer and its Tasks
    //Buzzer_register();
    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
