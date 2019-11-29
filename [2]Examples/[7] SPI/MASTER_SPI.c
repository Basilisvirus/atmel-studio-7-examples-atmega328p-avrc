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


//==================================SPI Functions===========
void spi_init(){
	//MOSI pin output, SCK pin output, SS pin output
	DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2); // MOSI,SCK,SS output
	
	//MISO remains input
	
	//Enable SPI in master mode
	SPCR |= (1 << SPE) | (1 << MSTR);

	//set clock rate /16
	//SPI2X = 0, SPR1 = 0, SPR0 = 1
	SPCR |= (1 << SPR0);
}

void spi_send(uint8_t spiData){
	
//	PORTB &= 0B11111011; //ss is 0
	
	SPDR = spiData;
	while(!(SPSR & (1 << SPIF))); //while spif is not 1 (aka while spif is 0)

	//PORTB |= (1 << PORTB2); //SS is 1	

}

void spi_receive(){
	
	while(!(SPSR & (1 << SPIF))); //while spif is not 1 (aka while spif is 0)

	//return SPDR;

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
	
	sw("master\n\r");
	
	spi_init();
	uint8_t z=0;
	PORTB &= 0B11111011; //ss is 0, activate slave
	
	while(1)
	{
		PORTB &= 0B11111011; //ss is 0
	z++;
	spi_send(z);
	sw("done sending ");
	swn(z, 10, 1); //print what you send
	
	spi_receive(); //will receive the z +10
	sw("received ");
	swn(SPDR, 10 , 1);//print the data received;
	
	PORTB |= (1 << PORTB2); //SS is 1	
		_delay_ms(500);

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
