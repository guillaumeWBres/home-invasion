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

#include "Sensors/mcp3008/spi.h"
#include "Sensors/mcp3008/mcp3008.h"
#define MCP3008_SPI_CHIPSELECT 0
#define MCP3008_SHARP_SENSOR_CHX 0

#include "Sensors/tsl256x/i2c.h"
#include "Sensors/tsl256x/tsl256x.h"
#define TSL256x_I2C_CHANNEL 2
#define TSL256x_I2C_CHIP_ADDR 0x29

#define CAMERA_BRIGHTNESS_THRESHOLD 100.0 
#define CAMERA_BRIGHTNESS_HI_ALPHA 0.65
#define CAMERA_BRIGHTNESS_HI_BETA 0.2

int main( int argc, char **argv ){
	
	struct mcp3008_t mcp;
	struct tsl256x_t tsl;
	int value;

	Mat source;
	VideoCapture video( 0 ); // default cam
	
	HOGDescriptor hog;
	vector<Rect> found;
	hog.setSVMDetector( HOGDescriptor::getDefaultPeopleDetector()); // load HOG descriptor

	mcp3008_init( &mcp, MCP3008_SPI_CHIPSELECT );
	
	tsl256x_init( &tsl, TSL256x_I2C_CHANNEL, TSL256x_I2C_CHIP_ADDR );
	std::cout<< "tsl256x manufacturer id "<< tsl256x_get_id( &tsl ) <<"\n";
	tsl256x_settings( &tsl, x1GAIN, AUTO_MODE, MS101 );

	while(1){

		value = mcp3008_capture( &mcp, 
			MCP3008_SHARP_SENSOR_CHX, SINGLE_ENDED_MODE );

		tsl256x_single_read( &tsl );
		luminance = tsl256x_compute_luminance( &tsl );

		if( luminance < CAMERA_BRIGHTNESS_THRESHOLD ){
			modify_brightness( source, 
				CAMERA_BRIGHTNESS_HI_ALPHA, CAMERA_BRIGHTNESS_HI_BETA );
		}

		video >> source;
		
		hog.detectMultiScale( source, found, 0, Size(8,8), Size(32,32), 1.05, 2 ); // detect HOG descriptor

		if( found.size() > 0 ){ // one obj. detected
		
			std::cout << "human shape detected\n";
			status = record_video_capture( "output.avi", 0, 10.0, video );

		}

	} 

	mcp3008_exit( &mcp );
	tsl256x_exit( &tcl );
	return 0;
}

int scp_to_server( void ){
	return system( SCP_SHELL_SCRIPT );
}
