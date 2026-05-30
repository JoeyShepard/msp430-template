#include <msp430.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "asm.h"
#include "main.h"

#define UART_RXD            BIT1  //P1.1
#define UART_TXD            BIT2  //P1.2
#define IO_CLOCK            BIT5  //P1.5 
#define IO_MISO             BIT6  //P1.6
#define IO_MOSI             BIT7  //P1.7

#define PWM_OUT             BIT6  //P2.6

//Function prototypes
void delay_ms(int16_t ms);
void SetPWM(uint8_t freq);
uint8_t SPI_Send(uint8_t data);
void UART_Hex(uint8_t data);
uint8_t UART_Receive();
void UART_Send(uint8_t data);
void UART_Text(const char *data);
void UART_CRLF();
void UART_Space();
void UART_uint16(uint16_t num);
void UART_uint32(uint32_t num);

int main(void)
{
    WDTCTL=WDTPW | WDTHOLD;
    
    BCSCTL1=CALBC1_16MHZ;
    DCOCTL=CALDCO_16MHZ;
   
    volatile int result=asm_test()+FOO;

    //P1OUT=;
    //P1DIR=;

    //P1SEL= IO_CLOCK|IO_MISO|IO_MOSI|UART_RXD|UART_TXD;
    //P1SEL2=IO_CLOCK|IO_MISO|IO_MOSI|UART_RXD|UART_TXD;

    //P2OUT=;
    //P2DIR=PWM_OUT;
    //P2SEL=PWM_OUT;
    //P2REN|=BUTTON;  //Pull up
    //P2IES|=BUTTON;  //Edge detection
    //P2IFG&=~BUTTON; //Clear IFG
    //P2IE|=BUTTON;   //Enable interrupt

    //SPI
    //===
    //UCB0CTL1=UCSWRST;
    //UCB0CTL0=UCCKPH|UCMST|UCSYNC|UCMSB;//mode 0
    //UCB0CTL1|=UCSSEL_2;
    //UCB0BR0=2;
    //UCB0BR1=0;
    //UCB0CTL1&=~UCSWRST;

    //UART
    //====
    //UCA0CTL1=UCSWRST|UCSSEL_2;
    //UCA0CTL0 = 0;
    
    //9.6k
    //UCA0MCTL = UCBRS_5+UCBRF_0;
    //UCA0BR0 = 0x82;
    //UCA0BR1 = 0x06;

    //57.6k
    //UCA0MCTL = UCBRS_6+UCBRF_0;
    //UCA0BR0 = 0x15;
    //UCA0BR1 = 0x01;

    //115.2k
    //UCA0MCTL = UCBRS_7+UCBRF_0;
    //UCA0BR0 = 0x8A;
    //UCA0BR1 = 0x00;

    //500k
    //UCA0MCTL = UCBRS_0+UCBRF_0;
    //UCA0BR0 = 0x20;
    //UCA0BR1 = 0x00;

    //1000k
    //UCA0MCTL = UCBRS_0+UCBRF_0;
    //UCA0BR0 = 0x10;
    //UCA0BR1 = 0x00;

    //UCA0CTL1&=~UCSWRST;

    //PWM
    //===
    //TA0CCR0=0;              //PWM roll over - off for now
    //TA0CCTL1=OUTMOD_7;      //PWM mode

    //Second timer
    //============
    //TA1CCR0=0;
    
    //Enable peripherals
    //UCB0CTL1&=~UCSWRST;             //Enable SPI
    //TA0CTL=TASSEL_2|MC_1;           //Start PWM timer - main clock, up mode
    //TA1CTL=TASSEL_2|MC_1|TACLR;     //Start second timer - main clock, up mode

    //LPM and wait for button interrupt
    //_enable_interrupts();
    //_low_power_mode_4();
    //_disable_interrupts();
       
    //TA0CCR0=;   //Start PWM timer
    //TA1CCR0=;   //Start second timer
        
    //Wait on second timer to finish
    //while((TA1CTL&TAIFG)==0);
    //TA1CTL&=~TAIFG;

    //TA0CCR0=0;  //Stop PWM timer
    //TA1CCR0=0;  //Stop second timer

    while(1);
}

void delay_ms(int16_t ms)
{
    while (ms--) __delay_cycles(16000);
}

void __interrupt_vec(PORT2_VECTOR) Port2_ISR(void)
{
    //P2IFG&=~BUTTON;
    _low_power_mode_off_on_exit();
    return;
}

void SetPWM(uint8_t freq)
{
    //TA0CTL=MC_0;            //Stop timer
    TA0CCR1=freq;           //New PWM value
    //TA0CTL=TASSEL_2+MC_1;   //Restart timer - main clock, up mode
}

uint8_t SPI_Send(uint8_t data)
{
    uint8_t buff;
    while(!(UC0IFG&UCB0TXIFG));
    UCB0TXBUF=data;
    while (UCB0STAT & UCBUSY);
    
    buff=UCB0RXBUF;
    return buff;
}

void UART_Hex(uint8_t data)
{
    uint8_t buff;
    buff=data/16;
    if (buff>9) buff+=55;
    else buff+='0';
    UART_Send(buff);
    buff=data%16;
    if (buff>9) buff+=55;
    else buff+='0';
    UART_Send(buff);
}

uint8_t UART_Receive()
{
    while (!(UC0IFG&UCA0RXIFG));
    return UCA0RXBUF;
}

void UART_Send(uint8_t data)
{
    while(!(UC0IFG&UCA0TXIFG));
    UCA0TXBUF=data;
    
    while (UCA0STAT & UCBUSY);
}

void UART_Text(const char *data)
{
    while (*data) 
    {
        UART_Send(*data);
        data++;
    }
}

void UART_CRLF()
{
    UART_Text("\r\n");
}

void UART_Space()
{
    UART_Send(' ');
}

void UART_uint16(uint16_t num)
{
    if (num==0) UART_Text("0");
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
                UART_Send('0'+digit);
                printed=true;
            }
            divisor/=10;
        }
    }
}

void UART_uint32(uint32_t num)
{
    if (num==0) UART_Text("0");
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
                UART_Send('0'+digit);
                printed=true;
            }
            divisor/=10;
        }
    }
}

