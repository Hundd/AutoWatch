//#################################################################################
//#################################################################################
//#################################################################################
/*	Library to use TWI with ATMEL Atmega family
	
	Copyright (C) 2010 Stefan Sicklinger

	For support check out http://www.sicklinger.com
    
	This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/
//#################################################################################
//#################################################################################
//#################################################################################
#include <avr/io.h>
#include <util/delay.h> 
#include <util/twi.h>

#ifndef _TWI_LIB_H_
#define _TWI_LIB_H_ 
//----------------------------------------
// Prototypes
//----------------------------------------
//! Set the I2C transaction bitrate (in KHz)
void i2cSetBitrate(uint16_t bitratekHz);
//! Send an I2C start condition in Master mode or repeated start condition
void i2cSendStart(void);
//! Send an I2C stop condition in Master mode
uint8_t i2cSendStop(void);
//! Send an (address|R/W) combination or a data byte over I2C
void i2cSendByte(unsigned char data);
//! Receive a data byte over I2C  
void i2cReceiveByteACK(void);
//! Receive a data byte over I2C  
void i2cReceiveByteNACK(void);
// get received byte back
uint8_t i2cGetReceivedByte(void);
//! Wait for current I2C operation to complete
uint8_t i2cWaitForComplete(void);
// Check for expected error code
uint8_t checki2cReturnCode(uint8_t expected_return_code);
//#################################################################################
//#################################################################################
//#################################################################################
//----------------------------------------
// 
//----------------------------------------
void i2cSetBitrate(uint16_t bitratekHz)
{
	uint8_t bitrate_div;
	// set i2c bitrate
	// Set Prescaler to one --> TWPS = 1
	TWSR =((0<<TWPS0)|(0<<TWPS1));
	//calculate bitrate division	
	bitrate_div = (((float)F_CPU/1000.0)/(float)bitratekHz);
	if(bitrate_div >= 16){
		bitrate_div = (float)(bitrate_div-16)/8;
	}
	//printf("DIV: %d \n",bitrate_div);
	TWBR=(uint8_t)bitrate_div;
	//TWBR=0; -> max speed
	PORTC |= ((1<<PINC4) | (1<<PINC5));
}
//----------------------------------------
// 
//----------------------------------------
void i2cSendStart(void)
{
	// send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
}
//----------------------------------------
// 
//----------------------------------------
uint8_t i2cSendStop(void)
{
	
	uint8_t i = 0;		//time out variable
	uint8_t error_code=0;
	// transmit stop condition
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	

	// wait until stop condition is executed and bus released
    while ((TWCR & (1<<TWSTO)) && (i < 90)){
		_delay_us(1);
		i++;
	}
	if (i>89){
		_delay_us(1);
		error_code=100;

	}
	return error_code;
}
//----------------------------------------
// 
//----------------------------------------
void i2cSendByte(unsigned char data)
{
	// save data to the TWDR
	TWDR = data;
	// begin send
	TWCR = (1<<TWINT)|(1<<TWEN);
}
//----------------------------------------
// 
//----------------------------------------
void i2cReceiveByteACK(void)
{

	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
}
//----------------------------------------
// 
//----------------------------------------
void i2cReceiveByteNACK(void)
{

	TWCR = (1<<TWINT)|(1<<TWEN);
}
//----------------------------------------
// 
//----------------------------------------
uint8_t i2cWaitForComplete(void)
{
	uint8_t i = 0;		//time out variable
	uint8_t error_code=0;	

	// wait for i2c interface to complete operation
    while ((!(TWCR & (1<<TWINT))) && (i < 90)){
		_delay_us(1);
		i++;
	}
	if (i>89){
		_delay_us(1);
		error_code=100;

	}
	return error_code;
}
//----------------------------------------
// 
//----------------------------------------
uint8_t i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return (uint8_t)TWDR;
}
//----------------------------------------
// 
//----------------------------------------
uint8_t checki2cReturnCode(uint8_t expected_return_code){
	uint8_t error_code=0;
	uint8_t i=0;
	//while (error_code!=0 && i!=0 && i<3){
		if ((TWSR & 0xF8) != expected_return_code){
			error_code=10;
		}
	i++;
//	}
	return error_code;
}
//----------------------------------------
// 
//----------------------------------------
#endif /* _TWI_LIB_H_ */
