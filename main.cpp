// CXX
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

#ifdef USE_OPENCV
	#include <opencv2/opencv.hpp>
	#include <opencv2/imgproc/imgproc.hpp>
	#include <opencv2/highgui/highgui.hpp>

	using namespace cv;
#endif

// Shell 
#define CAPTURE_OUTPUT "output.avi"
#define SCP_SHELL_SCRIPT "./scp_to_server.sh"

int scp_to_server( void );

// ADC
#if USE_ADC
	#include "Sensors/mcp3008/spi.h"
	#include "Sensors/mcp3008/mcp3008.h"
	#define MCP3008_SPI_CHIPSELECT 0
	#define MCP3008_CHANNEL0 0
	#define MCP3008_CHANNEL1 1
#endif

int main( int argc, char **argv ){
	
	struct mcp3008_t mcp;
	int value;

	/*
	Mat source;
	VideoCapture video( 0 ); // default cam
	
	HOGDescriptor hog;
	vector<Rect> found;
	hog.setSVMDetector( HOGDescriptor::getDefaultPeopleDetector()); // load HOG descriptor

	namedWindow( "main", CV_WINDOW_AUTOSIZE );
	resizeWindow( "main", 50, 50 );
	*/

	mcp3008_init( &mcp, MCP3008_SPI_CHIPSELECT );
	
	value = mcp3008_capture( &mcp, MCP3008_CHANNEL0, SINGLE_ENDED_MODE );
	std::cout<<"ADC value: "<<value<<"\n";

	/*
	while(1){

		video >> source;
		hog.detectMultiScale( source, found, 0, Size(8,8), Size(32,32), 1.05, 2 ); // detect HOG descriptor
		std::cout<< "cv::HOG.detectMultiScale found " << found.size() << " object(s)\n";

		for ( int k=0; k<found.size(); k++ )
			rectangle( source, found[k].tl(), found[k].br(), Scalar(0,255,0), 2 ); // draw shape that was found
	
		imshow( "main", source );
	} */

	mcp3008_exit( &mcp );
	return 0;
}

int scp_to_server( void ){
	return system( SCP_SHELL_SCRIPT );
}
