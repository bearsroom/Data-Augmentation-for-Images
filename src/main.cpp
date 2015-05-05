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

// return true if success
bool readData(const char* fileName, cv::Mat& image, vector<cv::Point2f>& landmarks) throw(const char*){

	char buffer[100];
	string bufferStr;
	sprintf(buffer, "%s.jpg", fileName);
	bufferStr = buffer;

	image = imread(bufferStr.c_str(), CV_LOAD_IMAGE_COLOR);

	if(!image.data){ // check if the image existed
		cout<<"No image data "<<fileName<<".jpg"<<endl;
		throw "No image data";
		return false;
	}

	sprintf(buffer, "%s.cor", fileName);
	bufferStr = buffer;
	landmarks = getLandmarks(bufferStr.c_str()); // get the landmarks from [id].cor file
	int lmSize = landmarks.size();
	if (lmSize != 5){
		cout<<"Errors occurred when retrieving landmarks from "<<fileName<<".cor"<<endl;
		throw "Errors occurred when retrieving landmarks";
		return false;
	}

	return true;
}

void image_augmentation(const char* fileName, cv::Mat image, vector<cv::Point2f> landmarks) throw(const char*){ //fileName = id

	char buffer[100];
	string bufferStr;

	Mat newImage; // output image
	double angle = calculateAngle(landmarks[0], landmarks[1]); // calculate the angle for rotation
	cv::Mat rot = rotate(image, angle, newImage, landmarks); // rotate the image

	vector<cv::Point2f> newLandmarks = coordinatesTransform(landmarks, rot); // calculate the new landmarks on new image after rotation
	sprintf(buffer, "%s_rotated.cor", fileName);
	bufferStr = buffer;
	outputLandmarks(bufferStr.c_str(), newLandmarks, cv::Size2f(newImage.cols, newImage.rows)); // write new landmarks into [id]_new.cor file

	sprintf(buffer, "%s_rotated.jpg", fileName);
	bufferStr = buffer;
	imwrite(bufferStr.c_str(), newImage); // create the new image after rotation

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

	bool resizeEyes = cropMultiROI(fileName, "eyes", image, newImage, eyesCenter, eyesOldCenter, eyesMaxSize, eyesScales, angle);

	//Crop the regions: nose
	cv::Point2f noseCenter = newLandmarks[2];
	cv::Point2f noseOldCenter = landmarks[2];
	cv::Size2f noseMaxSize = Size2f(eyesWidth*1.5, eyesWidth*1.5);
	vector<double> noseScales;
	noseScales.push_back(1);

	bool resizeNose = cropMultiROI(fileName, "nose", image, newImage, noseCenter, noseOldCenter, noseMaxSize, noseScales, angle);

	//Crop the full face
	cv::Point2f fullCenter = newLandmarks[2];
	cv::Point2f fullOldCenter = landmarks[2];
	cv::Size2f fullMaxSize = Size2f(eyesWidth*2, eyesWidth*2*4/3.);
	vector<double> fullScales;
	fullScales.push_back(1);

	bool resizeFull = cropMultiROI(fileName, "full", image, newImage, fullCenter, fullOldCenter, fullMaxSize, fullScales, angle);
	if (resizeEyes || resizeNose || resizeFull){
			throw "At least one ROI has been resized.";
		}

	return;
}

int main( int argc, char** argv ){
	set_unexpected(myunexpected);

	const char* dbPath = "/home/yinghongli/Documents/DeepFace2";
	const char* outputPath = "/home/yinghongli/Documents/DeepFace2Aug";
	ofstream log1("/home/yinghongli/Documents/DeepFace2Aug/log.txt", fstream::app); // create a log file
	log1<<"input Path: "<<dbPath<<endl;
	log1<<"output Path: "<<outputPath<<endl<<endl;

	vector<string> people;
	people = getPathList(dbPath, false);
	log1<<"Number of people: "<<people.size()<<endl<<endl;

	// for every person
	for (int i=0; i<people.size(); i++){
		int numError = 0;
		try{
			vector<string> imageNames;
			char inputPath[100];
			sprintf(inputPath, "%s/%s", dbPath, people[i].c_str());
			imageNames = getFileList(inputPath, ".jpg");

			char personPath[100];
			sprintf(personPath, "%s/%s", outputPath, people[i].c_str());
			mkdir(personPath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
			chdir(personPath);
			log1<<people[i]<<"/ : "<<imageNames.size()<<" images found"<<endl;
			cout<<"Current input path: "<<inputPath<<" "<<imageNames.size()<<" images"<<endl;
			cout<<"Current output path: "<<personPath<<endl<<endl;

			ofstream logFile("log.txt", fstream::app); // create a log file
			logFile<<"input Path: "<<inputPath<<endl;
			logFile<<"output Path: "<<personPath<<endl<<endl;
			char imagePath[100];

			// for every image
			for (int j=0; j<imageNames.size(); j++){
				const char* imageName = imageNames[j].c_str();
				sprintf(imagePath, "%s/%s", personPath, imageName);
				mkdir(imagePath, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
				chdir(imagePath);// change working directory
				try{
					// copy the original [id].jpg file
					char imageInput[100];
					char imageOutput[20];
					sprintf(imageInput, "%s/%s.jpg", inputPath, imageName);
					sprintf(imageOutput, "%s.jpg", imageName);
					bool hasImage = copyFile(imageInput, imageOutput);

					// copy the original [id].cor file
					char corInput[100];
					char corOutput[20];
					sprintf(corInput, "%s/%s.cor", inputPath, imageName);
					sprintf(corOutput, "%s.cor", imageName);
					bool hasCor = copyFile(corInput, corOutput);

					if (hasImage && hasCor){
						cv::Mat image;
						vector<cv::Point2f> landmarks;
						bool success = readData(imageName, image, landmarks);
						if (success)
							image_augmentation(imageName, image, landmarks);
					}
					else{
						numError++;
						throw "Cannot find image or cor file in working directory";
					}
				}catch (const char* message) {
					logFile<<imageName<<".jpg: "<<message<<endl;
				}
			}
			logFile.close();
		}catch (const char* message){
			log1<<people[i]<<"/ : "<<message<<endl;
		}
		log1<<people[i]<<"/ : "<<numError<<" image(s) non treated"<<endl<<endl;
	}

	log1.close();
	cout<<"Image pre-process completed"<<endl;
	return 0;
}




