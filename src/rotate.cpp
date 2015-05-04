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
#include <stdio.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "input_output.hpp"
#include "crop.hpp"

using namespace cv;
using namespace std;

double calculateAngle(cv::Point2f LE, cv::Point2f RE){
	// calculate the angle for rotation
	double param = (RE.y-LE.y)/sqrt(pow(RE.x-LE.x, 2)+pow(RE.y-LE.y, 2));
	double angle = asin(param)*180.0/M_PI;
	return angle;
}

vector<cv::Point2f> coordinatesTransform(vector<cv::Point2f> coordinates, cv::Mat& M){
	cv::Mat src = cv::Mat::zeros(3, coordinates.size(), cv::DataType<double>::type);
	cv::Mat dst;
	for (int i=0; i< coordinates.size(); i++){
//		cv::Mat p = cv::Mat(coordinates[i]);
//		p.copyTo(src.col(i));
		src.at<double>(0, i) += coordinates[i].x;
		src.at<double>(1, i) += coordinates[i].y;
		src.at<double>(2, i) += 1;
	}

	dst = M*src;

	vector<cv::Point2f> newCoordinates;
	for (int i=0; i<coordinates.size(); i++){
		newCoordinates.push_back(cv::Point2f(dst.col(i)));
	}
	return newCoordinates;
}

Mat rotate(cv::Mat& src, double angle, cv::Mat& dst, vector<cv::Point2f> coordinates){
    // get rotation matrix for rotating the image around its center
	cv::Point2f center(src.cols/2.0, src.rows/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

    // determine bounding rectangle
    cv::Rect bbox = cv::RotatedRect(center,src.size(), angle).boundingRect();

    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    cv::warpAffine(src, dst, rot, bbox.size());
    return rot;
}



