/*
 * input_output.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: yinghongli
 */

// File Input and Output

#include "input_output.hpp"

vector<string> getPathList(const char* path, bool absPath){
	// get a list of directories
		vector<string> outputList;
		struct dirent *pDirent;
		DIR *pDir;

		pDir = opendir(path);
		if (pDir == NULL)
			cout<<"Cannot open directory "<<path<<": not such a directory";
		else{
			while ((pDirent = readdir(pDir)) != NULL){
				if (strstr(pDirent->d_name, ".") == NULL){  // is a directory
					if (absPath){
						char* buffer;
						sprintf(buffer, "%s/%s", path, pDirent->d_name);
						outputList.push_back(buffer);
					}
					else
						outputList.push_back(pDirent->d_name);
				}
			}
		}
		closedir (pDir);
		return outputList; // outputList possible to be empty if the directory not existed
}

vector<string> getFileList(const char* path, const char* type){
	// get a list of filenames with type defined in a path provided
	vector<string> outputList;
	struct dirent *pDirent;
	DIR *pDir;
	int typeLength = strlen(type);

	pDir = opendir(path);
	if (pDir == NULL)
		cout<<"Cannot open directory "<<path<<": not such a directory";
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

bool searchFile(const char* path, const char* fileName){
	struct dirent *pDirent;
	DIR *pDir;

	pDir = opendir(path);
		if (pDir == NULL)
			cout<<"Cannot open directory "<<path<<": not such a directory";
		else{
			while ((pDirent = readdir(pDir)) != NULL){
				if (strcmp(pDirent->d_name, fileName) == 0)
					return true;
			}
		}
		closedir (pDir);
		return false;
}

vector<Point2f> getLandmarks(const char* fileName){
	string line;
	string str1, str2;
	int width, height;
	float x, y;
	ifstream infile(fileName);
	cv::Size2f size;
	vector<Point2f> landmarks;

	while (getline(infile, line)){
		istringstream iss(line);
		//cout << iss.str() <<endl;
		if (iss >> str1 >> x >> y)
			landmarks.push_back(cv::Point2f(x, y));   // order: LE, RE, N, LM, RM
		else if (iss >> str1 >> width >> str2 >> height) // width and height
			size = cv::Size2f(width, height);
	}
	return landmarks;
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
