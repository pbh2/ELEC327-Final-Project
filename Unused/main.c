#include <msp430.h>
#include "ws2812.h"
#include "buttons.h"

void gradualFill(u_int n, u_char r, u_char g, u_char b);

bool state = true; //true = static mode, false = cycling mode
volatile int ch1;
volatile int ch2;
volatile int ch3;

void initADC(void) {
    ADC10CTL0 &= ~ENC; //disable during configuration
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_0 + CONSEQ_2;
    ADC10CTL0 |= ENC + ADC10SC;
}

void sampleADC(void) {
    // Sample P1.0
    ADC10CTL1 &= ~INCH_0;
    ADC10CTL1 |= INCH_0;
    while (ADC10CTL1 & ADC10BUSY);
    ch1 = ADC10MEM;

    // Sample P1.1
    ADC10CTL1 &= ~INCH_0;
    ADC10CTL1 |= INCH_1;
    while (ADC10CTL1 & ADC10BUSY);
    ch2 = ADC10MEM;

    // Sample P1.2
    ADC10CTL1 &= ~INCH_0;
    ADC10CTL1 |= INCH_2;
    while (ADC10CTL1 & ADC10BUSY);
    ch3 = ADC10MEM;
}

int getColor(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 1023) {
        return 255;
    } else {
        return value / 4;
    }
}

int main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    //initialize ADC pins
    initADC();

    // initialize LED strip
    initStrip();

    initializeButtons();

    // set strip color red
    fillStrip(0xFF, 0x00, 0x00);

    showStrip();

    P1SEL |= BIT0 + BIT1 + BIT2; //configure p1.0, p1.1, and p1.2 to inputs
    P1SEL2 |= BIT0 + BIT1 + BIT2;

    while (1) {
        update_buttons();
        if (bneg) {
            state = !state;
            clearStrip();
        }

        sampleADC();

        if (state) {//code for static mode
            int r = getColor(ch1);
            int g = getColor(ch2);
            int b = getColor(ch3);
            static(r, g, b);
        } else {//code for cycling mode
            if ((ch1 > ch2) && (ch1 > ch3)) {
                int brightness = getColor(ch1);
                cycle(brightness, 0, 0);
            } else if (ch2 > ch3) {
                int brightness = getColor(ch2);
                cycle(0, brightness, 0);
            } else {
                int brightness = getColor(ch3);
                cycle(0, 0, brightness);
            }
        }
    }
}

