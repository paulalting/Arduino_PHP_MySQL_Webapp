#ifndef ioPinDef_H_
#define ioPinDef_H_

#if defined(__AVR_ATmega2560__)
	#define	REF1V1	INTERNAL1V1
#else
	#define	REF1V1	INTERNAL
#endif

#define	pin_0_temp_sensor		A0		// analog 0:
#define	pin_1_temp_sensor		A1		// analog 1:
#define	pin_2_temp_sensor		A2		// analog 2:
#define	pin_3_temp_sensor		A3		// analog 3:
#define	pin_4_temp_sensor		A4		// analog 4:
#define	pin_5_temp_sensor		A5		// analog 5:
#if defined(__AVR_ATmega2560__)
	#define	pin_6_temp_sensor	A6		// analog 6:
	#define	pin_7_temp_sensor	A7		// analog 7:
	#define	pin_8_temp_sensor	A8		// analog 8:
	#define	pin_9_temp_sensor	A9		// analog 9:
	#define	pin_10_temp_sensor	A10		// analog 10:
	#define	pin_11_temp_sensor	A11		// analog 11:
#endif

#define scaleADC		1012			// ADC error found by measuring ADC ref voltage, scaled by a factor of 1000:

#define	scale10			10				// raw to engineering scale divisor 10:
#define	scale20			20				// raw to engineering scale divisor 20:
#define	scale50			50				// raw to engineering scale divisor 50:
#define	scale100		100				// raw to engineering scale divisor 100:
#define	offSet0			0				// offset value 0:
#define	offSet512		-516			// offset (1023 � 1.012) / 2   [-512 for Alegro 712 current devices that float at Vcc � 2]:
#define scale0			0
#define scale1023		1023
#define initDeadBand	2				// initial deadband value for vBattery a value of 2 equates to 100 � (2 � 5) or 0.1Vdc:
#define initHighAlarm	1023			// initial high alarm value:
#define initLowAlarm	0				// initial low alarm value:

//		'tripBits' sensor status trigger bits
#define	COV				0x01			// Change Of Value, set when sensor value varies greater than deadband value:
#define	HHA				0x02			// High High Alarm, set when sensor value is greater then highHighAlarm trip point:
#define	HA				0x03			// High Alarm, set when sensor value is greater then highAlarm trip point:
#define	LA				0x04			// Low Alarm, set when sensor value is less then lowAlarm trip point:
#define	LLA				0x05			// Low Low Alarm, set when sensor value is less then lowLowAlarm trip point:
#define trendUp			0x06			// Value trending up:
#define trendDown		0x07			// Value trending down:

#define	pin_sdSelect	4
#define	pin_CSether		10
#define	pin_SPI_CS		53

#endif
