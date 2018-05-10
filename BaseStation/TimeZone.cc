#include "TimeZone.h"
#include <string>
#include <mysql.h>

#include <string.h>

using namespace std;

TimeZone::TimeZone(void){
	_setBusyToday(updateFromDB());
}

uint8_t TimeZone::updateFromDB(void){
	uint8_t status;

	MYSQL *sql;
	MYSQL_ROW row;
	MYSQL_RES *result;
	MYSQL_FIELD *field;

	char query[512];
	char date[16];
	time_t t = time(0);
	tm *today = localtime(&t);

	strftime(date, 16, "%Y-%m-%d", today);
	
	sql = mysql_init(NULL);
	sql = mysql_real_connect(sql, "localhost",
		"root", "root", "test",
			0, NULL, 0
	);

	// build query to retrieve events for today
	sprintf(query, 
		"select * from events where start > \"%s 00:00:00\" and end < \"%s 23:59:59\";",
			date, date
	);
	
	mysql_query(sql, query);
	result = mysql_store_result(sql);

	if (result->row_count == 0)
		status = 0x00;

	else {

		row = mysql_fetch_row(result);
		// create tm object from start & end timestamps
		strptime(row[2], "%Y-%m-%d %T", &_start);
		strptime(row[3], "%Y-%m-%d %T", &_end);
		status = 0x01;
	}
	
	mysql_close(sql);
	return status;
}

void TimeZone::print(void){
	char date[32];
	if (busyToday()){

		printf("Working today for %f minutes from %d:%d:%d to %d:%d:%d\n",
			durationMins(), 
			_start.tm_hour, _start.tm_min, _start.tm_sec,
			_end.tm_hour, _end.tm_min, _end.tm_sec
		);

	} else {
		printf("No events found for today\n");
	}
}

void TimeZone::setTitle(string title){
	_title = title;
}

uint8_t TimeZone::busyToday(void){
	return _busyToday;
}

void TimeZone::_setBusyToday(uint8_t busy){
	_busyToday = busy;
}

double TimeZone::duration(void){
	return difftime(mktime(&_end),mktime(&_start));
}

double TimeZone::durationMins(void){
	return duration()/60.0;
}

double TimeZone::durationHours(void){
	return duration()/3600.0;
}
