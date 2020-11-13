#include "msp432p401r.h"
#include "initialize.h"

#define DELAY_ONE_SEC   48
#define RED_DELAY       30
#define GREEN_DELAY     20
#define BLUE_DELAY      5

void reset(void);
void do_nothing(void);
void set_red(void);
void set_green(void);
void set_blue(void);

unsigned int count_delay;
int input = 0;
int end = 0;

// states
typedef enum
{
    S0 = 0,
    S1 = 1,
    S2 = 2
}State;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	set_up_clock();
	set_up_port_1();
	set_up_port_2();
	set_up_timer_A();
	// Enable port 1 interrupt on NVIC
    NVIC->ISER[1] |= 1 << ((PORT1_IRQn) & 31); // <- Port 1
    NVIC->ISER[0] |= 1 << ((TA0_0_IRQn) & 31);  // <- Timer A0
	reset();

	State current_state = S0;

	// lookup table for Action
	void (*state_action_table[3][2])(void) =
	{
	     {&do_nothing, &set_red},
	     {&do_nothing, &set_green},
	     {&do_nothing, &set_blue}
	};

	// lookup table for state transition
	State state_transition_table[3][2] =
	{
	     {S0, S1},
	     {S1, S2},
	     {S2, S0}
	};

	// enable all interrupts
	__enable_irq();

	while(1)
	{
	    if(input == 1)  // if user pushed P1.1
	    {
	        // do action
	        (*state_action_table [current_state][input])();
	        // change state
	        current_state = state_transition_table[current_state][input];
	        input = 0;
	    }
	    else
	    {
	        // do nothing
	        // Low Power Mode needs to be set instead maybe
	        (*state_action_table [current_state][input])();
	    }
	}
}

// Port 1
void PORT1_IRQHandler(void)
{
    if(P1->IFG & BIT1)
    {
        input = 1;
        // disable interrupt for bit
        P1->IE &= ~BIT1;
        // clear IFG bit for correspnding bit
        P1->IFG &= ~BIT1;
    }
}

// Timer A0 CC0
void TA0_0_IRQHandler(void)
{
    static unsigned int counter = 0;
    // if counter reached to ???????
    if(counter == count_delay)
    {
        // if not at the end of machine then connect moving through machine
        if(end == 0)
        {
            input = 1;
        }
        else    // reset end for next time
        {
            P1->IE |= BIT1;
            input = 0;
            end = 0;
        }
        counter = 0;
        // disable interrupt in Timer A0
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;

    }
    else    // still increment until actual delay has been achived
    {
        counter++;
    }
    // clear interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void reset(void)
{
    // set this bit high
    P2->OUT = BIT2;
    input = 0;
}

void do_nothing(void){}

void set_red(void)
{
    // set red led high
    P2->OUT = BIT0;
    count_delay = DELAY_ONE_SEC*RED_DELAY;
    // enable interrupt in Timer A0 CCIE
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}

void set_green(void)
{
    // set green led high
    P2->OUT = BIT1;
    count_delay = DELAY_ONE_SEC*GREEN_DELAY;
    // enable interrupt in Timer A0 CCIE
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}

void set_blue(void)
{
    // set blue led high
    P2->OUT = BIT2;
    count_delay = DELAY_ONE_SEC*BLUE_DELAY;
    // enable interrupt in Timer A0 CCIE
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
    end = 1;
}
