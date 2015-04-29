/*
 * input_output.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// File Input and Output

#include "input_output.hpp"

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

Mat getLandmarks(const char* filenames){
	return cv::Mat();
}

// write or append the data into a file
template<class T>
void outputToFile(const char* filename, vector<T> data, char* option){
//	if (option == "write")
//		FileStorage fs(filename, FileStorage::WRITE);
//	else if(option == "append")
//		FileStorage fs(filename, FileStorage::APPEND);
//	else{
//		cout<<"Option non valid: write or append allowed"<<endl;
//		return;
//	}

	FileStorage fs(filename, FileStorage::WRITE);
	fs<<"Number of files in this directory :"<<data.size();

	for (typename vector<T>::iterator it = data.begin(); it != data.end(); ++it){
		fs<<*it<<'\n';
	}

}
