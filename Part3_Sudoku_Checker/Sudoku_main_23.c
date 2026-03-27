#include <msp430.h>

typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;

#define SUDOKU_N           9u

#define IRQ_PIN_BIT        BIT7   /* P1.7 */

#define O_ROWS_ALL_OK      BIT0
#define O_COLS_ALL_OK      BIT1
#define O_SUBS_ALL_OK      BIT2
#define O_OVERALL_OK       BIT3
#define O_INIT_DONE        BIT4

static uint8_t sudoku_array[SUDOKU_N][SUDOKU_N];
static volatile uint8_t g_start_flag = 0u;

/* PRNG */
static uint16_t lfsr = 0xACE1u;
static uint16_t prng16(void)
{
    uint16_t lsb = (uint16_t)(lfsr & 1u);
    lfsr >>= 1;
    if (lsb) lfsr ^= 0xB400u;
    return lfsr;
}

static uint16_t bit(uint8_t v)
{
    return (uint16_t)(1u << (v - 1u));
}

static void sudoku_random_init(void)
{
    uint8_t r, i;
    for (r = 0u; r < SUDOKU_N; r++) {
        uint8_t tmp[SUDOKU_N];

        for (i = 0u; i < SUDOKU_N; i++) tmp[i] = (uint8_t)(i + 1u);

        for (i = (uint8_t)(SUDOKU_N - 1u); i > 0u; i--) {
            uint8_t j = (uint8_t)(prng16() % (uint16_t)(i + 1u));
            uint8_t t = tmp[i];
            tmp[i] = tmp[j];
            tmp[j] = t;
        }

        for (i = 0u; i < SUDOKU_N; i++) sudoku_array[r][i] = tmp[i];
    }
}

static uint8_t check_row(uint8_t r)
{
    uint16_t seen = 0u;
    uint8_t c;

    for (c = 0u; c < SUDOKU_N; c++) {
        uint8_t v = sudoku_array[r][c];
        if (v < 1u || v > 9u) return 0u;
        if (seen & bit(v)) return 0u;
        seen |= bit(v);
    }
    return (seen == 0x01FFu);
}

static uint8_t check_col(uint8_t c)
{
    uint16_t seen = 0u;
    uint8_t r;

    for (r = 0u; r < SUDOKU_N; r++) {
        uint8_t v = sudoku_array[r][c];
        if (v < 1u || v > 9u) return 0u;
        if (seen & bit(v)) return 0u;
        seen |= bit(v);
    }
    return (seen == 0x01FFu);
}

static uint8_t check_sub(uint8_t s)
{
    uint16_t seen = 0u;
    uint8_t r0 = (uint8_t)((s / 3u) * 3u);
    uint8_t c0 = (uint8_t)((s % 3u) * 3u);
    uint8_t r, c;

    for (r = r0; r < (uint8_t)(r0 + 3u); r++) {
        for (c = c0; c < (uint8_t)(c0 + 3u); c++) {
            uint8_t v = sudoku_array[r][c];
            if (v < 1u || v > 9u) return 0u;
            if (seen & bit(v)) return 0u;
            seen |= bit(v);
        }
    }
    return (seen == 0x01FFu);
}

static void gpio_init(void)
{
    /* Port2 outputs */
    P2DIR |= (O_ROWS_ALL_OK | O_COLS_ALL_OK | O_SUBS_ALL_OK | O_OVERALL_OK | O_INIT_DONE);
    P2OUT  = 0x00;

    /* Port1: P1.7 input */
    P1DIR &= (uint8_t)~IRQ_PIN_BIT;

    /* P1.7 falling edge interrupt */
    P1IES |= IRQ_PIN_BIT;
    P1IFG &= (uint8_t)~IRQ_PIN_BIT;
    P1IE  |= IRQ_PIN_BIT;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    if (P1IFG & IRQ_PIN_BIT) {
        P1IFG &= (uint8_t)~IRQ_PIN_BIT;
        g_start_flag = 1u;
        __bic_SR_register_on_exit(LPM0_bits);
    }
}

int main(void)
{
    uint8_t rows_ok, cols_ok, subs_ok, i;

    WDTCTL = WDTPW | WDTHOLD;
    gpio_init();
    __enable_interrupt();

    while (1) {
        __bis_SR_register(LPM0_bits | GIE);

        if (g_start_flag) {
            g_start_flag = 0u;

            __disable_interrupt();

            P2OUT &= (uint8_t)~O_INIT_DONE;
            sudoku_random_init();
            P2OUT |= O_INIT_DONE;

            rows_ok = cols_ok = subs_ok = 1u;
            for (i = 0u; i < SUDOKU_N; i++) {
                if (!check_row(i)) rows_ok = 0u;
                if (!check_col(i)) cols_ok = 0u;
                if (!check_sub(i)) subs_ok = 0u;
            }

            P2OUT = O_INIT_DONE;
            if (rows_ok) P2OUT |= O_ROWS_ALL_OK;
            if (cols_ok) P2OUT |= O_COLS_ALL_OK;
            if (subs_ok) P2OUT |= O_SUBS_ALL_OK;
            if (rows_ok && cols_ok && subs_ok) P2OUT |= O_OVERALL_OK;

            __enable_interrupt();
        }
    }
}



