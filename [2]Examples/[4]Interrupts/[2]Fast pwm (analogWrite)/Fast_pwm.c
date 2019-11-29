
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


int main (void){

	//select interrupt mode (FAST PWM)
	TCCR0A |= (1 << WGM01) | (1 << WGM00); 
	
	//For some reson, it doesnt work with the second Fast PWM mode, so leave it commented:
	//TCCR0B = (1 << WGM02); 
	
	//Select PWM mode
	TCCR0A |=  (1 << COM0A1);
	
	//No preslacer
	TCCR0B |= ( 1 << CS00);

	//enable interrupt
	TIMSK0 |= (1 << TOIE0);	
	
	//OCRA top value;
	OCR0A = 125; //(256/2) - 1

	//Activate global interrupts
	SREG |= ( 1 << 7);		

	
	//Set port 6 (PD6) as output
	DDRD |= (1 << PORTD6);

	//=========================For Serial monitor START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	//=========================For serial monitor END


		itoa(TCCR0A, buffer, 2);
		serialWrite("TCCR0A value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);
		
		itoa(TCCR0B, buffer, 2);
		serialWrite("TCCR0B value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);

		itoa(TIMSK0, buffer, 2);
		serialWrite("TIMSK0 value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);

		itoa(OCR0A, buffer, 2);
		serialWrite("OCR0A value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);

		itoa(SREG, buffer, 2);
		serialWrite("SREG value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);

		itoa(DDRD, buffer, 2);
		serialWrite("DDRD value is ");
		serialWrite(buffer);
		serialWrite("\n");
	_delay_ms(500);


while(1){
}

}

ISR  (TIMER0_OVF_vect){

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






