#include <msp430.h>
#include "ws2812.h"

// WS2812 takes GRB format
typedef struct {
    u_char green;
    u_char red;
    u_char blue;
} LED;

static LED leds[NUM_LEDS] = { { 0, 0, 0 } };

// Initializes everything needed to use this library. This clears the strip.
void initStrip() {
    P1SEL0 |= OUTPUT_PIN;    // configure output pin as SPI output
    P1SEL1 |= OUTPUT_PIN;
    UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC; // 3-pin, MSB, 8-bit SPI master
    UCB0CTL1 |= UCSSEL__SMCLK;   // SMCLK source (16 MHz)
    UCB0BR0 = 3;            // 16 MHz / 3 = .1875 us per bit
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;   // Initialize USCI state machine
    clearStrip();           // clear the strip
}


// Sets the color of a certain LED (0 indexed)
void setLEDColor(u_int p, u_char r, u_char g, u_char b) {
    leds[p].green = g;
    leds[p].red = r;
    leds[p].blue = b;
}

// Send colors to the strip and show them. Disables interrupts while processing.
void showStrip() {
    __bic_SR_register(GIE);  // disable interrupts

    // send RGB color for every LED
    unsigned int i, j;
    for (i = 0; i < NUM_LEDS; i++) {
        u_char *rgb = (u_char *)&leds[i]; // get GRB color for this LED

        // send green, then red, then blue
        for (j = 0; j < 3; j++) {
            u_char mask = 0x80;    // b1000000

            // check each of the 8 bits
            while (mask != 0) {
                //while (!(IFG2 & UCB0TXIFG))
                  //  ;    // wait to transmit
                if (rgb[j] & mask) {        // most significant bit first
                    UCB0TXBUF = HIGH_CODE;  // send 1
                } else {
                    UCB0TXBUF = LOW_CODE;   // send 0
                }

                mask >>= 1;  // check next bit
            }
        }
    }

    // send RES code for at least 50 us (800 cycles at 16 MHz)
    _delay_cycles(800);

    __bis_SR_register(GIE);    // enable interrupts
}

// Clear the color of all LEDs (make them black/off)
void clearStrip() {
    fillStrip(0x00, 0x00, 0x00);  // black
}

// Fill the strip with a solid color. This will update the strip.
void fillStrip(u_char r, u_char g, u_char b) {
    int i;
    for (i = 0; i < NUM_LEDS; i++) {
        setLEDColor(i, r, g, b);  // set all LEDs to specified color
    }
    showStrip();  // refresh strip
}

void initClock(void) {
    if (CALBC1_16MHZ == 0xFF || CALDCO_16MHZ == 0xff)
      while(1); // Erased calibration data? Trap!

    // configure clock to 16 MHz
    BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
    DCOCTL = CALDCO_16MHZ;
}

void cycle(u_char r, u_char g, u_char b) {
    int i;
    for (i = NUM_LEDS - 1; i > 0; i--) {
        u_char red = leds[i - 1].red;
        u_char green = leds[i - 1].green;
        u_char blue = leds[i - 1].blue;
        setLEDColor(i, red, green, blue);
    }
    setLEDColor(0, r, g, b);
    showStrip();
}

void static(u_char r, u_char g, u_char b) {
    int i;
    for (i = 0; i < NUM_LEDS / 3; i++) {
        setLEDColor(i, r, 0, 0);
    }
    for (i = NUM_LEDS / 3; i < 2 * NUM_LEDS / 3; i++) {
        setLEDColor(i, 0, g, 0);
    }
    for (i = 2 * NUM_LEDS / 3; i < NUM_LEDS; i++) {
        setLEDColor(i, 0, 0, b);
    }
    showStrip();
}

