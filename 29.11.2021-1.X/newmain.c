/*
 * File:   newmain.c
 * Author: OAKBATI-HPW
 *
 * Created on 29 Kas?m 2021 Pazartesi, 10:15
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic18f45k22.h>
#include <htc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC = HSHP
#pragma config WDTEN = OFF

void I2C_Init();
char I2C_Start(char slave_write_address);
void I2C_Ready();
char I2C_Write(unsigned char data);
char I2C_Stop();
char I2C_Read(char flag);
void I2C_Ack();
void I2C_Nack();

char text[20] = "$ is working on I2C";
int i = 0;

int main(int argc, char** argv) {

    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    I2C_Init();
    for (i = 0; i < 20; i++) {
        I2C_Start(0xA0);
        I2C_Write(i);
        I2C_Write(text[i]);
        I2C_Stop();
        __delay_ms(10);
    }


    while (1);


    return (EXIT_SUCCESS);
}

void I2C_Init() {
    ANSELC = 0;
    TRISC3 = 1; /* Set up I2C lines by setting as input */
    TRISC4 = 1;
    SSPSTAT = 0x80; /* Slew rate disabled, other bits are cleared */
    SSPCON1 = 0x28; /* Enable SSP port for I2C Master mode,
			clock = FOSC / (4 * (SSPADD+1))*/
    SSPCON2 = 0;
    SSPADD = 19; /* Clock 100 kHz */
    SSPIE = 1; /* Enable SSPIF interrupt */
    SSPIF = 0;
}

char I2C_Start(char slave_write_address) {
    SSPCON2bits.SEN = 1; /* Send start pulse */
    while (SSPCON2bits.SEN); /* Wait for completion of start pulse */
    SSPIF = 0;
    if (!SSPSTATbits.S) /* Check whether START detected last */
        return 0; /* Return 0 to indicate start failed */
    return (I2C_Write(slave_write_address)); /* Write slave device address
						with write to communicate */
}

void I2C_Ready() {
    while (BCLIF); /* Wait if bit collision interrupt flag is set*/

    /* Wait for Buffer full and read write flag*/
    while (SSPSTATbits.BF || (SSPSTATbits.R_nW));
    SSPIF = 0; /* Clear SSPIF interrupt flag*/
}

char I2C_Write(unsigned char data) {
    SSPBUF = data; /* Write data to SSPBUF*/
    I2C_Ready();
    if (SSPCON2bits.ACKSTAT) /* Check for acknowledge bit*/
        return 1;
    else
        return 2;
}

char I2C_Stop() {
    I2C_Ready();
    SSPCON2bits.PEN = 1; /* Stop communication*/
    while (SSPCON2bits.PEN); /* Wait for end of stop pulse*/
    SSPIF = 0;
    if (!SSPSTATbits.P) /* Check whether STOP is detected last */
        return 0; /* If not return 0 to indicate start failed*/
    return 1;
}

char I2C_Read(char flag) {
    int buffer = 0;
    SSPCON2bits.RCEN = 1; /* Enable receive */

    /* Wait for buffer full flag which when complete byte received */
    while (!SSPSTATbits.BF);
    buffer = SSPBUF; /* Copy SSPBUF to buffer */

    /* Send acknowledgment or negative acknowledgment after read to
    continue or stop reading */
    if (flag == 0)
        I2C_Ack();
    else
        I2C_Nack();
    I2C_Ready();
    return (buffer);
}

void I2C_Ack() {
    SSPCON2bits.ACKDT = 0; /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN = 1; /* Enable ACK to send */
    while (SSPCON2bits.ACKEN);
}

void I2C_Nack() {
    SSPCON2bits.ACKDT = 1; /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN = 1; /* Enable ACK to send */
    while (SSPCON2bits.ACKEN);
}