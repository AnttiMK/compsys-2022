/*
 * night.h
 *
 *  Created on: 19.11.2022
 *      Author: Matias
 */

#ifndef FUNCTIONS_NIGHT_NIGHT_H_
#define FUNCTIONS_NIGHT_NIGHT_H_

#include <ti/sysbios/knl/Task.h>
#include "sensors/opt3001.h"
#include "Board.h"
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

Void nightTask(UArg arg0, UArg arg1);

Void night_registerTask();

#endif /* FUNCTIONS_NIGHT_NIGHT_H_ */
