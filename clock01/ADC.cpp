#include "mainHeader.h"
class AdcClass
{
	double CalculateResistor(uint16_t Adc_num)
	{
		return (double)Adc_num*22./(1023. - Adc_num);
	}
	double CalculateVoltage(uint16_t Adc_num)
	{
		//return ((double)Adc_num)/400*11.;
		return ((double)Adc_num/18.618);
	}
	public:
	volatile static uint16_t Data;
	volatile static bool DataReady;
	double Resistor,Voltage;
	void Init()
	{
			
			
			SelectMixer(PORTC0);
			ADCSRA |=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
			ADCSRA |=(1<<ADIE);
			ADCSRA |=(1<<ADEN);
			//ADCSRA |=(1<<ADFR);
			ADCSRA |= (1<<ADSC);
	}
	// NumberOfPin example PC1 
	void SelectMixer(uint8_t NumberOfPin)	
	{
		ADMUX &=0xF0;
		ADMUX |= NumberOfPin;
	}
	AdcClass()
	{
		Data = 1024;
		DataReady = false;
		Resistor = INFINITY;
		Voltage = INFINITY;
	}
	void StartMesurement(uint8_t NumberOfPin)
	{	
		SelectMixer(NumberOfPin);
		ADCSRA |= (1<<ADSC);
	}
	static void setData()
	
		{Data = ADC;}
	
	void MesureVoltage(uint8_t NumberOfPin)
	{	//ADMUX |= 0xC0;
		ADMUX &=0xf;
		StartMesurement(NumberOfPin);
		//_delay_ms(2);
		while(!DataReady);
		Voltage = CalculateVoltage(Data);
		DataReady = false;
	}
	void MesureResistor(uint8_t NumberOfPin)
	{	ADMUX &=0xf;
		StartMesurement(NumberOfPin);
		while(!DataReady);
		Resistor = CalculateResistor(Data);
		DataReady = false;
	}		
	
};
volatile bool AdcClass::DataReady;
volatile uint16_t AdcClass::Data;

ISR(ADC_vect)
	{	
		AdcClass::Data = ADC;
		AdcClass::DataReady = true;
		_delay_us(10);
		//ADCSRA |= (1<<ADSC);
	}