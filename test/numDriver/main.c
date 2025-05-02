#include <msp430.h>

/**
 *  Final Project: BCD Test
 *
 *  Aaron McLean    EELE 465
 *
 *  Last Updated: 04/14/2025
 *
 *  A test program that takes in a digit and
 *  Outputs the binary over GPIO
 */

// PIN Definitions 

#define A BIT0
#define B BIT1
#define C BIT2
#define D BIT3

#define NIX1OUT P1OUT
#define NIX1DIR P1DIR

#define NIX2OUT P3OUT
#define NIX2DIR P3DIR

#define NIX3OUT P5OUT
#define NIX3DIR P5DIR

#define NIX4OUT P6OUT
#define NIX4DIR P6DIR

// Function Declarations
void delay();
void nixNum(int a, volatile unsigned char *NIXOUT);


int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    
    // Set P3 ABCD as Outputs & Clear
    NIX1DIR |= A | B | C | D;
    NIX1OUT &= ~(A | B | C | D);

    NIX2DIR |= A | B | C | D;
    NIX2OUT &= ~(A | B | C | D);

    NIX3DIR |= A | B | C | D;
    NIX3OUT &= ~(A | B | C | D);

    NIX4DIR |= A | B | C | D;
    NIX4OUT &= ~(A | B | C | D);

    // Disable digital I/O low-power default
    PM5CTL0 &= ~LOCKLPM5;

    // Start Main Program

    while(1){

        int j;
        int k;
        int l;
        int m;

        for(j = 0; j < 10; j++){
            for(k = 0; k < 10; k++){
                for(l = 0; l < 10; l++){
                    for(m = 0; m < 10; m++){
                        nixNum(j, &NIX1OUT);
                        nixNum(k, &NIX2OUT);
                        nixNum(l, &NIX3OUT);
                        nixNum(m, &NIX4OUT);
                        delay(); 
                    }
                }
            }
        }
    }

 
}

void nixNum(int a, volatile unsigned char *NIXOUT){
    *NIXOUT &= ~(A | B | C | D);
    switch(a){
        case(1):
            *NIXOUT |= (A);
            *NIXOUT &= ~(B | C | D);
            break;
        case(2):
            *NIXOUT |= (B);
            *NIXOUT &= ~(A | C | D);
            break;
        case(3):
            *NIXOUT |= (A | B);
            *NIXOUT &= ~(C | D);
            break;
        case(4):
            *NIXOUT |= (C);
            *NIXOUT &= ~(A | B | D);
            break;
        case(5):
            *NIXOUT |= (A | C);
            *NIXOUT &= ~(B | D);
            break;
        case(6):
            *NIXOUT |= (B | C);
            *NIXOUT &= ~(A | D);
            break;
        case(7):
            *NIXOUT |= (A | B | C);
            *NIXOUT &= ~(D);
            break;
        case(8):
            *NIXOUT |= (D);
            *NIXOUT &= ~(A | B | C);
            break;
        case(9):
            *NIXOUT |= (A | D);
            *NIXOUT &= ~(B | C);
            break;
        default:
            //P3OUT |= ();
            *NIXOUT &= ~(A | B | C | D);
            break;
    }
}

void delay() {
  volatile unsigned int i;
  for (i = 0xFFFF; i > 0; i--)
    ;
}