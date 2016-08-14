/*
 * termo_teplica.c
 *
 * Created: 07.07.2013 10:43:32
 *  Author: hundd
 */ 

//#undef F_CPU
//# define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
void SendByteToPort(volatile uint8_t *port,volatile uint8_t *ddr,uint8_t pin,uint8_t byte);
void reset(volatile uint8_t *port,volatile uint8_t * ddr,uint8_t pin);
void ReadBytesFromPort(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin,uint8_t *bytes,uint8_t number_of_bytes);
uint8_t CaseDigit(uint8_t digit);
uint8_t ReadTemperature          (volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin);
void ReadTemperature_StartConvert(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin);
uint8_t temperature_byte1 = 0xFF;
uint8_t temperature_byte2 = 0xFF;
/*int main(void)
{	DDRC=0xFF;
	PORTC=0;
	DDRD=0xFF;
	PORTD=0;
	PORTD=0;
	sei();
	TIMSK|=1<<TOIE0;
	TCCR0 = 4;
	wdt_enable(5);
    while(1)
    {	const uint8_t low_temp = 18;
		const uint8_t high_temp = 20;
		//uint8_t temp;
		ReadTemperature_StartConvert(&PORTD,&DDRD,&PIND,PD4);
		ReadTemperature_StartConvert(&PORTD,&DDRD,&PIND,PD5);
		_delay_ms(1000);
		temperature_byte1 = ReadTemperature(&PORTD,&DDRD,&PIND,PD4);
		temperature_byte2 = ReadTemperature(&PORTD,&DDRD,&PIND,PD5);
		//temp = PORTD;
		if(temperature_byte1 < low_temp) PORTD |=(1<<PD2);
		if(temperature_byte1 >= high_temp) PORTD &=~(1<<PD2);
		if(temperature_byte2 < low_temp) PORTD |= (1<<PD3);
		if(temperature_byte2 >= high_temp) PORTD &=~(1<<PD3);
		
    }
}
*/
void reset(volatile uint8_t *port,volatile uint8_t * ddr,uint8_t pin)
{
	*ddr|=(1<<pin);
	*port&=~(1<<pin);//pin off
	_delay_us(500);
	*port|=1<<pin;//pint on
	*ddr&=~(1<<pin);
	_delay_ms(1);
	
	
}
void SendToPort1(volatile uint8_t *port,volatile uint8_t *ddr,uint8_t pin)
{	cli();
	*ddr|=(1<<pin);
	*port&=~(1<<pin);//pin off
	_delay_us(4);
	*ddr&=~(1<<pin);
	//*port|=1<<pin;//pint on
	//sei();
	_delay_us(76);
}
void SendToPort0(volatile uint8_t *port,volatile uint8_t *ddr,uint8_t pin)
{	cli();
	*ddr|=(1<<pin);
	*port&=~(1<<pin);//pin off
	_delay_us(64);
	*ddr&=~(1<<pin);
	_delay_us(16);
	//sei();
}
	void SendByteToPort(volatile uint8_t *port,volatile uint8_t *ddr,uint8_t pin,uint8_t byte)
	{	int i; int 	mask=1;
		
		for (i=0;i<8;i++)
		{	if (byte&mask)
			SendToPort1(port,ddr,pin);
			else SendToPort0(port,ddr,pin);
			byte>>=1;
		
		}
	
		

	}
	void ReadBytesFromPort(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin,uint8_t *bytes,uint8_t number_of_bytes)
	{
		int i=0;//counter of bytes
		int j=0;//counter of bits
		cli();
		for (i=0;i<number_of_bytes;i++)
		{	bytes[i]=0;
			for (j=0;j<8;j++)
			{
				*ddr|=(1<<pin);
				*port&=~(1<<pin);//pin off
				_delay_us(2);//4
				*ddr&=~(1<<pin);
				_delay_us(16);//32
				//bytes[i]|=(*port_pin)&0x80;
				bytes[i]|=((*port_pin)&(1<<pin))<<(7-pin);
				if(j<7) bytes[i]>>=1;
				_delay_us(44);
			}			
		}
		sei();
	}
	
	
	uint8_t ReadTemperature(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin)
	{
		
		uint8_t temperature [9];
		uint8_t temperature_byte;
	//	uint8_t temperature_byte_l;
	//	uint8_t temperature_sign=0;
		reset(port,ddr,pin);
		SendByteToPort(port,ddr,pin,0xCC);//select all devices
		SendByteToPort(port,ddr,pin,0xbe);//read memory
		ReadBytesFromPort(port,ddr,port_pin,pin,temperature,9);
		temperature_byte=((temperature[0]&0xF0)>>4)|((temperature[1]&0xF)<<4);//здесь хранится целое значение температуры;
		//return temperature[8];
		/*temperature_sign=temperature[1]&0xF0;
		
		temperature_byte_l=temperature[0];
		if(temperature_sign)
		{
			temperature_byte=~temperature_byte;
			temperature_byte_l=~temperature_byte_l;
			temperature_byte_l&=0xF;
			temperature_byte_l++;
			if(temperature_byte_l>0xF)
			{
				temperature_byte++;
				temperature_byte_l&=0xF;
				
			}
			
			
		}
		else
		temperature_byte_l&=0xF;
		temperature_byte_l=temperature_byte_l*10/16;
		
		temperature_to_dispay[0]=temperature_byte_l;
		temperature_to_dispay[1]=temperature_byte%10;
		temperature_to_dispay[2]=temperature_byte>100?(temperature_byte-100)/10:temperature_byte/10;
		if(temperature_sign)
		temperature_to_dispay[3]=10;
		else if (temperature_byte/100) temperature_to_dispay[3]=1;
		else temperature_to_dispay[3]=0;*/
		
		
		return temperature_byte;
	}
	
	/*
	ISR(TIMER0_OVF_vect)
	{
		static uint8_t i=0;
		uint8_t  temp;
		const uint8_t mask = 0x3;
		wdt_reset();
		if(i)
		{
			
			temp = PORTD;
			temp|=mask;
			temp&=0xFE;
			PORTC=0xFF;
			PORTD = temp;
			PORTC = ~temperature_byte1;			
			i=0;
		}
		else
		{
			
			temp = PORTD;
			temp|=mask;
			temp&=0xFD;
			PORTC=0xFF;
			PORTD = temp;
			PORTC = ~temperature_byte2;			
			i=1;
		}
		
		
	}
	*/
	
	void ReadTemperature_StartConvert(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin)
	{
		reset(port,ddr,pin);
		SendByteToPort(port,ddr,pin,0xCC);//select all devices

		SendByteToPort(port,ddr,pin,0x44);//start convert
	}


void ReadTemperatureArr(volatile uint8_t *port,volatile uint8_t *ddr,volatile uint8_t *port_pin,uint8_t pin,uint8_t* temperature)
	{
		
		//uint8_t temperature [9];
		//uint8_t temperature_byte;
	//	uint8_t temperature_byte_l;
	//	uint8_t temperature_sign=0;
		reset(port,ddr,pin);
		SendByteToPort(port,ddr,pin,0xCC);//select all devices
		SendByteToPort(port,ddr,pin,0xbe);//read memory
		ReadBytesFromPort(port,ddr,port_pin,pin,temperature,9);
		//temperature_byte=((temperature[0]&0xF0)>>4)|((temperature[1]&0xF)<<4);//здесь хранится целое значение температуры;
		//return temperature[8];
		/*temperature_sign=temperature[1]&0xF0;
		
		temperature_byte_l=temperature[0];
		if(temperature_sign)
		{
			temperature_byte=~temperature_byte;
			temperature_byte_l=~temperature_byte_l;
			temperature_byte_l&=0xF;
			temperature_byte_l++;
			if(temperature_byte_l>0xF)
			{
				temperature_byte++;
				temperature_byte_l&=0xF;
				
			}
			
			
		}
		else
		temperature_byte_l&=0xF;
		temperature_byte_l=temperature_byte_l*10/16;
		
		temperature_to_dispay[0]=temperature_byte_l;
		temperature_to_dispay[1]=temperature_byte%10;
		temperature_to_dispay[2]=temperature_byte>100?(temperature_byte-100)/10:temperature_byte/10;
		if(temperature_sign)
		temperature_to_dispay[3]=10;
		else if (temperature_byte/100) temperature_to_dispay[3]=1;
		else temperature_to_dispay[3]=0;*/
		
		
		//return temperature;
	}