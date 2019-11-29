/*
Testin gthe stepper motor, connection 1.0

*/

//define the clock frequency of the micro controller for the delay to work correctly, 16Mhz
//==================================SERIAL START
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <string.h>
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//no semicolon needed at the end of the following lines
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)

#define RX_BUFFER_SIZE  128
char rxBuffer[RX_BUFFER_SIZE];

uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

char getChar(void);
char peekChar(void);

//==================================SERIAL END


int main(void)
{
//==================================SERIAL START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	
	//ENABLE interrupts
	sei();	
//==================================SERIAL END

	
	DDRB |= (1 <<DDB0);// set output
	
	while(1)
	{
		char c = getChar();
		
        if(c == '1')
        {
            sbi(PORTB, PORTB0);
        }
        else if(c =='0')
        {
            cbi(PORTB, PORTB0);
        }
        
	}
}

char peekChar(void)
{
    char ret = '\0';
     
    if(rxReadPos != rxWritePos)
    {
        ret = rxBuffer[rxReadPos];
    }
     
    return ret;
}
 
char getChar(void)
{
    char ret = '\0';
     
    if(rxReadPos != rxWritePos)
    {
        ret = rxBuffer[rxReadPos];
         
        rxReadPos++;
         
        if(rxReadPos >= RX_BUFFER_SIZE)
        {
            rxReadPos = 0;
        }
    }
     
    return ret;
}
 
ISR(USART_RX_vect)
{
    rxBuffer[rxWritePos] = UDR0;
     
    rxWritePos++;
     
    if(rxWritePos >= RX_BUFFER_SIZE)
    {
        rxWritePos = 0;
    }
}

