/*
 * night.h
 *
 *  Created on: 19.11.2022
 *      Author: Matias
 */

#ifndef FUNCTIONS_AMBIENTLIGHTSENSOR_AMBIENTLIGHT_H_
#define FUNCTIONS_AMBIENTLIGHTSENSOR_AMBIENTLIGHT_H_

#include <ti/sysbios/knl/Task.h>
#include "sensors/opt3001.h"
#include "Board.h"
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

Void nightTask(UArg arg0, UArg arg1);

Void AmbientLight_registerTask();

#endif /* FUNCTIONS_AMBIENTLIGHTSENSOR_AMBIENTLIGHT_H_ */
