/*
 * uart.h
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_MESSAGING_UART_H_
#define FUNCTIONS_MESSAGING_UART_H_

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/UART.h>
#include "Board.h"

void UART_registerTask();

#endif /* FUNCTIONS_MESSAGING_UART_H_ */
