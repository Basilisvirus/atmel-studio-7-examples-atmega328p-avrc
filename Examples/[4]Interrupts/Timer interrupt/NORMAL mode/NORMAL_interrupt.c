/*
*Description: How to setup timer interrupt for atmega328p-pu microcontroller, normal mode.

*Special thanks:
thanks https://www.youtube.com/watch?v=cAui6116XKc&list=PLA6BB228B08B03EDD&index=6 for the tutorial. This is where i got the code.
I made the comments to be as clear as possible. Make sure you always check my previous examples on github,
as i wont copy/paste all the same comments from the previous examples.
All my other tutorials are uploaded: https://github.com/Basilisvirus/atmel-studio-7-examples

*Library used: Avr C library, avr-gcc and avrdude
*Microcontroller: a atmega328p-pu is used here.
*Atmel version: i dont use atmel here. check https://github.com/Basilisvirus/atmel-studio-7-examples
*Extra software needed: Arduino ide to see the serial port output on the computer.
*OS: Ubuntu linux 18
*/ 

//define the clock frequency of the micro controller for the delay to work correctly, 16Mhz
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

int extraTime = 0;

int tcntoMax=0;

//=========================For serial monitor START
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)

#define TX_BUFFER_SIZE 128
char serialBuffer[TX_BUFFER_SIZE];

//create a dynamic queue method 
uint8_t serialReadPos = 0; //unsigned 8 bit integer
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char c[]);

//buffer to save the char 
int buffer [30];
//=========================for serial monitor END


int main(void)
{
	//Set interrupt mode
	TCCR0A = 0B00000000; //Normal mode

	//Set prescaler
	TCCR0B |=  (1<< CS00) | (1 << CS02); //1024 prescaler.

	//Enable specific interrupt (the interrupt that we want to use)
	TIMSK |= (1 << OCIE0A); //enable OCIE0A in TIMSK0 interrupt

	//enable all interrupts
	SREG |= 0B10000000;
	//alternatively, you may use sei(). from <avr/interrupt.h> library.
	//sei(); //set external interrupt. It sets the I-bit in the Status Register. [Datasheet page 111, Bit 2 ] 

	//If you also need to change the output of a port, 
	DDRB = 0B00000001; //Setting portB 0x01B as output 

	//=========================For Serial monitor START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	//=========================For serial monitor END



	

	while(1)
	{

//find out the max value of TCNT0. If you wait long enough, it will be 11111111 = 256;
	if(TCNT0 > tcntoMax){
		tcntoMax = TCNT0;
	}

		itoa(TIMSK1, buffer, 2);
		serialWrite("TIMSK1 value is ");
		serialWrite(buffer);
		serialWrite("\n");

		_delay_ms(1000);
	}
}

//Interrupt Service Routine. This is called when the certain interrupt occurs. 
ISR(TIMER0_COMPA_vect){//Timer0, ComparatorA. This interrupt is called a vector

	extraTime++;
	
	//no reason that this activates every 100 stacks.
	if (extraTime > 100){
	
		itoa(tcntoMax, buffer, 2);
		serialWrite("tcntoMax value is ");
		serialWrite(buffer);
		serialWrite("\n");		
	
	//And, if you want change the state of the led.
	PORTB ^= 0B11111111; //XOR Equal " ^= " [ebery time, it changes portb from 00H to FFH]
	
	extraTime =0;
	}

}

//=================================================For serial monitor START
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
			serialReadPos=0;
		}
	}
}
//==================================================For Serial Monitor END

