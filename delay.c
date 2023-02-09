#include "typedef.h"
#include "memreg.h"

#define SIMULATOR 1


/**
 * @brief Blocks execution for ~250 nanoseconds.
 */
void delay_250ns(void)
{
    // A value of 41 will delay for ~250ns.
    const uint DELAY_VALUE = 168/4 - 1;

    *(volatile uint*)STK_CTRL = 0;
    *(volatile uint*)STK_LOAD = DELAY_VALUE;
    *(volatile uint*)STK_VAL  = 0;
    *(volatile uint*)STK_CTRL = 5;

    // Wait until the shit hits 0.
    while ( *(volatile uint*)(STK_CTRL & 0x00010000) == 0 );

    *(volatile uint*)STK_CTRL = 0;
}


/**
 * @brief Delay the execution of code for a specified amount of mikroseconds.
 * @param us The duration to sleep in mikroseconds.
 */
void delay_mikro(uint us)
{
#if SIMULATOR
    us /= 1000;
    us++;
#endif
    while (us > 0)
    {
        delay_250ns();
        delay_250ns();
        delay_250ns();
        delay_250ns();

        us--;
    }
}


/**
 * @brief Delay the execution of code for a specified amount of milliseconds.
 * @param ms The duration to sleep in milliseconds.
 */
void delay_milli(uint ms)
{
#if SIMULATOR
    ms /= 1000;
    ms++;
#endif
    for (uint i = 0; i < 1000 * ms; i++)
    {
        delay_250ns();
        delay_250ns();
        delay_250ns();
        delay_250ns();
    }
}
