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
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

#ifndef CROP_HPP_
#define CROP_HPP_

struct Crop{ // represent a bounding box with center
	cv::Point2f center;
	cv::Size2f size;
	float angle;

	Crop(Point2f c, Size2f s, float a){
		center = c;
		size = s;
		angle = a;
	}
};

// determine the maximum size of bounding-box having ratio={width/height} in rotated image
Mat maxSizeCroppedRegion(cv::Point2d center, double ratio, cv::Mat rotatedImage, cv::RotatedRect oldBound);

// Crop the region of interest
Mat cropROI(cv::Mat& image, struct Crop& crop);

// judge whether the crop is in the image
// -1 if the boundingRect is in the image
// if not, return s as the scale of the crop to apply in order to have no intersection
double imageContainsRect(cv::Mat originalImage, struct Crop cropOnOI);




#endif /* CROP_HPP_ */
