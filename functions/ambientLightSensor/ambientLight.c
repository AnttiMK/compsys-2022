/*
 * night.c
 *
 *  Created on: 19.11.2022
 *      Author: Matias
 */

#include <functions/ambientLightSensor/ambientLight.h>
#include <functions/buzzer/buzzer.h>
#include "functions/movementSensor/movementSensor.h"

#include <xdc/runtime/System.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "sensors/opt3001.h"
#include "Board.h"

#define STACKSIZE 1024
Char nightTaskStack[STACKSIZE];

enum state {
    NIGHT = 1, DAY
};

enum state timeState = DAY;

static void nightTask(UArg arg0, UArg arg1) {

    // Wait for MPU9250 to power up
    while (MovementSensor_getState() != STANDBY) {
        Task_sleep(100000 / Clock_tickPeriod);
    }

    I2C_Handle i2c;
    I2C_Params i2cParams;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(Board_I2C_TMP, &i2cParams);

    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    }

    //otetaan opt3001 käyttöön

    Task_sleep(100000 / Clock_tickPeriod);
    opt3001_setup(&i2c);
    Task_sleep(100000 / Clock_tickPeriod);

    while (1) {
        while (MovementSensor_getState() == COLLECTING) {
            if (i2c != NULL) {
                I2C_close(i2c);
                i2c = NULL;
            }
            Task_sleep(100000 / Clock_tickPeriod);
        }

        while (i2c == NULL) {
            i2c = I2C_open(Board_I2C_TMP, &i2cParams);

            if (i2c == NULL) {
                Task_sleep(100000 / Clock_tickPeriod);
            }
        }

        double light = opt3001_get_data(&i2c);

        if (light >= 0 && light < 50) {
            timeState = NIGHT;
        }

        if (timeState == NIGHT) {
            //nukkuu ehkä 5 sekuntia ehkä tm
            playSong(sleep());
            Task_sleep(5000000 / Clock_tickPeriod);
            playSong(wakeup());
            timeState = DAY;
        }

        // Once per second, you can modify this
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

void AmbientLight_registerTask() {
    Task_Handle nightTaskHandle;
    Task_Params nightTaskParams;

    Task_Params_init(&nightTaskParams);
    nightTaskParams.stackSize = STACKSIZE;
    nightTaskParams.stack = &nightTaskStack;
    nightTaskParams.priority = 2;
    nightTaskHandle = Task_create(nightTask, &nightTaskParams,
    NULL);
    if (nightTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}
