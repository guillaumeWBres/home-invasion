#ifndef __NODE_H__
#define __NODE_H__

#define COMMAND_RETRY			128
#define NETWORK_COORDINATOR 	0x01
#define NETWORK_ROUTER 			(0x01<<1)
#define NETWORK_END_DEVICE 	(0x01<<2)

#include <string>

class Node {
	
public:
	// builds a Node with default 'End Device' network role 
	Node(void);

	// builds a Node with specified network role 
	Node(uint8_t role);

	int to_file(std::string fp);

	// returns PAN/Network ID
	std::string getATID(void);
	
	// returns self ID
	std::string getATMY(void);
	
	// returns current Dest address (higher bits)
	std::string getATDH(void);

	// returns current Dest address (lower bits)
	std::string getATDL(void);

	// returns ATBD value (baud rate)
	int getATBD(void);

	// returns ATCT value (CMD dead time)
	int getATCT(void);

	// serializes self in .csv format
	// indent: current indentation level
	std::string to_csv(int indent);

	// prints self in human readable format
	void print(void);

	// role within network 
	uint8_t isCoordinator(void);
	uint8_t isRouter(void);
	uint8_t isEndDevice(void);
	uint8_t getNetworkRole(void);

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

	// sets XBEE module up
	// with specified PAN (Network) ID,
	// MYID, Dest ADDR
	// and CT time value
	// returns -1 in case of failure
	int setSettings(const char *tty, 
		const char *ID, const char *MY, 
			const char *DH, const char *DL, 
				int CT
	);

	int send_API_frame(void);

private:
	// role within network
	uint8_t _role;
	void _setNetworkRole(uint8_t role);
	
	std::string _ATID; // PAN / Network ID
	std::string _ATMY; // self ADDR
	std::string _ATDL, _ATDH; // dest ADDR
	int _ATCT; // CMD dead time
	int _ATBD; // Baud rate

	void _setATID(std::string id);
	void _setATMY(std::string my);
	void _setATDL(std::string dl);
	void _setATDH(std::string dh);
	void _setATCT(int ct);
	void _setATBD(int bd);
	
};

#endif
