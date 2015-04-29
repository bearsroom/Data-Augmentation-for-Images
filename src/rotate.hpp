/*
 * rotate.hpp
 *
 *  Created on: Apr 29, 2015
 *      Author: yinghongli
 */

#ifndef ROTATE_HPP_
#define ROTATE_HPP_

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// calculate the angle for rotation
double calculateAngle(cv::Point2f LE, cv::Point2f RE);

vector<cv::Point2f> coordinatesTransform(vector<cv::Point2f> coordinates, cv::Mat& M);

Mat rotate(cv::Mat& src, double angle, cv::Mat& dst, vector<cv::Point2f> coordinates);





#endif /* ROTATE_HPP_ */
