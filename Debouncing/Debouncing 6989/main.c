//Ryan Boylan
//6989
//Debouncing
//Lab 4.1

#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       //stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;           //Disable the GPIO power-on default high-impedance mode

    P1DIR |= BIT0;                  //set direction to output
    P1OUT &= ~BIT0;                 //turn off output
    P1DIR &= ~(BIT1);               //set direction to input
    P1REN |= BIT1;                  //enable pull up resistor
    P1IE |= BIT1;                   //enable interrupt
    P1IES |= ~BIT1;                 //Hi/Lo edge triggered
    P1IFG &= ~BIT1;                 //clear interrupt flag

    TA0CCR0 = 64;                   //debouncing time
    TA0CCTL0 |= CCIE;               //enable interrupts in capture compare
    TA0CTL |= TASSEL_2 + MC__CONTINUOUS; //smclk, continuous mode

    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE);  //enable low power mode and interrupts


}

#pragma vector=TIMER0_A0_VECTOR     //interrupt protocol and memory allocation
__interrupt void Interrupt_1(void)  //interrupt name
{
    TA0CTL &= ~TAIFG;               //clear interrupt flag
    TA0CTL &= ~MC_3;                //timer stops
    if(!(P1IN & BIT1))              //if button is pressed
        P1OUT ^= BIT0;              //toggle LED
}
#pragma vector = PORT1_VECTOR       //interrupt protocol and memory allocation
__interrupt void Interrupt_2(void)  //interrupt name
{
    P1IFG &= ~BIT1;                 //clear interrupt flag
    TA0R = 0;                       //reset timer
    TA0CTL |= MC_2;                 //start timer in continuous mode
}
