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

vector<string> getFileList(const char* path, const char* type);



#endif /* INPUT_OUTPUT_HPP_ */
