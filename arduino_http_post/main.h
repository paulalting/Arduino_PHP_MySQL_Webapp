#ifndef main_H_
#define main_H_

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

/*
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __cplusplus
	} // extern "C"
#endif
*/

void setup();
void loop();
void interruptScan();
bool ntpGetRequest();
int16_t isDST();
void ntpPacketSend();
int freeRam();

#endif
