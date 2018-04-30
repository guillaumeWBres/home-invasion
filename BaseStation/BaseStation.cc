#include "BaseStation.h"

// C
#include <time.h>
#include <string.h>

// CXX
#include <vector>
#include <iostream>
#include <mysql.h>

using namespace std;

#define SERVER "HOST"
#define USER "USR"
#define PASSWORD "PSWD"
#define DATABASE "example"

BaseStation::BaseStation(void){

	// create network object
	vector<Node> nodes;
	nodes.push_back(Node(NETWORK_COORDINATOR));
	nodes.push_back(Node(NETWORK_END_DEVICE));

	_network = Network(nodes);

	Node coordinator = Node();
	_network.getNetworkCoordinator(coordinator);
	coordinator.setSettings("/dev/ttyUSB0",
		"1234", "5678",
			"0", "FFFF", // broadcast settings by default
			32, 10
	);

	coordinator.print(); // debug
	_updateTime();
}

void BaseStation::_updateTime(void){
	time_t t = time(0); // now
	_now = localtime(&t); 
}

void BaseStation::retrieveTimeZoneFromDB(void){
	int _offset = 0;
	char query[128];
	MYSQL *connection;
	MYSQL_ROW row;
	MYSQL_RES *result;
	MYSQL_FIELD *field;

	connection = mysql_init(NULL);
	connection = mysql_real_connect(connection, 
		"localhost", "root", "root", "test", 
			0, NULL, 0
	);

	// build query
	sprintf(query, 
		"select * from systemtimes where Date = \"%d",
			_now->tm_year+1900
	);
	
	_offset = strlen(query);
	
	if (_now->tm_mon < 10)
		sprintf(query+_offset, "-0%d", _now->tm_mon+1);
	else
		sprintf(query+_offset, "-%d", _now->tm_mon+1);

	_offset = strlen(query);
	sprintf(query+_offset, "-%d\";", _now->tm_mday);

	mysql_query(connection, query);

	result = mysql_store_result(connection);
	row = mysql_fetch_row(result);
	int num = mysql_num_fields(result);
	
	for (int i=0; i < num; i++){
		field = mysql_fetch_field(result);
		if (strcmp(field, "TON1") == 0){
			_setTON1();
		} else if (strcmp(field, "TON2") == 0){
			_setTON2();

		} else if (strcmp(field, "TINT1") == 0){
			_setTINT1();

		} else if (strcmp(field, "TINT2") == 0){
			_setTINT2();
		}
	}
	
	mysql_close (connection);
}

void BaseStation::send_email(const char *subject, const char *content, const char *dest){
	char command[128];
	sprintf(command, "echo \"%s\" | mail -s \"%s\" %s", content, subject, dest);
	printf("Sending out %s\n", command);
	//SYSTEM(buffer);
}

void BaseStation::print(void){
	_network.print();
}
