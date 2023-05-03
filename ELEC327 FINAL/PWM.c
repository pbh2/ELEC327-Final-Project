/*
 * PWM.c
 *
 *  Created on: May 2, 2023
 *      Author: seanhamilton
 */
// initializes P2.0 to be the output for a PWM wave, sets the period of the PWM wave
#include <msp430.h>

void initPWM(int period) {
    P2DIR |= BIT0;
    P2SEL |= BIT0;

    TA0CCTL0 = OUTMOD_7; // output mode reset/set
    TA0CTL = TASSEL_2 + MC_1; // SMCLK, up-mode

    TA0CCR0 = period; // set period of PWM wave

    TA0CCR1 = period / 2; // start with 50% duty cycle
}

// sets the duty cycle of the PWM wave, input should be an int less than the value of the period
void set_PWM(int duty) {
    TA0CCR1 = duty;
}



