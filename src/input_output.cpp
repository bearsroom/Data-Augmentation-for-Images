/*
 * input_output.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// File Input and Output

#include "input_output.hpp"

using namespace std;

vector<string> getFileList(const char* path, const char* type){
	// get a list of filenames with type defined in a path provided
	vector<string> fileNames;
	struct dirent *pDirent;
	DIR *pDir;

	pDir = opendir(path);
	if (pDir == NULL){
		cout<<"Cannot open directory "<<path<<": not such a directory";
	}
	else{
		while ((pDirent = readdir(pDir)) != NULL){
			if (strstr(pDirent->d_name, type) != NULL)
			fileNames.push_back(pDirent->d_name);
		}
	}
	closedir (pDir);
	return fileNames; // possible to be empty if the directory not existed
}


