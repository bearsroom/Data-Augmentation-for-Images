/*
 * input_output.hpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// File Input and Output

#ifndef INPUT_OUTPUT_HPP_
#define INPUT_OUTPUT_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

vector<string> getPathList(const char* path, bool absPath); // absPath = true absolute path

vector<string> getFileList(const char* path, const char* type); // without type at the end of filename

bool searchFile(const char* path, const char* fileName);

vector<Point2f> getLandmarks(const char* fileName);

void outputLandmarks(const char* filename, vector<cv::Point2f> newLandmarks, cv::Size2f newSize);

// copy in binary mode
bool copyFile(const char* SRC, const char* DEST);

#endif /* INPUT_OUTPUT_HPP_ */
