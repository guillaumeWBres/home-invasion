#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <iostream>
using namespace std;

#include "Node.h"

Node::Node(void){
	_setNetworkRole(NETWORK_END_DEVICE);
}

Node::Node(uint8_t role){
	_setNetworkRole(role);
}

std::string Node::to_csv(const char *tty, int indent){
	std::string result;
	
	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("<Node>\n");
	
	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("\t<ATMY>");
	result.append(getATMY());
	result.append("</ATMY>\n");

	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("\t<Role>");
	
	if (isCoordinator())
		result.append("Coordinator");
	else if (isRouter())
		result.append("Router");
	else 
		result.append("EndDevice");

	result.append("</Role>\n");
	
	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("\t<STS>WIP");
	//result.append(itoa(getState(tty), 10));
	result.append("</STS>\n");

	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("</Node>\n");

	return result;
}

void Node::print(void){
	cout << "PAN ID: " << getATID() << "\n";
	cout << "ATMY: " << getATMY() << "\n";
	cout << "ATDH: " << getATDH() << "\n";
	cout << "ATDL: " << getATDL() << "\n";
}

uint8_t Node::isCoordinator(void){
	if (_role & NETWORK_COORDINATOR)
		return 0x01;
	else
		return 0x00;
}

uint8_t Node::isRouter(void){
	if (_role & NETWORK_ROUTER)
		return 0x01;
	else
		return 0x00;
}

uint8_t Node::isEndDevice(void){
	if (_role & NETWORK_END_DEVICE)
		return 0x01;
	else
		return 0x00;
}

uint8_t Node::getNetworkRole(void){
	return _role;
}

void Node::_setNetworkRole(uint8_t role){
	_role = role;
}

int Node::send_command(int fd, const char *msg){
	char c;
	write(fd, msg, strlen(msg));

	while (1){
		read(fd, &c, 1);
		if (c == '\r')
			break;
	}

	return 0;
}

int Node::send_command_readback(int fd, const char *msg, char *answer){
	int size = 0;
	char c, buffer[64];
	write(fd, msg, strlen(msg));
	usleep(100000);
	while (1){
		read(fd, &c, 1);
		buffer[size++] = c;
		if (c == '\r')
			break;
	}

	strcpy(answer, buffer);
	return size;
}

int Node::broadcast(const char *tty, const char *payload){
	int fd;
	int size;

	fd = open(tty, O_RDWR);
	if (fd < 0){
		printf("Failed to open %s port\n", tty);
		return -1;
	}
	
	if ((getATDH() != "0")||(getATDL() != "FFFF")){
		// activate broadcast mode
		send_command(fd, "+++");
		send_command(fd, "ATDH0\r");
		send_command(fd, "ATDLFFFF\r");
		send_command(fd, "ATCN\r");
	}

	size = write(fd, payload, strlen(payload));
	close(fd);
	return size; 
}

int Node::unicast(const char *tty, 
		const char *dh, const char *dl, const char *payload
){
	int fd;
	int size;
	char buffer[16];

	fd = open(tty, O_RDWR);
	if (fd < 0){
		printf("Failed to open %s port\n", tty);
		return -1;
	}

	if ((getATDH().c_str() != dh)||(getATDL().c_str() != dl)){
		// set specified ADDR
		send_command(fd, "+++");	
		
		sprintf(buffer, "ATDH%s\r", dh); 
		send_command(fd, buffer);
		setATDH(dh);

		sprintf(buffer, "ATDL%s\r", dl); 
		send_command(fd, buffer);
		setATDL(dl);

		send_command(fd, "ATCN\r");
	}

	size = write(fd, payload, strlen(payload));
	close(fd);
	return size;
}

int Node::XBEE_settings(const char *tty){
	if (isCoordinator()){ // direct HW access
		char answer[64];
		int fd = open(tty, O_RDWR);

		if (fd < 0){
			printf("failed to open %s port\n", tty);
			return -1;
		}

		send_command(fd, "+++"); usleep(500000); // enter CMD mode
		send_command_readback(fd, "ATBD\r", answer);  // check baudrate
		printf("ATBD: %s\n", answer);
		
		send_command_readback(fd, "ATID\r", answer);  // PAN/Network ID 
		printf("ATID: %s\n", answer);
		
		send_command_readback(fd, "ATMY\r", answer);  // self ID 
		printf("ATMY: %s\n", answer);
		
		send_command_readback(fd, "ATDH\r", answer);  // dest ID 
		printf("ATDH: %s\n", answer);
		
		send_command_readback(fd, "ATDL\r", answer);  // dest ID 
		printf("ATDL: %s\n", answer);

		send_command(fd, "ATCN\r");

		close(fd);
		return 0;
	}

	return -1;
}

uint8_t Node::_checksum(const char *frame){
	int sum = 0;
	for (int i=2; i<strlen(frame); i++)
		sum += frame[i];
	sum &= 0xFF;
	return (uint8_t)(0xFF-sum);
}

int Node::sleep(const char *tty){
	unicast(tty, "0000", getATMY().c_str(), "NODE_HIBERNATE\r\n");
	//parse_status();
	return 0;
}

int Node::standBy(const char *tty){
	unicast(tty, "0000", getATMY().c_str(), "NODE_STAND_BY\r\n");
	//parse_status();
	return 0;
}

int Node::active(const char *tty){
	unicast(tty, "0000", getATMY().c_str(), "NODE_ACTIVE\r\n");
	//parse_status();
	return 0;
}

int Node::getState(const char *tty){
	unicast(tty, "0000", getATMY().c_str(), "NODE_STATUS\r\n");
	//parse_status();
	return 0;
}

int parse_status(char *payload, char *extra){
	//"<Node|i|STS|i>"
	//"<Node|i|STS|-1|extra>"
	int status;
	char *token;
	token = strtok(payload, "|");
	// check ID match
	token = strtok(NULL, "|");
	token = strtok(NULL, "|");
	status = atoi(token);
	if (status == -1){
		token = strtok(NULL, "|");
		strcpy(extra, token);
	}
	return status;
}

std::string Node::getATID(void){
	return _ATID;
}

void Node::setATID(string id){
	_ATID = id;
}

std::string Node::getATMY(void){
	return _ATMY;
}

void Node::setATMY(string my){
	_ATMY = my;
}

std:: string Node::getATDL(void){
	return _ATDL;
}

void Node::setATDL(string dl){
	_ATDL = dl;
}

std::string Node::getATDH(void){
	return _ATDH;
}

void Node::setATDH(string dh){
	_ATDH = dh;
}

int Node::isActive(const char *tty){
	return 0;
}

int Node::isSleeping(const char *tty){
	return 0;
}

int Node::isInStandBy(const char *tty){
	return 0;
}
