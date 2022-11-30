/*
 * sensors.c
 *
 *  Created on: 30 Nov 2022
 *      Author: Antti
 */

#include <functions/sensors/sensors.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

#include <functions/movementSensor/dataParser.h>

#include "Board.h"
#include "sensors/opt3001.h"
#include "sensors/bmp280.h"
#include "sensors/tmp007.h"
#include "sensors/mpu9250.h"
#include <functions/buzzer/buzzer.h>
#include <functions/messaging/uart.h>

#define STACKSIZE 2048
#define AVG_WINDOW 2
static Char sensorTaskStack[STACKSIZE];
// enum mpuState collectionState = INITIALIZING;

static I2C_Handle i2c;
static I2C_Params i2cParams;

/* Separate I2C interface & power pin for MPU9250 */
static I2C_Handle i2cMPU;
static I2C_Params i2cMPUParams;
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = { .pinSDA = Board_I2C0_SDA1,
                                               .pinSCL = Board_I2C0_SCL1 };
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = {
Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL
                                             | PIN_DRVSTR_MAX,
                                     PIN_TERMINATE };

double Sensors_ambientLight = 0.0;
double Sensors_temperature = 0.0;
double Sensors_airPressure = 0.0;
double Sensors_bmpTemp = 0.0;

float Sensors_mpuData[7][100];
float Sensors_accelData[4][100];
float floatingAvg[3][5];

static void initI2C() {
    /* Init I2C for other sensors */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error initializing I2C for other sensors\n");
    }

    /* Setup needed sensors */
    Task_sleep(100000 / Clock_tickPeriod);
    opt3001_setup(&i2c); // ambient light

    Task_sleep(100000 / Clock_tickPeriod);
    bmp280_setup(&i2c); // air pressure

    Task_sleep(100000 / Clock_tickPeriod);
    tmp007_setup(&i2c); // temperature

    /* Close I2C to init MPU9250 */
    I2C_close(i2c);
    i2c = NULL;
}

static void initMPUI2C() {
    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    // Note the different configuration below
    i2cMPUParams.custom = (uintptr_t) &i2cMPUCfg;

    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("Pin open failed!");
    }

    // MPU power on
    PIN_setOutputValue(hMpuPin, Board_MPU_POWER, Board_MPU_POWER_ON);

    // Wait 100ms for the MPU sensor to power up
    Task_sleep(100000 / Clock_tickPeriod);
    System_printf("MPU9250: Power ON\n");
    System_flush();

    // Open I2C for MPU9250
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        System_abort("Error initializing I2C for MPU9250\n");
    }

    // MPU setup and calibration
    System_printf("MPU9250: Setup and calibration...\n");
    System_flush();

    mpu9250_setup(&i2cMPU);
    I2C_close(i2cMPU);
    i2cMPU = NULL;

    System_printf("MPU9250: Setup and calibration OK\n");
    System_flush();
}

static void switchI2C() {
    if (i2cMPU == NULL) {
        I2C_close(i2c);
        i2c = NULL;

        i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
        if (i2cMPU == NULL) {
            System_abort("Error opening I2C for MPU9250\n");
        }
    } else {
        I2C_close(i2cMPU);
        i2cMPU = NULL;

        i2c = I2C_open(Board_I2C_TMP, &i2cParams);
        if (i2c == NULL) {
            System_abort("Error opening I2C for other sensors\n");
        }
    }
}

static void sensorTask(UArg arg0, UArg arg1) {

    initI2C();
    initMPUI2C();

    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error opening I2C for other sensors\n");
    }

//    char msg[200]; // Debug message buffer
    int i, j, count; // Loop variables
    float ax, ay, az, gx, gy, gz;
    float fAvgX = 0.0, fAvgY = 0.0, fAvgZ = 1.0;
    float fAvgX_prev = 0.0, fAvgY_prev = 0.0, fAvgZ_prev = 1.0;
    i = j = count = 0;

    /* Zero array for floating average */
    int arrayInit = 0;
    for (arrayInit = 0; arrayInit < 5; ++arrayInit) {
        floatingAvg[0][arrayInit] = 0.0;
        floatingAvg[1][arrayInit] = 0.0;
        floatingAvg[2][arrayInit] = 1.0;
    }

    bool isFirstIter = 1;
    int i2cSwitchTick = 20;
    int lightTick = 0;
    bool isSleeping = 0;
    int mpuTick = 0;
    while (1) {
        if (i2cSwitchTick < 19) {
            i2cSwitchTick++;
        } else {
            i2cSwitchTick = 0;
            if (isFirstIter == 0) switchI2C();
            isFirstIter = 0;

            Sensors_ambientLight = opt3001_get_data(&i2c);
            UART_notifyLightDataReady();

            if (Sensors_ambientLight >= 0 && Sensors_ambientLight < 50) { // check if it is dark
                if (lightTick >= 2) { // check if 2 iterations ("ticks") have passed in the dark
                    if (isSleeping == 0) {
                        Buzzer_playSong(sleep());
                        sendMessage("id:2420,MSG1:Sleeping");
                    }

                    isSleeping = 1;
                } else { // if 4 seconds haven't passed but we are in the dark
                    lightTick++; // increment
                }
            } else {
                if (lightTick >= 2 && isSleeping == 1) {
                    sendMessage("id:2420,MSG1:Awake");
                    Buzzer_playSong(wakeup());
                }

                lightTick = 0;
                isSleeping = 0;
            }

            //temperature = tmp007_get_data(&i2c);
            // UART_notifyTempDataReady();

            bmp280_get_data(&i2c, &Sensors_airPressure, &Sensors_bmpTemp);
            UART_notifyPresDataReady();

            switchI2C();
        }

        mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);

        floatingAvg[0][mpuTick] = ax;
        floatingAvg[1][mpuTick] = ay;
        floatingAvg[2][mpuTick] = az;

//        float dx, dy, dz;
//        if (mpuTick == 0) {
//            dx = fabsf(ax - floatingAvg[0][4]);
//            dy = fabsf(ay - floatingAvg[1][4]);
//            dz = fabsf(az - floatingAvg[2][4]);
//        } else {
//            dx = fabsf(ax - floatingAvg[0][mpuTick - 1]);
//            dy = fabsf(ay - floatingAvg[1][mpuTick - 1]);
//            dz = fabsf(az - floatingAvg[2][mpuTick - 1]);
//        }

        if (mpuTick == AVG_WINDOW - 1) {
            mpuTick = 0;
        }

        mpuTick++;

        for (j = 0; j < AVG_WINDOW; ++j) {
            fAvgX += floatingAvg[0][j];
            fAvgY += floatingAvg[1][j];
            fAvgZ += floatingAvg[2][j];
        }

        fAvgX = fAvgX / AVG_WINDOW;
        fAvgY = fAvgY / AVG_WINDOW;
        fAvgZ = fAvgZ / AVG_WINDOW;

        float dx, dy, dz;
        dx = fabsf(fAvgX - fAvgX_prev);
        dy = fabsf(fAvgY - fAvgY_prev);
        dz = fabsf(fAvgZ - fAvgZ_prev);

        fAvgX_prev = fAvgX;
        fAvgY_prev = fAvgY;
        fAvgZ_prev = fAvgZ;

//        sprintf(msg, "%f, %f, %f\n", dx, dy, dz);
//        System_printf(msg);
//        System_flush();
//        memset(msg, 0, 200);

        /*
         *
         *
         */
        if ((dx + dy + dz >= 0.2 && i == 0)
                || (dx + dy + dz >= 0.015 && i > 0 && i < 100)) {
            float time = (float) i * 0.05;
            Sensors_accelData[0][i] = time;
            Sensors_accelData[1][i] = fAvgX;
            Sensors_accelData[2][i] = fAvgY;
            Sensors_accelData[3][i] = fAvgZ;

            Sensors_mpuData[0][i] = time;
            Sensors_mpuData[1][i] = ax;
            Sensors_mpuData[2][i] = ay;
            Sensors_mpuData[3][i] = az;
            Sensors_mpuData[4][i] = gx;
            Sensors_mpuData[5][i] = gy;
            Sensors_mpuData[6][i] = gz;


            i++;
        } else {

            if (count > 5) { // kerää ainakin 3 arvoa ennen nollausta
                if (i > 0) { // jos dataa on kerätty analysoidaan se
                    float varX, varY, varZ;
                    calculateVariance2(Sensors_accelData, &varX, &varY, &varZ,
                                       i);
//                    sprintf(msg, "%f, %f, %f\n", varX, varY, varZ);
//                    System_printf(msg);
//                    System_flush();
//                    memset(msg, 0, 200);
                    Move recognized = recognizeMove(varX, varY, varZ);
                    if (recognized == LIFT) {
                        Buzzer_mustBeep(beep1());
                        UART_notifyMoveRecognized(LIFT);
                    } else if (recognized == SLIDE) {
                        Buzzer_mustBeep(beep1());
                        UART_notifyMoveRecognized(SLIDE);
                    } else if (recognized == JUMP) {
                        Buzzer_mustBeep(beep1());
                        UART_notifyMoveRecognized(JUMP);
                    } else if (recognized == STAIRS) {
                        Buzzer_mustBeep(beep1());
                        UART_notifyMoveRecognized(STAIRS);
                    } else {
                        UART_notifyMoveRecognized(NONE);
                    }
                    UART_notifyMpuDataReady(i);
                }

                // x1 = x2 = x3 = y1 = y2 = y3 = z1 = z2 = z3 = 0.0; // nollataan arvot
                i = j = count = 0;
            }
        }

        count++;
        Task_sleep(50000 / Clock_tickPeriod);
    }
}

void Sensors_registerTask() {
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;

    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = STACKSIZE;
    sensorTaskParams.stack = &sensorTaskStack;
    sensorTaskParams.priority = 2;
    sensorTaskHandle = Task_create(sensorTask, &sensorTaskParams, NULL);

    if (sensorTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}
