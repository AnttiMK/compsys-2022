/*
 * uart.h
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_MESSAGING_UART_H_
#define FUNCTIONS_MESSAGING_UART_H_

#include <functions/movementSensor/dataParser.h>

void UART_registerTask();

void UART_notifyMpuDataReady(int index);
void UART_notifyLightDataReady();
void UART_notifyTempDataReady();
void UART_notifyPresDataReady();
void UART_notifyBatteryDataReady();
void UART_notifyMoveRecognized(Move move);
void UART_notifyEat();

void sendMessage(char msg[], ...);

#endif /* FUNCTIONS_MESSAGING_UART_H_ */
