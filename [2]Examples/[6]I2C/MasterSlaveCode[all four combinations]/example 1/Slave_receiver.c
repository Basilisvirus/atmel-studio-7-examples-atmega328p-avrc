/*
Slave
*/

//define the clock frequency of the micro controller for the delay to work correctly, 16Mhz
//==================================SERIAL TX START
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <string.h>


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

//vars
volatile uint8_t  temp;

//==================================Functions=====================================

void TWI_Init(unsigned char Slave_Addr){
	TWAR = Slave_Addr;//Assign slave address
}

void TWI_Match_ACK(){//wait for address reference
	while((TWSR & (0xF8))!= 0x60)//while Own SLA+W has NOT been received; and ACK has NOT been returned
	{
		TWCR = (1<<TWINT)|(1 <<TWEN)|(1 << TWEA);//TWEA:enable the ackn. TWEN:enable TWI. TWINT:execute command
		/*(1 << TWINT) makes TWINT 0. We want the above line to check for incoming address non stop*/
		/* when command is done executed, TWINT will be 1*/
		while((TWCR & (0B10000000)) == 0);//WHile TWINT =0 wait (while command is being executed wait)
	}
}

unsigned char TWI_Read_Data(){
	TWCR = (1 << TWINT)|(1 <<TWEA)|(1<<TWEN);
	while((TWCR & (0B10000000)) == 0);//while TWINT is 0 do nothing (while command is being executed)
	while((TWSR & (0xF8))!= 0x80);//Previously addressed with own SLA+W; data has been received; ACK has been returned
	
	return TWDR;//return the data received
}

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
	sei();

	PORTC |= (1 << PORTC5) | (1 << PORTC4);//pull-ups on slave, no external pullup required.

	TWI_Init(0x20);
	
	
	sw("slave Started\n\r");
	
	while(1)
	{	
		TWI_Match_ACK();
		temp = TWI_Read_Data();
		sw("Data Read\n\r");
		itoa(temp , str, 10);
		sw(str);
		sw("\n\r");
	}//while(1)
}//int main()


//==============================================================SERIALS==========================================
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