#include <msp430.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "asm.h"
#include "common.h"
#include "main.h"
#include "pins.h"

int main(void)
{
    WDTCTL=WDTPW | WDTHOLD;
    
    BCSCTL1=CALBC1_16MHZ;
    DCOCTL=CALDCO_16MHZ;
   
    volatile int result=asm_test();

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

void __interrupt_vec(PORT2_VECTOR) Port2_ISR(void)
{
    //P2IFG&=~BUTTON;
    _low_power_mode_off_on_exit();
    return;
}

