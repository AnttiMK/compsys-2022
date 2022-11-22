/*
 * movement.h
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_
#define FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_

#include <xdc/std.h>
#include "stdbool.h"

enum mpuState {
    INITIALIZING, STANDBY, COLLECTING
};

void MovementSensor_registerTask();

void MovementSensor_collectData();

enum mpuState MovementSensor_getState();

#endif /* FUNCTIONS_MOVEMENTSENSOR_MOVEMENTSENSOR_H_ */
