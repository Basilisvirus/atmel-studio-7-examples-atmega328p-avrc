/*
Thanks to : https://www.youtube.com/watch?v=aT1tU0EnSHw for the tutorial.

External interrupt setup.

*/


#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>



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

int trig =0;

int main(void){
	
	//Set PD3 (INT1) as input. ( = 0 )
	DDRD &= 0B11110111;

	//Enable pull-up resitor of INT1 (PD3)
	PORTD = 0B00001000;
	
	//=========================For Serial monitor START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	//=========================For serial monitor END

	//Enable all interrupts
	SREG |= (1 << 7);

	itoa(DDRD, buffer, 2);
	serialWrite("DDRD value is ");
	serialWrite(buffer);
	serialWrite("\n");
	_delay_ms(1000);

	itoa(PORTD, buffer, 2);
	serialWrite("PORTD value is ");
	serialWrite(buffer);
	serialWrite("\n");
	_delay_ms(1000);

	//Mode of external interrupt
	EICRA |= ( 0 << ISC11) | ( 0 << ISC10); //low level INT1

	//Activate external interrupt
	EIMSK |= (1 << INT1);


	while(1){
		if(trig >= 1){
			itoa(trig, buffer, 10);
			serialWrite(buffer);
			serialWrite("\n")	;
			trig =0;
		}
}

}


ISR(INT1_vect){
	SREG |= (0 << 7); //prevent more interrupts
	trig ++;
	serialWrite("Trig");
	_delay_ms(200);
	SREG |= (1 << 7);
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




