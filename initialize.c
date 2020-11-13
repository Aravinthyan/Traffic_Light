#include "msp432p401r.h"
#include "initialize.h"

void set_up_port_1(void)
{
    // set input and output pins
    P1->DIR = 0;
    // enable pullup or pulldown resistor
    P1->REN = BIT1;
    // set pullup resistor for P1.1
    P1->OUT = BIT1;
    // set edge as falling
    P1->IES = BIT1;
    // clear IFG bit for correspnding bit
    P1->IFG = 0;
    // enable interrupt for bit
    P1->IE = BIT1;
}

void set_up_port_2(void)
{
    // set P2.0, P2.1 and P2.2 as outputs
    P2->DIR = BIT0 + BIT1 + BIT2;
    // set all outputs low
    P2->OUT = 0;
}

void set_up_timer_A(void)
{
    // set count to 62500
    // need static var to create a delay up to 1s
    TIMER_A0->CCR[0] = 62500;
    // clear TAR
    // UP mode
    // /1 divider
    // SMCLK
    TIMER_A0->CTL = TIMER_A_CTL_MC_1 | TIMER_A_CTL_ID_0 | TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_IE;
    //TIMER_A0->EX0 = TIMER_A_EX0_IDEX__8;

}

void set_up_clock(void)
{
    // enable clock registers to be modified
    CS->KEY = CS_KEY_VAL;
    // frequency select of DCO
    CS->CTL0 = CS_CTL0_DCORSEL_0;
    // set
    CS->CTL1 = CS_CTL1_SELM_3 | CS_CTL1_SELS_3 | CS_CTL1_DIVM_0 | CS_CTL1_DIVS_0;
}
