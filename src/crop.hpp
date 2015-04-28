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

struct Crop{
	cv::Point2f center;
	cv::Size2f size;
	float angle;
};

// determine the maximum size of bounding-box having ratio={width/height} in rotated image
Mat maxSizeCroppedRegion(cv::Point2d center, double ratio, cv::Mat rotatedImage, cv::RotatedRect oldBound);

// judge whether a boundingRect contains another boundingRect
bool rectContainsRect(cv::RotatedRect container, cv::RotatedRect content);




#endif /* CROP_HPP_ */
