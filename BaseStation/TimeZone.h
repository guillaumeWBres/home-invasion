#ifndef __TIMEZONE_H__
#define __TIMEZONE_H__

#include <time.h>
#include <string>

class TimeZone {

	public:
		// default constructor:
		// initializes with instructions for Today
		// directly queries the database
		TimeZone(void);

		// retrieves events for today
		// returns 0x01 if events were found
		// 0x00 otherwise
		uint8_t updateFromDB(void);

		// prints time zone in readable format
		void print(void);

		void setTitle(std::string title);
		std::string getTitle(void);

		// returns event duration
		double duration(void);
		double durationMins(void);
		double durationHours(void);

		uint8_t busyToday();

	private:
		std::string _title;
		tm _start, _end;
		uint8_t _busyToday;

		void _setBusyToday(uint8_t busy);
};

#endif
