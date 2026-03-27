#ifndef TIMERA_HAL_23_H
#define TIMERA_HAL_23_H    

#include <msp430.h>  


// Clock Source 
typedef enum {
  TIMER_A_CLOCK_ACLK  = TASSEL_1,           // Auxiliary Clock 
  TIMER_A_CLOCK_SMCLK = TASSEL_2,           // Sub-main Clock
  TIMER_A_CLOCK_INCLK = TASSEL_3            // External Clock
} TimerA_ClockSource_t;


// Timer Mode Control (for values)
typedef enum {
  TIMER_A_STOP       = MC_0,                // Timer is HALTED 
  TIMER_A_UP_MODE    = MC_1,                // Timer repeatedly counts from ZERO to the value of TACCR0 
  TIMER_A_CONTINUOUS = MC_2,                // Timer repeatedly counts from ZERO to 0xFFFF
  TIMER_A_UPDOWN     = MC_3                 // Timer repeatedly counts from ZERO to the value of TACCR0 and back to ZERO (creating triangles) 
} TimerA_Mode_t; 


// Slowing frequency down to count pulses (so it doesn't overflow)
typedef enum {
  TIMER_A_DIV_1   = ID_0,                   // Divides frequency by 0 (Speed remains the same)
  TIMER_A_DIV_2   = ID_1,                   // Divides frequency by 2
  TIMER_A_DIV_4   = ID_2,                   // Divides frequency by 4
  TIMER_A_DIV_8   = ID_3                    // Divides frequency by 8
} TimerA_Divider_t; 


// The "menu" for our costumer! The user fills this struct in main() 
typedef struct {
  TimerA_ClockSource_t    clockSource;      // Selects the clock source 
  TimerA_Divider_t        clockDivider;     // Selects the frequency divider
  TimerA_Mode_t           mode;             // Selects the counting mode        
  unsigned int            period;           // The value for TACCR0 
} TimerA_Config_t;

/*
* Initializes timer based on the configuration struct
*/
void TimerA_Init(const TimerA_Config_t *config); 

/*
* Starts the timer operation
*/
void TimerA_Start(void);

/*
* Stops the timer operation
*/
void TimerA_Stop(void);

/*
* Reads the current value of the Time Counter 
*/
unsigned int TimerA_ReadCounter(void);

/*
* Resets the Time Counter to 0
*/
void TimerA_ResetCounter(void);

/*
* Uses up mode: Timer counts up to "period" and resets
*/
void TimerA_PWM_Init(unsigned int period, unsigned int dutyCycle); 

#endif 
  

