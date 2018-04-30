#ifndef __BASESTATION_H__
#define __BASESTATION_H__

#include <time.h>

#include "Network.h"

class BaseStation {

public:
	BaseStation(void);

	// sends email to specified destination
	void send_email(const char *subject, const char *content, const char *dest);
	// prints self in human readable format
	void print(void);

	// retrieves time zone for current day
	// if any is found in .db
	void retrieveTimeZoneFromDB(void);

private:
	// updates time to current value
	void _updateTime(void);
	
	Network _network;
	tm *_now;
};

#endif
