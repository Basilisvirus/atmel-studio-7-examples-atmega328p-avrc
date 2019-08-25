/*EX2: Make an internal interrupt that uses OCR0B for every 0.5 sec.
This interrupt routine does nothing.
At the same time, use time use OCR0A for another timer of 1 second
that also does nothing.

 */
//==================================SERIAL START
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <string.h>


//no semicolon needed at the end of the following lines
#define BUAD 9600
#define BRC ((F_CPU/16/BUAD)-1)

#define TX_BUFFER_SIZE 128
char serialBuffer[TX_BUFFER_SIZE];

//create a dynamic queue method 
uint8_t serialReadPos = 0; //unsigned 8 bit integer
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char c[]);

//for numbers
char str[40];
//==================================SERIAL END

uint8_t overFlowA=0;
uint8_t lastOvA = 0;

int main(void)
{
//==================================SERIAL START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	
	//ENABLE interrupts
	sei();	
//==================================SERIAL END

	//Set prescaler, /1014 mode
	TCCR0B |= (1 <<CS02) | (0 << CS01) | (1 << CS00);

	//Enable both vectors A and B
	TIMSK0 |= (1 << OCIE0B) | (1 << OCIE0A);
	        
	//Interrupt mode CTC,  TIMER 0
	TCCR0B |= (0 << WGM02) ;
	
	TCCR0A |= (1 << WGM01) | (0 << WGM00);
	
	OCR0A = 255; //after 30Overflows, 132 ticks left.
	OCR0B = 156; //0.01 sec = 1 match. 100matches = 1 sec
	
	while(1)
	{
	      //  itoa(OCR0A, str, 2);
	      //  serialWrite(str);
	       // serialWrite("\n");
	      //serialWrite("hello \n");
		//_delay_ms(1000);
	}

_delay_ms(5000);
}

ISR(TIMER0_COMPA_vect){
	//each match (overflow here) adds +1
	overFlowA++;
	      itoa(overFlowA, str, 2);
	      SerialWrite(str);
	      serialWrite("\n");

	//last match
	if(lastOvA == 1){
		//0.5 sec passed
		OCR0A = 255;//reset OCR0A
		overFlowA =0;//Start counting again
		lastOvA = 0;//last overflow finished
		serialWrite("0.5");
	}


	if (overFlowA == 30){//after 30 matches(overflows),132 ticks left
		OCR0A = 132;	
		lastOvA = 1;//its the last match
	}



}

ISR(TIMER0_COMPB_vect){

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
