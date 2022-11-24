/*
 * uart.h
 *
 *  Created on: 18.11.2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_MESSAGING_UART_H_
#define FUNCTIONS_MESSAGING_UART_H_

void UART_registerTask();

void UART_notifyMpuDataReady();
void UART_notifyAmbientDataReady();

void sendMessage(char msg[], ...);

#endif /* FUNCTIONS_MESSAGING_UART_H_ */
