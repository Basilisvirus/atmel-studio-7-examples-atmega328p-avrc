/* Make a program that will send a pwm pulse of 3V on the PD6 pin (pin 6) contunuesly. When an external interrupt 
occurs on pin PD2 (pin 2), the pwm will stop for 1 second, and then continue. */

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

int trig = 0;

int main(void){

	//=========================For Serial monitor START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	sei();
	//=========================For serial monitor END
	
	//=========================FOR PWM START
	
	//Fast pwm mode interrupt
	TCCR0B |= (0 << WGM02);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	
	//Clear OC0A/Β/1A/1B on match, set them at BOTTOM, (non-inverting mode)
	TCCR0A |= (1 << COM0A1);
	
	//No prescaler
	TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00);	
	
	//Enable interrupt
	TIMSK0 |= (1 << TOIE0);
	
	//3V match
	OCR0A = 153;
	
	//Set PD6 as output
	DDRD |= 0B01000000;
	
	//============================FOR PWM END
	
	
	//============================FOR EXT INTERRUPT PD2 START
	
	//Interrupt mode
	EICRA |= (1 << ISC01) | (0 << ISC00);// The falling edge of INT0 generates an interrupt request.
	
	DDRD |= (0 << DDD2); // Set PD2 as output
	
	PORTD |= (1 << PORTD2); //Activate pullup resistor 
	
	//Enable Ext interrupt
	EIMSK |= (1 << INT0);
	
	//============================FOR EXT INT PD2 END
	
	while(1){
	
	}//end WHILE

}//End of MAIN


ISR  (TIMER0_OVF_vect){

}

ISR(INT0_vect){

//Disable ext interrupt, prevent more interrupts
EIMSK |= (0 << INT0);

trig++;

serialWrite("Activated\n");

//instead of a delay, use timer
_delay_ms(500);

//re-enable interrupt
EIMSK |= (1 << INT0);

}

//==================================SERIAL START
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
}//==================================SERIAL END
