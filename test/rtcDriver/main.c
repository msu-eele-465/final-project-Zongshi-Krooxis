#include <msp430.h>
#include <stdbool.h>

/**
 *  Final Project: Keypad Test (Modified for RTC I2C on UCB1)
 *
 *  Aaron McLean    EELE 465
 *
 *  Updated: 05/05/2025
 *
 *  Reads minutes and hours from RTC via UCB1 I2C (P4.6 = SCL, P4.7 = SDA)
 */

#include <msp430.h>

unsigned char TimeData[2];  // Store minutes and hours
unsigned char RXByteCtr = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer

    // 1. Put eUSCI_B1 into software reset
    UCB1CTLW0 |= UCSWRST;

    // 2. Configure eUSCI_B1
    UCB1CTLW0 |= UCSSEL_3;          // SMCLK
    UCB1BRW = 10;                   // 100kHz SCL
    UCB1CTLW0 |= UCMODE_3 | UCMST;  // I2C mode, Master mode
    UCB1I2CSA = 0x68;               // Slave address (DS3231M)
    UCB1TBCNT = 2;                  // Receive 2 bytes
    UCB1CTLW1 |= UCASTP_2;          // Auto STOP when TBCNT reached

    // 3. Configure I2C Pins for UCB1
    P4SEL1 &= ~(BIT6 | BIT7);
    P4SEL0 |= BIT6 | BIT7;          // SDA = P4.6, SCL = P4.7
    PM5CTL0 &= ~LOCKLPM5;           // Disable high-impedance mode

    // 4. Release eUSCI_B1 from SW reset
    UCB1CTLW0 &= ~UCSWRST;

    // 5. Enable interrupts
    UCB1IE |= UCTXIE0 | UCRXIE0;    // Enable Tx and Rx interrupts
    __enable_interrupt();

    while (1)
    {
        RXByteCtr = 0;

        // Transmit register address (0x01 for Minutes)
        UCB1CTLW0 |= UCTR;          // TX mode
        UCB1CTLW0 |= UCTXSTT;       // Generate START condition

        while ((UCB1IFG & UCSTPIFG) == 0);  // Wait for STOP
        UCB1IFG &= ~UCSTPIFG;

        // Read 2 bytes from slave
        UCB1CTLW0 &= ~UCTR;         // RX mode
        UCB1CTLW0 |= UCTXSTT;       // Generate START condition

        while ((UCB1IFG & UCSTPIFG) == 0);  // Wait for STOP
        UCB1IFG &= ~UCSTPIFG;

        __delay_cycles(50000);      // Delay between reads
    }

    return 0;
}


#pragma vector=EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void)
{
    switch (UCB1IV)
    {
        case 0x16:  // RXIFG0
            if (RXByteCtr < 2)
            {
                TimeData[RXByteCtr++] = UCB1RXBUF;
            }
            break;

        case 0x18:  // TXIFG0
            UCB1TXBUF = 0x01;  // Register address to read from (Minutes)
            break;

        default:
            break;
    }
}

