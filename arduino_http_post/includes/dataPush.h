#ifndef dataPush_H_
#define dataPush_H_

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif


//bool publishLive();
//bool publishStat();
void livePost();
void trendSensor();
void statPeriodPost();
void statDayPost();
void tripBitsPost();
void debugPost();
void httpPOST(char*, uint16_t);
bool connectHost();
#endif
