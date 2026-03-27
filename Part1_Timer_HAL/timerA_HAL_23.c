#include "msp430.h"
#include "timerA_Hal_23.h"


// Global variable to store the selected TimerA mode
static TimerA_Mode_t g_timerMode = TIMER_ASTOP ; 


void TimerA_Init(const TimerA_Config *config) {
  
  if(config == 0){
    // Safety check: Do nothing if NULL pointer is passed
    return   
  }
  
  // Stop TimerA and clear previous configuration
  TACTL = 0;
  
  // Set timer period (in Up mode, TAR counts from 0 to TACCR0)
  TACCR0 = config->period; 
  
  // Store selected mode for later use in TimerA_Start()
  g_timerA_Mode = config->mode; 
  
  // Configure TimerA clock source, divider and clear TAR
  // Cast enums to unsigned int to aviod compiler warnings
  TACTL = (unsinged int) config->clockSource | (unsigned int) config->clockDivider | TACLR;
  
}


void TimerA_Start(void) {
  
  // Clear MC bits: put TimerA into STOP mode first
  TACTL &= ~MC3;                 // MC_3 mask clears both mode bits
  TACTL |= g_timerMode;          // Apply the mode
  
}


void TimerA_Stop(void) {
  
  TACTL &= ~MC3;        // MC_0: Stop mode
  
}


unsigned int TimerA_ReadCounter(void) {

  return TAR;                  // Holds the current count of Timer A
  
}

void TimerA_ResetCounter(void) {
  
  TACTL |= TACLR;             // Clears the TAR register to 0
  
}

void TimerA_PWM_Init(unsigned int period, unsigned int dutyCycle) {
  
  // Ensure duty cycle does not exceed the period  
  if(dutyCycle > period){
    dutyCycle = period;
  }

  TACTL &= ~MC_3;                     // Stop TimerA (clear MC bits) 
  
  TACCR0 = period;                    // Timer counts from 0 up to this value
  
  TACCR1 = dutyCycle;                 // While the timer is less than this value, output stays HIGH
  
  TACCTL1 = OUTMOD_7;                 // Output goes HIGH when timer resets to 0, output goes LOW when timer reaches TACCR1 value
  
}


