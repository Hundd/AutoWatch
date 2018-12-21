#define MAXGRADES 15
#define TIMER_ALARM_TIME 27

#include "mainHeader.h"
#include "termo_teplica.c"
#include "ADC.cpp"
volatile uint8_t screen_arr[5];
volatile int8_t opacity = MAXGRADES;
volatile void EnableTimer0Interrupt();
AdcClass adcOb;
uint8_t rtcCycle();
uint8_t dsMode();
uint8_t acbMode();
uint8_t setupMinutes();
uint8_t setupHours();
uint8_t setupTimer();
uint8_t keyChecker();
uint8_t timerMode();
void clrScreenArray();
void displaySeconds(uint16_t secondsTotal);
void buzzer(bool on);

uint8_t alarmTime = TIMER_ALARM_TIME;

uint8_t *EEP_TIMER_ALARM_TIME = 0;

int main(void)
{

	// initialize EEPROM
	if (eeprom_read_byte(EEP_TIMER_ALARM_TIME) == 0xFF)
	{
		eeprom_write_byte(EEP_TIMER_ALARM_TIME, alarmTime);
	}
	else
	{
		alarmTime = eeprom_read_byte(EEP_TIMER_ALARM_TIME);
	}

	uint8_t currentScreen = 0;

	DDRD &= ~(1 << 3 | 1 << PORTD4);	//Change direction to input
	PORTD |= 1 << PORTD3 | 1 << PORTD4; //Enable pull-up resistors

	clrScreenArray(); //Start from blank screen
	EnableTimer0Interrupt();
	sei();
	rtcInit();
	adcOb.Init();
	buzzer(false); // Turn off buzzer

	while (1)
	{
		switch (currentScreen)
		{
		case 0:
			currentScreen = rtcCycle();
			break;
		case 1:
			currentScreen = dsMode();
			break;
		case 2:
			currentScreen = acbMode();
			break;
		case 3:
			currentScreen = setupMinutes();
			break;
		case 4:
			currentScreen = setupHours();
			break;
		case 5:
			currentScreen = timerMode();
			break;
		case 6:
			currentScreen = setupTimer();
			break;
		default:
			currentScreen = rtcCycle();
			break;
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t iov = 0;
	static uint8_t opacityDelay = 0;

	if (opacityDelay == MAXGRADES)
	{
		ClrScr();
		Display7seg(screen_arr[iov], iov == screen_arr[4]);
		NextDigit(4 - iov);
		iov += 1;
		if (iov > 3)
			iov = 0;
	}
	else if (opacityDelay == (MAXGRADES - opacity))
	{
		ClrScr();
	}
	opacityDelay = opacityDelay < MAXGRADES ? opacityDelay + 1 : 0;
}

volatile void EnableTimer0Interrupt()
{
	TIMSK0 |= 1 << TOIE0;
	TCCR0B = 1; //Prescaler
}

void resetTimer()
{
	screen_arr[4] = 2; // DP position
	screen_arr[3] = 0;
	screen_arr[2] = 0;
	screen_arr[1] = 0;
	screen_arr[0] = 0;
}

void buzzer(bool on)
{
	DDRB |= 1 << PORTB6;

	if (on)
	{
		PORTB |= 1 << PORTB6;
	}
	else
	{
		PORTB &= ~(1 << PORTB6);
	}
}

void doubleBeep()
{
	buzzer(true);
	_delay_ms(50);
	buzzer(false);
	_delay_ms(50);
	buzzer(true);
	_delay_ms(50);
	buzzer(false);
}

void longBeep()
{
	buzzer(true);
	_delay_ms(500);
	buzzer(false);
}

void singleBeep()
{
	buzzer(true);
	_delay_ms(100);
	buzzer(false);
}

uint8_t setupTimer()
{
	while (true)
	{
		displaySeconds(alarmTime);
		uint8_t key = keyChecker();

		if (key)
		{
			if (key == 1 && alarmTime < 255)
			{
				// short press button 1
				alarmTime += 1;
				singleBeep();
			}

			if (key == 2 && alarmTime > 0)
			{
				// short press button 2
				alarmTime -= 1;
				singleBeep();
			}

			if (key == 3)
			{
				// long press button 2
				eeprom_write_byte(EEP_TIMER_ALARM_TIME, alarmTime);
				return 0;
			}
		}
	}
}

void displaySeconds(uint16_t secondsTotal)
{
	screen_arr[0] = secondsTotal % 10;
	screen_arr[1] = secondsTotal % 60 / 10;
	screen_arr[2] = secondsTotal / 60 % 10;
	screen_arr[3] = secondsTotal / 60 / 10;
}

uint8_t timerMode()
{
	uint8_t timeArr[0x13];
	uint8_t currentSeconds = 0;
	uint8_t lastSeconds = 0;
	uint16_t secondsTotal = 0;
	bool conunerEnabled = false;
	bool buzzering = false;
	resetTimer();
	doubleBeep();

	while (true)
	{
		readTime(timeArr);
		currentSeconds = timeArr[0];

		if (conunerEnabled && currentSeconds != lastSeconds)
		{
			secondsTotal += 1;
		}

		lastSeconds = currentSeconds;

		displaySeconds(secondsTotal);

		if (conunerEnabled)
		{
			if (secondsTotal == alarmTime)
			{
				buzzering = true;
				buzzer(true);
			}
			if (buzzering && secondsTotal <= (uint16_t)(alarmTime + 4) && secondsTotal > alarmTime)
			{
				buzzer((secondsTotal - alarmTime) % 2 - 1);
			}

			if (buzzering && secondsTotal > (uint16_t)(alarmTime + 4))
			{
				buzzer(false);
			}
		}

		if (secondsTotal > 15 * 60)
		{
			return 0;
		}

		uint8_t key = keyChecker();

		if (key)
		{
			if (key == 1)
			{
				// short press button 1
				singleBeep();
				buzzering = false;
				conunerEnabled = true;
			}

			if (key == 2)
			{
				// short press button 2
				doubleBeep();
				secondsTotal = 0;
				conunerEnabled = false;
				buzzering = false;
				resetTimer();
			}

			if (key == 3)
			{
				// long press button 2
				longBeep();
				return 0;
			}
		}

		_delay_ms(1);
	}
}

uint8_t rtcCycle()
{
	int8_t startLowEnergy = -1;
	int8_t endLowEnergy = -1;
	static double adcAvg = 1023;

	uint8_t timeArr[0x13];

	bool powerGoodDelay = true;

	while (true)
	{
		bool powerGood = PIND & 1;
		//bool powerGood = false;

		const uint8_t POWERDELAY = 5; //Delay Before the lights is off

		readTime(timeArr);

		if (powerGood || powerGoodDelay)
		{
			screen_arr[0] = timeArr[1] & 0x0F;
			screen_arr[1] = timeArr[1] >> 4;
			screen_arr[2] = timeArr[2] & 0x0F;
			screen_arr[3] = timeArr[2] >> 4;
			screen_arr[4] = 2; //Dot Point Position
		}
		if (powerGood)
		{
			startLowEnergy = -1;
			endLowEnergy = -1;
			powerGoodDelay = true;
		}
		else
		{
			if (startLowEnergy == -1)
			{
				startLowEnergy = ((timeArr[1] & 0x0F) + (timeArr[1] >> 4) * 10 + POWERDELAY) % 60;
			}
			else
			{
				endLowEnergy = (timeArr[1] & 0x0F) + (timeArr[1] >> 4) * 10;
				if (startLowEnergy == endLowEnergy)
				{
					clrScreenArray();
					powerGoodDelay = false;
				}
			}
		}

		uint8_t key = keyChecker();
		if (key)
		{
			if (key == 1)
			{
				// short press button 1
				if (!powerGoodDelay)
				{
					powerGoodDelay = true;
					startLowEnergy = -1;
					endLowEnergy = -1;
				}
				else
					return 1;
			}

			if (key == 2)
			{
				// short press button 2
				return 5;
			}

			if (key == 3)
			{
				// long press button 2
				return 3;
			}
		}
		adcOb.MesureVoltage(PORTC1);

		//Opacity Correction
		const double corrector = 1024;
		adcAvg = adcAvg * (corrector - 1) / corrector + (double)adcOb.Data / corrector;
		opacity = MAXGRADES - adcAvg / (1024 / (MAXGRADES + 1));
		if (opacity < 0)
			opacity = 0;
	}
}

uint8_t dsMode()
{
	int16_t temper = 0;
	uint8_t temperatureArr[9];

	screen_arr[4] = 1;
	screen_arr[3] = '-';
	screen_arr[2] = '-';
	screen_arr[1] = '-';
	screen_arr[0] = '-';
	uint16_t inner_counter = 0;
	uint8_t exit_counter = 0;
	while (true)
	{
		if (inner_counter == 0)
		{
			ReadTemperature_StartConvert(&PORTD, &DDRD, &PIND, PORTD2);
			sei();
		}
		if (inner_counter == 1000)
		{
			ReadTemperatureArr(&PORTD, &DDRD, &PIND, PIND2, temperatureArr);
			temper = temperatureArr[1];
			temper <<= 8;
			temper |= temperatureArr[0];

			if (temper < 0)
			{
				screen_arr[3] = '-';
				temper *= -1;
				temper += 1;
			}
			else
			{
				screen_arr[3] = -1;
			}

			screen_arr[0] = ((temper & 0x0F) * 10) / 16;

			temper >>= 4;
			if (temper >= 100)
			{
				screen_arr[3] = temper / 100;
				temper %= 100;
			}
			screen_arr[2] = (temper / 10) ? (temper / 10) : -1;
			screen_arr[1] = temper % 10;
		}

		uint8_t key = keyChecker();
		if (key)
		{
			if (key == 1)
				return 2;

			if (key == 2)
			{
				// short press button 2
				return 5;
			}
		}
		_delay_ms(1);
		if (inner_counter <= 1000)
			inner_counter += 1;
		else
		{
			inner_counter = 0;
			exit_counter += 1;
			if (exit_counter > 7)
				return 0;
		}
	}
}

uint8_t acbMode()
{
	screen_arr[4] = 1;
	screen_arr[3] = 'u';
	screen_arr[2] = -1;
	screen_arr[1] = -1;
	screen_arr[0] = -1;
	sei();
	uint16_t exitCouter = 0;
	adcOb.MesureVoltage(PORTC0);
	_delay_ms(10);
	adcOb.MesureVoltage(PORTC0);
	screen_arr[2] = (uint8_t)adcOb.Voltage / 10;
	screen_arr[1] = (uint8_t)adcOb.Voltage % 10;
	screen_arr[0] = ((uint8_t)(adcOb.Voltage * 10)) % 10;
	while (true)
	{

		uint8_t key = keyChecker();
		if (key)
		{
			if (key == 1)
				return 0;
			//if (key == 2) return 3;
		}
		_delay_ms(1);
		exitCouter += 1;
		if (exitCouter % 1000 == 0)
		{
			//Do it every second
			adcOb.MesureVoltage(PORTC0);
			_delay_ms(1);
			screen_arr[2] = (uint8_t)adcOb.Voltage / 10;
			screen_arr[1] = (uint8_t)adcOb.Voltage % 10;
			screen_arr[0] = ((uint8_t)(adcOb.Voltage * 10)) % 10;
		}
		if (exitCouter > 7000)
			return 0;
	}
}

uint8_t setupMinutes()
{
	uint8_t minutes = screen_arr[0] + 10 * screen_arr[1];
	uint16_t blinker = 0;
	while (true)
	{
		uint8_t key = keyChecker();
		if (key)
		{
			if (key == 1)
			{
				minutes += 1;
				minutes %= 60;
			}
			if (key == 2)
			{
				rtcSendMinutes(minutes);
				screen_arr[1] = minutes / 10;
				screen_arr[0] = minutes % 10;
				return 4;
			}
		}
		if (blinker > 250)
		{
			screen_arr[1] = minutes / 10;
			screen_arr[0] = minutes % 10;
		}
		else
		{
			screen_arr[1] = -1;
			screen_arr[0] = -1;
		}
		if (blinker < 500)
		{
			blinker += 1;
		}
		else
		{
			blinker = 0;
		}
		_delay_ms(1);
	}
}
uint8_t setupHours()
{
	uint8_t hours = screen_arr[2] + 10 * screen_arr[3];
	uint16_t blinker = 0;
	while (true)
	{
		uint8_t key = keyChecker();
		if (key)
		{
			if (key == 1)
			{
				hours += 1;
				hours %= 24;
			}
			if (key == 2)
			{
				rtcSendHours(hours);

				return 6;
			}
		}
		if (blinker > 250)
		{
			screen_arr[3] = hours / 10;
			screen_arr[2] = hours % 10;
		}
		else
		{
			screen_arr[3] = -1;
			screen_arr[2] = -1;
		}
		if (blinker < 500)
		{
			blinker += 1;
		}
		else
		{
			blinker = 0;
		}
		_delay_ms(1);
	}
}

uint8_t keyChecker()
{
	/*
*PORTD3 MODE
*PORTD4 SETUP
*/
	static uint8_t keyStatePrevious = 0;
	static uint16_t times = 0;
	static uint8_t savedKeyState = 0;
	uint8_t keyState = PIND;
	keyState = (~keyState >> 3) & 3;

	if (keyState == keyStatePrevious && keyState)
	{
		if (times < 1500)
		{
			times += 1;
			_delay_ms(1);
		}
		if (times == 10)
		{
			savedKeyState = keyState;
		}
		if (times == 600)
		{
			longBeep();
			savedKeyState = 3;
		}
	}
	else if (savedKeyState)
	{
		uint8_t tempKeyState = savedKeyState;
		savedKeyState = 0;

		return tempKeyState;
	}
	else
	{
		keyStatePrevious = keyState;
		times = 0;
	}

	return 0;
}
void clrScreenArray()
{
	screen_arr[0] = -1;
	screen_arr[1] = -1;
	screen_arr[2] = -1;
	screen_arr[3] = -1;
	screen_arr[4] = -1;
}