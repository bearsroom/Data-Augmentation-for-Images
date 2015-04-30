/*
 * main.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: yinghongli
 */

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "rotate.hpp"
#include "input_output.hpp"
#include "crop.hpp"

using namespace std;
using namespace cv;

int main( int argc, char** argv ){
	//char* imageName = "/home/yinghongli/Documents/DeepFace2/Adele/79.jpg";

	vector<string> imageNames;
	vector<string> corNames;
	char* inputPath = "/home/yinghongli/Documents/DeepFace2/Adele";
	imageNames = getFileList(inputPath, ".jpg");
	corNames = getFileList(inputPath, ".cor");
	cout<<imageNames[0]<<endl;
	cout<<corNames[0]<<endl;
	//outputToFile("/home/yinghongli/Documents/Image_preprocessing/filenames.yaml", imageNames, "write");

	Mat image;
	char fileName[100];
	sprintf(fileName, "%s/%s%s", inputPath, imageNames[0].c_str(), ".jpg");
	cout<<fileName<<endl;
	image = imread(fileName, CV_LOAD_IMAGE_COLOR);

	if(!image.data){
		printf("No image data \n");
		return -1;
	}

	Mat newImage; // output image

	vector<cv::Point2f> landmarks;
	landmarks.push_back(cv::Point2f(78.52, 58.90)); // LE
	landmarks.push_back(cv::Point2f(127.92, 80.79)); // RE
	landmarks.push_back(cv::Point2f(102.26, 96.34)); // N
	landmarks.push_back(cv::Point2f(59.25, 117.68)); // LM
	landmarks.push_back(cv::Point2f(97.67, 135.35)); // RM

	double angle = calculateAngle(landmarks[0], landmarks[1]);
	cv::Mat rot = rotate(image, angle, newImage, landmarks);
	vector<cv::Point2f> newLandmarks = coordinatesTransform(landmarks, rot);

	char* output_path = "/home/yinghongli/Documents/Image_preprocessing";
	sprintf(fileName, "%s/testC.jpg", output_path);
	imwrite(fileName, newImage);

	//Crop the regions
	cv::Point2f center = Point2f((newLandmarks[0].x+newLandmarks[1].x)/2., (newLandmarks[0].y+newLandmarks[1].y)/2.);
	cv::Point2f oldCenter = Point2f((landmarks[0].x+landmarks[1].x)/2., (landmarks[0].y+landmarks[1].y)/2.);
	cv::Size2f maxSize = Size2f((newLandmarks[1].x-newLandmarks[0].x)*2, (newLandmarks[1].x-newLandmarks[0].x)*3/2.);
	vector<double> scales;
	scales.push_back(1);
	scales.push_back(0.8);
	scales.push_back(0.5);
	vector<Crop> ROIs = multiScaleROI(center, maxSize, scales, 0);

	//Check if the region intersect the bounding box of old image
	vector<Crop> ROIsOnOI = multiScaleROI(oldCenter, maxSize, scales, -angle);
	output_path = "/home/yinghongli/Documents/Image_preprocessing";
	for (int i=0; i<ROIs.size(); i++){
		double scale = imageContainsRect(image, ROIsOnOI[i]);
		if (scale != -1){
			ROIs[i].size.width = ROIs[i].size.width*scale;
			ROIs[i].size.height = ROIs[i].size.height*scale;
		}

		Mat newROI = cropROI(newImage, ROIs[i]);
		sprintf(fileName, "%s/testCropped_%.1f.jpg", output_path, scales[i]);
		imwrite(fileName, newROI);
	}

	return 0;
}




