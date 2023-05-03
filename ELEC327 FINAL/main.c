#include <msp430.h> 
#include "adc_3pin.h"
#include "PWM.h"

/**
 * main.c
 */

void init_wdt(void){
    BCSCTL3 |= LFXT1S_2;     // ACLK = VLO
    WDTCTL = WDT_ADLY_1000;    // WDT 1000 ms = 1s
    IE1 |= WDTIE;            // Enable WDT interrupt
}

void init_button(void){
    P2DIR &= BIT2; //Button is connected to pin 2.2
    P2REN = BIT2; //Enable pullup resistor
    P2OUT = BIT2; //Set resistor to pullup
    P2IES = BIT2; //
    P2IFG &= ~BIT2;
    P2IE = BIT2;
}


unsigned int read_val0, read_val1, read_val2; //Temporary variables to hold ADC read
unsigned int PWM_switch0, PWM_switch1, PWM_switch2;
unsigned int mode_toggle;

const unsigned int vis_period = (1000000/120); //120 Hz, set so there is no blinking in the LEDs

int main(void)
{
  BCSCTL1 = CALBC1_1MHZ; //Set basic clock system to 1MHz
  DCOCTL = CALDCO_1MHZ; //Use calibration data for digitally controlled oscillator

  initPWM(vis_period); //Set PWM pins for LEDs
  Setup_HW(); //Set up ADC
  init_wdt(); //Initialize WDT

  PWM_switch0 = 100;
  PWM_switch1 = 1000;
  PWM_switch2 = 5000;
  mode_toggle = 1;

  while (1)
  {
      if(mode_toggle){
        Read_Acc(); // This function reads the ADC and stores the pin0, pin1 and pin2 values
        read_val0 = pin0; //ADC value from pin 1.0
        read_val1 = pin1; //ADC value from pin 1.1
        read_val2 = pin2; //ADC value from pin 1.2

        //Code is set up for pin 1.2, if using programmable LEDs, set up for all three ADC pins
        if(read_val2 <= 100){
            set_PWM(vis_period / 4); // 25% duty cycle
        }
        else if(read_val2 <= 1000){
            set_PWM(vis_period / 2); // 50% duty cycle
        }
        else{
            set_PWM(0.75 * vis_period);
        }
      }
      __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
   }
}

//Three ISRs: ADC, button press and watch dog timer

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

// WDT ISR
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    __bic_SR_register_on_exit(CPUOFF);  // Used to run program
}

#pragma vector = PORT2_VECTOR
__interrupt void button(void)
{
    if (!(P2IN & BIT2)){
        mode_toggle ^= 1; //Toggle mode between lights on and lights off
    }
    P2IFG &= ~BIT2; ///Clear interrupt flag to allow for another interrupt
}
