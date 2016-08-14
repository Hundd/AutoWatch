#include "mainHeader.h"



void Display7seg(uint8_t digit, bool dp = false)
{
	ClrScr();
	if(dp) PORTB |= 1<<PORTB4;
	
	switch(digit){
		/*
			  PORTD1
		PORTD5		PORTB0
			  PORTB2
		PORTC2		PORTB3
			  PORTB5		PORTB4

		*/
		
		case 0: PORTB |=1<<PORTB0|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1|1<<PORTD5;
				PORTC |=1<<PORTC2;
				break;
		case 1: PORTB |=1<<PORTB0|1<<PORTB3;
				break;
		case 2: PORTB |=1<<PORTB0|1<<PORTB2|1<<PORTB5;
				PORTD |=1<<PORTD1;
				PORTC |=1<<PORTC2;
				break;
		case 3: PORTB |=1<<PORTB0|1<<PORTB2|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1;
				break;
		case 4: PORTB |=1<<PORTB0|1<<PORTB2|1<<PORTB3;
				PORTD |=1<<PORTD5;
				break;
		case 5: PORTB |=1<<PORTB2|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1|1<<PORTD5;
				break;
		case 6: PORTB |=1<<PORTB2|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1|1<<PORTD5;
				PORTC |=1<<PORTC2;
				break;
		case 7: PORTB |=1<<PORTB0|1<<PORTB3;
				PORTD |=1<<PORTD1|1<<PORTD5;
				break;
		case 8: PORTB |=1<<PORTB0|1<<PORTB2|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1|1<<PORTD5;
				PORTC |=1<<PORTC2;
				break;
		case 9: PORTB |=1<<PORTB0|1<<PORTB2|1<<PORTB3|1<<PORTB5;
				PORTD |=1<<PORTD1|1<<PORTD5;
				break;
	  case 'u': PORTB |=1<<PORTB3|1<<PORTB5;
				PORTC |=1<<PORTC2;
				break;
	  case '-': PORTB |= 1<<PORTB2;
				break;
		default: ClrScr();
				break;

	}
}
void ClrScr()
{
	DDRC  |= 1<<PORTC2;
	PORTC &= ~(1<<PORTC2);

	DDRD  |= 1<<PORTD1|1<<PORTD5;
	PORTD &= ~(1<<PORTD1|1<<PORTD5);

	DDRB  |= 1<<PORTB0|1<<PORTB2|1<<PORTB3|1<<PORTB4|1<<PORTB5;
	PORTB &= ~(1<<PORTB0|1<<PORTB2|1<<PORTB3|1<<PORTB4|1<<PORTB5);
	///
	DDRB |= 1<<PORTB1;
	DDRC |= 1<<PORTC3;
	DDRD |= 1<<PORTD6|1<<PORTD7;
	PORTB |= 1<<PORTB1;
	PORTC |= 1<<PORTC3;
	PORTD |= 1<<PORTD6|1<<PORTD7;


	//PORTB &= ~(1<<PORTB4 | 1<<PORTB3);
	//PORTD &= ~(1<<PORTD1 | 1<<PORTD2);
	//PORTC &= ~(1<<PORTC0);
}

void NextDigit(uint8_t number)
{
	
	switch(number)
	{
		case 1:
		PORTC &= ~(1<<PORTC3);
		break;
		case 2:
		PORTD &= ~(1<<PORTD6);
		break;
		case 3:
		PORTD &= ~(1<<PORTD7);
		break;
		case 4:
		PORTB &= ~(1<<PORTB1);
		break;
	}
	
}