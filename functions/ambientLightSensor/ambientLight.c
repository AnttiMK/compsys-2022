/*
 * night.c
 *
 *  Created on: 19.11.2022
 *      Author: Matias
 */

/*
 * NOTE: Legacy code, all sensors implemented in functions/sensors/sensors.c
 */

//#include <functions/ambientLightSensor/ambientLight.h>
//#include <functions/buzzer/buzzer.h>
//#include "functions/movementSensor/movementSensor.h"
//#include "functions/messaging/uart.h"
//
//#include <xdc/runtime/System.h>
//#include <ti/drivers/I2C.h>
//#include <ti/drivers/i2c/I2CCC26XX.h>
//#include <ti/sysbios/knl/Task.h>
//#include <ti/sysbios/knl/Clock.h>
//#include "sensors/opt3001.h"
//#include "Board.h"
//
//#define STACKSIZE 512
//Char nightTaskStack[STACKSIZE];
//
//double AmbientLight_amount = 0;
//
//static void nightTask(UArg arg0, UArg arg1) {
//
//    // Wait for MPU9250 to power up
//    while (MovementSensor_getState() != STANDBY) {
//        Task_sleep(100000 / Clock_tickPeriod);
//    }
//
//    I2C_Handle i2c;
//    I2C_Params i2cParams;
//
//    I2C_Params_init(&i2cParams);
//    i2cParams.bitRate = I2C_400kHz;
//
//    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
//
//    if (i2c == NULL) {
//        System_abort("Error Initializing I2C\n");
//    }
//
//    //otetaan opt3001 käyttöön
//
//    Task_sleep(100000 / Clock_tickPeriod);
//    opt3001_setup(&i2c);
//    Task_sleep(100000 / Clock_tickPeriod);
//
//    double *light = &AmbientLight_amount;
//
//    while (1) {
//        while (MovementSensor_getState() == COLLECTING) {
//            if (i2c != NULL) {
//                I2C_close(i2c);
//                i2c = NULL;
//            }
//            Task_sleep(100000 / Clock_tickPeriod);
//        }
//
//        while (i2c == NULL) {
//            i2c = I2C_open(Board_I2C_TMP, &i2cParams);
//
//            if (i2c == NULL) {
//                Task_sleep(100000 / Clock_tickPeriod);
//            }
//        }
//
//        *light = opt3001_get_data(&i2c);
//        UART_notifyLightDataReady();
//
//        int i = 0;
//        while (*light >= 0 && *light < 50) {
//
//            if (i > 4) {
//
//                /*
//                 * Sleeps after 4 seconds in the dark,
//                 * for as long as it is dark and 2 seconds after
//                 * in going back into light.
//                 */
//
//                Buzzer_playSong(sleep());
//
//                sendMessage("id:2420,MSG1:Sleeping");
//
//                while (*light >= 0 && *light < 50) {
//                    Task_sleep(2000000 / Clock_tickPeriod);
//                    *light = opt3001_get_data(&i2c);
//                    UART_notifyLightDataReady();
//                }
//
//                sendMessage("id:2420,MSG1:Awake");
//                Buzzer_playSong(wakeup());
//                i = 0;
//            }
//            i++;
//        }
//        //Once per second
//        Task_sleep(1000000 / Clock_tickPeriod);
//    }
//}
//
//void AmbientLight_registerTask() {
//    Task_Handle nightTaskHandle;
//    Task_Params nightTaskParams;
//
//    Task_Params_init(&nightTaskParams);
//    nightTaskParams.stackSize = STACKSIZE;
//    nightTaskParams.stack = &nightTaskStack;
//    nightTaskParams.priority = 2;
//    nightTaskHandle = Task_create(nightTask, &nightTaskParams,
//    NULL);
//    if (nightTaskHandle == NULL) {
//        System_abort("Task create failed!");
//    }
//}
