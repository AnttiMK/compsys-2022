/*
 * night.c
 *
 *  Created on: 19.11.2022
 *      Author: Matias
 */

#include <functions/night/night.h>
#include <functions/buzzer/buzzer.h>

#define STACKSIZE 1024
Char nightTaskStack[STACKSIZE];

enum state {NIGHT = 1, DAY};

enum state timeState = DAY;

Void nightTask(UArg arg0, UArg arg1) {

    //koska ei käytetä muita sensoreita voidaan tehdä i2c täällä

    I2C_Handle i2c;
    I2C_Params i2cParams;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(Board_I2C_TMP, &i2cParams);

    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    }


    //otetaan opt3001 käyttöön

    Task_sleep(1000);
    opt3001_setup(&i2c);

    while (1) {

        double light = opt3001_get_data(&i2c);

        if (light < 200) {
            timeState = NIGHT;
        }

        if (timeState == NIGHT) {
            //nukkuu ehkä 5 sekuntia ehkä tm
            playSong(sleep());
            Task_sleep(50000);
            playSong(wakeup());
            timeState = DAY;
            I2C_close(Board_I2C_TMP);
        }

        // Once per second, you can modify this
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

void night_registerTask() {
    Task_Handle nightTaskHandle;
    Task_Params nightTaskParams;

    Task_Params_init(&nightTaskParams);
    nightTaskParams.stackSize = STACKSIZE;
    nightTaskParams.stack = &nightTaskStack;
    nightTaskHandle = Task_create(nightTask, &nightTaskParams,
    NULL);
    if (nightTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}
