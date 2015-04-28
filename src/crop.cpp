/*
 * crop.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// Related functions for cropping a region

#include "crop.hpp"

// determine the maximum size of bounding-box having ratio={width/height} in rotated image
// output: {maxWidth, maxHeight}
Mat maxSizeCroppedRegion(cv::Point2d center, double ratio, cv::Mat rotatedImage, cv::RotatedRect oldBound){

}

// judge whether the crop is valid on image
bool rectContainsRect(cv::Mat originalImage, struct Crop crop, double angle){
	// structure of Mat crop: (crop[0], crop[1])
	cv::Rect container(0, 0, originalImage.cols, originalImage.rows);
	cv::RotatedRect content(crop.center, crop.size, crop.angle-angle);
	Point2f contentVs[4];
	content.points(contentVs);
	if (container.contains(contentVs[0]))
		return true;
	else
		return false;
}

