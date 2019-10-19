//Ryan Boylan
//G2
//Software PWM
//Lab 4.2

#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   //stop watchdog timer

    P1DIR |= BIT0;                              //set direction to output
    P1DIR |= BIT6;                              //set direction to output
    P1OUT &= ~BIT6;                             //turn pin off
    P1DIR &= ~BIT3;                             //set direction to input
    P1REN |= BIT3;                              //enable pull up resistor
    P1OUT |= BIT3;                              //enable pull up resistor
    P1IES |= BIT3;                              //Lo/High trigger
    P1IE |= BIT3;                               //enable interrupt
    P1IFG &= ~BIT3;                             //clear interrupt flags

    TA1CTL = TASSEL_2 | MC_2;                   //smclk, continuous mode
    TA1CCR0 = 64;                               //debounce time
    TA1CCTL0 |= CCIE;                           //capture control register interrupt
    TA1CTL |= TAIE;                             //enable interrupts

    TA0CTL = TASSEL_2 | MC_1;                   //smclk, up mode
    TA0CCR0 = 1000;                             //100% duty cycle
    TA0CCR1 = 500;                              //50% duty cycle
    TA0CCTL0 |= CCIE;                           //capture control register interrupt
    TA0CCTL1 |= CCIE;                           //capture control register interrupt
    TA0CTL |= TAIE;                             //enable interrupts

    __bis_SR_register(LPM0_bits + GIE);         //low power mode and enable interrupts

}

#pragma vector=PORT1_VECTOR                     //interrupt protocol and memory allocation
__interrupt void PORT_1(void)                   //interrupt name

{
    P1IFG &= ~BIT3;                             //clear interrupt flag
    TA1R = 0;                                   //reset timer
    TA1CTL = TASSEL_1 | MC_2;                   //aclk, continuous mode
}

#pragma vector=TIMER1_A0_VECTOR                 //interrupt protocol and memory allocation
__interrupt void Timer1_A0 (void)               //interrupt name
{
    TA1CTL &= ~TAIFG;                           //clear interrupt flag
    TA1CTL &= ~MC_3;                            //turn off debouncing
    if(!(P1IN & BIT3) && (P1IES & BIT3))        //if button is pressed
    {
        P1IES &= ~BIT3;                         //Hi/Lo edge trigger
        P1OUT |= BIT6;                          //turn on LED
        if(TA0CCR1 <= 900)                      //if the duty cycle is not 100%
        {
            TA0CCR1 += 100;                     //increase duty cycle by 10%
            TA0CTL |= MC_1;                     //up mode
        }
        else
        {
            TA0CCR1 = 0;                        //set duty cycle to 0
            TA0CTL &= ~MC_3;                    //stop timer
            P1OUT &= ~BIT0;                     //turn off LED
        }
    }
    if((P1IN & BIT3) && !(P1IES & BIT3))        //if button is not pressed
    {
        P1IES |= BIT3;                          //Lo/Hi edge trigger
        P1OUT &= ~BIT6;                         //turn off LED
    }
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
    TA0CTL &= ~TAIFG;                           //clear interrupt flag
    P1OUT |= BIT0;                              //turn on LED
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1 (void)
{
    if(TA0IV == 0x02)                           //if CCR1 interrupt
        P1OUT &= ~BIT0;                         //turn off LED
}
