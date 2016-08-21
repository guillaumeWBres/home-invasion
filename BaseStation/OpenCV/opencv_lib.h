#ifndef __OPENCV_LIB_H__
#define __OPENCV_LIB_H__

#include <opencv2/opencv.hpp>

int record_video_capture( std::string filename, 
	int format, float seconds, cv::VideoCapture cap );

void modify_brightness( cv::Mat matrice, 
						float alpha, float beta );

#endif
