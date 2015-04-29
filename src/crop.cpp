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
	cv::Rect ROI(topLeft, crop.size);
	return image(ROI).clone();
}

// judge whether the crop is valid on image, validate on original source oI(not rotated)
// judge whether an image contains a boundingRect
// need transform of the crop ROI to equivalent ROI on original source
double imageContainsRect(cv::Mat oI, struct Crop cropOnOI){
	cv::Rect container(0, 0, oI.cols, oI.rows);
	cv::RotatedRect content(cropOnOI.center, cropOnOI.size, cropOnOI.angle);
	Point2f Vs[4]; // 4 vertices
	content.points(Vs);
	bool contain[4];
	contain[0] = container.contains(Vs[0]); // check if vertice 0 is in OI
	contain[1] = container.contains(Vs[1]); // check if vertice 1 is in OI
	contain[2] = container.contains(Vs[2]); // check if vertice 2 is in OI
	contain[3] = container.contains(Vs[3]); // check if vertice 3 is in OI
	if (contain[0]&&contain[1]&&contain[2]&&contain[3])
		return -1;
	else{
		double s = 1;
		double s1 = 1;
		double h1;
		double w1;
		for (int i=0; i<4; i++){
			if (contain[i] == false){
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
			}
			if (s1 < s)
				s = s1;
		}
		return s;
	}
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
