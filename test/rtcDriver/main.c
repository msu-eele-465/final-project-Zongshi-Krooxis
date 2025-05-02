#include <msp430.h>
#include <stdbool.h>

/**
 *  PROJECT 6: Keypad Test (Modified for RTC I2C on UCB1)
 *
 *  Aaron McLean & Alex Deadmond    EELE 465
 *
 *  Updated: 05/02/2025
 *
 *  Reads minutes and hours from RTC via UCB1 I2C (P4.6 = SCL, P4.7 = SDA)
 */

volatile unsigned char TimeData[2]; // Minutes and Hours
volatile unsigned char RXByteCtr = 0;
volatile bool receiveDone = false;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // --- Configure UCB1 for I2C Master Mode ---
    UCB1CTLW0 |= UCSWRST;                     // Hold eUSCI in reset
    UCB1CTLW0 = UCSSEL_2 | UCMST | UCMODE_3 | UCSWRST; // SMCLK, Master, I2C mode
    UCB1BRW = 10;                             // SCL = SMCLK / 10 = 100kHz
    UCB1I2CSA = 0x0068;                         // RTC I2C address (DS3231)
    UCB1CTLW1 |= UCASTP_2;                    // Auto stop after byte count
    PM5CTL0 &= ~LOCKLPM5;                     // Enable I/O

    // --- Set up P4.6 (SCL) and P4.7 (SDA) ---
    P4SEL0 |= BIT6 | BIT7;                    // I2C SCL/SDA
    P4SEL1 &= ~(BIT6 | BIT7);

    UCB1CTLW0 &= ~UCSWRST;                    // Release from reset
    UCB1IE |= UCTXIE0 | UCRXIE0;              // Enable TX and RX interrupts

    __enable_interrupt();                     // Global interrupt enable

    while (1) {
        RXByteCtr = 0;
        receiveDone = false;

        // Step 1: Set register pointer to 0x01 (minutes)
        UCB1TBCNT = 1;
        UCB1CTLW0 |= UCTR | UCTXSTT; // TX mode + START

        while (!receiveDone); // Wait until ISR finishes

        // Step 2: Read 2 bytes (minutes, hours)
        RXByteCtr = 0;
        receiveDone = false;
        UCB1TBCNT = 2;
        UCB1CTLW0 &= ~UCTR;      // RX mode
        UCB1CTLW0 |= UCTXSTT;    // START

        while (!receiveDone); // Wait until 2 bytes received


        unsigned char min_bcd = TimeData[0];
        unsigned char hour_bcd = TimeData[1];

        unsigned char min_tens = (min_bcd >> 4) & 0x0F;
        unsigned char min_ones = min_bcd & 0x0F;
        unsigned char hour_tens = (hour_bcd >> 4) & 0x0F;
        unsigned char hour_ones = hour_bcd & 0x0F;

        // Process the time if needed, or delay
        __delay_cycles(100000);
    }
}

// --- I2C ISR for UCB1 ---
#pragma vector=EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_ISR(void)
{
    switch (UCB1IV) {
        case 0x16:  // RXIFG0
            TimeData[RXByteCtr++] = UCB1RXBUF;
            if (RXByteCtr >= 2) {
                receiveDone = true;
            }
            break;

        case 0x18:  // TXIFG0
            UCB1TXBUF = 0x01; // Set register pointer to minutes (0x01)
            receiveDone = true;
            break;

        default:
            break;
    }
}
