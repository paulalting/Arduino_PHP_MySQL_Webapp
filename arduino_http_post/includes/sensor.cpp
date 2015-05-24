/*---------------------------------------------------------------------------------
 *	sensor.cpp
 *
 *	Copyright Paul Alting van Geusau 2015
 *
 *	Author:		Paul Alting van Geusau
 *	Created	: 	19 Sep 2013
 *	Last Mod: 	05 Jan 2014
 *
 */
#include	"sensor.h"
#include	"defineIO.h"
#include	"utilities.h"

/*---------------------------------------------------------------------------------
 *	constructor for analogSensor
 */
analogSensor::analogSensor(uint8_t _pin, int16_t _offSet, uint8_t _initDeadBand, uint16_t _initHighAlarm, uint16_t _initLowAlarm) {
	_first = true;
	_next = 0;
	_adjuster = LIST_LEN / 2;

	pin			=	_pin;													// passed in as parameter:
	pvPrev		=	0;
//	scaleLow	=	_scale0;
//	scaleHigh	=	_scale1023;
	offSet		=	_offSet;												// passed in as parameter:
	deadBand	=	_initDeadBand;											// passed in as parameter:
	tripBits	= 	0x00;													// all trip bits clear:
	tripAck		=	0x00;													// all trip acknowledge bits clear:
	tripMask	=	0xff;													// all trip mask bits set:
	highAlarm	=	_initHighAlarm;											// passed in as parameter:
	lowAlarm	=	_initLowAlarm;											// passed in as parameter:
	totPeriod		=	0;
	totDay		=	0;
	minPeriod	=	1023;
	maxPeriod	=	0;
	minDay		=	1023;
	maxDay		=	0;
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::read()
 *	read analog input;
 *	if first time through filter then fill filter elements with current sensor value
 *	else shift in new sensor value into filter, loosing oldest value:
 *	consider writing own analogRead routine at soem point:
 */
void analogSensor::read() {
	_analogRaw = analogRead(pin) + offSet;									// read specified analog sensor port {79 � 64 or 316 � 16 = 5056(mV)}:
	if (_analogRaw < 0) {													// if _analogRaw is less than scaleLow value then clamp to value:
		_analogRaw = 0;
	}

	if (_analogRaw > 1023) {												// if _analogRaw is greater than scaleHigh value then clamp to value:
		_analogRaw = 1023;
	}

	if (_first) {															// if first time through, fill the filter with current sensor value:
		_first = false;														// once done we don't need to come back here again:
		_next = 0;
		_sum = (uint16_t)_analogRaw * LIST_LEN;								// the sum is filter size � current sensor value:
		for (uint8_t i=0; i<LIST_LEN; i++) {
			_filter[i] = (uint16_t)_analogRaw;								// fill filter array with curent sensor value:
			tripBits = 0x00;												// clear all tripBits so we don't generate alarms or cov trigger:
		}
	}
	else {
		_sum = _sum - _filter[_next] + (uint16_t)_analogRaw;
		_filter[_next] = (uint16_t)_analogRaw;
		_next = ++_next % LIST_LEN;

		pv = (_sum + _adjuster) / LIST_LEN;									// calculate moving average, first add round-up then divide by array length:

		if (maxDay < pv) {													// check for maximum data value:
			maxDay = pv;
		}
		else if (minDay > pv) {												// check for minimum data value:
			minDay = pv;
		}

		if (maxPeriod < pv) {												// check for maximum data value:
			maxPeriod = pv;
		}
		else if (minPeriod > pv) {											// check for minimum data value:
			minPeriod = pv;
		}
/*
		if (pv > (pvPrev + deadBand)) {										// check for upward change of value
//			tripBits = tripBits & trendUp & ~trendDown;						// set trendUp and clear trendDown bits:
			tripBits = tripBits & trendUp;									// set trendUp and clear trendDown bits:
			tripBits = tripBits & ~trendDown;								// set trendUp and clear trendDown bits:
		}
		if (pv < (pvPrev - deadBand)) {										// check for downward change of value:
//			tripBits = tripBits & trendDown & ~trendUp;						// set trendDown and clear trendUp bits:
			tripBits = tripBits & trendDown;								// set trendUp and clear trendDown bits:
			tripBits = tripBits & ~trendUp;									// set trendUp and clear trendDown bits:
		}
*/
/*
		if ((pv > highAlarm) && (tripMask & HA)) {							// check for high alarm, if pv is greater than high alarm trip point value:
			tripBits = tripBits | HA;										// set high alarm bit in tripBits:
		}
		else {
			tripBits = tripBits & ~HA;										// clear high alarm bit in tripBits:
		}

		if ((pv < lowAlarm) && (tripMask & LA)) {							// check for low alarm, if pv is greater than low alarm trip point value:
			tripBits = tripBits | LA;										// set low alarm bit in tripBits:
		}
		else {
			tripBits = tripBits & ~LA;										// clear low alarm bit in tripBits:
		}
*/
/*
		_deltaDiff = int16_t(pv - pvPrev);									// difference between current and previous pv value:
		if (abs(_deltaDiff) >= int16_t(deadBand)) {							// if difference is greater than deadband value then modify COV status bit:
			tripBits = tripMask & COV;
		}
*/
	}
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::covReset()
 *	the COV bit needs to be cleared after being set and initaiting the cov trigger:
 *	tigger is typically used to initaite a publish of the analog sensor data:
 */
void analogSensor::covReset() {
	tripBits = tripBits & ~COV;												// clear COV bit:
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::alarmAck(uint8_t alarm)
 *	user acknowledge specified alarm:
 */
void analogSensor::alarmAck(uint8_t alarmBit) {
	if (tripBits && alarmBit) {												// first check if alarm is active:
		tripAck = tripAck | alarmBit;										// set alarm acknowlege bit:
	}
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::alarmEnable(uint8_t alarm)
 *	user enable specified alarm:
 *	alarms are enabled by default at start:
 */
void analogSensor::alarmEnable(uint8_t alarmBit) {
	tripMask = tripMask | alarmBit;											// set specified alarm enable bit:
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::alarmEnable(uint8_t alarm)
 *	user disable specified alarm:
 */
void analogSensor::alarmDisable(uint8_t alarmBit) {
	tripMask = tripMask & ~alarmBit;										// clear specified alarm enable bit:
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::statMaxMin()
 *	calculate hourly and daily maximum and minimum values from analog sensors:
 */
void analogSensor::statMaxMin() {
	if (maxDay < pv) {														// check for maximum data value:
		maxDay = pv;
	}
	else if (minDay > pv) {													// check for minimum data value:
		minDay = pv;
	}

	if (maxPeriod < pv) {														// check for maximum data value:
		maxPeriod = pv;
	}
	else if (minPeriod > pv) {												// check for minimum data value:
		minPeriod = pv;
	}
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::statAccum()
 *	calculate hourly and daily accumulated totals from analog sensors:
 */
void analogSensor::statAccum() {
	totPeriod = totPeriod + (pv / 3600.0);
	totDay = totDay + (pv / 3600.0);
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::dayReset()
 *	RTC reset of daily stats:
 */
void analogSensor::dayReset() {
	maxDay = pv;
	minDay = pv;
	totDay = 0;
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::periodReset()
 *	RTC reset of hourly stats:
 */
void analogSensor::periodReset() {
	maxPeriod = pv;
	minPeriod = pv;
	totPeriod = 0;
}

/*---------------------------------------------------------------------------------
 *	void analogSensor::totReset()
 *	RTC reset of totaliser:
 */
void analogSensor::totReset() {
	totPeriod = 0;
}

//=================================================================================
//	long readVCC()
//	Read VCC: http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
//	Read 1.1V reference against AVcc
//	set the reference to Vcc and the measurement to the internal 1.1V reference
//
long readVcc() {
	#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
		ADMUX = _BV(MUX5) | _BV(MUX0);
	#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		ADMUX = _BV(MUX3) | _BV(MUX2);
	#else
		ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#endif

	delay(5);																// Wait for Vref to settle, was 2
	ADCSRA |= _BV(ADSC);													// Start conversion
	while (bit_is_set(ADCSRA,ADSC));										// measuring

	uint8_t low  = ADCL;													// must read ADCL first - it then locks ADCH
	uint8_t high = ADCH;													// unlocks both

	long result = (high<<8) | low;
	result = 1125300L / result;												// Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result;															// Vcc in millivolts
}
