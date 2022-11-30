/*
 * sensors.h
 *
 *  Created on: 30 Nov 2022
 *      Author: Antti
 */

#ifndef FUNCTIONS_SENSORS_SENSORS_H_
#define FUNCTIONS_SENSORS_SENSORS_H_

extern double Sensors_ambientLight;
extern double Sensors_temperature;
extern double Sensors_airPressure;
extern double Sensors_bmpTemp;

extern float Sensors_mpuData[7][100];

void Sensors_registerTask();

#endif /* FUNCTIONS_SENSORS_SENSORS_H_ */
