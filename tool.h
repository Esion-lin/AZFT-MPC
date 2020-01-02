// Auther: Tainpei Lu
// Creation: 11/02 2019 
#ifndef _TOOL_H_
#define _TOOL_H_
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HW/crypto.h"
Json::Value stream2json(unsigned char stream[]);
unsigned char * json2stream(Json::Value value);
int tran_op(std::string op);
int tran_op(int a, int b, std::string op);
void to_byte16(uint64_t org, unsigned char output[]);
void to_byte16(uint64_t org1, uint64_t org2, unsigned char output[]);
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, unsigned char output[]);
void to_byte(uint32_t org[], unsigned int len, unsigned char output[], unsigned int &out_len);
void to_ll(unsigned char input[], uint64_t &output);
void to_ll(unsigned char input[], uint64_t &output1, uint64_t &output2);
void to_int(unsigned char input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4);
void to_int(unsigned char input[], unsigned int arr_len, uint32_t arr[]);
bool is_num(std::string data , int &num);
std::string get_item(std::map<std::string,int> dic, std::string str);
#endif