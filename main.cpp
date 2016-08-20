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

#define CAPTURE_OUTPUT "output.avi"
#define SCP_SHELL_SCRIPT "./scp_to_server.sh"

#include "Sensors/mcp3008/spi.h"
#include "Sensors/mcp3008/mcp3008.h"
#define MCP3008_SPI_CHIPSELECT 0
#define MCP3008_LUM_SENSOR_CHX 0
#define MCP3008_SHARP_SENSOR_CHX 1

#define CAMERA_BRIGHTNESS_THRESHOLD 25
#define CAMERA_BRIGHTNESS_HI_ALPHA 0.65
#define CAMERA_BRIGHTNESS_HI_BETA 0.2

int main( int argc, char **argv ){
	
	struct mcp3008_t mcp;
	int value;

	Mat source;
	VideoCapture video( 0 ); // default cam
	
	HOGDescriptor hog;
	vector<Rect> found;
	hog.setSVMDetector( HOGDescriptor::getDefaultPeopleDetector()); // load HOG descriptor

	namedWindow( "main", CV_WINDOW_AUTOSIZE );
	resizeWindow( "main", 50, 50 );
	
	mcp3008_init( &mcp, MCP3008_SPI_CHIPSELECT );

	while(1){
	
		value = mcp3008_capture( &mcp,
			MCP3008_LUM_SENSOR_CHX, SINGLE_ENDED_MODE );

		if( value < CAMERA_BRIGHTNESS_THRESHOLD ){
			modify_brightness( source, 
				CAMERA_BRIGHTNESS_HI_ALPHA, CAMERA_BRIGHTNESS_HI_BETA );
		}

		video >> source;
		
		hog.detectMultiScale( source, found, 0, Size(8,8), Size(32,32), 1.05, 2 ); // detect HOG descriptor
		std::cout<< "cv::HOG.detectMultiScale found " << found.size() << " object(s)\n";

		for ( int k=0; k<found.size(); k++ )
			rectangle( source, found[k].tl(), found[k].br(), Scalar(0,255,0), 2 ); // draw shape that was found
		
		imshow( "main", source ); 
	} 

	mcp3008_exit( &mcp );
	return 0;
}

int scp_to_server( void ){
	return system( SCP_SHELL_SCRIPT );
}
