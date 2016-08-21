// CXX
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int record_video_capture( std::string filename, 
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

void modify_brightness( Mat matrice, 
					float alpha, float beta ){
	for ( int y=0; y<matrice.rows; y++ ){
		for( int x=0; x<matrice.cols; x++ ){
			for ( int ch=0; ch<3; ch++ ){
				float _prev = matrice.at<Vec3b>(y,x)[ch];
				matrice.at<Vec3b>(y,x)[ch] = _prev * alpha + beta;
			}
		}
	}
}
