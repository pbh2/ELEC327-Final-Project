#include <msp430.h>

//#define SWITCH_1_MASK 0x20 // Port 2.5
//#define SWITCH_2_MASK 0x80 // Port 1.7

int b_pressed = 0;


int bpos = 0;
int bneg = 0;


void initialize_buttons() {
    P2DIR &= ~BIT2; // set to input
    P2REN = BIT2; // enable pullup/down resistors
    P2OUT = BIT2; // set resistors to pull up

}

void update_buttons() {
    // Reset possible events
    int bpos = 0;
    int bneg = 0;

    if (!b_pressed && !P2IN)){      // neg-edge of switch 1 (switch is default high)
        b_pressed = 1;
        bneg = 1;
    } else if (b_pressed && P2IN) {// pos-edge of switch 1
        b_pressed = 0;
        bpos = 1;
    }

}
