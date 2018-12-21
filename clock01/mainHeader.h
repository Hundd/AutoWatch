/*
 * mainHeader.cpp
 *
 * Created: 13.08.2016 13:55:52
 *  Author: Hundd
 */
#pragma once
#ifndef MAINHEADER 
	#define MAINHEADER
	
	#define F_CPU 8000000L
	#include <avr/io.h>
	#include <avr/eeprom.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	void rtcInit();
	void readTime(uint8_t* timeArr);

	void ClrScr();
	void Display7seg(uint8_t digit, bool dp);
	void NextDigit(uint8_t number);
	void DisplayFreqency(uint16_t frequency);
	uint16_t ReadKey();
	void rtcSendMinutes (uint8_t minutes);
	void rtcSendHours (uint8_t hours);
#endif