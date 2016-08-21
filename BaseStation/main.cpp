// CXX
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

int scp_to_server( void );

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "OpenCV/opencv_lib.h"
using namespace cv;

#define SCP_SHELL_SCRIPT "./scp_to_server.sh"

#define CAMERA_BRIGHTNESS_THRESHOLD 100.0 
#define CAMERA_BRIGHTNESS_HI_ALPHA 0.65
#define CAMERA_BRIGHTNESS_HI_BETA 0.2

int main( int argc, char **argv ){
	
	Mat source;
	VideoCapture video( 0 ); // default cam
	
	HOGDescriptor hog;
	vector<Rect> found;
	hog.setSVMDetector( HOGDescriptor::getDefaultPeopleDetector()); // load HOG descriptor
	
	while(1){

		video >> source;
		hog.detectMultiScale( source, found, 0, Size(8,8), Size(32,32), 1.05, 2 ); // detect HOG descriptor
		if( found.size() > 0 ){ // one obj. detected
			std::cout << "human shape detected\n";
			record_video_capture( "output.avi", 0, 10.0, video );
		}
	} 

	return 0;
}

int scp_to_server( void ){
	return system( SCP_SHELL_SCRIPT );
}
