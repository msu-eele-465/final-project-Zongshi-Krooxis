#include <msp430.h>

volatile int counter = 0;

void setupGPIO(void) {
    // Set P2.0 (CLK / A) and P2.1 (DT / B) as inputs with pull-up resistors
    P2DIR &= ~(BIT0 | BIT1);             // Inputs
    P2REN |= (BIT0 | BIT1);              // Enable pull-up/down resistors
    P2OUT |= (BIT0 | BIT1);              // Select pull-up

    // Configure interrupt on P2.0 (CLK)
    P2IES |= BIT0;                       // Interrupt on high-to-low edge
    P2IFG &= ~BIT0;                      // Clear interrupt flag
    P2IE |= BIT0;                        // Enable interrupt
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    setupGPIO();

    PM5CTL0 &= ~LOCKLPM5;               // Disable LPM

    __enable_interrupt();               // Enable global interrupts

    while (1) {
        __no_operation();               // Idle loop; watch `counter`
    }
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    if (P2IFG & BIT0) {                 // Check if interrupt was from P2.0 (CLK)
        unsigned char stateA = (P2IN & BIT0) >> 0;
        unsigned char stateB = (P2IN & BIT1) >> 1;

        if (stateB != stateA) {
            counter++;                 // Clockwise
        } else {
            counter--;                 // Counterclockwise
        }

        P2IFG &= ~BIT0;                // Clear interrupt flag
    }
}
