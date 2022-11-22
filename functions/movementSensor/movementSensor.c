/*
 * movement.c
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#include <functions/movementSensor/movementSensor.h>

#include "stdio.h"
#include "string.h"

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

#include <functions/movmentSensor/dataParser.h>
#include <functions/movmentSensor/movmentTask.h>

#include "Board.h"
#include "sensors/mpu9250.h"
#include <functions/buzzer/buzzer.h>

#define STACKSIZE 4096
static Char movementTaskStack[STACKSIZE];
enum mpuState collectionState = INITIALIZING;

// MPU power pin global variables
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;

static I2C_Handle i2cMPU; // Own i2c-interface for MPU9250 sensor
static I2C_Params i2cMPUParams;

static float sensorData[7][100];

// MPU power pin
static PIN_Config MpuPinConfig[] = {
Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL
                                             | PIN_DRVSTR_MAX,
                                     PIN_TERMINATE };

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = { .pinSDA = Board_I2C0_SDA1,
                                               .pinSCL = Board_I2C0_SCL1 };

static bool tryLockI2C() {
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        return false;
    }
    return true;
}

static void movementTask(UArg arg0, UArg arg1) {
    float ax, ay, az, gx, gy, gz;

    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    // Note the different configuration below
    i2cMPUParams.custom = (uintptr_t) &i2cMPUCfg;

    // MPU power on
    PIN_setOutputValue(hMpuPin, Board_MPU_POWER, Board_MPU_POWER_ON);

    // Wait 100ms for the MPU sensor to power up
    Task_sleep(100000 / Clock_tickPeriod);
    System_printf("MPU9250: Power ON\n");
    System_flush();

    // MPU open i2c
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        System_abort("Error Initializing I2CMPU\n");
    }

    // MPU setup and calibration
    System_printf("MPU9250: Setup and calibration...\n");
    System_flush();

    mpu9250_setup(&i2cMPU);
    I2C_close(i2cMPU);

    System_printf("MPU9250: Setup and calibration OK\n");
    System_flush();
    collectionState = STANDBY;

    char msg[200];

    // Loop forever
    while (1) {
        // TODO insert condition here
        if (collectionState == COLLECTING) {
            while (!tryLockI2C()) {
                Task_sleep(100000 / Clock_tickPeriod);
            }
            playSong(beep1());
            int i;
            for (i = 0; i < 100; ++i) {
                mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
                sensorData[0][i] = (float) i * 0.05;
                sensorData[1][i] = ax;
                sensorData[2][i] = ay;
                sensorData[3][i] = az;
                sensorData[4][i] = gx;
                sensorData[5][i] = gy;
                sensorData[6][i] = gz;

                /* Sleep 100ms */
                Task_sleep(50000 / Clock_tickPeriod);
            }
            playSong(beep1());

            I2C_close(i2cMPU);
            collectionState = STANDBY;

            int j;
            for (j = 0; j < 100; ++j) {
                sprintf(msg, "%f,%f,%f,%f,%f,%f,%f\n", sensorData[0][j], sensorData[1][j], sensorData[2][j], sensorData[3][j], sensorData[4][j], sensorData[5][j], sensorData[6][j]);

                System_printf(msg);
                System_flush();
                memset(msg, 0, 60);
            }

            float x1,y1,z1,xg1,yg1,zg1 = calculateSD(&sensorData);

            sprintf(msg, "%f,%f,%f,%f,%f,%f,%f\n", x1, y1, z1, xg1, yg1, zg1);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);

            recognizeMove(float x1, float y1, float z1);
            
            float x2,y2,z2,xg2,yg2,zg2 = calculateVariance(&sensorData);

            sprintf(msg, "%f,%f,%f,%f,%f,%f,%f\n", x2, y2, z2, xg2, yg2, zg2);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);


        }

        // Sleep 100ms
        Task_sleep(100000 / Clock_tickPeriod);
    }

    // Program never gets here..
    // MPU close i2c
    // I2C_close(i2cMPU);
    // MPU power off
    // PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_OFF);
}

void MovementSensor_registerTask() {
    Task_Handle movementTaskHandle;
    Task_Params movementTaskParams;

    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("Pin open failed!");
    }

    Task_Params_init(&movementTaskParams);
    movementTaskParams.stackSize = STACKSIZE;
    movementTaskParams.stack = &movementTaskStack;
    movementTaskParams.priority = 2;
    movementTaskHandle = Task_create(movementTask, &movementTaskParams,
    NULL);
    if (movementTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}

void MovementSensor_collectData() {
    collectionState = COLLECTING;
}

enum mpuState MovementSensor_getState() {
    return collectionState;
}
