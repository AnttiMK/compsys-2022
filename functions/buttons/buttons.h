/*
 * buttons.h
 *
 *  Created on: 16.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_BUTTONS_BUTTONS_H_
#define FUNCTIONS_BUTTONS_BUTTONS_H_

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "Board.h"

void Buttons_registerTasks();


#endif /* FUNCTIONS_BUTTONS_BUTTONS_H_ */
