#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>

#include <vector>
using namespace std;

#include "Node.h"

class Network {
public:
	Network(vector<Node> nodes);
	
	// adds given node to network
	void addNode(Node n);
	
	// removes given node to network
	void removeNode(Node n);

	// serializes self into .csv format
	int to_csv(const char *csv);

	// prints self in human readable format
	void print(void);

	// returns number of nodes in network
	int size(void);

	// returns ith element in network
	int getNode(Node &n, int i);

	// returns 1 if network uses encryption
	uint8_t encryption(void);

	// enables/disables network encryption
	void setEncryption(uint8_t enc);

	// locates network coordinator within network
	// returns -1 if not found
	// returns [i] where i is position within network list
	int getNetworkCoordinator(Node &node);
	
	// returns list of End Devices within network
	// returns size of vector list
	int getNetworkEndDevices(vector<Node> &nodes);

	int scan(void);
private:

	vector<Node> nodes;
	uint8_t _encryption; 

};

#endif
