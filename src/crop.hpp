/*
 * crop.hpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// Related functions for cropping a region

#include <iostream>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "rotate.hpp"

using namespace std;
using namespace cv;

#ifndef CROP_HPP_
#define CROP_HPP_

struct Crop{ // represent a ROI
	cv::Point2f center;
	cv::Size2f size;
	float angle;

	Crop(Point2f c, Size2f s, float a){
		center = c;
		size = s;
		angle = a;
	}
};

// Crop the region of interest
Mat cropROI(cv::Mat& image, struct Crop& crop)throw (const char*);

// judge whether the crop is in the image
// -1 if the boundingRect is in the image
// if not, return s as the scale of the crop to apply in order to have no intersection
double imageContainsRect(cv::Mat originalImage, struct Crop cropOnOI);

// return multiple ROIs of multiple scales with the same center
vector<struct Crop> multiScaleROI(cv::Point2f center, cv::Size2f maxSize, vector<double> scales, double angle);

// crop the sequence of ROIs and save to current working directory
// true = resize at least one ROI, false = keep all the original sizes
bool cropMultiROI(const char* fileName, const char* ROIName, Mat image, Mat newImage, Point2f center, Point2f oldCenter, Size2f maxSize, vector<double> scales, double angle) throw(const char*);

#endif /* CROP_HPP_ */
