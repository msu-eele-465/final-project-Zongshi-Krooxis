#include <msp430.h>

volatile int counter = 0;
volatile unsigned char last_state_A = 0;

void setupGPIO(void) {
    // Set P1.1 (CLK / A) and P1.2 (DT / B) as inputs with pull-up resistors
    P1DIR &= ~(BIT1 | BIT2);             // Inputs
    P1REN |= (BIT1 | BIT2);              // Enable pull-up/down resistors
    P1OUT |= (BIT1 | BIT2);              // Select pull-up

    // Configure interrupt on P1.1 (CLK)
    P1IES |= BIT1;                       // Interrupt on high-to-low edge
    P1IFG &= ~BIT1;                      // Clear interrupt flag
    P1IE |= BIT1;                        // Enable interrupt
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    setupGPIO();

    __enable_interrupt();               // Enable global interrupts

    while (1) {
        // Main loop does nothing; all logic handled in ISR
        __no_operation();               // Can place breakpoint here to watch `counter`
    }
}

// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IFG & BIT1) {                 // Check if interrupt was from P1.1 (CLK)
        unsigned char stateA = (P1IN & BIT1) >> 1;
        unsigned char stateB = (P1IN & BIT2) >> 2;

        if (stateB != stateA) {
            counter++;                 // Clockwise
        } else {
            counter--;                 // Counterclockwise
        }

        P1IFG &= ~BIT1;                // Clear interrupt flag
    }
}
