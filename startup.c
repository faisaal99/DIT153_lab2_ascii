__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");      /* Set stack    */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");                 /* Call main    */
__asm__ volatile(".L1: B .L1\n");               /* Never return */
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
 * @brief Set some specific bits to 1 in the control register.
 * @param x A char containing enabled bits that'll be set to 1 in the control
 *          register.
 */
void ascii_ctrl_bit_set(uchar x)
{
    uchar c = gpio_e->ODR_LOW;
    gpio_e->ODR_LOW = B_SELECT | x | c;
}


/**
 * @brief Set some specific bits to 0 in the control register.
 * @param x A char containing enabled bits that'll be set to 0 in the control
 *          register.
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
 * @brief Write a byte to the output. This function abstracts away
 *        synchronization with the hardware.
 * @param byte The byte to output.
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
 * @brief Read a byte from the input. This function abstracts away
 *        synchronization from the hardware.
 */
uchar ascii_read_controller()
{
    ascii_ctrl_bit_set(B_E);


    // Delay 360ns

    uchar rv = gpio_e->IDR_HIGH;
    ascii_ctrl_bit_clear(B_E);

    return rv;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Write a command to the ASCII display. This is low-level. Use
 *        ascii_command() instead.
 * @param cmd The byte-sized command to send to the ASCII display.
 */
void ascii_write_cmd(uchar cmd)
{
    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_clear(B_RW);

    ascii_write_controller(cmd);
}


/**
 * @brief Write data to the ASCII display. This is low-level. Use ascii_data()
 *        instead.
 * @param data The byte-sized data to send to the ASCII display.
 */
void ascii_write_data(uchar data)
{
    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_clear(B_RW);

    ascii_write_controller(data);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Read the status of the ASCII display.
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
 * @brief Read data from the ASCII display.
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
 * @brief Send an instruction to the ASCII display, while specifying a required
 *        duration to wait, for synchronization. This function is high-level and
 *        should be the ONLY way of communicating with the hardware.
 * @param cmd The byte-sized command to send to the ASCII display.
 * @param delay_func The specific delay function to call.
 * @param delay_dur The duration to wait. This number is relative to the
 *        delay_func.
 */
void ascii_command(
    uchar cmd,
    void(*delay_func)(uint),
    uint  delay_dur
)
{
    while ( (ascii_read_status() & 0x80) == 0x80 );

    delay_mikro     (     8     );
    ascii_write_cmd (    cmd    );
    delay_func      ( delay_dur );
}


/**
 * @brief Send data to the ASCII display, while specifying a required duration
 *        to wait, for synchronization. This function is high-level and should
 *        be the ONLY way of communicating with the hardware.
 * @param cmd The byte-sized data to send to the ASCII display.
 * @param delay_func The specific delay function to call.
 * @param delay_dur The duration to wait. This number is relative to the
 *        delay_func.
 */
void ascii_data(
    uchar cmd,
    void(*delay_func)(uint),
    uint  delay_dur
)
{
    while ( (ascii_read_status() & 0x80) == 0x80 );

    delay_mikro      (     8     );
    ascii_write_data (    cmd    );
    delay_func       ( delay_dur );
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Initialize the ASCII display.
 */
void ascii_init(void)
{
    // Function Set: 2 rows, 5x8-point characters.
    ascii_command(0b00111000, delay_mikro, 40);
    // Display Control
    ascii_command(0b00001110, delay_mikro, 40);
    // Clear Display
    ascii_command(0b00000001, delay_milli,  2);
    // Entry Mode Set
    ascii_command(0b00000100, delay_mikro, 40);
}


/**
 * @brief Defines a way of accessing the rows and columns of the ASCII display.
 * @param row An integer in range [1, 20]
 * @param column An integer in range [1, 2]
 */
void ascii_goto(uint row, uint column)
{
    uint address = row - 1;

    if (column == 2)
        address += 0x40;

    ascii_write_cmd( 0x80 | address );
}


/**
 * @brief Write a character to the ASCII display.
 * @param c The character to output. Viewed as ASCII.
 */
void ascii_write_char(uchar c)
{
    ascii_data(c, delay_mikro, 43);
}


// =============================================================================
//                                    MAIN

void main(void)
{
    // Data used in the application.
    char *s;
    const char test1[] = "Faisal ";
    const char test2[] = "is an idiot";

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

