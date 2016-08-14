#include "mainHeader.h"
#define RTC_adr_write 0xD0
#define RTC_adr_read 0xD1
#include "twi_lib.h"
void rtcInit () {
	i2cSetBitrate(400);
	i2cSendStart();
	i2cWaitForComplete();
	i2cSendByte(RTC_adr_write);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0x0E);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0b00100000);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0b00001000);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendStop();
}

void readTime (uint8_t* timeArr) {
	i2cSendStart();
	i2cWaitForComplete();
	i2cSendByte(RTC_adr_write);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0x00);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendStop();

	//Reading

	i2cSendStart();
	i2cWaitForComplete();
	i2cSendByte(RTC_adr_read);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);
	i2cWaitForComplete();

	for (int i = 0; i < 0x13; i += 1) {
		i2cReceiveByteACK();
		i2cWaitForComplete();
		timeArr[i] = i2cGetReceivedByte();
		i2cWaitForComplete();
		checki2cReturnCode(TW_MR_DATA_ACK);
	}
	i2cReceiveByteNACK();
	i2cWaitForComplete();
	timeArr[6] = i2cGetReceivedByte();
	i2cWaitForComplete();
	checki2cReturnCode(TW_MR_DATA_NACK);

	

}
void rtcSendMinutes (uint8_t minutes) {
	uint8_t bdc = 0;
	bdc = minutes/10;
	bdc <<= 4;
	bdc |= minutes%10;
	i2cSendStart();
	i2cWaitForComplete();
	i2cSendByte(RTC_adr_write);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0x00);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(bdc);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendStop();
}

void rtcSendHours (uint8_t hours) {
	uint8_t bdc = 0;
	bdc = hours/10;
	bdc <<= 4;
	bdc |= hours%10;
	i2cSendStart();
	i2cWaitForComplete();
	i2cSendByte(RTC_adr_write);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(0x02);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendByte(bdc);
	i2cWaitForComplete();
	checki2cReturnCode(TW_MT_DATA_ACK);

	i2cSendStop();
}