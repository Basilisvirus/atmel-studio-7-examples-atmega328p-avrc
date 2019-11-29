/*
*Description: How to setup serial port in atmel studio and atmega328p-pu microcontroller.

*Special thanks:
thanks https://www.youtube.com/watch?v=3_omxGIL0kw for the tutorial.
I made the comments to be as clear as possible. Make sure you always check my previous examples on 
gitlab. https://gitlab.com/Basilisvirus/atmel-studio-examples-atmega328p-avrc

*Library used: Avr C library
*Microcontroller: a atmega328p-pu is used here.
*Atmel version: Atmel Studio 7 (Version: 7.0.1931 - )
*Extra software needed: Tera Term to see the serial port output on the computer.
*OS: Windows 10

 */ 

//==================================TX START=====================
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <string.h>

/*
Functions are:
void sw(char c[]); 
	It SerialWrites an character array [] to the computer/device connected with the mcu.

void swn(int num, int type, int ln);
	It SerialWrites the number 'num' to the computer/device connected with the mcu.
	num = the intreger number/Register to print. 
	type = 2 to print in binary, type = 10 for decimal.
	ln = 1 to change line (\n\r) at the end of the print or ln=0 to do nothing more
	
void readUntill(char c);
	It enters a while() infinite loop, untill the user sends the 'c' character to the serial.
	then the c character is received, the output is stored in rxBuffer variable
	
when the user does not use readUntill () , every character he types is stored inside
udr0 variable and can be used in the main program instantly.
*/

//=====================TX START========================================
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)

volatile uint8_t len, i=0; //unsigned 8 bit integer
volatile char str[40];

void sw(char c[]);
void swn(int num, int type, int ln);

//==================================TX END=============================
//==================================RX START==============================
char rxBuffer[128];

volatile char udr0 = '\0', rx_stop='\0'; /*  NULL = \0  */
	
volatile uint8_t rxReadPos = 0, rxWritePos = 0, readString =0;

void readUntill(char c);
//==================================RX END================================

int main(void)
{
//==================================TX START====================================
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0); //Trans enable
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame

//==================================TX END=====================================

//================================== RX START=============================================
UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
//================================== RX END=============================================
	
	//ENABLE interrupts
	sei();	
	
	sw("write something that ends with '+' ");
	readUntill('+');
	sw(rxBuffer);
	sw("\n\r");
	
	while(1)
	{
		
		if(udr0 == '1')
		{
			udr0 = '\0';
			sw("TCCR0A is ");
			swn(TCCR0A,10,1);
			_delay_ms(1000);
		}
		
	}
}


//==================================TX START========================

void sw(char c[]){
	len = strlen(c); //take size of characters to be printed
	i=0;	//initialize i
	UDR0=0; //make sure UDR0 is 0
	
	while (i<len) //while i is less than the total length of the characters to be printed
	{
		if (  ((UCSR0A & 0B00100000) == 0B00100000)  ){ //if UDRE0 is 1 (aka UDR0 is ready to send)
			UDR0 = c[i]; //put the next character to be sent (now, UDRE0 is 0)
			i++;		//increase the position, and wait until UDRE0 is 1 again
		}
	}
}

void swn(int num, int type, int ln) //take an int and print it on serial
{
	char str[50];//declare a string of 50 characters
	itoa(num, str, type);//convert from int to char and save it on str
	sw(str); //serial write str
	if(ln == 1) //if we want a new line,
	{
		sw("\n\r");	
	}
}
//==================================TX END=====================================

//==================================RX START==========================

void readUntill(char c)
{
	rxWritePos = 0;//begin writing in the buffer from pos 0
	readString = 1;//interrupt will read and save strings
	rx_stop = c; //interrupt will use the global var rx_stop to stop reading the string
	do{
		
	}while(readString == 1);
}

ISR(USART_RX_vect)
{
	if(readString == 1)
	{	
		rxBuffer[rxWritePos] = UDR0;
		if(rxBuffer[rxWritePos] == rx_stop)
		{
			readString = 0;
			/*when you initialize a character array by listing all of its characters 
			separately then you must supply the '\0' character explicitly */
			rxBuffer[rxWritePos] = '\0' ;
		}
		rxWritePos++;
	}
	else
	{
		udr0 = UDR0;	
	}
}
//================================RX END==================