/*
 * main.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: yinghongli
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "rotate.hpp"
#include "input_output.hpp"
#include "crop.hpp"

using namespace std;
using namespace cv;

bool image_augmentation(const char* inputPath, const char* fileName){ //fileName = id

	Mat image;
	char buffer[100];
	char bufferInput[100];
	sprintf(bufferInput, "%s/%s%s", inputPath, fileName, ".jpg");
	image = imread(bufferInput, CV_LOAD_IMAGE_COLOR);

	if(!image.data){ // check if the image existed
		cout<<"No image data"<<endl;
		return false;
	}
	sprintf(buffer, "%s.jpg", fileName);
	copyFile(bufferInput, buffer); // copy the original image

	// check if the associated .cor file existed
	sprintf(buffer, "%s.cor", fileName);
	if (!searchFile(inputPath, buffer)){
		cout<<"No landmarks data"<<endl;
		return false;
	}

	sprintf(bufferInput, "%s/%s.cor", inputPath, fileName);
	vector<cv::Point2f> landmarks = getLandmarks(bufferInput); // get the landmarks from [id].cor file
	sprintf(buffer, "%s_old.cor", fileName);
	copyFile(bufferInput, buffer); // copy the original [id].cor file

	Mat newImage; // output image
	double angle = calculateAngle(landmarks[0], landmarks[1]); // calculate the angle for rotation
	cv::Mat rot = rotate(image, angle, newImage, landmarks); // rotate the image

	vector<cv::Point2f> newLandmarks = coordinatesTransform(landmarks, rot); // calculate the new landmarks on new image after rotation
	sprintf(buffer, "%s_new.cor", fileName);
	outputLandmarks(buffer, newLandmarks, cv::Size2f(newImage.cols, newImage.rows)); // write new landmarks into [id]_new.cor file

	sprintf(buffer, "%s_test.jpg", fileName);
	imwrite(buffer, newImage); // create the new image after rotation

	//Crop the regions
	cv::Point2f center = Point2f((newLandmarks[0].x+newLandmarks[1].x)/2., (newLandmarks[0].y+newLandmarks[1].y)/2.);
	cv::Point2f oldCenter = Point2f((landmarks[0].x+landmarks[1].x)/2., (landmarks[0].y+landmarks[1].y)/2.);
	cv::Size2f maxSize = Size2f((newLandmarks[1].x-newLandmarks[0].x)*2, (newLandmarks[1].x-newLandmarks[0].x)*3/2.);
	vector<double> scales;
	scales.push_back(1);
	scales.push_back(0.8);
	scales.push_back(0.5);
	vector<Crop> ROIs = multiScaleROI(center, maxSize, scales, 0);

	//Check if a region intersect the bounding box of old image
	vector<Crop> ROIsOnOI = multiScaleROI(oldCenter, maxSize, scales, -angle);
	for (int i=0; i<ROIs.size(); i++){
		double scale = imageContainsRect(image, ROIsOnOI[i]);
		if (scale != -1){ // the ROI intersect
			ROIs[i].size.width = ROIs[i].size.width*scale; // resize the ROI
			ROIs[i].size.height = ROIs[i].size.height*scale;
		}

		Mat newROI = cropROI(newImage, ROIs[i]); // new cropped image
		sprintf(buffer, "%s_testCropped_%.1f.jpg", fileName, scales[i]);
		imwrite(buffer, newROI);
	}
	return true;
}

int main( int argc, char** argv ){

	char* dbPath = "/home/yinghongli/Documents/DeepFace2";
	vector<string> people;
	people = getPathList(dbPath, false);
	cout<<"Number of people: "<<people.size()<<endl;

	vector<string> imageNames;
	char inputPath[100];
	sprintf(inputPath, "%s/%s", dbPath, people[0].c_str());
//	cout<<inputPath<<endl;
	imageNames = getFileList(inputPath, ".jpg");

	const char* outputPath = "/home/yinghongli/Documents/Image_preprocessing";
	char personPath[100];
	sprintf(personPath, "%s/%s", outputPath, people[0].c_str());
	mkdir(personPath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);

	char imagePath[100];
	for (int j=0; j<imageNames.size(); j++){
		sprintf(imagePath, "%s/%s", personPath, imageNames[j].c_str());
		mkdir(imagePath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
		chdir(imagePath);// change working directory
		image_augmentation(inputPath, imageNames[j].c_str());
	}

	return 0;
}




