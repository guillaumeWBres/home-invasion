#ifndef __BASESTATION_H__
#define __BASESTATION_H__

#include <time.h>

#include "Network.h"
#include "TimeZone.h"

class BaseStation {

public:
	BaseStation(void);

	// sends email to specified destination
	void send_email(const char *subject, const char *content, const char *dest);

	// prints self in human readable format
	void print(void);

private:
	Network _network; // node network
	TimeZone _timeZone; // current event timezone
};

#endif
