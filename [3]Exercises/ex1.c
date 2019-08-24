
/*
EX1: Make an interrupt of every 1 second. In its route handle, it will roll a '1' to the right on PORTB. When the '1' reaches the end 
of PORTB, it wil begin again from the start.
example of PORTB:
10000000
01000000
...
00000001
10000000
01000000
..etc

Use AVR-C and any other library you may need.

You can use serial print to print the output. (From folder [2]Examples/[3]Serial)

Solution:

*/

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


//==================================SERIAL START
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
//==================================SERIAL END

//tick counter
int ticks = 0;

char str[40];




int main(void){
   //==================================SERIAL START
	UBRR0H = (BRC >> 8); //Put BRC to UBRR0H and move it right 8 bits.
	UBRR0L = BRC;
	UCSR0B = (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 BIT data frame
	//ENABLE interrupts
	sei();
	//==================================SERIAL END

   //CTC mode will be used for 1 sec interrupt. From Atmega memory map, TCCR0B will select CTC mode.
   //Looking in the datasheet, Mode 2 is CTC.
   TCCR0A = 0B00000010; //WGM01 =1 , WGM00 =0
   TCCR0B = 0B00000000;  //WGM02 =0
   
   //TCCR0B also controls the prescaling. /1024 prescaling used
     TCCR0B |= 0B00000101; //TCCR0B OR 00000101 [CS02 = 1, CS01 = 0, CS00 = 1]
     
   //We will use OCR0A Match interrupt (Timer0 Match A Interrupt).
   //From folder[2]Examples/[4]Interrupts/[1]Timer interrupt/[1]CTC mode/[1]Timer0_OCIE0A,
   //we use the link: https://eleccelerator.com/avr-timer-calculator/
   //With 1 sec at "Real Time (sec):" we get 15625 of total ticks, 61 OV counts and 9 remainder timer ticks.
   OCR0A = 0B11111111; //TOP.
   
     
   //Interrupt enable. Using TIMSK0
   TIMSK0 |= (1 << OCIE0A); //Enable Timer0 Match A interrupt.  
   


	while(1)
	{
	   itoa(ticks, str, 10);
	   
	   serialWrite(str);
	   serialWrite("\n");
	   _delay_ms(1000);
	}

   _delay_ms(5000);


}




ISR(TIMER0_COMPA_vect){
   
   ticks ++;
   
   if (ticks ==  61){
      ticks =0;
      //1 sec passed
      serialWrite("Hello\n\r");
   }



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


















