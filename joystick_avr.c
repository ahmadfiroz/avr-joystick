/*
 * joystick.c
 *
 *  Created on: Dec 5, 2015
 *      Author: ahmad
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define F_CPU 16000000UL


#define FOSC 16000000 /**< Clock speed for UBRR calculation. refer page 179 of 328p datasheet. */
#define BAUD 9600 /**< Baud Rate in bps. refer page 179 of 328p datasheet. */
#define MYUBRR FOSC/16/BAUD-1 /**< UBRR = (F_CPU/(16*Baud))-1 for asynch USART page 179 328p datasheet. Baud rate 9600bps, assuming  16MHz clock UBRR0 becomes 0x0067*/
int X_Analog, Y_Analog, B, A, F, G, E, D, C;///Initializing required variable



/**
 * @brief Initialize USART for 8 bit data transmit no parity and 1 stop bit.
 *
 *@details This is a code snippet from datasheet page 182
 *
 * @param ubrr The UBRR value calculated in macro MYUBRR
 * @see MYUBRR
 */

void USART_init(unsigned int ubrr)
{

    UCSR0C = (0<<USBS0)|(3<<UCSZ00); /// Step 1. Set UCSR0C in Asynchronous mode, no parity, 1 stop bit, 8 data bits
    UCSR0A = 0b00000000;/// Step 2. Set UCSR0A in Normal speed, disable multi-proc

    UBRR0H = (unsigned char)(ubrr>>8);/// Step 3. Load ubrr into UBRR0H and UBRR0L
    UBRR0L = (unsigned char)ubrr;


    UCSR0B = 0b00011000;/// Step 4. Enable Tx Rx and disable interrupt in UCSR0B
}
/**
 * @brief Send 8bit data.
 *
 *@details This is a code snippet from datasheet page 184
 *
 * @param data The 8 bit data to be sent
 */

int USART_send(char c, FILE *stream)
{

    while ( !( UCSR0A & (1<<UDRE0)) )/// Step 1.  Wait until UDRE0 flag is high. Busy Waiting
    {;}

    UDR0 = c; /// Step 2. Write char to UDR0 for transmission
}
/**
 * @brief Receive 8bit sata.
 *
 *@details This is a code snippet from datasheet page 187
 *
 * @return Returns received data from UDR0
 */
int USART_receive(FILE *stream )
{

    while ( !(UCSR0A & (1<<RXC0)) )/// Step 1. Wait for Receive Complete Flag is high. Busy waiting
        ;

    return UDR0;/// Step 2. Get and return received data from buffer
}

/**
 * @brief Initialize ADC
 */
void init_ADC()
{
    /** ADMUX section 23.9.1 page 262
    *	BIT 7 and BIT 6 >> AVCC with external capacitor at AREF pin REFS0 =0 and REFS1= 1
    * 	BIT 5 >> ADC Right Adjust Result ADLAR = 0
    *	BIT 3:0 >> MUX3:0 0b0000 for channel 0
    */
    ADMUX = 0b01000000;
    /**DIDR0 >> Digital Input Disable Register 0 section Section 23.9.4 page 265 - 266
    * 	Disable digital input buffer of ADC0 to save power consumption
    */
    DIDR0 = 0b00000001;

    /** ADSCRA ADC Control and Status Register A Section 23.9.2 page 263 -264
    *	Bit 7 >> ADEN: ADC Enable =1 enable ADC
    *	Bit 6 >> ADSC: ADC Start Conversion =0 do not start conversion
    *	Bit 5 >> ADATE: ADC Auto Trigger Enable = 0 Disable trigger
    *	Bit 4 >> ADIF: ADC Interrupt Flag = 0
    *	Bit 3 >> ADIE: ADC Interrupt Enable = 0 Disable ADC interrupt
    *	Bits 2:0 >> ADPS2:0: ADC Prescaler Select Bits 010 division factor = 4
    */
    ADCSRA = 0b10000010;
}

///@brief Initialize Digital Input buttons
/**
 *	D3(PD3)(E) Small Push Button E
 *
 *	D4(PD4)(D) Small Push Button D
 *
 *	D5(PD5)(C) Joystick Push Button C
 *
 *	D6(PD6)(B) Large Push Button B
 *
 *	D7(PD7)(A) Large Push Button A
 *
 *	D8(PB0)(F) Large Push Button F
 *
 *	D9(PB1)(G) Large Push Button G
 */
void scan_buttons(void)/// Read for input & change variable state
	{
		 if ((PINB & (1 << PD3)) == 0)
		 {
			 	E=1;
		 }

		 if ((PIND & (1 << PD4)) == 0)
		 {
			 	D=1;
		 }
		 if ((PIND & (1 << PD5)) == 0)
		 {
		 		C=1;
		 }
		 if ((PIND & (1 << PD6)) == 0)
		 {
		 		B=1;
		 }
		 if ((PIND & (1 << PD7)) == 0)
		 {
		 		A=1;
		 }
		 if ((PINB & (1 << PB0)) == 0)
		 {
		 		F=1;
		 }
		 if ((PINB & (1 << PB1)) == 0)
		 {
				G=1;
		 }


	}


int main()
{

    init_ADC();
    USART_init(MYUBRR);
    stdout = fdevopen(USART_send, NULL); /// USART Sending And Recieving
    stdin = fdevopen(NULL, USART_receive);
    DDRD &=  ~(_BV(PD3))| (DDRD & ~_BV(PD4))|(DDRD & ~_BV(PD5))|(DDRD & ~_BV(PD6))|(DDRD & ~_BV(PD7));///Setting Digital PORTS as output
    DDRB  &= ~(_BV(PB0))| (DDRB &~_BV(PB1));
    PORTD=0xFF;
    PORTB = 0XFF;// pull up
    while(1)///infinite  loop
    {
    	ADMUX = ADMUX & ~_BV(MUX0);
        ADCSRA |= (1<<ADSC);/// Enable Conversion For ADC0 PIN 0 or A0
        while (bit_is_set(ADCSRA,ADSC))///Wait for conversion complete.
        {
            ;
        }
        X_Analog = ADC;///Store the value in X Analog
        _delay_ms(100);
        ADCSRA &= ~(1<<ADSC);///Disable Conversion
        ADMUX |= _BV(MUX0);/// Change pin to ADC1 or A1
        ADCSRA |= (1<<ADSC);///Enable Conversion again
        while (bit_is_set(ADCSRA,ADSC))
               {
                   ;
               }
        Y_Analog = ADC;///Store the value in Y Analog
        _delay_ms(100);
        scan_buttons();///Scan for digital button input
        printf("%d\n %d\n %d\n %d\n %d\n %d\n %d\n %d\n %d\n ", X_Analog, Y_Analog, B, A, F, G, E, D, C );///Send All input information via USART
        _delay_ms(100);
        B=A=F=G=E=D=C= 0;///Reset Button State
    }
}
