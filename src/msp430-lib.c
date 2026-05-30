#include <msp430.h>
#include <stdbool.h>
#include <stdint.h> 

#include "msp430-lib.h"

void delay_ms(uint16_t ms)
{
    while (ms--) __delay_cycles(16000);
}

void set_pwm(uint8_t freq)
{
    //TA0CTL=MC_0;            //Stop timer
    TA0CCR1=freq;           //New PWM value
    //TA0CTL=TASSEL_2+MC_1;   //Restart timer - main clock, up mode
}

uint8_t spi_send(uint8_t data)
{
    uint8_t buff;
    while(!(UC0IFG&UCB0TXIFG));
    UCB0TXBUF=data;
    while (UCB0STAT & UCBUSY);
    
    buff=UCB0RXBUF;
    return buff;
}

void uart_hex(uint8_t data)
{
    uint8_t buff;
    buff=data/16;
    if (buff>9) buff+=55;
    else buff+='0';
    uart_send(buff);
    buff=data%16;
    if (buff>9) buff+=55;
    else buff+='0';
    uart_send(buff);
}

uint8_t uart_receive()
{
    while (!(UC0IFG&UCA0RXIFG));
    return UCA0RXBUF;
}

void uart_send(uint8_t data)
{
    while(!(UC0IFG&UCA0TXIFG));
    UCA0TXBUF=data;
    
    while (UCA0STAT & UCBUSY);
}

void uart_text(const char *data)
{
    while (*data)
    {
        uart_send(*data);
        data++;
    }
}

void uart_crlf()
{
    uart_text("\r\n");
}

void uart_space()
{
    uart_send(' ');
}

void uart_uint16(uint16_t num)
{
    if (num==0) uart_text("0");
    else
    {
        uint16_t divisor=10000;
        bool printed=false;
        while (divisor>0)
        {
            uint16_t digit=num/divisor;
            num%=divisor;
            if ((digit!=0)||(printed==true))
            {
                uart_send('0'+digit);
                printed=true;
            }
            divisor/=10;
        }
    }
}

void uart_uint32(uint32_t num)
{
    if (num==0) uart_text("0");
    else
    {
        uint32_t divisor=1000000000;
        bool printed=false;
        while (divisor>0)
        {
            uint32_t digit=num/divisor;
            num%=divisor;
            if ((digit!=0)||(printed==true))
            {
                uart_send('0'+digit);
                printed=true;
            }
            divisor/=10;
        }
    }
}

