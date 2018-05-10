#ifndef __NODE_H__
#define __NODE_H__

#define COMMAND_RETRY			128
#define NETWORK_COORDINATOR 	0x01
#define NETWORK_ROUTER 			(0x01<<1)
#define NETWORK_END_DEVICE 	(0x01<<2)

#define NODE_STAND_BY 	0
#define NODE_ACTIVE 		1
#define NODE_HIBERNATE 	2
#define NODE_UNKNOWN		(-1)

#include <string>

class Node {
	
public:
	// builds a Node with default 'End Device' network role 
	Node(void);

	// builds a Node with specified network role 
	Node(uint8_t role);

	int to_file(std::string fp);

	// controls XBEE parameters
	std::string getATID(void);
	std::string getATMY(void);
	std::string getATDH(void);
	std::string getATDL(void);

	// controls XBEE parameters
	void setATID(std::string id);
	void setATMY(std::string my);
	void setATDL(std::string dl);
	void setATDH(std::string dh);

	// serializes self in .csv format
	// indent: current indentation level
	std::string to_csv(const char *tty, int indent);

	// prints self in human readable format
	void print(void);

	// role within network 
	uint8_t isCoordinator(void);
	uint8_t isRouter(void);
	uint8_t isEndDevice(void);
	uint8_t getNetworkRole(void);
	
	// parses status command received from node
	int parseStatus(char *message, char *extra);
	
	// Node state control
	int sleep(const char *tty); // node hibernates
	int standBy(const char *tty); // set stand by
	int active(const char *tty); // node is now active
	int getState(const char *tty); // returns current state

	int isActive(const char *tty);
	int isSleeping(const char *tty);
	int isInStandBy(const char *tty);

	// sends required message as unicast
	// returns -1 in case of failure
	// returns size_t otherwise
	int unicast(const char *tty, const char *dh, const char *dl, const char *payload);

	// broadcasts message over the network
	// returns -1 in case of failure
	// returns size_t on success
	int broadcast(const char *tty, const char *payload);

	// sends given message to DEST address
	// returns -1 in case of failure
	// returns size_t on success
	int send_message(const char *tty, const char *dest, const char *payload);
	
	// sends message using opened serial port
	// returns 0 if 'OK\r' received
	// returns -1 otherwise
	int send_command(int fd, const char *msg);

	// sends message using opened serial port
	// returns answer
	int send_command_readback(int fd, const char *msg, char *answer);

	// retrieves node settings
	// if self.isCoordinator: HW access is directly provided
	// returns -1 in case of failure
	// failure: HW access was not granted
	// failure: baud rate mismatch?
	int readSettings(const char *tty);

	// prints current XBEE settings using serial port
	int XBEE_settings(const char *tty);

	// sets node to sleep mode
	void sleep(void);

	// wakes node up
	void wakeup(void);

	void setBaudRate(int bd);
	int getBaudRate(void);

	void setGuardTime(int gt);
	int getGuardTime(void);

private:
	// role within network
	uint8_t _role;
	void _setNetworkRole(uint8_t role);
	
	std::string _ATID; // PAN / Network ID
	std::string _ATMY; // self ADDR
	std::string _ATDL, _ATDH; // dest ADDR

	// evaluates checksum of current API frame 
	uint8_t _checksum(const char *APIframe);
};

#endif
