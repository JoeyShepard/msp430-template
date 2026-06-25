#include <msp430.h>
#include <stdbool.h>
#include <stdint.h> 

#include "common.h"

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
    while (UCB0STAT&UCBUSY);
    
    buff=UCB0RXBUF;
    return buff;
}

//I2C - see slau144
void i2c_start_send(uint8_t address)
{
	UCB0I2CSA=address;
	UCB0CTL1|=UCTXSTT|UCTR;         //Transmit mode, send start bit
    //uart_text("i2c_start_send 0\r\n");
    //while (UCB0CTL1&UCTXSTT);       //Wait for start bit to send
    //while((IFG2&UCB0TXIFG)==0);     //Wait for buffer to be ready
    //uart_text("i2c_start_send 1\r\n");
}

int i2c_send(uint8_t data)
{
	while((IFG2&UCB0TXIFG)==0);     //Wait for buffer to be ready
	UCB0TXBUF=data;
    //uart_text("i2c_send 0\r\n");
    while(((IFG2&UCB0TXIFG)==0)&&((UCB0STAT&UCNACKIFG)==0));
	//while((IFG2&UCB0TXIFG)==0);     //Wait for data to send
    //uart_text("i2c_send 1\r\n");

    if ((UCB0STAT&UCNACKIFG)==0)
    {
        //NACK received
        return -1;
    }
    else
    {
        //Transfer succeeded
        return 0;
    }
}

void i2c_start_receive(uint8_t address)
{
	UCB0I2CSA=address;
	UCB0CTL1&=~UCTR;                //Receiver mode
	UCB0CTL1|=UCTXSTT;              //Send start bit
    //uart_text("i2c_start_receive 0\r\n");
    while (UCB0CTL1&UCTXSTT);       //Wait for start bit to send
    //uart_text("i2c_start_receive 1\r\n");
}

uint8_t i2c_receive()
{
    //uart_text("i2c_receive 0\r\n");
	while((IFG2&UCB0RXIFG)==0);     //Wait for data to be received
    //uart_text("i2c_receive 1\r\n");
    uint8_t return_val=UCB0RXBUF;
    //uart_text("i2c_receive: ");
    //uart_hex(return_val);
    //uart_crlf();
	return return_val;
}

uint8_t i2c_receive_nack()
{
    UCB0CTL1|=UCTXSTP;              //Send stop bit before reading received byte
	while((IFG2&UCB0RXIFG)==0);     //Wait for data to be received
    uint8_t return_val=UCB0RXBUF;
	while (UCB0CTL1&UCTXSTP);       //Wait for stop bit to send
	return return_val;
}

uint8_t i2c_receive_single(uint8_t address)
{
	UCB0I2CSA=address;
	UCB0CTL1&=~UCTR;                //Receiver mode
    //uart_text("i2c_receive_single 0\r\n");
	UCB0CTL1|=UCTXSTT;              //Send start bit
    while (UCB0CTL1&UCTXSTT);       //Wait for start bit to send
    UCB0CTL1|=UCTXSTP;              //Send stop bit
    //uart_text("i2c_receive_single 1\r\n");
	while((IFG2&UCB0RXIFG)==0);     //Wait for data to be received
    //uart_text("i2c_receive_single 2\r\n");
    uint8_t return_val=UCB0RXBUF;
    //uart_text("i2c_receive_single: ");
    //uart_hex(return_val);
    //uart_crlf();
	while (UCB0CTL1&UCTXSTP);       //Wait for stop bit to send
	return return_val;
}

void i2c_stop()
{
	UCB0CTL1|=UCTXSTP;              //Send stop bit
    //uart_text("i2c_stop 0\r\n");
	while (UCB0CTL1&UCTXSTP);       //Wait for stop bit to send
    //uart_text("i2c_stop 1\r\n");
}

void text_hex(char *buffer,uint8_t data)
{
    uint8_t digit;
    digit=data/16;
    if (digit>9) digit+='A'-10;
    else digit+='0';
    buffer[0]=digit;
    digit=data%16;
    if (digit>9) digit+=55;
    else digit+='0';
    buffer[1]=digit;
    buffer[2]=0;
}

void text_uint16(char *buffer,uint16_t num)
{
    if (num==0)
    {
        buffer[0]='0';
        buffer[1]=0;
    }
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
                *buffer='0'+digit;
                buffer++;
                *buffer=0;
                printed=true;
            }
            divisor/=10;
        }
    }
}

void text_int16(char *buffer,int16_t num)
{
    if (num==0)
    {
        buffer[0]='0';
        buffer[1]=0;
    }
    else
    {
        if (num<0)
        {
            *buffer='-';
            buffer++;
            *buffer=0;
            num=-num;
        }
        int16_t divisor=10000;
        bool printed=false;
        while (divisor>0)
        {
            int16_t digit=num/divisor;
            num%=divisor;
            if ((digit!=0)||(printed==true))
            {
                *buffer='0'+digit;
                buffer++;
                *buffer=0;
                printed=true;
            }
            divisor/=10;
        }
    }
}

void text_uint32(char *buffer,uint32_t num)
{
    if (num==0)
    {
        buffer[0]='0';
        buffer[1]=0;
    }
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
                *buffer='0'+digit;
                buffer++;
                *buffer=0;
                printed=true;
            }
            divisor/=10;
        }
    }
}

void text_int32(char *buffer,int32_t num)
{
    if (num==0)
    {
        buffer[0]='0';
        buffer[1]=0;
    }
    else
    {
        if (num<0)
        {
            *buffer='-';
            buffer++;
            *buffer=0;
            num=-num;
        }
        int32_t divisor=1000000000;
        bool printed=false;
        while (divisor>0)
        {
            int32_t digit=num/divisor;
            num%=divisor;
            if ((digit!=0)||(printed==true))
            {
                *buffer='0'+digit;
                buffer++;
                *buffer=0;
                printed=true;
            }
            divisor/=10;
        }
    }
}

void uart_hex(uint8_t data)
{
    char buffer[HEX8_MAX_DIGITS+1];
    text_hex(buffer,data);
    uart_text(buffer);
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
    char buffer[UINT16_MAX_DIGITS+1];
    text_uint16(buffer,num);
    uart_text(buffer);
}

void uart_int16(int16_t num)
{
    char buffer[INT16_MAX_DIGITS+1];
    text_int16(buffer,num);
    uart_text(buffer);
}

void uart_uint32(uint32_t num)
{
    char buffer[UINT32_MAX_DIGITS+1];
    text_uint32(buffer,num);
    uart_text(buffer);
}

void uart_int32(int32_t num)
{
    char buffer[INT32_MAX_DIGITS+1];
    text_int32(buffer,num);
    uart_text(buffer);
}
