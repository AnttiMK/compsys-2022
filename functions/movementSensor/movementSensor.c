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

#include <functions/movementSensor/dataParser.h>
#include <functions/movementSensor/movementTask.h>

#include "Board.h"
#include "sensors/mpu9250.h"
#include <functions/buzzer/buzzer.h>
#include <functions/messaging/uart.h>

#define STACKSIZE 2048
static Char movementTaskStack[STACKSIZE];
enum mpuState collectionState = INITIALIZING;

// MPU power pin global variables
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;

static I2C_Handle i2cMPU; // Own i2c-interface for MPU9250 sensor
static I2C_Params i2cMPUParams;

float MovementSensor_sensorData[7][100];

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
    int movment;
    int movment2;

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
                MovementSensor_sensorData[0][i] = (float) i * 0.05;
                MovementSensor_sensorData[1][i] = ax;
                MovementSensor_sensorData[2][i] = ay;
                MovementSensor_sensorData[3][i] = az;
                MovementSensor_sensorData[4][i] = gx;
                MovementSensor_sensorData[5][i] = gy;
                MovementSensor_sensorData[6][i] = gz;

                /* Sleep 100ms */
                Task_sleep(50000 / Clock_tickPeriod);
            }
            playSong(beep1());
            UART_notifyMpuDataReady();

            I2C_close(i2cMPU);
            collectionState = STANDBY;

            movment = 0;
            movment2 = 0;

            /*
            Idea, prosessoida dataa 25kpl kokoisissa ikkunoissa,
            tarkistaa mill� akselilla on tapahtunut eniten muutosta ikkunan sis�ll� ja lis�� sen liike summaan.
            t�ll� tavalla voidaan tunnistaa liikkeitä yhdestä int arvosta.

            Esim tehdään portaat --> ylös oikealle ylös oikealla 
             - ensimmäisessä ikkunussa Z arvo muuttuu eniten ja se on positiivinen joten liike + 5 * 10 ^0, liike = 5
             - toisessa ikkunussa Y arvo muuttuu eniten ja se on positiivinen joten liike + 3 * 10 ^ 1, jolloin liike = 35
             - kolmannessa ikkunussa Z arvo muuttuu eniten ja se on positiivinen joten liike + 5 * 10 ^2, jolloin liike = 535
             - neljännessä ikkunussa Y arvo muuttuu eniten ja se on positiivinen joten liike + 3 * 10 ^3, jolloin liike = 3535

            nyt movement = 3535 joka tunnistetaan portaaksi

            portaat alaspäin olisi taas = 6464, joka voitaisiin tunnistaa liikkeeksi portaat alaspäin
            
            */


//            movmentValue(MovementSensor_sensorData, &movment, 0, 0);
//            movmentValue(MovementSensor_sensorData, &movment, 1, 25);
//            movmentValue(MovementSensor_sensorData, &movment, 2, 50);
//            movmentValue(MovementSensor_sensorData, &movment, 3, 75);
//            recognizeMove2(movment);

            /*
            Testi 2, sama idea mutta lisättynä paikallaan olo ja tilanteen jossa Z ja Y akseli muuttuu melkein yhtä paljon
            Nyt eri ikkunassa olevia liikkeitä voi olla 6 sijasta 11 eli potenssia pitää nostaa aina kahdella
            Esimerkiksi:
              - 1. ikkunussa Z arvo muuttuu eniten eikä Y arvo muutu tarpeeksi, ja Z on positiivinen joten liike + 20 * 10 ^0, liike = 20
              - 2. ikkunussa Z ja Y muuttuu lähens yhtä paljon ja molemmat on positiivinen joten liike + 14 * 10 ^2, liike = 1420
              - 3. ikkuna liike + arvo * 10 ^ 4 = XX1420
              - 4. ikkuna liiker + arvo * 10 ^6 = YYXX1420

            Nyt eri liikkeitä olisi esim: 
            Portaat (oikein päin) = 18201820
            Hissi ylös = 20202020
            Hissi alas = 21212121
            Ympyrä (neljä eri tapaa riippuen mistä suunnasta alottaako 1 i -1 vai -i )
            14151716 tai 16141517 tai 17161415 tai 15171614

            2 hyppyä /\/\ olisi = 16141614
            jne....
            */

//           movmentValue(MovementSensor_sensorData, &movment2, 0, 0);
//           movmentValue(MovementSensor_sensorData, &movment2, 2, 25);
//           movmentValue(MovementSensor_sensorData, &movment2, 4, 50);
//           movmentValue(MovementSensor_sensorData, &movment2, 6, 75);
//           recognizeMove3(movment2);



//            int j;
//            for (j = 0; j < 100; ++j) {
//                sprintf(msg, "%f,%f,%f,%f,%f,%f,%f\n", MovementSensor_sensorData[0][j], MovementSensor_sensorData[1][j], MovementSensor_sensorData[2][j], MovementSensor_sensorData[3][j], MovementSensor_sensorData[4][j], MovementSensor_sensorData[5][j], MovementSensor_sensorData[6][j]);
//
//                System_printf(msg);
//                System_flush();
//                memset(msg, 0, 60);
//            }

            float x1, y1, z1, xg1, yg1, zg1;
            float sum_x = 0, sum_y = 0, sum_z = 0;

            //calculateSD(MovementSensor_sensorData, &x1, &y1, &z1, &xg1, &yg1, &zg1);
            calculateSD2(MovementSensor_sensorData, &x1, &y1, &z1, &xg1, &yg1,
                         &zg1, 0);
            sprintf(msg, "SD1: %f,%f,%f,%f,%f,%f\n", x1*100, y1*100, z1*100, xg1*100, yg1*100, zg1*100);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);
            sum_x += x1;
            sum_y += y1;
            sum_z += z1;
            calculateSD2(MovementSensor_sensorData, &x1, &y1, &z1, &xg1, &yg1,
                         &zg1, 25);
            sprintf(msg, "SD2: %f,%f,%f,%f,%f,%f\n", x1*100, y1*100, z1*100, xg1*100, yg1*100, zg1*100);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);
            sum_x += x1;
            sum_y += y1;
            sum_z += z1;
            calculateSD2(MovementSensor_sensorData, &x1, &y1, &z1, &xg1, &yg1,
                         &zg1, 50);
            sprintf(msg, "SD3: %f,%f,%f,%f,%f,%f\n", x1*100, y1*100, z1*100, xg1*100, yg1*100, zg1*100);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);
            sum_x += x1;
            sum_y += y1;
            sum_z += z1;
            calculateSD2(MovementSensor_sensorData, &x1, &y1, &z1, &xg1, &yg1,
                         &zg1, 75);

            sprintf(msg, "SD4: %f,%f,%f,%f,%f,%f\n", x1*100, y1*100, z1*100, xg1*100, yg1*100, zg1*100);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);
            sum_x += x1;
            sum_y += y1;
            sum_z += z1;

            sum_x = sum_x/4;
            sum_y = sum_y/4;
            sum_z = sum_z/4;

            sprintf(msg, "SD4: %f,%f,%f,%f,%f,%f\n", sum_x*100, sum_y*100, sum_z*100, xg1*100, yg1*100, zg1*100);
                        System_printf(msg);
                        System_flush();
                        memset(msg, 0, 60);



            float x2, y2, z2, xg2, yg2, zg2;
            calculateVariance(MovementSensor_sensorData, &x2, &y2, &z2, &xg2,
                              &yg2, &zg2);

            sprintf(msg, "V: %f,%f,%f,%f,%f,%f\n", x2, y2, z2, xg2, yg2, zg2);
            System_printf(msg);
            System_flush();
            memset(msg, 0, 60);
            recognizeMove(x2, y2, z2);

        }

        // Sleep 500ms
        Task_sleep(500000 / Clock_tickPeriod);
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
