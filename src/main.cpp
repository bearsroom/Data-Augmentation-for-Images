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

void myunexpected(){
	throw "Unexpected error";
}

bool image_augmentation(const char* inputPath, const char* fileName) throw(const char*){ //fileName = id

	Mat image;
	char buffer[100];
	char bufferInput[100];
	sprintf(bufferInput, "%s/%s%s", inputPath, fileName, ".jpg");
	image = imread(bufferInput, CV_LOAD_IMAGE_COLOR);

	if(!image.data){ // check if the image existed
		throw "No image data";
		return false;
	}
	sprintf(buffer, "%s.jpg", fileName);
	copyFile(bufferInput, buffer); // copy the original image

	// check if the associated .cor file existed
	sprintf(buffer, "%s.cor", fileName);
	if (!searchFile(inputPath, buffer)){
		throw "No landmarks data";
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
	cv::Size2f eyesMaxSize = Size2f(eyesWidth*2, eyesWidth*2*3/4.);
	vector<double> eyesScales;
	eyesScales.push_back(1);
	eyesScales.push_back(0.9);
	eyesScales.push_back(0.8);
	eyesScales.push_back(0.7);

	bool resize = cropMultiROI(fileName, "eyes", image, newImage, eyesCenter, eyesOldCenter, eyesMaxSize, eyesScales, angle);
	if (resize == true){
		throw "At least one ROI of eyes has been resized.";
	}

	//Crop the regions: nose
	cv::Point2f noseCenter = newLandmarks[2];
	cv::Point2f noseOldCenter = landmarks[2];
	cv::Size2f noseMaxSize = Size2f(eyesWidth*1.5, eyesWidth*1.5);
	vector<double> noseScales;
	noseScales.push_back(1);

	resize = cropMultiROI(fileName, "nose", image, newImage, noseCenter, noseOldCenter, noseMaxSize, noseScales, angle);
	if (resize == true){
			throw "At least one ROI of nose has been resized.";
		}

	//Crop the full face
	cv::Point2f fullCenter = newLandmarks[2];
	cv::Point2f fullOldCenter = landmarks[2];
	cv::Size2f fullMaxSize = Size2f(eyesWidth*2, eyesWidth*2*4/3.);
	vector<double> fullScales;
	fullScales.push_back(1);

	resize = cropMultiROI(fileName, "full", image, newImage, fullCenter, fullOldCenter, fullMaxSize, fullScales, angle);
	if (resize == true){
			throw "At least one ROI of full face has been resized.";
		}

	return true;
}

int main( int argc, char** argv ){

	char* dbPath = "/home/yinghongli/Documents/DeepFace2";
	const char* outputPath = "/home/yinghongli/Documents/DeepFace2Aug";
	ofstream log1("/home/yinghongli/Documents/DeepFace2Aug/log.txt", fstream::app); // create a log file
	log1<<"input Path: "<<dbPath<<endl;
	log1<<"output Path: "<<outputPath<<endl<<endl;

	vector<string> people;
	people = getPathList(dbPath, false);
	log1<<"Number of people: "<<people.size()<<endl<<endl;

	for (int i=0; i<people.size(); i++){
		try{
			vector<string> imageNames;
			char inputPath[100];
			sprintf(inputPath, "%s/%s", dbPath, people[i].c_str());
			imageNames = getFileList(inputPath, ".jpg");

			char personPath[100];
			sprintf(personPath, "%s/%s", outputPath, people[i].c_str());
			mkdir(personPath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
			chdir(personPath);

			ofstream logFile("log.txt", fstream::app); // create a log file
			logFile<<"input Path: "<<inputPath<<endl;
			logFile<<"output Path: "<<personPath<<endl<<endl;
			char imagePath[100];
			for (int j=0; j<imageNames.size(); j++){
				sprintf(imagePath, "%s/%s", personPath, imageNames[j].c_str());
				mkdir(imagePath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
				chdir(imagePath);// change working directory
				try{
					image_augmentation(inputPath, imageNames[j].c_str());
				}catch (const char* message) {
					logFile<<imageNames[j]<<".jpg: "<<message<<endl;
				}
			}
			logFile.close();
		}catch (const char* message){
			log1<<people[i]<<"/ : "<<message<<endl;
		}
	}

	log1.close();
	cout<<"Image pre-process completed"<<endl;
	return 0;
}




