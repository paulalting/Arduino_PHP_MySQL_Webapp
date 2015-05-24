/*---------------------------------------------------------------------------------
 *	dataPush.cpp
 *
 *	Copyright Paul Alting van Geusau 2015
 *
 *	Author:		Paul Alting van Geusau
 *	Created	: 	10 Jan 2013
 *	Last Mod: 	20 May 2015
 *
 */
//#define		json 1														// mime type for json:
//#define		urlencoded 2												// mime type for urlencoded:

#include	"dataPush.h"
#include	"Ethernet.h"
#include	"sensor.h"														// header for reading analogue sensors:
#include	"defineIO.h"
#include	"utilities.h"

/*-------------------------------------------------------------------------------------------------------------------
 *	const	type		variable			=	value;							comment
 */
			uint32_t	connectOK			= 0;							// successful host connects:
			uint32_t	connectFail			= 0;							// unsuccessful host connects:

			uint16_t	okCountPOST			= 0;							// number of successful POST connects:
			uint16_t	fltCountPOST		= 0;							// number of unsuccessful POST connects:

	extern	uint32_t	rtcTime;
	extern	uint16_t	dstState;
	extern	uint32_t	bootTime;
	extern	uint32_t	ntpTimeLocal;

	extern	EthernetClient	client;

	extern	class analogSensor tempSensor_1;
	extern	class analogSensor tempSensor_2;
	extern	class analogSensor tempSensor_3;
	extern	class analogSensor tempSensor_4;
	extern	class analogSensor tempSensor_5;
	extern	class analogSensor tempSensor_6;

/*---------------------------------------------------------------------------------
 *	void livePost()
 *	Prepare data for sending to host server file live.json via HTTP POST:
 *	Example of format: id=liveUno&rtc=1388574360&v1=1631&v2=1648&v3=1654&v4=1669&v5=1654&v6=1620	|| 74 chars, allow 80/160
 */
void livePost() {
	uint16_t dataSize;

	char postData[80];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=live&rtc=%lu&v1=%u&v2=%u&v3=%u&v4=%u&v5=%u&v6=%u",
			rtcTime, tempSensor_1.pv, tempSensor_2.pv, tempSensor_3.pv, tempSensor_4.pv, tempSensor_5.pv, tempSensor_6.pv);

//	dPrint(F("Live : "));
//	dPrintln(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}

/*---------------------------------------------------------------------------------
 *	void trendSensor15m()
 *	Prepare data for sending to host server SQL table trendSensor via HTTP POST:
 */
void trendSensor() {
	uint16_t dataSize;

	char postData[80];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=trend&rtc=%lu&v1=%u&v2=%u&v3=%u&v4=%u&v5=%u&v6=%u",
			rtcTime, tempSensor_1.pv, tempSensor_2.pv, tempSensor_3.pv, tempSensor_4.pv, tempSensor_5.pv, tempSensor_6.pv);

//	dPrint(F("Trend :"));
//	dPrintln(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}


/*---------------------------------------------------------------------------------
 *	void statPeriodPost()
 *	Prepare data for sending to host server SQL table trendStats and file statsPeriod.json via HTTP POST:
 */
void statPeriodPost() {
	uint16_t dataSize;

	char postData[160];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=stat&rtc=%lu&v1=%u&v2=%u&v3=%u&v4=%u&v5=%u&v6=%u&v7=%u&v8=%u&v9=%u&v10=%u&v11=%u&v12=%u",
			rtcTime, tempSensor_1.maxPeriod, tempSensor_1.minPeriod, tempSensor_2.maxPeriod, tempSensor_2.minPeriod, tempSensor_3.maxPeriod, tempSensor_3.minPeriod, tempSensor_4.maxPeriod, tempSensor_4.minPeriod,
			tempSensor_5.maxPeriod, tempSensor_5.minPeriod, tempSensor_6.maxPeriod, tempSensor_6.minPeriod);

//	dPrint(F("StatPeriod :"));
//	dPrintln(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}

/*---------------------------------------------------------------------------------
 *	void statDayPost()
 *	Prepare data for sending to host server SQL table trendStats and file statsDay.json via HTTP POST:
 */
void statDayPost() {
	uint16_t dataSize;

	char postData[160];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=statDay&rtc=%lu&v1=%u&v2=%u&v3=%u&v4=%u&v5=%u&v6=%u&v7=%u&v8=%u&v9=%u&v10=%u&v11=%u&v12=%u",
			rtcTime, tempSensor_1.maxDay, tempSensor_1.minDay, tempSensor_2.maxDay, tempSensor_2.minDay, tempSensor_3.maxDay, tempSensor_3.minDay, tempSensor_4.maxDay, tempSensor_4.minDay,
			tempSensor_5.maxDay, tempSensor_5.minDay, tempSensor_6.maxDay, tempSensor_6.minDay);

	dPrint(F("StatDay :"));
	dPrintln(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}

/*---------------------------------------------------------------------------------
 *	void tripBitsPost()
 *	Prepare data for sending to host server file tripBits.json via HTTP POST:
 */
/*
void tripBitsPost() {
	uint16_t dataSize;

	char postData[80];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=tripBits&rtc=%lu&v1=%u&v2=%u&v3=%u&v4=%u&v5=%u&v6=%u",
			rtcTime, tempL1P1.tripBits, tempL1P2.tripBits, tempL1P3.tripBits, tempL1P4.tripBits, tempL1P5.tripBits, rhL1P5.tripBits);

//	dPrint(F("Trips : "));
//	dPrintln(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}
*/

/*---------------------------------------------------------------------------------
 *	void debugPost()
 *	Prepare data for sending to host server SQL table trendStats and file stats.json via HTTP POST:
 *	id=debug&rtc=1388970000&v1=1388969297&v2=1388969297&v3=1&v4=7&v5=0
 *
 */
void debugPost() {
	uint16_t dataSize;

	char postData[80];														// allow enough buffer space to hold formatted chars:
	dataSize = sprintf(postData, "id=debug&rtc=%lu&v1=%lu&v2=%lu&v3=%i&v4=%lu&v5=%lu",
			rtcTime, bootTime, ntpTimeLocal, dstState, connectOK, connectFail);

//	dPrint(F("Debug :"));
//	dPrint(dataSize);
//	dPrint(F(" : "));
//	dPrintln(postData);
	httpPOST(postData, dataSize);
}

/*---------------------------------------------------------------------------------
 *	bool httpPOST(char* thisData, uint16_t dataSize)
 *	POST data to host server:
 */
void httpPOST(char* thisData, uint16_t dataSize) {
	char outBuf[32];

//	dPrint(dataSize);
//	dPrint(F(" : "));
//	dPrintln(thisData);

	if (!client.connected()) {
		connectHost();
	}

	client.println(F("POST /some-folder-on-server/arduinoPOST.php HTTP/1.1\r\nHost: www.your-web-site.com\r\nConnection: close\r\nContent-Type: application/x-www-form-urlencoded"));
	sprintf(outBuf,"Content-Length: %u\r\n", dataSize);
	client.println(outBuf);
	client.println(thisData);
	delay(1);
	client.flush();
	client.stop();

//	dPrint(F("HTTP ok "));
/*
//	receive and print server response headers as it helps debugging, normally commented out:
	uint8_t inChar;
	uint16_t connectLoop = 0;
	while(client.connected()) {
		while(client.available()) {
			inChar = client.read();
			Serial.write(inChar);
			connectLoop = 0;
		}
		delay(1);
		connectLoop++;
		if(connectLoop > 5000) {											// wait up to 5000 x 1mSec (5sec) for reply from host:
			dPrintln(F("HTTP timeout"));
			client.stop();
		}
	}
*/

}


/*---------------------------------------------------------------------------------
 *	bool connectHost()
 *
 *	Connect to the host server on startup and reconnect in case of any drop-out:
 */
bool connectHost() {
	uint16_t connectLoop = 0;

	IPAddress	myHostIP(192,168,1,100);									// your server IP address:

	for (uint8_t i = 1; (i < 6); i++) {										// retry count is six:
		connectLoop = 0;
		client.connect(myHostIP, 80);										// or you can client.connect("www.your-web-site.com", 80);
		while (!client.connected()) {
			delay(1);
			connectLoop++;
			if (connectLoop > 5000) {										// wait up to 5000 x 1mSec (5sec) for connection from host:
				dPrintln(F("connection timeout"));
				break;
			}
		}
		if (client.connected()) {
			break;
		}
		else {
			dPrint(F("connection retry : "));
			dPrintln(i);
			client.flush();
			client.stop();
			delay(500);
		}
	}
	if (client.connected()) {
		connectOK++;
//		dPrint(connectOK);
//		dPrint(F(": "));
//		dPrint(connectFail);
//		dPrintln(F(": connected"));
		if (connectLoop) {
			dPrintln(F("connected"));
		}
//		dPrint(connectLoop);
//		dPrintln(F(" mS"));
		return true;
	}
	else {
		connectFail++;
		dPrintln(F("connection failed"));
		return false;
	}
}
