__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");      /* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");                 /* call main */
__asm__ volatile(".L1: B .L1\n");               /* never return */
}

// =============================================================================
//                         INCLUDES & PRE-PROCESSOR

#include "typedef.h"
#include "memreg.h"


// =============================================================================
//                                GLOBAL DATA

volatile gpio_t *gpio_d = (volatile gpio_t*)GPIOD;
volatile gpio_t *gpio_e = (volatile gpio_t*)GPIOE;


// =============================================================================
//                                  SET-UP

void app_init(void)
{
    // Start clocks for port D and port E.
    *(volatile ulong*)0x40023830 = 0x18;

    gpio_e->MODER   = 0x55555555;
    gpio_e->OSPEEDR = 0x55555555;
}


// =============================================================================
//                                 FUNCTIONS

extern void delay_250ns(void);
extern void delay_mikro(uint);
extern void delay_milli(uint);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief TODO;
 * @param x
 */
void ascii_ctrl_bit_set(uchar x)
{
    uchar c = gpio_e->ODR_LOW;
    gpio_e->ODR_LOW = B_SELECT | x | c;
}


/**
 * @brief TODO;
 * @param x
 */
void ascii_ctrl_bit_clear(uchar x)
{
    uchar c = gpio_e->ODR_LOW;
    c = c & ~x;
    gpio_e->ODR_LOW = B_SELECT | c;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void  ascii_write_cmd(uchar cmd);
void  ascii_write_data(uchar data);
uchar ascii_read_status(void);
uchar ascii_read_data(void);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief TODO;
 * @param byte
 */
void ascii_write_controller(uchar byte)
{
    // Delay 40ns

    ascii_ctrl_bit_set(B_E);
    gpio_e->ODR_HIGH = byte;


    // Delay 230ns

    ascii_ctrl_bit_clear(B_E);

    // Delay 10ns
}

/**
 * @brief TODO;
 * @param byte
 */
uchar ascii_read_controller()
{
    ascii_ctrl_bit_set(B_E);


    // Delay 360ns

    uchar rv = gpio_e->ODR_HIGH;
    ascii_ctrl_bit_clear(B_E);

    return rv;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief TODO;
 * @param cmd
 */
void ascii_write_cmd(uchar cmd)
{
    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_clear(B_RW);

    ascii_write_controller(cmd);
}


/**
 * @brief TODO;
 * @param data
 */
void ascii_write_data(uchar data)
{
    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_clear(B_RW);

    ascii_write_controller(data);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief TODO;
 */
uchar ascii_read_status(void)
{
    gpio_e->MODER_HIGH = 0x0000;

    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_set(B_RW);

    uchar rv = ascii_read_controller();

    gpio_e->MODER_HIGH = 0x5555;

    return rv;
}


/**
 * @brief TODO;
 */
uchar ascii_read_data(void)
{
    gpio_e->MODER_HIGH = 0x0000;

    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_set(B_RW);

    uchar rv = ascii_read_controller();

    gpio_e->MODER_HIGH = 0x5555;

    return rv;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief TODO;
 */
void ascii_init(void)
{
    //
}


/**
 * @brief TODO;
 * @param x
 * @param y
 */
void ascii_goto(int x, int y)
{

}


/**
 * @brief TODO;
 * @param c
 */
void ascii_write_char(uchar c)
{

}


// =============================================================================
//                                    MAIN

void main(void)
{
    // Data used in the application.
    char *s;
    const char test1[] = "Alfanumerisk ";
    const char test2[] = "Display - test";

    // Program set-up.
    app_init();
    ascii_init();

    // Move to the 1st row and set the output-pointer to the 1st string.
    ascii_goto(1, 1);
    s = test1;

    // Print out each character.
    while (*s)
        ascii_write_char(*s++);

    // Move to the 2nd row and set the output-pointer to the 2nd string.
    ascii_goto(1, 2);
    s = test2;

    // Again, print out each character.
    while (*s)
        ascii_write_char(*s++);

    return 0;
}

