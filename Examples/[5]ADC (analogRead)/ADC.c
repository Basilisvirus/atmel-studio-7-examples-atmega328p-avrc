/*
Thanks to : https://www.youtube.com/watch?v=51QJ_WHN7u0 for the tutorial.

In this tutorial, we will be reading a value from an analog pin ADC5 and display it to the screen.

*/



#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

int extraTime = 0;


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


void setupADC()
{
	//Select voltage reference section
	ADMUX |= (1 << REFS0); // (AVcc with external capacitor)
	
	//Select which ADC pin will be used
	ADMUX |= (1 << MUX0) | (1 << MUX2); // (ADC5)

	//Left-adjusted values, 10-bit precision. (1024 max)
	ADMUX |= (1 << ADLAR);

	//Enable ADC
   ADCSRA |= (1 << ADEN);

	//Enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	//ADC Prescaler select
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); //128 prescaler
   
//Disable input digital buffer. So you dont try to read it by mistake and take a digital value. Only analog values.
	DIDR0 |= (1 << ADC5D);
     
    startConversion();
}
 
void startConversion()
{
	//Resets every time
   ADCSRA |= (1 << ADSC);
}


int main(void){

	//=========================For Serial monitor START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	//=========================For serial monitor END

	//Enable all interrupts
	SREG |= (1 << 7);

	setupADC();

	itoa(ADMUX, buffer, 2);
	serialWrite("ADMUX value is ");
	serialWrite(buffer);
	serialWrite("\n");
	_delay_ms(1500);

	itoa(ADCSRA, buffer, 2);
	serialWrite("ADCSRA value is ");
	serialWrite(buffer);
	serialWrite("\n");
	_delay_ms(1500);

	itoa(DIDR0, buffer, 2);
	serialWrite("DIDR0 value is ");
	serialWrite(buffer);
	serialWrite("\n");
	_delay_ms(1500);
	

	while(1){
		itoa(ADCL, buffer, 2);		
		serialWrite("ADCL value is ");
		serialWrite(buffer);
		serialWrite("\n");

		itoa(ADCH, buffer, 2);
		serialWrite("ADCH value is ");
		serialWrite(buffer);
		serialWrite("\n");

	_delay_ms(1500);

	}


}



ISR(ADC_vect)
{
    startConversion();
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




