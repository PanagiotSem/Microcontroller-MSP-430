#include <msp430.h>

#define SignalLen   16u
#define KernelLen   5u
#define ResultLen   (SignalLen + KernelLen - 1u)

#define SMCLK_HZ    1000000UL
#define LED_PIN_BIT BIT0

static volatile unsigned int signal[SignalLen];
static const double kernel[KernelLen] = { 0.1, 0.15, 0.5, 0.15, 0.1 };
static volatile double result[ResultLen];

static volatile unsigned int  writeIndex    = 0;
static volatile unsigned char newSampleFlag = 0;
static volatile unsigned char systemEnabled = 1;   /* simulator: always enabled */

static volatile unsigned long g_proc_ticks = 0;
static volatile unsigned long g_proc_us    = 0;
static volatile unsigned long g_max_hz     = 0;

static void Clock_Init_1MHz(void)
{
#ifdef CALBC1_1MHZ
    DCOCTL  = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
#endif
}

static void GPIO_Init(void)
{
    P1DIR |= LED_PIN_BIT;
    P1OUT &= (unsigned char)~LED_PIN_BIT;
}

static void Buffers_Init(void)
{
    unsigned int i;
    for (i = 0; i < SignalLen; ++i) signal[i] = 0;
    for (i = 0; i < ResultLen; ++i) result[i] = 0.0;
    writeIndex = 0;
    newSampleFlag = 0;
}

static void Timing_TimerA_Init(void)
{
    TACTL = MC_0;
    TACTL = TASSEL_2 | ID_0 | MC_2 | TACLR;   /* SMCLK, /1, continuous, clear */
}

static inline unsigned int Timing_Ticks_Now(void)
{
    return TAR;
}

static inline unsigned long Timing_Ticks_To_us(unsigned long ticks)
{
    return (ticks * 1000000UL) / SMCLK_HZ;
}

static void FIR_Convolution(void)
{
    unsigned int k, i;
    unsigned int baseIndex = writeIndex;

    for (k = 0; k < ResultLen; ++k) {
        double acc = 0.0;

        for (i = 0; i < SignalLen; ++i) {
            int j = (int)k - (int)i;
            if (j < 0 || j >= (int)KernelLen) continue;

            unsigned int phys = baseIndex + i;
            if (phys >= SignalLen) phys -= SignalLen;

            acc += (double)signal[phys] * kernel[j];
        }
        result[k] = acc;
    }
}

static void BusyDelay(volatile unsigned int n)
{
    while (n--) __no_operation();
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    Clock_Init_1MHz();
    GPIO_Init();
    Buffers_Init();
    Timing_TimerA_Init();

    unsigned int fake = 0;

    while (1) {

        /*"Sleep" / wait period (simulates sampling period)*/
        BusyDelay(20000);

        /* "ADC sample ready" (simulates ISR) */
        fake = (fake + 123u) & 0x0FFFu;

        {
            unsigned int idx = writeIndex;
            signal[idx] = fake;
            idx++;
            if (idx >= SignalLen) idx = 0;
            writeIndex = idx;
        }

        newSampleFlag = 1;

        /* main processing ONCE per sample */
        if (systemEnabled && newSampleFlag) {
            unsigned int t0, t1;
            unsigned long dt;

            /* no real interrupts, but keep the structure similar */
            newSampleFlag = 0;

            t0 = Timing_Ticks_Now();
            FIR_Convolution();
            t1 = Timing_Ticks_Now();

            dt = (unsigned long)((unsigned int)(t1 - t0));
            g_proc_ticks = dt;
            g_proc_us    = Timing_Ticks_To_us(dt);
            g_max_hz     = (g_proc_us > 0) ? (1000000UL / g_proc_us) : 0;

            /* Visible activity */
            P1OUT ^= LED_PIN_BIT;
        }
    }
}















