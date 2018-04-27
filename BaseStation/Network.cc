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

int Network::discoverDevices(const char *tty){
	int fd;
	Node n;
	int size = 0;
	char c, buffer[1024];

	fd = open(tty, O_RDWR);
	if (fd < 0){
		printf("Failed to open %s port\n", tty);
		return -1;
	}
	
	getNetworkCoordinator(n);
	n.send_command(fd, "+++"); // CMD
	n.send_command(fd, "ATNT50\r"); // 5 sec discovery timeout
	n.send_command(fd, "ATAC\r");
	n.send_command(fd, "ATND%s\r"); // Network discovery
	
	while (1){
		read(fd, &c, 1);
		buffer[size] = c;
		
		if (c == '\r'){
			if (buffer[size-1] == '\r'){
				break;
			}
		}

		if (size == 1024-1){
			size = 0;
		} else
			size ++;

		// parsing:
		// MY<CR>
		// SH<CR>
		// SL<CR>
		// ...
		// ID<CR><CR>
	}

	return 0;
}
