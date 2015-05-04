/*
 * crop.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// Related functions for cropping a region

#include "crop.hpp"

// Crop the region of interest
Mat cropROI(cv::Mat& image, struct Crop& crop){
	cv::Point2f topLeft(crop.center.x-crop.size.width/2., crop.center.y-crop.size.height/2.);
	cv::Mat ROI;
	if (0 <= topLeft.x && 0 <= crop.size.width && topLeft.x + crop.size.width <= image.cols && 0 <= topLeft.y && 0 <= crop.size.height && topLeft.y + crop.size.height <= image.rows){
		ROI = image(Rect(topLeft, crop.size));
	}
	return ROI;
}

// judge whether the crop is valid on image, validate on original source oI(not rotated)
// judge whether an image contains a boundingRect
// need transform of the crop ROI to equivalent ROI on original source
// return scale to restrict the ROI (if image contains rect, return 1)
// if center is not in the image return -1
double imageContainsRect(cv::Mat oI, struct Crop cropOnOI){
	if (!(cropOnOI.center.x > 0 && cropOnOI.center.x < oI.cols && cropOnOI.center.y > 0 && cropOnOI.center.y < oI.rows))
		return -1; // if center is not in oI

	Point2f Vs[4]; // 4 vertices of cropOnOI region
	double cosA = cos(cropOnOI.angle);
	double sinA = sin(cropOnOI.angle);
	double d = sqrt(cropOnOI.size.width*cropOnOI.size.width+cropOnOI.size.height*cropOnOI.size.height);

	Vs[0] = Point2f(cropOnOI.center.x+0.5*(cosA*cropOnOI.size.width+sinA*cropOnOI.size.height), cropOnOI.center.y+0.5*(sinA*cropOnOI.size.width-cosA*cropOnOI.size.height));
	Vs[1] = Point2f(cropOnOI.center.x+0.5*(cosA*cropOnOI.size.width-sinA*cropOnOI.size.height), cropOnOI.center.y+0.5*(sinA*cropOnOI.size.width+cosA*cropOnOI.size.height));
	Vs[2] = Point2f(cropOnOI.center.x-0.5*(cosA*cropOnOI.size.width+sinA*cropOnOI.size.height), cropOnOI.center.y-0.5*(sinA*cropOnOI.size.width-cosA*cropOnOI.size.height));
	Vs[3] = Point2f(cropOnOI.center.x-0.5*(cosA*cropOnOI.size.width-sinA*cropOnOI.size.height), cropOnOI.center.y-0.5*(sinA*cropOnOI.size.width+cosA*cropOnOI.size.height));

	double s = 1;

	for (int i=0; i<4; i++){
		double s1 = 1;
		double h1;
		double w1;

		if (Vs[i].y <= 0 && Vs[i].x >= Vs[i].y && Vs[i].x >= oI.cols-Vs[i].y){
			// case 1
			h1 = cropOnOI.center.y;
			s1 = h1/(cropOnOI.center.y - Vs[i].y);
		}
		else if (Vs[i].x >= oI.cols && Vs[i].y >= oI.cols-Vs[i].x && Vs[i].y <= Vs[i].x-oI.cols+oI.rows){
			// case 2
			w1 = oI.cols - cropOnOI.center.x;
			s1 = w1/(Vs[i].x - cropOnOI.center.x);
		}
		else if (Vs[i].y >= oI.rows && Vs[i].x >= oI.rows-Vs[i].y && Vs[i].x <= Vs[i].y+oI.cols-oI.rows){
			// case 3
			h1 = oI.rows - cropOnOI.center.y;
			s1 = h1/(Vs[i].y - cropOnOI.center.y);
		}
		else if (Vs[i].x <= 0 && Vs[i].y >= Vs[i].x && Vs[i].y <= oI.rows-Vs[i].x){
			// case 4
			w1 = cropOnOI.center.x;
			s1 = w1/(cropOnOI.center.x - Vs[i].x);
		}

		if (s1 < s)
			s = s1;
	}
	return s;
}

// return multiple ROIs of multiple scales with the same center
vector<struct Crop> multiScaleROI(cv::Point2f center, cv::Size2f maxSize, vector<double> scales, double angle){
	vector<Crop> ROIs;
	for (vector<double>::iterator it = scales.begin(); it != scales.end(); ++it){
		ROIs.push_back(Crop(
				center,
				cv::Size2f((*it)*maxSize.width, (*it)*maxSize.height),
				angle
		));
	}
	return ROIs;
}

// crop the sequence of ROIs and save to current working directory
bool cropMultiROI(const char* fileName, const char* ROIName, Mat image, Mat newImage, Point2f center, Point2f oldCenter, Size2f maxSize, vector<double> scales, double angle) throw(const char*){
	//Crop the regions
	bool resize = false;
	vector<Crop> ROIs = multiScaleROI(center, maxSize, scales, 0);

	//Check if a region intersect the bounding box of old image
	vector<Crop> ROIsOnOI = multiScaleROI(oldCenter, maxSize, scales, angle);
	for (int i=0; i<ROIs.size(); i++){
		double scale = imageContainsRect(image, ROIsOnOI[i]);
		if (scale == -1){ // the center of ROI is out of image
			throw "the center of ROI is out of image";
			return false;
		}
		if (scale != 1){ // the ROI intersect
			ROIs[i].size.width = ROIs[i].size.width*scale; // resize the ROI
			ROIs[i].size.height = ROIs[i].size.height*scale;
			resize = true;
		}

		Mat newROI = cropROI(newImage, ROIs[i]); // new cropped image
		if (!newROI.empty()){
			char buffer[100];
			sprintf(buffer, "%s_%s_%.2f.jpg", fileName, ROIName, scales[i]);
			imwrite(buffer, newROI);
		}
	}
	return resize;
}
