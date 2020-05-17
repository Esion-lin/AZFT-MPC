// Auther: Tainpei Lu
// Creation: 11/02 2019 
#ifndef _TOOL_H_
#define _TOOL_H_
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <openssl/evp.h>
#include "component.h"
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
void to_byte16(uint64_t org, uint8_t output[]);
void to_byte16(uint64_t org1, uint64_t org2, uint8_t output[]);
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, uint8_t output[]);
void to_byte(uint32_t org[], uint32_t len, uint8_t output[], uint32_t &out_len);
void to_ll(uint8_t input[], uint64_t &output);
void to_ll(uint8_t input[], uint64_t &output1, uint64_t &output2);
void to_int(uint8_t input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4);
void to_int(uint8_t input[], uint32_t arr_len, uint32_t arr[]);
bool is_num(std::string data , int64_t &num);
std::string get_item(std::map<std::string,int64_t> dic, std::string str);
std::vector<std::string> getFiles(std::string path);
std::string sha3_k(uint8_t *message, int message_len);
void sha3(uint8_t *digest, const uint8_t *message, size_t message_len);
void store_data_to_file(uint8_t * data, int data_len, std::string filename);
void load_data_frome_file(uint8_t * output, int data_len, std::string filename);
void store_file_with_string(std::string data, std::string filename);
#endif