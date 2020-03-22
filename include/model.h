#ifndef _MODEL_H
#define _MODEL_H
#include <stdio.h>
#include <string.h>
#include <string>
#include <tuple.h>
template<typename T>
void save_model(T * data, int data_len, std::string filename){
	FILE * pFile;
  	if((pFile = fopen (filename.c_str(), "wb"))==NULL){
    	printf("cant open the file\n");
      	exit(0);
  	}

	fwrite (&data_len , sizeof(int), 1, pFile);
	fwrite (data , sizeof(T), data_len, pFile);
	fclose (pFile);
};
template<typename T>
void load_model(T * output, int attempt_data_len, std::string filename){
	FILE * pFile;
  	if((pFile = fopen (filename.c_str(), "r"))==NULL){
    	printf("cant open the file\n");
      	exit(0);
  	}
  	int data_len;
  	fread(&data_len, sizeof(int), 1, pFile);
  	if(data_len > attempt_data_len){
  		printf("array is too small\n");
  		exit(0);
  	}
  	fread(output, sizeof(T), data_len, pFile);
  	fclose (pFile);
}
void load_model_len(int& data_len, std::string filename){
  FILE * pFile;
    if((pFile = fopen (filename.c_str(), "r"))==NULL){
      printf("cant open the file\n");
        exit(0);
    }
    fread(&data_len, sizeof(int), 1, pFile);
    fclose (pFile);
}
#endif