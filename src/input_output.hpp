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
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


vector<string> getFileList(const char* path, const char* type);

Mat getLandmarks(const char* filename);

// write or append the data into a file
template<class T>
void outputToFile(const char* filename, vector<T> data, char* option);

#endif /* INPUT_OUTPUT_HPP_ */
