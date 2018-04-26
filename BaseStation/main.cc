// CXX
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
using namespace std;

#include "Network.h"

//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "OpenCV/opencv_lib.h"
//using namespace cv;

//#define CAMERA_BRIGHTNESS_THRESHOLD 100.0 
//#define CAMERA_BRIGHTNESS_HI_ALPHA 0.65
//#define CAMERA_BRIGHTNESS_HI_BETA 0.2

void send_email(const char *subject, const char *content, const char *dest){
	char command[128];
	sprintf(command, "echo \"%s\" | mail -s \"%s\" %s", content, subject, dest);
	printf("Sending out %s\n", command);
	//SYSTEM(buffer);
}

int main(int argc, char **argv){
	// build network object
	vector<Node> nodes;
	nodes.push_back(Node(NETWORK_COORDINATOR));
	nodes.push_back(Node(NETWORK_END_DEVICE));

	Network network = Network(nodes);

	Node coordinator = Node();
	network.getNetworkCoordinator(coordinator);

	coordinator.setSettings("/dev/ttyUSB0", 
		"1234", "5678", 
			"0", "FFFF", // broadcast settings are default for coord.
				128
	);

	coordinator.print(); 

	return 0;
	//network.to_csv("test.csv");
}
