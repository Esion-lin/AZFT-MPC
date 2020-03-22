// Auther: Tainpei Lu
// Creation: 11/02 2019 
#ifndef _TOOL_H_
#define _TOOL_H_
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "crypto.h"
#include <vector>
#include <algorithm>
#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
int tran_op(std::string op);
int64_t tran_op(int64_t a, int64_t b, std::string op);
void to_byte16(uint64_t org, unsigned char output[]);
void to_byte16(uint64_t org1, uint64_t org2, unsigned char output[]);
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, unsigned char output[]);
void to_byte(uint32_t org[], unsigned int len, unsigned char output[], unsigned int &out_len);
void to_ll(unsigned char input[], uint64_t &output);
void to_ll(unsigned char input[], uint64_t &output1, uint64_t &output2);
void to_int(unsigned char input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4);
void to_int(unsigned char input[], unsigned int arr_len, uint32_t arr[]);
bool is_num(std::string data , int64_t &num);
std::string get_item(std::map<std::string,int64_t> dic, std::string str);
std::vector<std::string> getFiles(std::string path);
#endif