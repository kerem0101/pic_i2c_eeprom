/*
 * File:   newmain.c
 * Author: OAKBATI-HPW
 *
 * Created on 29 Kas?m 2021 Pazartesi, 11:22
 */


#include <stdio.h>
#include <stdlib.h>
#include <pic18f45k22.h>
#include <htc.h>
#define _XTAL_FREQ 8000000
#pragma config FOSC = HSHP
#pragma config WDTEN = OFF

#define CS1 PORTBbits.RB0
#define CS2 PORTBbits.RB1
#define RS PORTBbits.RB2
#define RW PORTBbits.RB3
#define EN PORTBbits.RB4
#define RST PORTBbits.RB5
#define RD7 PORTDbits.RD7
#define LCD_DATA PORTD

void trig();
void glcd_init();
void clr_scr();
void place_text(unsigned int start_char, unsigned int page, unsigned char *s);
void I2C_Init();
char I2C_Start(char slave_write_address);
void I2C_Ready();
char I2C_Write(unsigned char data);
char I2C_Stop();
char I2C_Read(char flag);
void I2C_Ack();
void I2C_Nack();

const unsigned char charset[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // SPACE
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x00, 0x03, 0x00, 0x03, 0x00, // "
    0x14, 0x3E, 0x14, 0x3E, 0x14, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
    0x43, 0x33, 0x08, 0x66, 0x61, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, // )
    0x14, 0x08, 0x3E, 0x08, 0x14, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /

    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x04, 0x02, 0x7F, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x22, 0x41, 0x49, 0x49, 0x36, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3E, 0x49, 0x49, 0x49, 0x32, // 6
    0x01, 0x01, 0x71, 0x09, 0x07, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x26, 0x49, 0x49, 0x49, 0x3E, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x08, 0x14, 0x22, 0x41, 0x00, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x00, 0x41, 0x22, 0x14, 0x08, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?

    0x3E, 0x41, 0x59, 0x55, 0x5E, // @
    0x7E, 0x09, 0x09, 0x09, 0x7E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x41, 0x3E, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x41, 0x49, 0x3A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x30, 0x40, 0x40, 0x40, 0x3F, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x02, 0x04, 0x08, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O

    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x1E, 0x21, 0x21, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x09, 0x09, 0x76, // R
    0x26, 0x49, 0x49, 0x49, 0x32, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x7F, 0x20, 0x10, 0x20, 0x7F, // W
    0x41, 0x22, 0x1C, 0x22, 0x41, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x7F, 0x41, 0x00, 0x00, // [
    0x02, 0x04, 0x08, 0x10, 0x20, // backslash
    0x00, 0x00, 0x41, 0x7F, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _

    0x00, 0x01, 0x02, 0x04, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x7F, 0x44, 0x44, 0x44, 0x38, // b
    0x38, 0x44, 0x44, 0x44, 0x44, // c
    0x38, 0x44, 0x44, 0x44, 0x7F, // d
    0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x04, 0x04, 0x7E, 0x05, 0x05, // f
    0x08, 0x54, 0x54, 0x54, 0x3C, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, // j
    0x7F, 0x10, 0x28, 0x44, 0x00, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x7C, 0x04, 0x78, 0x04, 0x78, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x38, 0x44, 0x44, 0x44, 0x38, // o

    0x7C, 0x14, 0x14, 0x14, 0x08, // p
    0x08, 0x14, 0x14, 0x14, 0x7C, // q
    0x00, 0x7C, 0x08, 0x04, 0x04, // r
    0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x04, 0x04, 0x3F, 0x44, 0x44, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y
    0x44, 0x64, 0x54, 0x4C, 0x44, // z
    0x00, 0x08, 0x36, 0x41, 0x41, // {
    0x00, 0x00, 0x7F, 0x00, 0x00, // |
    0x41, 0x41, 0x36, 0x08, 0x00, // }
    0x02, 0x01, 0x02, 0x04, 0x02 // ~
};

char text[20] = "$ is working on I2C";
char text_read[50];
char text_disp[80];
int i=0;
char buf=0xFF;
int j=0;
int k=0;

void main(void) {
    ANSELB=0;
    TRISB=0xC0;
    
    ANSELD=0;
    TRISD=0xFF;
    RST=1;
    __delay_us(1);
    glcd_init();
    clr_scr();
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
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    __delay_ms(90);
    i=0;
    buf=0xFF;
    while (buf!=0){
        I2C_Start(0xA0);
        I2C_Write(i);
        I2C_Stop();
        I2C_Start(0xA1);
        text_read[i]=I2C_Read(1);
        buf=text_read[i];
        I2C_Stop();
        i++;
    }
    
    j=0;
    k=0;
    buf=0xFF;
    while(buf!=0){
        if(text_read[j]=='$'){
            text_disp[k]='O';
            text_disp[k+1]='N';
            text_disp[k+2]='U';
            //text_disp[k+3]='R';
            k+=3;
            text_read[j]='R';
            
        }
        text_disp[k]=text_read[j];
        buf=text_read[j];
        j++;
        k++;
        
    }
    
    place_text(0,0,text_disp);
    
    while(1);
}

void trig(){
    unsigned char k=0;
    EN=0;
    RW=0;
    EN=1;
    __delay_us(1);
    EN=0;
    __delay_us(1);
    TRISD=0xFF;
    RS=0;
    RW=1;
    EN=1;
    __delay_us(1);
    EN=0;
    __delay_us(1);
    while(RD7 && k<50){
        __delay_us(1);
        k++;
    }
    RW=0;
}

void glcd_init(){
    CS1=1;
    CS2=0;
    
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=0x3F;
    trig();
    
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=0x40;
    trig();
    
    CS1=0;
    CS2=1;
    
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=0x3F;
    trig();
    
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=0x40;
    trig();
}

void clr_scr(){
    unsigned int k=0;
    while (k<1024){
        CS2=(k & 0x0040)>0;
        CS1=!CS2;
        
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=(0xB8) + k/128;
    trig();
    
    RS=0;
    RW=0;
    TRISD=0x00;
    LCD_DATA=(0x40) + k%64;
    trig();
    
    RS=1;
    RW=0;
    TRISD=0x00;
    LCD_DATA=0x00;
    trig();
    
    k++;
    }
}

void place_text(unsigned int start_char, unsigned int page, unsigned char *s) {
    unsigned int k;
    unsigned k_start;
    unsigned int j = 0;
    k = (page * 128) + start_char * 6;
    k_start = k;
    while (s[j]) {
        CS2 = (k & 0x0040) > 0;
        CS1 = !CS2;

        RS = 0;
        RW = 0;
        TRISD = 0x00;
        LCD_DATA = (0xB8) + k / 128;
        trig();

        RS = 0;
        RW = 0;
        TRISD = 0x00;
        LCD_DATA = (0x40) + k % 64;
        trig();

        RS = 1;
        RW = 0;
        TRISD = 0x00;

        if ((k % 128) >= 126) {
            LCD_DATA = 0x00;
        } else if (((k % 128) % 6) == 0) {
            LCD_DATA = 0x00;
        } else {
            LCD_DATA = charset[(s[(((k - k_start) % 128) / 6)+((k - k_start) / 128)*21] - 0x20)*5 + (((k - k_start) % 128) % 6) - 1];
        }

        trig();
        k++;
        if ((k - k_start) % 6 == 0) {
            j++;
        }
    }
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