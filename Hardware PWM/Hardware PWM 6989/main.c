//Ryan Boylan
//6989
//Hardware PWM
//Lab 4.3

#include <msp430.h>

int pwm = 500;                          //initialize pwm variable
int state = 0;                          //initialize state variable

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           //stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;               // Disable the GPIO power-on default high-impedance mode

    P1OUT &= ~BIT0;                     //turn off pin
    P1DIR |= BIT0;                      //set direction to output

    TA0CCR0 = 1000;
    TA0CTL = TASSEL_2 + MC_1 + TACLR;   //smclk, up mode, clear timer

    TA1CCTL0 = CCIE;                    //debounce timer interrupt
    TA1CCR0 = 50000;                    //reset for debounce

    P1DIR &= ~BIT2;                     //set direction to input
    P1REN |= BIT2;                      //enable pull up resistor
    P1OUT |= BIT2;                      //turn on pin
    P1IE |= BIT2;                       //enable interrupt
    P1IFG &= ~BIT2;                     //clear interrupt flag
    P1IES |= BIT2;                      //HI/Lo edge trigger

    __bis_SR_register(GIE);             //low power mode and enable interrupts
    while(1)                            //pwm loop
    {
        if(TA0R <= pwm){                //if timer is less than pwm
            P1OUT ^= BIT0;              //flip bit
        }
        else if (TA0R > pwm){           //if timer is greater than pwm
            P1OUT &= ~BIT0;
        }
    }
}
#pragma vector=PORT1_VECTOR             //interrupt protocol and memory allocation
__interrupt void PORT_1(void)           //interrupt name
{
        switch(state)                   //switch statement
        {
        case 0:                         //off case
            TA1CTL = TASSEL_2 + MC_1 + TACLR;       //smclk, up mode, clear timer
            P1IFG &= ~BIT2;                         //clear interrupt flag
            P1IE &= ~BIT2;                          //disable interrupts
            break;                                  //break from case
        case 1:                         //on case
            TA1CTL = TASSEL_2 + MC_1 + TACLR;       //smclk, up mode, clear timer
            P1IFG &= ~BIT2;                         //clear interrupt flag
            P1IE &= ~BIT2;                          //disable interrupts
            break;                                  //break from case
        }
}
#pragma vector=TIMER1_A0_VECTOR                     //interrupt protocol and memory allocation
__interrupt void Timer_A0 (void)                    //interrupt name
{
        switch(state)                               //switch statement
        {
        case 0:                                     //on case
            if(pwm < 1000) pwm += 100;              //if pwm not max then increment by 10%
            else pwm = 0;                           //if pwm = 0
            P1IE |= BIT2;                           //enable interrupt
            P1IES &= ~BIT2;                         //Lo/Hi edge trigger
            TA1CTL &= ~TASSEL_2;                    //stop timer
            TA1CTL |= TACLR;                        //clear timer
            state = 1;                              //change to next case
            break;
        case 1:                                     //off case
            P1IE |= BIT2;                           //enable interrupt
            P1IFG &= ~BIT2;                         //clear interrupt flag
            P1IES |= BIT2;                          //Hi/Lo edge trigger
            TA1CTL &= ~TASSEL_2;                    //stop timer
            TA1CTL |= TACLR;                        //clear timer
            state = 0;                              //set back to first state
            break;
        }
}
