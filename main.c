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
#include "buzzer/buzzer.h"
#include "buzzer/songs.h"

/* Task */
#define STACKSIZE 2048
Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];
Char movementTaskStack[STACKSIZE];

// JTKJ: Tehtävä 3. Tilakoneen esittely
// JTKJ: Exercise 3. Definition of the state machine
enum state { WAITING=1, DATA_READY };
enum state programState = WAITING;

// JTKJ: Tehtävä 3. Valoisuuden globaali muuttuja
// JTKJ: Exercise 3. Global variable for ambient light
double ambientLight = -1000.0;

// JTKJ: Tehtävä 1. Lisää painonappien RTOS-muuttujat ja alustus
// JTKJ: Exercise 1. Add pins RTOS-variables and configuration here
static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;

PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE
};

PIN_Config ledConfig[] = {
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

// MPU power pin global variables
static PIN_Handle hMpuPin;
static PIN_State  MpuPinState;

// MPU power pin
static PIN_Config MpuPinConfig[] = {
    Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};

static PIN_Handle hBuzzer;
static PIN_State sBuzzer;
static PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

void buttonFxn(PIN_Handle handle, PIN_Id pinId) {

    // JTKJ: Tehtävä 1. Vilkuta jompaa kumpaa lediä
    // JTKJ: Exercise 1. Blink either led of the device
    // Vaihdetaan led-pinnin tilaa negaatiolla
       uint_t pinValue = PIN_getOutputValue( Board_LED1 );
       pinValue = !pinValue;
       PIN_setOutputValue( ledHandle, Board_LED1, pinValue );
}

/* Task Functions */
Void uartTaskFxn(UArg arg0, UArg arg1) {
    return;

    // JTKJ: Tehtävä 4. Lisää UARTin alustus: 9600,8n1
    // JTKJ: Exercise 4. Setup here UART connection as 9600,8n1
    // UART-kirjaston asetukset

    char echo_msg[30];
       UART_Handle uart;
       UART_Params uartParams;

       // Alustetaan sarjaliikenne
       UART_Params_init(&uartParams);
       uartParams.writeDataMode = UART_DATA_TEXT;
       uartParams.readDataMode = UART_DATA_TEXT;
       uartParams.readEcho = UART_ECHO_OFF;
       uartParams.readMode=UART_MODE_BLOCKING;
       uartParams.baudRate = 9600; // nopeus 9600baud
       uartParams.dataLength = UART_LEN_8; // 8
       uartParams.parityType = UART_PAR_NONE; // n
       uartParams.stopBits = UART_STOP_ONE; // 1

    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
       System_abort("Error opening the UART");
    }

    while (1) {

        // JTKJ: Tehtävä 3. Kun tila on oikea, tulosta sensoridata merkkijonossa debug-ikkunaan
        //       Muista tilamuutos
        // JTKJ: Exercise 3. Print out sensor data as string to debug window if the state is correct
        //       Remember to modify state

        if (programState == DATA_READY) {
            sprintf(echo_msg, "%f\n", ambientLight);
            System_printf(echo_msg);
            programState = WAITING;
        }

        // JTKJ: Tehtävä 4. Lähetä sama merkkijono UARTilla
        // JTKJ: Exercise 4. Send the same sensor data string with UART
        UART_write(uart, echo_msg, strlen(echo_msg));
        // Just for sanity check for exercise, you can comment this out
        System_printf("uartTask\n");
        System_flush();

        // Once per second, you can modify this
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

Void sensorTaskFxn(UArg arg0, UArg arg1) {
    return;

    I2C_Handle      i2c;
    I2C_Params      i2cParams;

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

float ax, ay, az, gx, gy, gz;

Void movementTaskFxn(UArg arg0, UArg arg1) {


        I2C_Handle i2cMPU; // Own i2c-interface for MPU9250 sensor
        I2C_Params i2cMPUParams;

        I2C_Params_init(&i2cMPUParams);
        i2cMPUParams.bitRate = I2C_400kHz;
        // Note the different configuration below
        i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;

        // MPU power on
        PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);

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

        System_printf("MPU9250: Setup and calibration OK\n");
        System_flush();

        char msg[60];

        Song song = nokia();
        playSong(&song, hBuzzer);

        // Loop forever
        while (1) {
            System_printf("ticks before: %d\n", Clock_getTicks());

            // MPU ask data
            mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
            sprintf(msg, "ax: %f, ay: %f, az:%f, gx: %f, gy: %f, gz:%f,\n", ax, ay, az, gx, gy, gz);
            System_printf(msg);
            System_printf("ticks before: %d\n", Clock_getTicks());

            // Sleep 100ms
            // Task_sleep(100000 / Clock_tickPeriod);
        }

        // Program never gets here..
        // MPU close i2c
        // I2C_close(i2cMPU);
        // MPU power off
        // PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_OFF);
}

Int main(void) {

    // Task variables
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;
    Task_Handle movementTaskHandle;
    Task_Params movementTaskParams;

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
    ledHandle = PIN_open( &ledState, ledConfig );
    if(!ledHandle) {
       System_abort("Error initializing LED pin\n");
    }

    // Painonappi käyttöön ohjelmassa
    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if(!buttonHandle) {
       System_abort("Error initializing button pin\n");
    }

    // Painonapille keskeytyksen käsittellijä
    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
       System_abort("Error registering button callback function");
    }

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

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskParams.priority=2;
    uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
    if (uartTaskHandle == NULL) {
        System_abort("Task create failed!");
    }
    */

    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("Pin open failed!");
    }

    Task_Params_init(&movementTaskParams);
    movementTaskParams.stackSize = STACKSIZE;
    movementTaskParams.stack = &movementTaskStack;
    movementTaskHandle = Task_create(movementTaskFxn, &movementTaskParams, NULL);
    if (movementTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    /* Sanity check */
    System_printf("Hello world!\n");
    System_printf("tickperiod is %d\n", Clock_tickPeriod);
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
