 
#include "mainHeader.h"
#include "termo_teplica.c"
#include "ADC.cpp"
volatile uint8_t screen_arr[5];
volatile void EnableTimer0Interrupt();
uint8_t rtcCycle();
uint8_t dsMode();
uint8_t acbMode();
uint8_t setupMinutes();
uint8_t setupHours();
uint8_t keyChecker();
void clrScreenArray ();

int main(void)
{
	uint8_t currentScreen = 0;
	
	/*
		current Screen = 0 - regular clock mode;
		current Screen = 1 - ds18b20 mode;
		current Screen = 2 - accumulator mode;
		current Screen = 3 - setup minutes mode;
		current Screen = 4 - setum hours mode;
	*/

	DDRD &=~(1<<PORTD3|1<<PORTD4); //Change direction to input
	PORTD |= 1<<PORTD3|1<<PORTD4; //Enable pull-up resistors

	clrScreenArray();//Start from blank screen
	EnableTimer0Interrupt();
	sei();
	rtcInit();
    while (1) 
    {
		switch (currentScreen) {
			case 0: currentScreen = rtcCycle();
					break;
			case 1: currentScreen = dsMode();
					break;
			case 2: currentScreen = acbMode();
					break;
			case 3: currentScreen = setupMinutes();
					break;
			case 4: currentScreen = setupHours();
					break;
			default: currentScreen = rtcCycle();
					 break;
		}

    }
}

ISR(TIMER0_OVF_vect){
	static uint8_t iov=0;
	ClrScr();
	Display7seg(screen_arr[iov],iov == screen_arr[4]);
	NextDigit(4-iov);
	//Display7seg(screen_arr[iov],iov == screen_arr[4]);
	iov += 1;
	if(iov > 3) iov=0;
}
volatile void EnableTimer0Interrupt() {
	
	TIMSK0|=1<<TOIE0;
	TCCR0B = 2;//Prescaler
}
uint8_t rtcCycle () {
	uint8_t timeArr[0x13];
	while (true) {
		readTime(timeArr);
		screen_arr[0] = timeArr[1] & 0x0F;
		screen_arr[1] = timeArr[1] >> 4;
		screen_arr[2] = timeArr[2] & 0x0F;
		screen_arr[3] = timeArr[2] >> 4;
		screen_arr[4] = 2;//Dot Point Position
		//_delay_ms(100);
		uint8_t key = keyChecker();
		if(key) {
			if (key == 1) return 1;
			if (key == 3) {
			
			return 3;
			}

		}
	}
	
}

uint8_t dsMode() {
	int16_t temper = 0;
	uint8_t temperatureArr[9];

	screen_arr[4] = 1;
	screen_arr[3] = '-';
	screen_arr[2] = '-';
	screen_arr[1] = '-';
	screen_arr[0] = '-';
	uint16_t inner_counter = 0;
	uint8_t exit_counter = 0;
	while(true){
		if(inner_counter == 0)
		{
			ReadTemperature_StartConvert(&PORTD,&DDRD,&PIND,PORTD2);
			sei();
		}
		if(inner_counter == 1000)
		{
			ReadTemperatureArr(&PORTD,&DDRD,&PIND,PIND2,temperatureArr);
			temper = temperatureArr[1];
			temper <<= 8;
			temper |= temperatureArr[0];
					
					
			if(temper < 0) {
				screen_arr[3] = '-';
				temper *= -1;
				temper += 1;
			}
			else {
				screen_arr[3] = -1;
			}
					
					
			screen_arr[0] = ((temper & 0x0F)*10)/16;
					
			temper>>=4;
			if(temper >= 100) {
				screen_arr[3] = temper/100;
				temper %= 100;
			}
			screen_arr[2] = (temper/10)?(temper/10):-1;
			screen_arr[1] = temper%10;
		}


		uint8_t key = keyChecker();
		if(key) {
			if (key == 1) return 2;
			//if (key == 2) return 3;

		}
		_delay_ms(1);
		if(inner_counter <=1000)
		inner_counter +=1;
		else {
			inner_counter =0;
			exit_counter +=1;
			if(exit_counter>7) return 0;
		}
	}
}

uint8_t acbMode() {
		screen_arr[4] = 1;
		screen_arr[3] = 'u';
		screen_arr[2] = -1;
		screen_arr[1] = -1;
		screen_arr[0] = -1;
		AdcClass adcOb;
		adcOb.Init();
		sei();
		uint16_t exitCouter = 0;
		while(true){
			adcOb.MesureVoltage(PORTC0);
			screen_arr[2] = (uint8_t)adcOb.Voltage/10;
			screen_arr[1] = (uint8_t)adcOb.Voltage%10;
			screen_arr[0] = ((uint8_t)(adcOb.Voltage*10))%10;


			uint8_t key = keyChecker();
			if(key) {
				if (key == 1) return 0;
				//if (key == 2) return 3;

			}
			_delay_ms(1);
			exitCouter += 1;
			if(exitCouter > 7000) return 0;
		}
}

uint8_t setupMinutes () {
	uint8_t minutes = screen_arr[0] + 10 * screen_arr [1];
	uint16_t blinker = 0;
	while(true){
		uint8_t key = keyChecker();
		if(key) {
			if (key == 1) {
				minutes += 1;
				minutes %= 60;
				
			}
			if (key == 2) {
			rtcSendMinutes (minutes);
			screen_arr [1] = minutes / 10;
			screen_arr [0] = minutes % 10;
			return 4;
			}

		}
		if (blinker > 250) {
			screen_arr [1] = minutes / 10;
			screen_arr [0] = minutes % 10;
		}
		else {
			screen_arr [1] = -1;
			screen_arr [0] = -1;
		}
		if (blinker < 500) {
			blinker += 1;
		}
		else {
			blinker = 0;
		}
		_delay_ms(1);

	}
}
uint8_t setupHours () {
	uint8_t hours = screen_arr[2] + 10 * screen_arr [3];
	uint16_t blinker = 0;
	while(true){
		uint8_t key = keyChecker();
		if(key) {
			if (key == 1) {
				hours += 1;
				hours %= 24;
				
			}
			if (key == 2) {
			rtcSendHours (hours);
			return 0;
			}

		}
		if (blinker > 250) {
			screen_arr [3] = hours / 10;
			screen_arr [2] = hours % 10;
		}
		else {
			screen_arr [3] = -1;
			screen_arr [2] = -1;
		}
		if (blinker < 500) {
			blinker += 1;
		}
		else {
			blinker = 0;
		}
		_delay_ms(1);


	}
}

uint8_t keyChecker () {
/*
*PORTD3 MODE
*PORTD4 SETUP
*/	static uint8_t keyStatePrevious = 0;
	static uint16_t times = 0;
	//DDRD &=~(1<<PORTD3|1<<PORTD4); //Change direction to input
	//PORTD |= 1<<PORTD3|1<<PORTD4; //Enable pull-up resistors
	uint8_t keyState = PIND;
	 keyState = (~keyState >> 3) & 3;
	if (keyState == keyStatePrevious && keyState) {
		if (times < 1500) {
			times += 1;
			_delay_ms(1);
		}
		if (times == 10) {
			return keyState;
		}
		if (times == 1000) {
			return 3;
		}
	}
	else {
		keyStatePrevious = keyState;
		times = 0;
	}

	return 0;
}
void clrScreenArray () {
	screen_arr[0]=-1;
	screen_arr[1]=-1;
	screen_arr[2]=-1;
	screen_arr[3]=-1;
	screen_arr[4]=-1;

}