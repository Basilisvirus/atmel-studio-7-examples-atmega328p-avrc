/*
thanks https://www.youtube.com/watch?v=YR4I0G_ILk4 for the tutorial. This is where i got the code.
I made the comments to be as clear as possible.

a atmega328p-pu is used here.
 */ 

//define the clock frequency of the micro controller for the delay to work correctly, 16Mhz
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>// note here that if you use avr/delay.h, it will error: "This file has been moved to <util/delay.h>." [-Wcpp]	Button_led_control	c:\program files (x86)\atmel\studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr\include\avr\delay.h
 

//I used mostly Hex values instead of binaries as a way to exercise and learn better.

int main(void)
{
	//setting DDRB to 0x01 = 0B00000001, we set port B0 as output, and all the other ports (B1, B2..) as inputs.
	DDRB = 0x01;
	
	//By setting PORTB=0x02=0B00000010, port B0 outputs 0v, port B1 activates its internal pull up resistor.
	//So, if you give a value at an input pin (value 1 to B1 here for example), it activates its pull up.
	PORTB = 0x02;
	
    /* Replace with your application code */
    while (1) 
    {
		/*
		PINx is a register, that automatically gets updated with the port's values on every machine cyrcle.
		Here, PINB gets port B state continuously. 
		*/		
		if(!(PINB & 0x02)){ //if (!(PINB == 1)) aka if (!(PINB == 0B00000010)), meaning if port B1 connects to Ground, we pressed the pin, and we can activate the B0
			PORTB |= 0X01; //turn on B0 using OR so that i wont affect the other bits 
		}
		else{
			PORTB &= 0xFE;	//use AND to turn off only B0 so that i wont affect other bits.
		}
    }
}

