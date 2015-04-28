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

double calculate_angle(vector<double> LE, vector<double> RE)
{
	// calculate the angle for rotation
	double param = (RE[0]-LE[0])/sqrt(pow(RE[0]-LE[0], 2)+pow(RE[1]-LE[1], 2));
	double angle = acos(param)*180.0/M_PI;
	cout<<angle<<endl;
	return angle;
}

Mat coordinates_transform(cv::Mat& coordinates, cv::Mat& M)
{
	// verify the dimension of input coordinates: a 2-by-n matrix
	if (coordinates.rows != 2)
		;
		//return false;
	else{
		cv::Mat row = cv::Mat::ones(1, coordinates.cols, coordinates.type());
		coordinates.push_back(row);
		cv::Mat new_coordinates = M*coordinates;
		coordinates.pop_back();
		return new_coordinates;
	}
}

Mat rotate(cv::Mat& src, double angle, cv::Mat& dst, cv::Mat& coordinates)
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
    return coordinates_transform(coordinates, rot);
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
	Mat landmarks = Mat(2, 5, cv::DataType<double>::type);
	landmarks.at<double>(0,0) = 78.52; // X of LE
	landmarks.at<double>(1,0) = 58.90; // Y of LE
	landmarks.at<double>(0,1) = 127.92; // X of RE
	landmarks.at<double>(1,1) = 80.79; // Y of RE
	landmarks.at<double>(0,2) = 102.26; // X of N
	landmarks.at<double>(1,2) = 96.34; // Y of N
	landmarks.at<double>(0,3) = 59.25; // X of LM
	landmarks.at<double>(1,3) = 117.68; // Y Of LM
	landmarks.at<double>(0,4) = 97.67; // X of RM
	landmarks.at<double>(1,4) = 135.35; // Y of RM

	vector<double> LE;
	LE.push_back(landmarks.at<double>(0,0));
	LE.push_back(landmarks.at<double>(1,0));
	vector<double> RE;
	RE.push_back(landmarks.at<double>(0,1));
	RE.push_back(landmarks.at<double>(1,1));
	double angle = calculate_angle(LE, RE);
	Mat new_landmarks = rotate(image, angle, new_image, landmarks);
	cout<<landmarks<<endl;
	cout<<new_landmarks<<endl;

	char* output_path = "/home/yinghongli/Documents/Image_preprocessing/79.jpg";
	imwrite(output_path, new_image);
	return 0;
}



