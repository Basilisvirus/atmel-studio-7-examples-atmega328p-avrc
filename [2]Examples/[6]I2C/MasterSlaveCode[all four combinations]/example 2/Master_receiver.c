/*
Master

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

volatile uint8_t len, k=0; //unsigned 8 bit integer
volatile char str_tx[40];

void sw(char tx_write[]);
void swn(int num, int type, int ln);

//==================================TX END=============================

//==================================RX START==============================
char rxBuffer[128];

volatile char udr0 = '\0', rx_stop='\0'; /*  NULL = \0  */

volatile uint8_t rxReadPos = 0, rxWritePos = 0, readString =0;

void readUntill(char fin_rx);
//==================================RX END================================


//==================================TWI Functions===========
void TWI_Init(){
	TWBR = 0x0C; //Set bit rate
}

void TWI_Start(){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);// (1 << TWINT) makes twint 0
	/*Immediately after this operation, TWINT is set to 0, a Start is issued
	 until the transfer is done. When command is done, TWINT becomes 1*/
	while((TWCR&(0B10000000))==0);//while TWINT =0 do nothing. the TWINT will be 1 when start is done.
	/* While the start condition has not been transmitted, do nothing */
	while((TWSR & (0xF8))!= 0x08); //0x08 = 11111000 //A START condition has been transmitted
}

void TWI_Write_Addr(unsigned char Addr){
	TWDR = Addr; //write the address + WRITE (SLA+R) to be sent
	TWCR = (1 << TWINT) | (1 << TWEN); //Enable TWINT(make it 0) to execute command, Enable TWI.
	while((TWCR& (0B10000000))==0);//while TWINT =0 do nothing. the TWINT will be 1 when address transfer is done.
	while((TWSR&(0xF8)) != 0x40); //0x40 //SLA+R has been transmitted; ACK has been received
}

void TWI_READ_Data(){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); //Execute command (TWINT), enable TWI (TWEN).
	while((TWCR&(0B10000000)) == 0); //while TWINT =0 do nothing. the TWINT will be 1 when data transfer is done.
	while((TWSR&(0xF8)) != 0x50); //Data byte has been read; ACK has been returned
}

void TWI_Stop(){
	TWCR = (1 << TWINT)|(1 << TWSTO); //TWI stop and TWI interrupt enable to execute command
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
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0); //RX enable and Receive complete interrupt enable
//================================== RX END=============================================
	//ENABLE interrupts
	sei();
	

	sw("master started, wait 5 sec\n\r");
	_delay_ms(5000);
	TWI_Init();
	
	TWI_Start();
	sw("twi started \n\r");
	TWI_Write_Addr(0x21);//WRITE SLA+R
	sw("SLA+R has been transmitted; ACK has been received\n\r");
	
	while(1)
	{
		TWI_READ_Data();
		swn(TWDR, 10, 1);
		//TWI_Stop();
	}//while(1)
}//int main()


//==============================================================SERIALS==========================================
//==================================TX START========================

void sw(char tx_write[]){
	len = strlen(tx_write); //take size of characters to be printed
	k=0;	//initialize i
	UDR0=0; //make sure UDR0 is 0
	
	while (k<len) //while i is less than the total length of the characters to be printed
	{
		if (  ((UCSR0A & 0B00100000) == 0B00100000)  ){ //if UDRE0 is 1 (aka UDR0 is ready to send)
			UDR0 = tx_write[k]; //put the next character to be sent (now, UDRE0 is 0)
			k++;		//increase the position, and wait until UDRE0 is 1 again
		}
	}
}

void swn(int num, int type, int ln) //take an int and print it on serial
{
	str_tx[50];//declare a string of 50 characters
	
	itoa(num, str_tx, type);//convert from int to char and save it on str
	
	sw(str_tx); //serial write str
	
	if(ln == 1) //if we want a new line,
	{
		sw("\n\r");	
	}
	
}
//==================================TX END=====================================

//==================================RX START==========================

void readUntill(char fin_rx)
{
	rxWritePos = 0;//begin writing in the buffer from pos 0
	readString = 1;//interrupt will read and save strings
	rx_stop = fin_rx; //interrupt will use the global var rx_stop to stop reading the string
	
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
