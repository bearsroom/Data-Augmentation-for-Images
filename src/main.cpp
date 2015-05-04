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
//		cout<<"No image data"<<endl;
		return false;
	}
	sprintf(buffer, "%s.jpg", fileName);
	copyFile(bufferInput, buffer); // copy the original image

	// check if the associated .cor file existed
	sprintf(buffer, "%s.cor", fileName);
	if (!searchFile(inputPath, buffer)){
//		cout<<"No landmarks data"<<endl;
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

	float eyesWidth = newLandmarks[1].x-newLandmarks[0].x;
	//Crop the regions: eyes
	cv::Point2f eyesCenter = Point2f((newLandmarks[0].x+newLandmarks[1].x)/2., (newLandmarks[0].y+newLandmarks[1].y)/2.);
	cv::Point2f eyesOldCenter = Point2f((landmarks[0].x+landmarks[1].x)/2., (landmarks[0].y+landmarks[1].y)/2.);
	cv::Size2f eyesMaxSize = Size2f(eyesWidth*2.5, eyesWidth*2.5*3/4.);
	vector<double> eyesScales;
	eyesScales.push_back(1);
	eyesScales.push_back(0.8);
	eyesScales.push_back(0.5);

	cropMultiROI(fileName, "eyes", image, newImage, eyesCenter, eyesOldCenter, eyesMaxSize, eyesScales, angle);

	//Crop the regions: nose
	cv::Point2f noseCenter = newLandmarks[2];
	cv::Point2f noseOldCenter = landmarks[2];
	cv::Size2f noseMaxSize = Size2f(eyesWidth*1.5, eyesWidth*1.5);
	vector<double> noseScales;
	noseScales.push_back(1);

	cropMultiROI(fileName, "nose", image, newImage, noseCenter, noseOldCenter, noseMaxSize, noseScales, angle);

	//Crop the full face
	cv::Point2f fullCenter = newLandmarks[2];
	cv::Point2f fullOldCenter = landmarks[2];
	cv::Size2f fullMaxSize = Size2f(eyesWidth*2, eyesWidth*2*4/3.);
	vector<double> fullScales;
	fullScales.push_back(1);

	cropMultiROI(fileName, "full", image, newImage, fullCenter, fullOldCenter, fullMaxSize, fullScales, angle);

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
		try{
			image_augmentation(inputPath, imageNames[j].c_str());
		}catch (std::exception& e) {
		    std::cerr << "Error occurred when processing on image "<<people[0]<<"/"<<imageNames[j]<<".jpg"<<endl;
		  }
	}

	cout<<"Image pre-process completed"<<endl;
	return 0;
}




