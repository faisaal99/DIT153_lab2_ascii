#include "typedef.h"
#include "memreg.h"

#define SIMULATOR 1


/**
 * @brief Locks execution for ~250 nanoseconds.
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
 * @brief Delay the
 * @param us
 */
void delay_mikro(uint us)
{
    while (us > 0)
    {
        delay_250ns();
        delay_250ns();
        delay_250ns();
        delay_250ns();

        us--;
    }
}


void delay_milli(uint ms)
{
#if SIMULATOR
    ms /= 1000;
    ms++;
#endif
    delay_mikro( 1000 * ms );
}
