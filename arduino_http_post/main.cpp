/*---------------------------------------------------------------------------------
 *
 *	Example Project to have Arduino send data via HTTP POST
 *	to a HTTP server with PHP and MySQL installed to process and store data.
 *
 *	main.cpp
 *
 *	Copyright Paul Alting van Geusau 2015
 *
 *	Author:		Paul Alting van Geusau
 *	Created:	19 Sep 2012
 *	Modified:	20 May 2015
 *
 */

#define DEBUG false															// set to true for debug prints:

//#include	<avr/wdt.h>														// watchdog:
#include	"main.h"														// header for main entry point:
#include	"Ethernet.h"													// header for main Ethernet service:
#include	"EthernetUdp.h"													// header for UDP service, used by NTP:
#include	"Time.h"														// header for time and date library:
#include	"TimerOne.h"													// header for TimerOne on UNO:
//#include	"TimerThree.h"													// header for TimerThree on Mega:
#include	"includes/defineIO.h"											// header for hardware pin definitions:
#include	"includes/sensor.h"												// header for reading analogue sensors:
#include	"includes/dataPush.h"											// header for POSTing data to host server:
#include	"includes/utilities.h"

//-------------------------------------------------------------------------------------------------------------------
//	const	type		variable			=	value;							comment
			boolean		firstScan			=	true;
			boolean		rtcOK				=	false;						// set to true when we get NTP:
			boolean		rtc5SecOS			=	false;						// Oneshot 5 second trigger:
			boolean		rtcMinOS			=	false;						// Oneshot 60 second trigger:
			boolean		rtc5MinOS			=	false;						// Oneshot 300 second trigger:
			boolean		rtc15MinOS			=	false;						// Oneshot 900 second trigger:
			boolean		rtcHourOS			=	false;						// Oneshot 3600 second trigger:
			boolean		rtcDayOS			=	false;						// Oneshot end of day trigger:

			uint32_t	rtcTime				=	0;							// internal software rtc time in seconds:
			uint16_t	dstState			=	0;							// dst state, either on or off. Use int for POST sprintf:
			uint32_t	ntpTimeLocal		=	0;							// local time in seconds:
			uint32_t	bootTime			=	0;							// power up time, stored to SQLdb:

			uint8_t			mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE3};	// MAC address, default is {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}:

			IPAddress		ip(192,168,1,11);
			IPAddress		gateway(192,168,1, 1);
			IPAddress		dnServer(192,168,1,1);
			IPAddress		subnet(255, 255, 255, 0);

			EthernetClient	client;

	#define					NTP_PACKET_SIZE			48						// NTP time stamp is in the first 48 bytes of the message:
	const	uint16_t		udpLocalPort		=	8888;					// local port to listen for UDP packets:
			uint8_t			packetBuffer[NTP_PACKET_SIZE];					// buffer to hold incoming and outgoing packets:
			boolean			ntpPacketSent		=	false;

			EthernetUDP 	Udp;											// UDP instance to let us send and receive packets over UDP:

			analogSensor	tempSensor_1(pin_0_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);
			analogSensor	tempSensor_2(pin_1_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);
			analogSensor	tempSensor_3(pin_2_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);
			analogSensor	tempSensor_4(pin_3_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);
			analogSensor	tempSensor_5(pin_4_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);
			analogSensor	tempSensor_6(pin_5_temp_sensor, offSet0, initDeadBand, initHighAlarm, initLowAlarm);

/*---------------------------------------------------------------------------------
 *	void setup()
 */
void setup() {
	analogReference(REF1V1);												// set analog Vref to 1v1 according to cpu model:
	delay(1500);
	Serial.begin(115200);													// set serial port and speed:
	dPrint(F("SRAM: "));
	dPrintln(freeRam());

	pinMode(pin_0_temp_sensor, INPUT);
	pinMode(pin_1_temp_sensor, INPUT);
	pinMode(pin_2_temp_sensor, INPUT);
	pinMode(pin_3_temp_sensor, INPUT);
	pinMode(pin_4_temp_sensor, INPUT);
	pinMode(pin_5_temp_sensor, INPUT);

	pinMode(pin_CSether, OUTPUT);											// set ethernet CS as output:
	pinMode(pin_SPI_CS, OUTPUT);											// set ATMega-2560, pin 53 hardware cs pin output, even if un-used:

	Ethernet.begin(mac, ip, dnServer, gateway, subnet);						// initialise the ethernet interface:

	dPrintln(F("connecting to host"));
	connectHost();

	Udp.begin(udpLocalPort);

	Timer1.initialize(1000000);												// initialize Timer3, and set a 1000 milli second period:
	Timer1.attachInterrupt(interruptScan);									// attach interrupt to timerInterrupt routine:
}

/*---------------------------------------------------------------------------------
 *	void loop()
 */
void loop() {
	if (!ntpPacketSent) {
		if (firstScan || rtcHourOS) {										// if firstScan or if '00:00:00', trying on the hour:
			tempSensor_1.read();											// read temperature probe:
			tempSensor_2.read();											// read temperature probe:
			tempSensor_3.read();											// read temperature probe:
			tempSensor_4.read();											// read temperature probe:
			tempSensor_5.read();											// read temperature probe:
			tempSensor_6.read();											// read temperature probe:

			ntpPacketSend();												// send an NTP packet to a time server:
			ntpPacketSent = true;
			firstScan = false;												// only ever true for the first scan on startup:
		}
	}

	if (ntpPacketSent && ntpGetRequest()) {									// if valid time from NTP server:
		if (dstState) {
			dPrint(F("DTS ON :: Local Time : "));
		}
		else {
			dPrint(F("DTS OFF :: Local Time : "));
		}
		dPrintln(ntpTimeLocal);
		rtcTime = ntpTimeLocal;
		ntpPacketSent = false;
		rtcOK = true;
	}

	if (!bootTime && rtcOK) {												// on power up record the current NTP time:
		bootTime = ntpTimeLocal;											// hold it in bootTime:
	}

	if (rtc5SecOS && rtcOK) {												// 5 second check loop, do what you want here every 5 seconds:
		tempSensor_1.read();												// read temperature probe:
		tempSensor_2.read();												// read temperature probe:
		tempSensor_3.read();												// read temperature probe:
		tempSensor_4.read();												// read temperature probe:
		tempSensor_5.read();												// read temperature probe:
		tempSensor_6.read();												// read temperature probe:

		livePost();															// store sensor data to http server file:

//		dPrint(F("tempSensor_1 : "));										// example of some printing of sensor data:
//		dPrint(tempSensor_1.pv);
//		dPrint(F(" tempSensor_2 : "));
//		dPrintln(tempSensor_2.pv);
		rtc5SecOS = false;
	}

	if (rtcMinOS && rtcOK) {												// 60 second check loop, do what you want here every 60 seconds:
		rtcMinOS = false;
		trendSensor();														// store sensor data to http server MySQL database:
	}

/*	if (rtc5MinOS && rtcOK) {												// 300 second check loop, do what you want here every 300 seconds:
		rtc5MinOS = false;
	}
*/
/*	if (rtc15MinOS && rtcOK) {												// 900 second check loop, do what you want here every 900 seconds:
		rtc15MinOS = false;
	}
*/

	if (rtcHourOS && rtcOK) {												// check for hour rollover:
		statPeriodPost();														// store sensor stats data to http server file:
		tempSensor_1.periodReset();											// reset hourly stats:
		tempSensor_2.periodReset();
		tempSensor_3.periodReset();
		tempSensor_4.periodReset();
		tempSensor_5.periodReset();
		tempSensor_6.periodReset();

		if (rtcDayOS) {
			statDayPost();													// store sensor stats data to http server file:
			tempSensor_1.dayReset();										// reset daily stats:
			tempSensor_2.dayReset();
			tempSensor_3.dayReset();
			tempSensor_4.dayReset();
			tempSensor_5.dayReset();
			tempSensor_6.dayReset();
		}
		debugPost();														// store debug data to http server file:
		rtcHourOS = false;
		rtcDayOS = false;
	}
}

/*---------------------------------------------------------------------------------
 *	void interruptScan()
 *	this routine is called periodically to check the rtc and
 *	set time flags which perform specific functions:
 */
void interruptScan() {
	++ rtcTime;																// increment software rtc value:
	if (!(rtcTime % 5)) {													// check five second:
		rtc5SecOS = true;
		if (!(rtcTime % 60)) {												// check 60 second:
			rtcMinOS = true;
			if (!(rtcTime % 300)) {											// check 300 second:
				rtc5MinOS = true;
				if (!(rtcTime % 900)) {										// check 900 second:
					rtc15MinOS = true;
					if (!(rtcTime % 3600)) {								// check 3600 second:
						rtcHourOS = true;
						if (!(rtcTime % 86400L)) {							// check 86400 second:
							rtcDayOS = true;
						}
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------------
 *	boolean isDST(time_t t)
 *
 *	For Tasmania:
 *	Daylight Saving Time begins at 2am on the first Sunday in October, when clocks are put forward one hour.
 *	It ends at 2am (which is 3am Daylight Saving Time) on the first Sunday in April, when clocks are put back one hour.
 *
 *	For further info:
 *	http://australia.gov.au/about-australia/our-country/time
 *	http://www.tablespace.net/papers/unix_time.html
 *	http://playground.arduino.cc//Code/Time
 *	http://forum.arduino.cc/index.php/topic,40286.0.html
 *	http://www.epochconverter.com/
 */
int16_t isDST(time_t t) {
	time_t	dstStart;
	time_t	dstEnd;
	tmElements_t te;

	te.Year = year(t) - 1970;
	te.Month = 10;							// set month to 10th month being October:
	te.Day = 1;
	te.Hour = 0;
	te.Minute = 0;
	te.Second = 0;

	dstStart = makeTime(te);
	dstStart = nextSunday(dstStart);		// first sunday in October
	dstStart += 2 * SECS_PER_HOUR;			// at 02h00:

	te.Month = 4;							// set month to 4th month being April:
	dstEnd = makeTime(te);
	dstEnd = nextSunday(dstEnd);			// first Sunday in April:
//	dstEnd = nextSunday(dstStart);			// second Sunday
	dstEnd += 2 * SECS_PER_HOUR;			// at 02h00:

//	dPrint(F("Ntp Time : "));
//	dPrintln(t);

//	dPrint(F("dstStart : "));
//	Serial.println(dstStart);

//	dPrint(F("dstEnd   : "));
//	dPrintln(dstEnd);

	if (t >= dstEnd && t < dstStart) {
//		dPrint(F("DTS OFF :: Ntp Time : "));
//		dPrintln(t);
		dstState = 0;
		return (0UL);
	}
	else {
//		dPrint(F("DTS ON :: Ntp Time : "));
//		dPrintln(t);
		dstState = 1;
		return (3600UL);
	}
}

/*---------------------------------------------------------------------------------
 *	bool ntpGetRequest()
 *	Receive the data back from the NTP server and parse it:
 *	Code taken from original Arduino version 1.0 UdpNtpClient example
 *	The NTP timestamp starts at byte 40 of the received packet and is four bytes or two words long:
 *	NTP time is seconds since January 1 1900:
 *
 *	For further info:
 *	NTP server list at http://tf.nist.gov/tf-cgi/servers.cgi
 *	http://playground.arduino.cc//Code/NTPclient
 *	http://forum.arduino.cc//index.php?topic=137789.0
 */
bool ntpGetRequest() {
	#define	TZ_OFFSET		36000UL											// time zone offset for Eastern Australian Standard Time (EAST) UTC + 10:
	#define	SEVENTYYEARS	2208988800UL									// unix time starts on Jan 1 1970. In seconds, that's 2208988800:

	if (Udp.parsePacket()) {												// check for packet received:
//		dPrintln(F("NTP Packet Parse"));
		Udp.read(packetBuffer, NTP_PACKET_SIZE);							// read the packet into the buffer:
		uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);		// extract the two words starting at byte 40:
		uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);

		uint32_t secsSince1900 = highWord << 16 | lowWord;					// combine the two words (four bytes) into a long integer:
		uint32_t ntpTime = secsSince1900 - SEVENTYYEARS;					// subtract seventy years:
		ntpTimeLocal = ntpTime + TZ_OFFSET;									// add +10 hours for AEST
		ntpTimeLocal = ntpTimeLocal + isDST(ntpTimeLocal);							// check for DST offset:
		return true;														// return true if successful:
	}
		return false;
}

/*---------------------------------------------------------------------------------
 *	void ntpPacketSend()
 *	Send request to NTP server for current UTC time:
 *
 *	char timeServer[] = "0.north-america.pool.ntp.org";  //Time server you wish to use
 */
void ntpPacketSend() {														// send an NTP request:
	memset(packetBuffer, 0, NTP_PACKET_SIZE);								// set all bytes in the buffer to 0:
	// Initialise values needed to form NTP request
	packetBuffer[0] = 0b11100011;											// LI, version, mode:
	packetBuffer[1] = 0;													// stratum, or type of clock:
	packetBuffer[2] = 6;													// polling interval:
	packetBuffer[3] = 0xEC;													// peer clock precision:
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;
	IPAddress ntpServer(129, 6, 15, 30);									// time.nist.gov NTP server list at http://tf.nist.gov/tf-cgi/servers.cgi
	Udp.beginPacket(ntpServer, 123);										// NTP requests are to port 123:
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
//	dPrintln(F("NTP Packet Sent"));
}

/*---------------------------------------------------------------------------------
 *	int freeRam()
 *
 */
int freeRam() {
	extern int __heap_start,*__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);
}
