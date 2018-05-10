#include "BaseStation.h"

// C
#include <time.h>
#include <string.h>
#include <stdio.h>

// CXX
#include <vector>
#include <iostream>

using namespace std;

BaseStation::BaseStation(void){

	// network description 
	vector<Node> nodes;

	Node n = Node(NETWORK_COORDINATOR);
	n.setATID("5110"); // PAN ID
	n.setATMY("5100"); // BS
	n.setATDH("0000");
	n.setATDL("FFFF"); // broadcast by default
	nodes.push_back(n);

	n = Node(NETWORK_COORDINATOR);
	n.setATID("5110"); // PAN ID
	n.setATMY("5101"); // BS
	n.setATDH("0000");
	n.setATDL("5100"); // BS by default 
	nodes.push_back(n);

	_network = Network(nodes);

	// create TimeZone object
	// will directly get instructions for today from .db
	_timeZone = TimeZone();
	_timeZone.print();
}

void BaseStation::send_email(const char *subject, const char *content, const char *dest){
	char command[128];
	sprintf(command, "echo \"%s\" | mail -s \"%s\" %s", content, subject, dest);
	printf("Sending out %s\n", command);
	//SYSTEM(command);
}

void BaseStation::print(void){
	_network.print();
}
