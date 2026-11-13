
#include <xc.h>
#define _XTAL_FREQ 20000000

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// LCD connections
#define RS RC0
#define EN RC1

// Relay connections
#define FORWARD_RELAY RD0
#define REVERSE_RELAY RD1

// Push button connections (active LOW)
#define FORWARD_BUTTON RB0
#define REVERSE_BUTTON RB1

// LCD Functions
void lcd_cmd(unsigned char cmd)
{
    RS = 0;
    PORTC = (PORTC & 0x0F) | (cmd & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTC = (PORTC & 0x0F) | ((cmd << 4) & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_data(unsigned char data)
{
    RS = 1;
    PORTC = (PORTC & 0x0F) | (data & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;

    PORTC = (PORTC & 0x0F) | ((data << 4) & 0xF0);
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_string(const char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

void lcd_init(void)
{
    __delay_ms(20);

    RS = 0;
    PORTC = (PORTC & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(5);
    EN = 0;

    PORTC = (PORTC & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    PORTC = (PORTC & 0x0F) | 0x30;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    PORTC = (PORTC & 0x0F) | 0x20;
    EN = 1;
    __delay_ms(1);
    EN = 0;

    lcd_cmd(0x28);  // 4-bit, 2-line LCD
    lcd_cmd(0x0C);  // Display ON
    lcd_cmd(0x06);  // Auto increment
    lcd_cmd(0x01);  // Clear display
    __delay_ms(2);
}

void display_status(const char *message)
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("Motor Direction");
    lcd_cmd(0xC0);
    lcd_string(message);
}

// Motor control
void motor_stop(void)
{
    FORWARD_RELAY = 0;
    REVERSE_RELAY = 0;
}

void motor_forward(void)
{
    motor_stop();
    __delay_ms(100);

    FORWARD_RELAY = 1;
    REVERSE_RELAY = 0;

    display_status("FORWARD");
}

void motor_reverse(void)
{
    motor_stop();
    __delay_ms(100);

    FORWARD_RELAY = 0;
    REVERSE_RELAY = 1;

    display_status("REVERSE");
}

void main(void)
{
    // Configure I/O pins
    ADCON1 = 0x06;  // Configure analog pins as digital

    TRISC = 0x00;   // LCD output
    TRISD = 0x00;   // Relay output
    TRISB = 0x03;   // RB0 and RB1 inputs

    PORTC = 0x00;
    PORTD = 0x00;
    PORTB = 0x00;

    // Enable PORTB weak pull-ups
    OPTION_REGbits.nRBPU = 0;

    // Initialize outputs
    motor_stop();

    // Initialize LCD
    lcd_init();
    display_status("MOTOR STOPPED");

    while(1)
    {
        // Forward button pressed
        if(FORWARD_BUTTON == 0 &&
           REVERSE_BUTTON == 1)
        {
            __delay_ms(30);

            if(FORWARD_BUTTON == 0 &&
               REVERSE_BUTTON == 1)
            {
                motor_forward();

                while(FORWARD_BUTTON == 0)
                {
                    __delay_ms(10);
                }
            }
        }

        // Reverse button pressed
        else if(REVERSE_BUTTON == 0 &&
                FORWARD_BUTTON == 1)
        {
            __delay_ms(30);

            if(REVERSE_BUTTON == 0 &&
               FORWARD_BUTTON == 1)
            {
                motor_reverse();

                while(REVERSE_BUTTON == 0)
                {
                    __delay_ms(10);
                }
            }
        }

        // Both buttons pressed
        else if(FORWARD_BUTTON == 0 &&
                REVERSE_BUTTON == 0)
        {
            motor_stop();
            display_status("MOTOR STOPPED");

            while(FORWARD_BUTTON == 0 ||
                  REVERSE_BUTTON == 0)
            {
                __delay_ms(10);
            }
        }
    }
}
