/*
 * adc_3pin.h
 *
 *  Created on: May 2, 2023
 *      Author: seanhamilton
 */

#ifndef ADC_3PIN_H_
#define ADC_3PIN_H_

extern unsigned int adc[3];  // This will hold the different pin values
extern unsigned int pin0;
extern unsigned int pin1;
extern unsigned int pin2;

void Setup_HW(void);        // Setup watchdog timer, clockc, ADC ports
void Read_Acc(void);    // This function reads the ADC and stores the pin values

#endif /* ADC_3PIN_H_ */
