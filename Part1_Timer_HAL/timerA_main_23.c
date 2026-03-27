#include <msp430.h>
#include "timerA_HAL_23.h"

static void GPIO_Init (void) {

    P1DIR |= BIT2;  //Set P1.2 as output
    P1SEL |= BIT2;  //Select peripheral function instead of GPIO

}

int main (void) {
    WDTCTL = WDTPW | WDTHOLD;   //Stop the watchdog timer
    GPIO_Init ();   //Initialize GPIO for PWM output on P1.2

    // Configure TimerA base settings via HAL
    unsigned int period = 1000u;
    unsigned int dutyCycle = 500u; //50% duty cycle

    TimerA_Config_t timerCfg;

    timerCfg.clockSource = TIMERA_CLOCK_SMCLK;  //Use SMCLK as Timer_A clock source
    timerCfg.clockDivider = TIMERA_DIV_1;   //No clock division
    timerCfg.mode = TIMERA_UP_MODE; //Up mode
    timerCfg.period = period;   //Load TACCR0

    TimerA_Init(&timerCfg);

    TimerA_PWM_Init (period, dutyCycle);    //Configure PWM on CCR1 via HAL

    TimerA_Start();  //Start Timer_A using the mode stored in TimerA_Init()

    __bis_SR_register(LPM0_bits);   //Enter LPM0
    __no_operation();   //For debugger breakpoint

    return 0;

}