/*
 * movement.c
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#include <functions/movementSensor/movementSensor.h>
#define STACKSIZE 2048
static Char movementTaskStack[STACKSIZE];

// MPU power pin global variables
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;

// MPU power pin
static PIN_Config MpuPinConfig[] = {
Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL
                                             | PIN_DRVSTR_MAX,
                                     PIN_TERMINATE };

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = { .pinSDA = Board_I2C0_SDA1,
                                               .pinSCL = Board_I2C0_SCL1 };

Void movementTask(UArg arg0, UArg arg1) {

    float ax, ay, az, gx, gy, gz;

    I2C_Handle i2cMPU; // Own i2c-interface for MPU9250 sensor
    I2C_Params i2cMPUParams;

    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    // Note the different configuration below
    i2cMPUParams.custom = (uintptr_t) &i2cMPUCfg;

    // MPU power on
    PIN_setOutputValue(hMpuPin, Board_MPU_POWER, Board_MPU_POWER_ON);

    // Wait 100ms for the MPU sensor to power up
    Task_sleep(1000000 / Clock_tickPeriod);
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

    char msg[60];

    // Loop forever
    while (1) {
        // TODO insert condition here
        if (false) {
            I2C_open(Board_I2C, &i2cMPUParams);
            // MPU ask data
            mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
            I2C_close(i2cMPU);
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

void Movement_registerTask() {
    Task_Handle movementTaskHandle;
    Task_Params movementTaskParams;

    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("Pin open failed!");
    }

    Task_Params_init(&movementTaskParams);
    movementTaskParams.stackSize = STACKSIZE;
    movementTaskParams.stack = &movementTaskStack;
    movementTaskHandle = Task_create(movementTask, &movementTaskParams,
    NULL);
    if (movementTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
}
