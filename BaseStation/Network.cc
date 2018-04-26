#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <vector>
using namespace std;

#include "Network.h"
#include "Node.h"

Network::Network(vector<Node> n){
	for (unsigned int i=0; i<n.size(); i++)
		nodes.push_back(n[i]);
}

void Network::addNode(Node n){
	nodes.push_back(n);
}

void Network::removeNode(Node n){
	for (int i=0; i<size(); i++){
		if (nodes[i].getATMY() == n.getATMY())
			nodes.erase(nodes.begin()+i);
	}
}

int Network::getNode(Node &n, int id){
	for (int i=0; i<size(); i++)
	//	if (nodes[i].get_ID() == id){
	//		n = nodes[i];
			return i;
	//	}

	return -1;
}

void Network::print(void){
	printf("----- NETWORK ----\n");
	printf("---\n");
	for (int i=0; i<size(); i++){
		nodes[i].print();
		printf("---\n");
	}
	printf("------------------\n");
}

int Network::size(void){
	return nodes.size();
}

int Network::to_csv(const char *fp){
	FILE *fd;
	fd = fopen(fp, "w");
	if (fd == NULL){
		printf("failed to open %s\n", fp);
		return -1;
	}

	fprintf(fd, "<Network>\n");
	for (int i=0; i<size(); i++)
		fprintf(fd, "%s", nodes[i].to_csv(1).c_str());
	fprintf(fd, "</Network>\n");

	fclose(fd);
	return 0;
}

void Network::setEncryption(uint8_t enc){
	_encryption = enc;
}

uint8_t Network::encryption(void){
	return _encryption;
}

int Network::getNetworkCoordinator(Node &n){
	for (int i=0; i<size(); i++){
		if (nodes[i].isCoordinator()){
			n = nodes[i];
			return i;
		}
	}
	return -1;
}

int Network::getNetworkEndDevices(vector<Node> &nodes){
	vector<Node> result;
	for (int i=0; i<size(); i++){
		if (nodes[i].isEndDevice())
			result.push_back(nodes[i]);
	}
	nodes = result;
	return result.size();
}
