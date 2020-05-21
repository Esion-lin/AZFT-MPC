#include <string.h>
#include "component.h"
#include "model.h"
#include <vector>
#include <map>
using namespace std;
void add_ins(uint8_t op, uint8_t* label1, uint8_t* label2, uint8_t* label3, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code->pos_s + pos_itr, &s_itr, POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = op;
	memcpy(code->S + s_itr, label1, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = FLOAT_TP;
	memcpy(code->S + s_itr, label2, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = FLOAT_TP;
	memcpy(code->S + s_itr, label3, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = FLOAT_TP;
}
void add_goto(uint8_t* label, uint32_t tar, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code->pos_s + pos_itr, &s_itr, POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = GOTO;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code->S + s_itr, &tar, sizeof(uint32_t));
	s_itr += sizeof(uint32_t);
}
void add_cov(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint32_t kernel_size, uint32_t kernel_w, uint32_t kernel_l, uint32_t kernel_h, bool is_pending, uint8_t stride, uint8_t ex_for_tee, uint32_t &W_pos, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = COV;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(kernel_size/256);
	code->S[s_itr++] = (uint8_t)(kernel_size%256);
	code->S[s_itr++] = (uint8_t)is_pending;
	code->S[s_itr++] = stride;
	code->S[s_itr++] = (uint8_t)(kernel_w/256);
	code->S[s_itr++] = (uint8_t)(kernel_w%256);
	code->S[s_itr++] = (uint8_t)(kernel_l/256);
	code->S[s_itr++] = (uint8_t)(kernel_l%256);
	code->S[s_itr++] = (uint8_t)(kernel_h/256);
	code->S[s_itr++] = (uint8_t)(kernel_h%256);
	code->S[s_itr++] = ex_for_tee;
	memcpy(Structure + itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += kernel_w*kernel_l*kernel_h*kernel_size*sizeof(float);
}
void add_pooling(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint8_t pooling_type, uint32_t kernel_w, uint32_t kernel_l, bool is_pending, uint8_t stride, uint8_t ex_for_tee, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = POOLING;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = pooling_type;
	code->S[s_itr++] = (uint8_t)is_pending;
	code->S[s_itr++] = stride;
	code->S[s_itr++] = (uint8_t)(kernel_w/256);
	code->S[s_itr++] = (uint8_t)(kernel_w%256);
	code->S[s_itr++] = (uint8_t)(kernel_l/256);
	code->S[s_itr++] = (uint8_t)(kernel_l%256);
	code->S[s_itr++] = ex_for_tee;
}
void add_ReLU(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, float alpha, uint8_t ex_for_tee, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = RELU;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code->S + s_itr, &alpha, sizeof(float));
	s_itr += sizeof(float);
	code->S[s_itr++] = ex_for_tee;
}
void add_BN(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint8_t ex_for_tee, uint32_t &W_pos, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = BN_ID;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = ex_for_tee;
	memcpy(Structure + itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += 5*sizeof(float);
}
void add_FC(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in, uint8_t* label_out, uint32_t weight_width, uint8_t ex_for_tee, uint32_t &W_pos, uint8_t &code, uint32_t &pos_itr, uint32_t &s_itr){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = FC_ID;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	memcpy(code->S + s_itr, &weight_width, sizeof(uint32_t));
	s_itr += sizeof(uint32_t);
	code->S[s_itr++] = ex_for_tee;
	memcpy(Structure + itr, &W_pos, POS_LEN);
	s_itr += POS_LEN;
	W_pos += w*l*h*weight_width*sizeof(float);
}
void add_short(uint32_t w, uint32_t l, uint32_t h, uint8_t* label_in,uint32_t w2, uint32_t l2, uint32_t h2, uint8_t* label2_in,uint8_t* label_out,uint8_t ex_for_tee){
	memcpy(code.pos_s + pos_itr, &s_itr,POS_LEN);
	pos_itr += POS_LEN;
	code->S[s_itr++] = SHORTCUT;
	memcpy(code->S + s_itr, label_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w/256);
	code->S[s_itr++] = (uint8_t)(w%256);
	code->S[s_itr++] = (uint8_t)(l/256);
	code->S[s_itr++] = (uint8_t)(l%256);
	code->S[s_itr++] = (uint8_t)(h/256);
	code->S[s_itr++] = (uint8_t)(h%256);
	memcpy(code->S + s_itr, label2_in, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = (uint8_t)(w2/256);
	code->S[s_itr++] = (uint8_t)(w2%256);
	code->S[s_itr++] = (uint8_t)(l2/256);
	code->S[s_itr++] = (uint8_t)(l2%256);
	code->S[s_itr++] = (uint8_t)(h2/256);
	code->S[s_itr++] = (uint8_t)(h2%256);
	memcpy(code->S + s_itr, label_out, LABEL_LEN);
	s_itr += LABEL_LEN;
	code->S[s_itr++] = ex_for_tee;
}

bool read_code(std::string file_path){
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
        	output.push_back(mid);
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
        		cout<<"out:"<<target<<"|"<<a"|"<<endl;
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
    /*cnd -> Code*/  
    Code code;
    code.code_size = cmd.size();
    code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
    code.S = (uint8_t*)malloc(INS_LEN*cmd.size());
    uint32_t s_itr = 0;
    uint32_t pos_itr = 0;
    for(int i = 0; i < cmd.size(); i++){
    	if(!cmd[i].is_goto){
    		add_ins(tran_op(cmd[i].op), &cmd[i].operand1, &cmd[i].operand2, &cmd[i].output, code, pos_itr, s_itr);
    	}else{
    		add_goto(cmd[i].operand1, dic_goto[cmd[i].output], code, pos_itr, s_itr);
    	}
    }

}

int main(){}