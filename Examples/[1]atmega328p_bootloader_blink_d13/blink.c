//define the clock frequency of the microcontroller for the delay to work correctly, 16Mhz
#define F_CPU 16000000UL

//the delay.h is only needed when we use the delay function
#include <util/delay.h>

//io.h is always needed cause it includes specific functions for Atmega328.
#include <avr/io.h>

int main(void)
{
	//set the bit 5 of the data directory register of PORT B to 1. this indicates that we want to use
	//the PORT B5 as an output. "|" stands as "OR". 
	DDRB |= 0B00100000;

	while (1)
	{	//by setting bit 5 of PORTB Register, the output bit 5 is set to high
		PORTB |= 0B00100000;
		_delay_ms(1000); //milliseconds
		//Clearing bit 5 in PORTB register, the output of bit 5 is set to low.
		//"&" stands as AND
		PORTB &= 0B11011111;
		_delay_ms(1000);
	}
}
