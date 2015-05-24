#ifndef utilities_H_
#define utilities_H_

#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
	#define dPrint(x...) Serial.print(x)
	#define dPrintln(x...) Serial.println(x)
#else
	#define dPrint(x...)
	#define dPrintln(x...)
#endif

template<class T>											// no-cost stream operator as described at http://sundial.org/arduino/?page_id=119
	inline Print &operator << (Print &obj, T arg)
	{ obj.print(arg); return obj; }

#endif
