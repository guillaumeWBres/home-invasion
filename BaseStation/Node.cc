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

std::string Node::to_csv(int indent){
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
	result.append("\t<STS>OK</STS>\n");

	for (int i=0; i<indent; i++)
		result.append("\t");
	result.append("</Node>\n");

	return result;
}

void Node::print(void){
	cout << "ATBD: " << getATBD() << "\n";
	cout << "PAN ID: " << getATID() << "\n";
	cout << "ATMY: " << getATMY() << "\n";
	cout << "ATDH: " << getATDH() << "\n";
	cout << "ATDL: " << getATDL() << "\n";
	cout << "ATCT: " << getATCT() << "\n";
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
		_setATDH(dh);

		sprintf(buffer, "ATDL%s\r", dl); 
		send_command(fd, buffer);
		_setATDL(dl);

		send_command(fd, "ATCN\r");
	}

	size = write(fd, payload, strlen(payload));
	close(fd);
	return size;
}

int Node::readSettings(const char *tty){
	if (isCoordinator()){ // direct HW access
		char answer[64];
		int fd = open(tty, O_RDWR);

		if (fd < 0){
			printf("failed to open %s port\n", tty);
			return -1;
		}

		send_command(fd, "+++"); usleep(500000); // enter CMD mode
		send_command_readback(fd, "ATBD\r", answer);  // check baudrate
		_setATBD(atoi(answer));
		
		send_command_readback(fd, "ATID\r", answer);  // PAN/Network ID 
		_setATID(string(answer));
		
		send_command_readback(fd, "ATMY\r", answer);  // self ID 
		_setATMY(string(answer));
		
		send_command_readback(fd, "ATDH\r", answer);  // dest ID 
		_setATDH(string(answer));
		
		send_command_readback(fd, "ATDL\r", answer);  // dest ID 
		_setATDL(string(answer));

		send_command_readback(fd, "ATCT\r", answer); // CMD mode timeout
		_setATCT(atoi(answer));

		send_command_readback(fd, "ATGT\r", answer); // AT cmd gard time
		_setATGT(atoi(answer));

		send_command(fd, "ATCN\r"); // we're done
		close(fd);
		return 0;
	}

	return -1;
}

int Node::setSettings(const char *tty, 
	const char *ID, const char *MY, 
		const char *DH, const char *DL, 
			int CT, int GT
){
	if (isCoordinator()){ // direct HW access
		char cmd[64];
		int fd = open(tty, O_RDWR);

		if (fd < 0){
			printf("failed to open %s port\n", tty);
			return -1;
		}

		send_command(fd, "+++"); // enter CMD mode
		// set CT: CMD mode timeout value
		sprintf(cmd, "ATCT%d\r", CT);
		send_command(fd, cmd);
		printf("%s\n", cmd);
		_setATCT(CT);

		// set GT: cmd gard time
		sprintf(cmd, "ATGT%d\r", GT);
		send_command(fd, cmd);
		_setATGT(GT);

		// set PAN ID
		sprintf(cmd, "ATID%s\r", ID);
		send_command(fd, cmd); 
		printf("%s\n", cmd);
		_setATID(string(ID));

		// set MY ID
		sprintf(cmd, "ATMY%s\r", MY);
		send_command(fd, cmd); 
		printf("%s\n", cmd);
		_setATMY(string(MY));

		// set DH ADDR
		sprintf(cmd, "ATDH%s\r", DH);
		send_command(fd, cmd); 
		printf("%s\n", cmd);
		_setATDH(string(DH));

		// set DL ADDR
		sprintf(cmd, "ATDL%s\r", DL);
		send_command(fd, cmd); 
		printf("%s\n", cmd);
		_setATDL(string(DL));

		send_command(fd, "ATCN\r"); // we're done
		printf("ATCN\n");

		close(fd);
		return 0;
	}
		
	return -1;
}

int Node::send_API_frame(const uint16_t size){
	char frame[64];
	int _offset = 0;
	uint8_t MSB, LSB;
//	strcpy(frame+0, 0x7E, 1);
	MSB = (size>>4)&0x0F;
	LSB = (size)&0x0F;
//	strcpy(frame+1, MSB, 2);
//	strcpy(frame+3, LSB, 2);
}

uint8_t Node::_checksum(const char *frame){
	int sum = 0;
	for (int i=2; i<strlen(frame); i++)
		sum += frame[i];
	sum &= 0xFF;
	return (uint8_t)(0xFF-sum);
}

std::string Node::getATID(void){
	return _ATID;
}

void Node::_setATID(string id){
	_ATID = id;
}

std::string Node::getATMY(void){
	return _ATMY;
}

void Node::_setATMY(string my){
	_ATMY = my;
}

std:: string Node::getATDL(void){
	return _ATDL;
}

void Node::_setATDL(string dl){
	_ATDL = dl;
}

std::string Node::getATDH(void){
	return _ATDH;
}

void Node::_setATDH(string dh){
	_ATDH = dh;
}

int Node::getATBD(void){
	return _ATBD;
}

void Node::_setATBD(int bd){
	_ATBD = bd;
}

int Node::getATCT(void){
	return _ATCT;
}

void Node::_setATCT(int ct){
	_ATCT = ct;
}

void Node::_setATGT(int gt){
	_ATGT = gt;
}

int Node::getATGT(void){
	return _ATGT;
}
