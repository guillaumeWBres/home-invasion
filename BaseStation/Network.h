#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>

#include <vector>
using namespace std;

#include "Node.h"

class Network {
public:
	Network(void);
	Network(vector<Node> nodes);
	
	// adds given node to network
	void addNode(Node n);
	
	// removes given node to network
	void removeNode(Node n);

	// serializes self into .csv format
	int to_csv(const char *tty, const char *csv);

	// prints self in human readable format
	void print(void);

	// returns number of nodes in network
	int size(void);

	// returns ith element in network
	int getNode(Node &n, int i);

	// locates network coordinator within network
	// returns -1 if not found
	// returns [i] where i is position within network list
	int getNetworkCoordinator(Node &node);
	
	// returns list of End Devices within network
	// returns size of vector list
	int getNetworkEndDevices(vector<Node> &nodes);

	// discovers all devices over this network 
	int discoverDevices(const char *tty);

	// hibernation ctrl
	// sets end device(s) in hibernation 
	int sleep(const char *tty);
	int sleep(const char *tty, Node n); 
	int sleep(const char *tty, const char *id); 

	// stand by ctrl
	// sets end device(s) in stand by state
	int standBy(const char *tty);
	int standBy(const char *tty, Node n);
	int standBy(const char *tty, const char *id);

	// active ctrl
	// sets end device(s) in active mode
	int active(const char *tty);
	int active(const char *tty, Node n);
	int active(const char *tty, const char *id);

	// get list of currently active nodes
	std::vector<Node> getActiveNodes(const char *tty);

	// get list of currently sleeping nodes
	std::vector<Node> getSleepingNodes(const char *tty);

	// get list of nodes in stand by
	std::vector<Node> getStandByNodes(const char *tty);

private:
	vector<Node> nodes;

};

#endif
