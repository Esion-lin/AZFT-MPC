#ifndef _MODEL_H
#define _MODEL_H
#include <stdio.h>
#include <string.h>
#include <string>

template<typename T>
static void save_model(T * data, uint32_t data_len, std::string filename){
	FILE * pFile;
  	if((pFile = fopen (filename.c_str(), "wb+"))==NULL){
    	printf("cant open the file\n");
      	exit(0);
  	}

	fwrite (&data_len , sizeof(uint32_t), 1, pFile);
	fwrite (data , sizeof(T), data_len, pFile);
	fclose (pFile);
};
template<typename T>
static void load_model(T * output, uint32_t attempt_data_len, std::string filename){
	FILE * pFile;
  	if((pFile = fopen (filename.c_str(), "r"))==NULL){
    	printf("cant open the file\n");
      	exit(0);
  	}
  	uint32_t data_len;
  	fread(&data_len, sizeof(uint32_t), 1, pFile);
  	if(data_len > attempt_data_len){
  		printf("array is too small\n");
  		exit(0);
  	}
  	fread(output, sizeof(T), data_len, pFile);
  	fclose (pFile);
}
static void load_model_len(uint32_t& data_len, std::string filename){
  FILE * pFile;
    if((pFile = fopen (filename.c_str(), "r"))==NULL){
      printf("cant open the file\n");
        exit(0);
    }
    fread(&data_len, sizeof(uint32_t), 1, pFile);
    fclose (pFile);
}
#endif