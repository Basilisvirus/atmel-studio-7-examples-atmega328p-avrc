/*
*Description: How to setup serial port in atmel studio and atmega328p-pu microcontroller.

*Special thanks:
thanks https://www.youtube.com/watch?v=3_omxGIL0kw for the tutorial. This is where i got the code.
I made the comments to be as clear as possible. Make sure you always check my previous examples on github,
as i wont copy/paste all the same comments from the previous examples.

*Library used: Avr C library
*Microcontroller: a atmega328p-pu is used here.
*Atmel version: Atmel Studio 7 (Version: 7.0.1931 - )
*Extra software needed: Tera Term to see the serial port output on the computer.
*OS: Windows 10

Notes: It gives you 2 warning errors when you compile, but it still uploads and runs correctly.

 */ 

//define the clock frequency of the micro controller for the delay to work correctly, 16Mhz
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>


//no semicolon needed at the end of the following lines
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)

#define TX_BUFFER_SIZE 128
char serialBuffer[TX_BUFFER_SIZE];

//create a dynamic queue method 
uint8_t serialReadPos = 0; //unsigned 8 bit integer
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char c[]);

int main(void)
{
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	
	//ENABLE interrupts
	sei();
	
	
	while(1)
	{
		serialWrite("Hello\n\r");
		_delay_ms(1000);
	}
}

void appendSerial(char c){
	serialBuffer[serialWritePos] =c;
	serialWritePos++;
	
	if (serialWritePos >= TX_BUFFER_SIZE){
		serialWritePos = 0;
	}
}

void serialWrite(char c[]){
	for (uint8_t i=0; i< strlen(c); i++){
		appendSerial(c[i]);
	}
	if (UCSR0A & (1 << UDRE0)){
		UDR0 = 0; //send an off character	
	}
}

//set up the interrupt vector 
//This interrupt gets triggered when the transmitting (sending data) is done
ISR(USART_TX_vect){
	if(serialReadPos != serialWritePos){
		UDR0 = serialBuffer[serialReadPos];
		serialReadPos++;
		
		if(serialReadPos >= TX_BUFFER_SIZE){
			serialReadPos++;
		}
	}
}

