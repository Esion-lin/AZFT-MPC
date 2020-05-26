#include <string.h>
#include "component.h"
#include "model.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "some_struct.h"
#include "tool.h"
#include "tee.h"
using namespace std;
bool pro_test = true;
void add_out(uint8_t* label,struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr, POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = OUT_OP;
	memcpy(code.S + s_itr, label, LABEL_LEN);
	s_itr += LABEL_LEN;
}
void add_ins(uint8_t op, uint8_t* label1, uint8_t* label2, uint8_t* label3, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr, POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = op;
	memcpy(code.S + s_itr, label1, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = FLOAT_TP;
	memcpy(code.S + s_itr, label2, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = FLOAT_TP;
	memcpy(code.S + s_itr, label3, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = FLOAT_TP;
}
void add_goto(uint8_t* label, uint32_t tar, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr, POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = GOTO;
	memcpy(code.S + s_itr, label, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code.S + s_itr, &tar, sizeof(uint32_t));
	s_itr += sizeof(uint32_t);
}
void add_cov(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint32_t kernel_size, uint32_t kernel_w, uint32_t kernel_l, uint32_t kernel_h, bool is_pending, uint8_t stride, uint8_t ex_for_tee, uint32_t &W_pos, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = COV;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(kernel_size/256);
	code.S[s_itr++] = (uint8_t)(kernel_size%256);
	code.S[s_itr++] = (uint8_t)is_pending;
	code.S[s_itr++] = stride;
	code.S[s_itr++] = (uint8_t)(kernel_w/256);
	code.S[s_itr++] = (uint8_t)(kernel_w%256);
	code.S[s_itr++] = (uint8_t)(kernel_l/256);
	code.S[s_itr++] = (uint8_t)(kernel_l%256);
	code.S[s_itr++] = (uint8_t)(kernel_h/256);
	code.S[s_itr++] = (uint8_t)(kernel_h%256);
	code.S[s_itr++] = ex_for_tee;
	memcpy(code.S + s_itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += kernel_w*kernel_l*kernel_h*kernel_size*sizeof(float);
}
void add_pooling(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint8_t pooling_type, uint32_t kernel_w, uint32_t kernel_l, bool is_pending, uint8_t stride, uint8_t ex_for_tee, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = POOLING;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = pooling_type;
	code.S[s_itr++] = (uint8_t)is_pending;
	code.S[s_itr++] = stride;
	code.S[s_itr++] = (uint8_t)(kernel_w/256);
	code.S[s_itr++] = (uint8_t)(kernel_w%256);
	code.S[s_itr++] = (uint8_t)(kernel_l/256);
	code.S[s_itr++] = (uint8_t)(kernel_l%256);
	code.S[s_itr++] = ex_for_tee;
}
void add_ReLU(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, float alpha, uint8_t ex_for_tee, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = RELU;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code.S + s_itr, &alpha, sizeof(float));
	s_itr += sizeof(float);
	code.S[s_itr++] = ex_for_tee;
}
void add_BN(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint8_t ex_for_tee, uint32_t &W_pos, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = BN_ID;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = ex_for_tee;
	memcpy(code.S + s_itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += 5*sizeof(float);
}
void add_FC(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint32_t weight_width, uint8_t ex_for_tee, uint32_t &W_pos, struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = FC_ID;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code.S + s_itr, &weight_width, sizeof(uint32_t));
	s_itr += sizeof(uint32_t);
	code.S[s_itr++] = ex_for_tee;
	memcpy(code.S + s_itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += w*l*h*weight_width*sizeof(float);
}
void add_short(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in,uint32_t w2, uint32_t l2, uint32_t h2, uint8_t* label2_in,uint8_t* label_out,uint8_t ex_for_tee,struct Code &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code.S[s_itr++] = SHORTCUT;
	memcpy(code.S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w/256);
	code.S[s_itr++] = (uint8_t)(w%256);
	code.S[s_itr++] = (uint8_t)(l/256);
	code.S[s_itr++] = (uint8_t)(l%256);
	code.S[s_itr++] = (uint8_t)(h/256);
	code.S[s_itr++] = (uint8_t)(h%256);
	memcpy(code.S + s_itr, label2_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = (uint8_t)(w2/256);
	code.S[s_itr++] = (uint8_t)(w2%256);
	code.S[s_itr++] = (uint8_t)(l2/256);
	code.S[s_itr++] = (uint8_t)(l2%256);
	code.S[s_itr++] = (uint8_t)(h2/256);
	code.S[s_itr++] = (uint8_t)(h2%256);
	memcpy(code.S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code.S[s_itr++] = ex_for_tee;
}
std::string index_pattern_of_string(std::string src, uint32_t index){
	bool flip = true;
	for(int i = 0; i< src.length(); i++){
		if(src[i] != ' ' && flip){
			index --;
			flip = false;
		}
		if(src[i] == ' '){
			flip = true;
		}
		if(index == 0){
			src = src.substr(i);
			return src.substr(0, src.find(" "));
		}
	}
}
uint32_t idx_in_shape(std::string src, uint32_t index){
	uint32_t ret = 0;
	if(src[0] != '[' || src[src.length()-1] != ']'){
		return 0;
	}
	src = src.substr(1, src.length() - 2);
	char*p;
	p = strtok((char*)src.c_str(),",");
	
	while(p != NULL){
		index --;
		if(index == 0){
			ret = atoi(p);
			break;
		}
		p = strtok(NULL,",");
	}
	return ret;
}
uint32_t size_of_shape(std::string src){
	uint32_t ret;
	uint32_t ans = 1;
	int i = 1;
	src = src.substr(src.find("["));
	while((ret = idx_in_shape(src, i))!=0){
		i ++;
		ans *= ret;
	}
	return ans;
}
bool scan_once(std::string file_path, uint32_t &slen, uint32_t &wlen, uint32_t &ilen){
	ifstream file;
	string strLine;
	file.open(file_path,ios::in);
	if(!file.is_open()){
        printf("protocol file does not exist\n");
        file.close();
        return false;
    }
    while(getline(file,strLine)){
    	if(strLine.empty())
            continue;
        if(strLine.find("//") != strLine.npos){
        	continue;
        }
    	if(strLine.find("COV") != strLine.npos){
    		ilen ++;
    		slen += COV_LEN;
    		wlen += sizeof(float) * atoi(index_pattern_of_string(strLine, 6).c_str()) * size_of_shape(index_pattern_of_string(strLine, 7));
    	}
    	else if(strLine.find("POOLING") != strLine.npos){
    		ilen ++;
    		slen += POOLING_LEN;
    	}
    	else if(strLine.find("ReLU") != strLine.npos){
    		ilen ++;
			slen += RELU_LEN;
    	}
    	else if(strLine.find("BN") != strLine.npos){
    		ilen ++;
    		slen += BN_LEN;
    		wlen += 5*sizeof(float);
    	}
    	else if(strLine.find("SHORTCUT") != strLine.npos){
    		ilen ++;
    		slen += SHORTCUT_LEN;
    	}
    	else if(strLine.find("FC") != strLine.npos){
    		ilen ++;
    		slen += FC_LEN;
    		wlen += sizeof(float) *atoi(index_pattern_of_string(strLine, 4).c_str()) * size_of_shape(index_pattern_of_string(strLine, 3));
    	}else if(strLine.find("OUT") != strLine.npos){
    		slen += OUT_LEN;
    		ilen ++;
    	}else{
    		printf("illagel protocol file\n");
    		file.close();
    		return false;
    	}
    }
    file.close();
    return true;
}
void to_char(string str, char* out_str){
	for(int i = 0; i < str.length(); i++){
		out_str[i] = str[i];
	}
	for(int i = str.length(); i < LABEL_LEN; i++){
		out_str[i] = '\0';
	}
	
}
bool load_weight(std::string path, uint32_t len, uint8_t* data){
	uint32_t out_len;
	load_model_len(out_len, path);
	if(out_len != len){
		return false;
	}
	load_model<uint8_t>(data, len, path);
	return true;
}
bool read_model(std::string file_path){
	TEE *tru = new TEE();
	uint32_t slen = 0;
	uint32_t wlen = 0;
	uint32_t ilen = 0;
	if(!scan_once(file_path, slen, wlen, ilen)){
		return false;
	}
	printf("scan successful! structure len : %u, weight len : %u, code size : %u found\n", slen, wlen, ilen);
	struct Code code;
	code.now_pos = 0;
	code.code_size = ilen;
	code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
	code.S = (uint8_t*)malloc(slen);
	code.S_len = slen;
	code.W = (uint8_t*)malloc(wlen);
	code.W_len = wlen;
	uint32_t s_itr = 0;
    uint32_t pos_itr = 0;
    uint32_t w_itr = 0;
	ifstream file;
	file.open(file_path,ios::in);
	if(!file.is_open()){
        printf("protocol file does not exist\n");
        return false;
    }
    std::string strLine;
    while(getline(file,strLine)){
    	if(strLine.empty())
            continue;
        if(strLine.find("//") != strLine.npos){
        	continue;
        }
        strLine = strLine.substr(strLine.find_first_not_of(" "));
        if(strLine.find("OUT") != strLine.npos){
        	char str[LABEL_LEN];
        	string tmp = index_pattern_of_string(strLine, 3);
        	copy(tmp.begin(), tmp.end(), str);
        	add_out((uint8_t*)str, code, pos_itr, s_itr);
        	continue;
        }else if(strLine.find("COV") != strLine.npos){
        	uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	uint32_t kernel_size = atoi(index_pattern_of_string(strLine, 6).c_str());
        	uint32_t kernel_w = idx_in_shape(index_pattern_of_string(strLine, 7), 1);
        	uint32_t kernel_l = idx_in_shape(index_pattern_of_string(strLine, 7), 2);
        	uint32_t kernel_h = idx_in_shape(index_pattern_of_string(strLine, 7), 3);
        	uint8_t stride = (uint8_t)atoi(index_pattern_of_string(strLine, 5).c_str());
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 9).c_str());
        	bool is_pending = (index_pattern_of_string(strLine, 4) == "pending");
        	char label_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 11), label_out);
			//add weight
        	string weight_path = index_pattern_of_string(strLine, 8);
    		weight_path = weight_path.substr(weight_path.find(":") + 1);
    		uint32_t weight_len = sizeof(float) * atoi(index_pattern_of_string(strLine, 6).c_str()) * size_of_shape(index_pattern_of_string(strLine, 7));
    		load_weight(weight_path, weight_len, code.W + w_itr);
    		//itrate structure
        	add_cov( w, l, h, (uint8_t*)label_in, (uint8_t*)label_out, kernel_size, kernel_w, kernel_l, kernel_h, is_pending, stride, ex_for_tee, w_itr, code, pos_itr, s_itr);
    		
    	}
    	else if(strLine.find("POOLING") != strLine.npos){
    		uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	uint32_t kernel_w = idx_in_shape(index_pattern_of_string(strLine, 7), 1);
        	uint32_t kernel_l = idx_in_shape(index_pattern_of_string(strLine, 7), 2);
        	uint8_t stride = (uint8_t)atoi(index_pattern_of_string(strLine, 6).c_str());
        	bool is_pending = (index_pattern_of_string(strLine, 5) == "pending");
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 8).c_str());
        	char label_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 10), label_out);
        	uint8_t pooling_type = index_pattern_of_string(strLine, 4) == "maxpooling"?max_pooling:avg_pooling;

    		add_pooling(w, l, h, (uint8_t*)label_in, (uint8_t*)label_out, pooling_type, kernel_w, kernel_l, is_pending, stride, ex_for_tee, code, pos_itr, s_itr);

    	}
    	else if(strLine.find("ReLU") != strLine.npos){
    		uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	char label_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 7), label_out);
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 5).c_str());
        	float alpha = atof(index_pattern_of_string(strLine, 4).c_str());
    		add_ReLU( w, l, h, (uint8_t*)label_in, (uint8_t*)label_out, alpha, ex_for_tee, code, pos_itr, s_itr);

    	}
    	else if(strLine.find("BN") != strLine.npos){
    		uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	char label_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 7), label_out);
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 5).c_str());

        	//add weight
			string weight_path = index_pattern_of_string(strLine, 4);
    		weight_path = weight_path.substr(weight_path.find(":") + 1);
    		load_weight(weight_path, 5*sizeof(float), code.W + w_itr);


    		add_BN( w, l, h, (uint8_t*)label_in, (uint8_t*)label_out, ex_for_tee, w_itr, code, pos_itr, s_itr);
    		
    	}
    	else if(strLine.find("SHORTCUT") != strLine.npos){
    		uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	uint32_t w2 = idx_in_shape(index_pattern_of_string(strLine, 5), 1);
        	uint32_t l2 = idx_in_shape(index_pattern_of_string(strLine, 5), 2);
        	uint32_t h2 = idx_in_shape(index_pattern_of_string(strLine, 5), 3);
        	char label_in[LABEL_LEN];
        	char label2_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 4), label2_in);
        	to_char(index_pattern_of_string(strLine, 8), label_out);
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 6).c_str());
    		add_short(w, l, h, (uint8_t*)label_in, w2, l2, h2, (uint8_t*)label2_in, (uint8_t*)label_out, ex_for_tee,code, pos_itr, s_itr);
    	}
    	else if(strLine.find("FC") != strLine.npos){
    		uint32_t w = idx_in_shape(index_pattern_of_string(strLine, 3), 1);
        	uint32_t l = idx_in_shape(index_pattern_of_string(strLine, 3), 2);
        	uint32_t h = idx_in_shape(index_pattern_of_string(strLine, 3), 3);
        	char label_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(index_pattern_of_string(strLine, 2), label_in);
        	to_char(index_pattern_of_string(strLine, 8), label_out);
        	uint32_t weight_width = atoi(index_pattern_of_string(strLine, 4).c_str());
        	uint8_t ex_for_tee = (uint8_t)atoi(index_pattern_of_string(strLine, 5).c_str());
        	//add weight
        	uint32_t weight_len = sizeof(float) *atoi(index_pattern_of_string(strLine, 4).c_str()) * size_of_shape(index_pattern_of_string(strLine, 3));
        	string weight_path = index_pattern_of_string(strLine, 6);
    		weight_path = weight_path.substr(weight_path.find(":") + 1);
    		load_weight(weight_path, weight_len, code.W + w_itr);

        	add_FC(w, l, h, (uint8_t*)label_in, (uint8_t*)label_out, weight_width, ex_for_tee, w_itr, code, pos_itr, s_itr);
        	/*FC lay2 [] 10 1 w:fc . lay*/
    	}

    }
    //file.close();
    uint8_t* out_stream = (uint8_t*)malloc(code.S_len + code.W_len + code.code_size*sizeof(uint32_t) + 4*sizeof(uint32_t));
	uint32_t out_stream_len;
	uint32_t mac_stream_len = 32;
	uint8_t* mac_stream = (uint8_t*)malloc(mac_stream_len);

	code_deserialize(code, out_stream, &out_stream_len);
	tru->sign_protocol(out_stream, out_stream_len, mac_stream, &mac_stream_len);
	save_model<uint8_t>(out_stream,out_stream_len,"./code_data.code");
	save_model<uint8_t>(mac_stream,mac_stream_len,"./code_mac.code");
	printf("... everything done\n");
	free(out_stream);
	free(code.W);
	free(code.pos_s);
	free(code.S);
	delete tru;

}
bool read_code(std::string file_path){
	vector<truple>cmd;
	map<string,int>dic_goto;
	ifstream file;
	file.open(file_path,ios::in);
	if(!file.is_open()){
        printf("protocol file does not exist\n");
        return false;
    }
    std::string strLine;
    int step = 0;
    while(getline(file,strLine)){
    	//cout<<strLine<<endl;
        if(strLine.empty())
            continue;
        strLine = strLine.substr(strLine.find_first_not_of(" "));
        if(strLine.find("IN") != strLine.npos){
        	continue;
        }
        if(strLine.find("OUT") != strLine.npos){
        	string mid = strLine.substr(strLine.find(">")+2,strLine.find(";") - strLine.find(">") - 3);
        	if(pro_test){
        		cout<<"out:"<<mid<<"|"<<endl;
        	}
        	cmd.push_back({false,"","",mid,"out"});
        	step ++;
        	continue;
        }
        // if goto
        if(strLine.find("if") != strLine.npos){
        	string mid = strLine.substr(strLine.find("if")+3,strLine.find("goto") - strLine.find("if") - 4);
        	
			string a = mid.substr(0, mid.find(" "));
			// mid = mid.substr(mid.find(" ")+1);
			// string op = mid.substr(0, mid.find(" "));
			// string b = mid.substr(mid.find(" ")+1);
			string target = strLine.substr(strLine.find("label"), strLine.find(";") - strLine.find("label"));
			if(pro_test){
        		cout<<"out:"<<target<<"|"<<a<<"|"<<endl;
        	}
			cmd.push_back({true,a,"",target,""});
			step ++;
			continue;
    	}
    	//just goto
    	if(strLine.find("goto") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(";")-strLine.find("label"));
			if(pro_test){
        		cout<<"out:"<<mid<<"|goto"<<endl;
        	}
    		cmd.push_back({true,"","",mid,"goto"});
        	step ++;
			continue;
    	}
    	// 
    	if(strLine.find("label") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(":"));
    		if(pro_test){
        		cout<<"out:"<<mid<<"|"<<endl;
        	}
    		dic_goto[mid] = step;
    		continue;
    	}
    	if(strLine.find(":=") != strLine.npos){
    		continue;
    	}
    	if(strLine.find("=") != strLine.npos){
    		if(strLine.find("MPC") != strLine.npos) continue;
    		if(strLine.find("newarray") != strLine.npos) continue;
    		string mid = strLine.substr(strLine.find_first_not_of("\t"),strLine.find(";") - strLine.find_first_not_of("\t"));
    		string target = mid.substr(0,mid.find(" "));
    		mid = mid.substr(mid.find("=")+2);
    		string a;
    		if(mid.find(" ") != mid.npos){
    			a = mid.substr(0,mid.find(" "));
    			mid = mid.substr(mid.find(" ") + 1);
    			string op = mid.substr(0,mid.find(" "));
    			string b = mid.substr(mid.find(" ")+1);
				if(pro_test){
        			cout<<"out:"<<target<<"|"<<a<<"|"<<op<<"|"<<b<<"|"<<endl;
        		}
    			cmd.push_back({false,a,b,target,op});
    			//if plain_var <- cipher_var, make plain varible become cipher varible
    		}
    		else{
    			a = mid.substr(0,mid.find(";"));
				if(pro_test){
        			cout<<"out:"<<target<<"|"<<a<<"|"<<endl;
        		}
    			cmd.push_back({false,a,"",target,""});
    		}
    		step ++;
    		continue;

    	}
    	
    }
    file.close();
    /*cmd . Code*/  
    Code code;
    code.now_pos = 0;
    code.code_size = cmd.size();
    code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
    code.S = (uint8_t*)malloc(INS_LEN*cmd.size());
    uint32_t s_itr = 0;
    uint32_t pos_itr = 0;
    for(int i = 0; i < cmd.size(); i++){
    	if(!cmd[i].is_goto){
    		char label_in[LABEL_LEN];
    		char label2_in[LABEL_LEN];
        	char label_out[LABEL_LEN];
        	to_char(cmd[i].operand1, label_in);
        	to_char(cmd[i].operand2, label2_in);
        	to_char(cmd[i].output, label_out);
    		add_ins(tran_op(cmd[i].op), (uint8_t*)label_in, (uint8_t*)label2_in, (uint8_t*)label_out, code, pos_itr, s_itr);
    	}else{
    		char label_out[LABEL_LEN];
    		to_char(cmd[i].output, label_out);
    		add_goto((uint8_t*)label_out, dic_goto[cmd[i].output], code, pos_itr, s_itr);
    	}
    }

}

int main(int argc, char* argv[]){
	string path;

	if(argc >= 2){
        path = argv[1];
    }
    else{
        printf("please input file name\n");
        exit(0);
    }
    string suffix = path.substr(path.find_last_of('.') + 1);
    if(suffix == "jimple"){
    	printf("start load %s\n", path);
    	read_code(path);
    }else if(suffix == "mlc"){
    	printf("start load %s\n", path);
    	read_model(path);
    }
}