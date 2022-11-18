/* C Standard library */
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"
#include <functions/buttons/buttons.h>
#include <functions/buzzer/buzzer.h>
#include <functions/movementSensor/movementSensor.h>
#include <functions/messaging/uart.h>

/* Task */
#define STACKSIZE 2048
Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];

// JTKJ: Tehtävä 3. Tilakoneen esittely
// JTKJ: Exercise 3. Definition of the state machine
enum state { WAITING = 1, DATA_READY };
enum state programState = WAITING;

// JTKJ: Tehtävä 3. Valoisuuden globaali muuttuja
// JTKJ: Exercise 3. Global variable for ambient light
double ambientLight = -1000.0;

/* Task Functions */
Void sensorTaskFxn(UArg arg0, UArg arg1) {
    return;

    I2C_Handle i2c;
    I2C_Params i2cParams;

    // JTKJ: Tehtävä 2. Avaa i2c-väylä taskin käyttöön
    // JTKJ: Exercise 2. Open the i2c bus

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(Board_I2C_TMP, &i2cParams);

    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    }

    // JTKJ: Tehtävä 2. Alusta sensorin OPT3001 setup-funktiolla
    //       Laita enne funktiokutsua eteen 100ms viive (Task_sleep)
    // JTKJ: Exercise 2. Setup the OPT3001 sensor for use
    //       Before calling the setup function, insertt 100ms delay with Task_sleep
    Task_sleep(1000);
    opt3001_setup(&i2c);

    while (1) {
        // JTKJ: Tehtävä 2. Lue sensorilta dataa ja tulosta se Debug-ikkunaan merkkijonona
        // JTKJ: Exercise 2. Read sensor data and print it to the Debug window as string
        double light = opt3001_get_data(&i2c);
        //sprintf(out, "%f\n", light);
        //System_printf(out);
        // JTKJ: Tehtävä 3. Tallenna mittausarvo globaaliin muuttujaan
        //       Muista tilamuutos
        // JTKJ: Exercise 3. Save the sensor value into the global variable
        //       Remember to modify state
        ambientLight = light;
        programState = DATA_READY;
        // Just for sanity check for exercise, you can comment this out
        //System_printf("sensorTask\n");
        System_flush();

        // Once per second, you can modify this
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

Int main(void) {
    // Task variables
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;

    // Initialize board
    Board_initGeneral();
    Init6LoWPAN();

    // JTKJ: Tehtävä 2. Ota i2c-väylä käyttöön ohjelmassa
    // JTKJ: Exercise 2. Initialize i2c bus
    Board_initI2C();
    // JTKJ: Tehtävä 4. Ota UART käyttöön ohjelmassa
    // JTKJ: Exercise 4. Initialize UART
    //Board_initUART();
    // JTKJ: Tehtävä 1. Ota painonappi ja ledi ohjelman käyttöön
    //       Muista rekisteröidä keskeytyksen käsittelijä painonapille
    // JTKJ: Exercise 1. Open the button and led pins
    //       Remember to register the above interrupt handler for button
    // Ledi käyttöön ohjelmassa

    UART_registerTask();
    Movement_registerTask();

    // Rekisteröidään painonapit ja niiden taskit
    Buttons_registerTasks();

    // Rekisteröidään summeri
    //Buzzer_register();

    /* Task */
    /*
     Task_Params_init(&sensorTaskParams);
     sensorTaskParams.stackSize = STACKSIZE;
     sensorTaskParams.stack = &sensorTaskStack;
     sensorTaskParams.priority=2;
     sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
     if (sensorTaskHandle == NULL) {
     System_abort("Task create failed!");
     }
     */

    /* Sanity check */
    System_printf("Hello world!\n");
    System_printf("tickperiod is %d\n", Clock_tickPeriod);
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
