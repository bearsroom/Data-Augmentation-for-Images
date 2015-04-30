/*
 * input_output.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// File Input and Output

#include "input_output.hpp"

vector<string> getFileList(const char* path, const char* type){
	// get a list of filenames or directories' names with type defined in a path provided
	vector<string> outputList;
	struct dirent *pDirent;
	DIR *pDir;
	int typeLength = strlen(type);
	cout<<typeLength<<endl;

	pDir = opendir(path);
	if (pDir == NULL){
		cout<<"Cannot open directory "<<path<<": not such a directory";
	}
	else{
		while ((pDirent = readdir(pDir)) != NULL){
			if (strstr(pDirent->d_name, type) != NULL){
				char fileName[strlen(pDirent->d_name)-typeLength];
				int fileNameLength = sizeof(fileName);
				for (int i=0; i<fileNameLength; i++)
					fileName[i] = pDirent->d_name[i];
				fileName[fileNameLength] = '\0'; // the end of the char array
				outputList.push_back(fileName);
			}
		}
	}
	closedir (pDir);
	return outputList; // outputList possible to be empty if the directory not existed
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
