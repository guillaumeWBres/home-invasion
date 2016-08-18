// CXX
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int record_video_capture( String filename, 
		int format, float seconds, VideoCapture cap );

// GoogleVoice
#include "GoogleVoice/gvoice.h"
#define GOOGLE_VOICE_INFOS "GoogleVoice/.gvoice"

int google_voice( void );
std::string google_voice_build_msg( void );

// Custom 
#define CAPTURE_OUTPUT "output.avi"
#define SCP_SHELL_SCRIPT "./scp_to_server.sh"

int scp_to_server( void );

int main( int argc, char **argv ){

	Mat source;
	VideoCapture video( 0 ); // default cam
	
	HOGDescriptor hog;
	vector<Rect> found;
	hog.setSVMDetector( HOGDescriptor::getDefaultPeopleDetector()); // load HOG descriptor

	namedWindow( "main", CV_WINDOW_AUTOSIZE );
	resizeWindow( "main", 50, 50 );

	while(1){

		video >> source;
		hog.detectMultiScale( source, found, 0, Size(8,8), Size(32,32), 1.05, 2 ); // detect HOG descriptor
		std::cout<< "cv::HOG.detectMultiScale found " << found.size() << " object(s)\n";

		for ( int k=0; k<found.size(); k++ )
			rectangle( source, found[k].tl(), found[k].br(), Scalar(0,255,0), 2 ); // draw shape that was found
	
		imshow( "main", source );
	}

	return 0;
}

int record_video_capture( String filename, 
		int format, float seconds, VideoCapture cap )
{
	VideoWriter writer;
	Mat frame;

	int codec_type;
	Size size;
	float fps;

	int _frame_index = 0;
	int _frames_to_capture;

	codec_type = cap.get( CV_CAP_PROP_FOURCC );

	size = Size( cap.get( CV_CAP_PROP_FRAME_WIDTH ), 
						cap.get( CV_CAP_PROP_FRAME_HEIGHT ));

	fps = cap.get( CV_CAP_PROP_FPS );

	writer.open( filename, codec_type, fps, size, true );

	if( ! writer.isOpened() ){
		std::cout << "failed to open output video file\n";
		return -1;
	}
	
	_frames_to_capture = int(seconds*fps);
	while( _frame_index < _frames_to_capture ){
		cap >> frame; 
		writer << frame;
		_frame_index++;
	}
	
	std::cout << "Video file " << filename << " has been recorded\n";
	return 0;
}

int scp_to_server( void ){
	return system( SCP_SHELL_SCRIPT );
}

int google_voice( void ){
	GoogleVoice gv;
	std::ifstream file( GOOGLE_VOICE_INFOS );
	std::string username, password;

	std::string message;
	int r, _length;
	

	if(gv.Init()){
		std::cout << "GoogleVoice() failed to initialize.\n";
		return -1;
	}

	getline( file, username );
	getline( file, password );
	file.close();

	r = gv.Login( username, password );

	if((gv.debug & 1) || r )
		std::cout << "GoogleVoice login returned "<< r << " \n";
	
	message = google_voice_build_msg(); 
	r = gv.SendSMS( "1234567", message );

	if((gv.debug & 2 ) || r )
		std::cout << "GoogleVoice.sendSMS() returned " << r << "\n";
	if(gv.debug & 1 && !r )
		std:cout << "SMS sent successfully\n";
	
	return r;
}

std::string google_voice_build_msg( void ){
	std::string _msg = "toto";

	return _msg;
}
