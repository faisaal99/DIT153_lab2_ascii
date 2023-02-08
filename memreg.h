#ifndef _KEYB_H_
#define _KEYB_H_


#include "typedef.h"


/* GPIOD */

#define GPIOD            0x40020C00
#define GPIOD_MODER_LOW  (GPIOD)
#define GPIOD_MODER_HIGH (GPIOD +  2)
#define GPIOD_OTYPER     (GPIOD +  4)
#define GPIOD_OSPEEDR    (GPIOD +  8)
#define GPIOD_PUPDR      (GPIOD + 12)
#define GPIOD_IDR        (GPIOD + 16)
#define GPIOD_ODR        (GPIOD + 20)
#define GPIOD_BSRR       (GPIOD + 24)


/* GPIOE */

#define GPIOE            0x40021000
#define GPIOE_MODER_LOW  (GPIOE)
#define GPIOE_MODER_HIGH (GPIOE +  2)
#define GPIOE_OTYPER     (GPIOE +  4)
#define GPIOE_OSPEEDR    (GPIOE +  8)
#define GPIOE_PUPDR      (GPIOE + 12)
#define GPIOE_IDR        (GPIOE + 16)
#define GPIOE_ODR        (GPIOE + 20)
#define GPIOE_BSRR       (GPIOE + 24)


/* GPIO REGISTER */

/**
 * @brief A struct defining the layout of a GPIO register. Do NOT use the
 *        fields beginning with an underscore, as they represent reserved
 *        memory.
 */
typedef struct
{
    union
    {
        uint MODER;
        struct
        {
            ushort MODER_LOW;
            ushort MODER_HIGH;
        };
    };
    ushort OTYPER;
    ushort _otyper_reserved;
    uint   OSPEEDR;
    uint   PUPDR;
    union
    {
        ushort IDR;
        struct
        {
            uchar IDR_LOW;
            uchar IDR_HIGH;
        };
    };
    ushort _idr_reserved;
    union
    {
        ushort ODR;
        struct
        {
            uchar ODR_LOW;
            uchar ODR_HIGH;
        };
    };
    ushort _odr_reserved;
    uint   BSRR;
} gpio_t;


/* SysTick */

#define SYSTICK   0xE000E010
#define STK_CTRL  (SYSTICK)
#define STK_LOAD  (SYSTICK +  4)
#define STK_VAL   (SYSTICK +  8)
#define STK_CALIB (SYSTICK + 12)


/* CONTROL REGISTER BITS */

#define B_E      0x40 /* Enable-signal          */
#define B_SELECT 0x04 /* Choose ASCII-display   */
#define B_RW     0x02 /* 1 = Write, 0 = Read    */
#define B_RS     0x01 /* 1 = Data,  0 = Control */


#endif // _KEYB_H_