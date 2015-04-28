/*
 * rotate.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: yinghongli
 */

// compile command: g++ -o rotate rotate.cpp `pkg-config --libs --cflags opencv`

#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void rotate(cv::Mat& src, double angle, cv::Mat& dst)
{
    // get rotation matrix for rotating the image around its center
	cv::Point2f center(src.cols/2.0, src.rows/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

    // determine bounding rectangle
    cv::Rect bbox = cv::RotatedRect(center,src.size(), angle).boundingRect();

    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    cv::warpAffine(src, dst, rot, bbox.size());
}

double calculate_angle(double LE[2], double RE[2])
{
	// calculate the angle for rotation
	double param = (RE[0]-LE[0])/sqrt(pow(RE[0]-LE[0], 2)+pow(RE[1]-LE[1], 2));
	double angle = acos(param)*180.0/M_PI;
	//cout<<angle;
	return angle;
}

int main( int argc, char** argv )
{
	char* image_name = "/home/yinghongli/Documents/DeepFace2/Adele/79.jpg";

	Mat image;
	image = imread(image_name, CV_LOAD_IMAGE_COLOR);

	if(!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	Mat new_image; // output image
	double LE[2] = {78.52, 58.90}; // landmark of left eye
	double RE[2] = {127.92, 80.79}; // landmark of right eye
	double angle = calculate_angle(LE, RE);
	rotate(image, angle, new_image);

	char* output_path = "/home/yinghongli/Documents/Image_preprocessing/79.jpg";
	imwrite(output_path, new_image);
	return 0;
}



