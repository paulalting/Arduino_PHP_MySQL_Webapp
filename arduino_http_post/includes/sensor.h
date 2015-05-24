#ifndef sensor_H_
#define sensor_H_

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#define		LIST_LEN 6														// averaging array lenght:

/*
 *	'tripBits' sensor status trigger bits
 *	bit	short	description
 *	0	COV		Change Of Value, set when sensor value varies greater than deadband value:
 *	1	HHA		High High Alarm, set when sensor value is greater then highHighAlarm trip point:
 *	2	HA		High Alarm, set when sensor value is greater then highAlarm trip point:
 *	3	LA		Low Alarm, set when sensor value is less then lowAlarm trip point:
 *	4	LLA		Low Low Alarm, set when sensor value is less then lowLowAlarm trip point:
 *	5	trendUp
 *	6	trendDn
 *	7
 *
 */

class analogSensor {
	public:
//		struct sensorDef {													// earlier method for storing data in a struct:
//		} sensor;

		uint8_t		pin;													// analog pin for voltage:
		uint8_t		deadBand;												// deadband value to generate change of state trigger:
		int16_t		offSet;													// sensor offset:
//		uint16_t	scale;													// scale factor to engineering scale:
//		uint16_t	scaleLow;												// low range scale of raw ADC count:
//		uint16_t	scaleHigh;												// high range scale of raw ADC count:
		uint16_t	pv;														// present value for general use by program:
		uint16_t	pvPrev;													// previous sampled and filtered value of pv:
		uint16_t	maxPeriod;												// period max value, reset at specified time:
		uint16_t	minPeriod;												// period min value, reset at specified time:
		uint16_t	maxDay;													// daily max value, reset at 00:00:
		uint16_t	minDay;													// daily min value, reset at 00:00:
		uint16_t	totPeriod;												// hourly totaliser, reset at XX:00;
		uint16_t	totDay;													// daily totaliser, reset at 00:00:
		uint16_t	highAlarm;												// alarm high trip point:
		uint16_t	lowAlarm;												// alarm low trip point:
		uint8_t		tripBits;												// status trip bits, true for alarm:
		uint8_t		tripAck;												// status trip acknowledge bits, true for acknowledged:
		uint8_t		tripMask;												// status trip mask enable bits, true for enable alarm check:

		analogSensor(uint8_t _pin, int16_t _offSet, uint8_t _initDeadBand, uint16_t _initHighalarm, uint16_t _initLowAlarm);

		void read();
		void covReset();
		void alarmAck(uint8_t);
		void alarmEnable(uint8_t);
		void alarmDisable(uint8_t);
		void statMaxMin();
		void statAccum();
		void dayReset();
		void periodReset();
		void totReset();
		void alarm();
//		void read(sensorDef *sensor);										// syntax when using separate struct for data:

	private:
		boolean 	_first;													// first run trigger:
		uint8_t 	_next;													// index to the next reading:
		int16_t		_analogRaw;												// direct reading from analog channel:
		int16_t		_deltaDiff;												// difference between sensor.pv and previous sensor.pv value:
		uint16_t 	_filter[LIST_LEN];										// filter array, result goes into sensor.pv:
		uint16_t 	_sum;													// sum of the _filter array:
		uint16_t	_adjuster;												// will be LIST_LEN / 2 for biasing up to integer for small values of _sum:
};

long readVcc();

#endif
