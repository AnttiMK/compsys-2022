/*
 * movement.h
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_
#define FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

#include "Board.h"
#include "sensors/mpu9250.h"

Void movementTask(UArg arg0, UArg arg1);

Void Movement_registerTask();

#endif /* FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_ */
