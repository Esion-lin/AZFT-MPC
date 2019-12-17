#include "Merkle_Hash.h"
#include "HW/crypto.h"
using namespace std;
int op_form(std::string op){
    if(op == ">>>" || op == ">>"){
        return SHF_RI;
    }else if(op == "<<<" || op == "<<"){
        return SHF_LE;
    }else if(op == "&"){
        return AND_OP;
    }else if(op == "|"){
        return OR_OP;
    }else if(op == "+"){
        return ADD_OP;
    }else if(op == "-"){
        return SUB_OP;
    }else if(op == "*"){
        return MUL_OP;
    }else if(op == "/"){
        return DIV_OP;
    }else if(op == "xor"){
        return NAND_OP;
    }else if(op == ">"){
        return GREAT_OP;
    }else if(op == ">="){
        return GE_OP;
    }else if(op == "<"){
        return LESS_OP;
    }else if(op == ">="){
        return LE_OP;
    }else if(op == "=="){
        return EQ_OP;
    }
}
//overwrite sha256 function
void sha256(const string str, unsigned char output[])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(output, &sha256);
}
string sha256(const string str)
{
	char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    string str2 = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%x",hash[i]);
        str2 = str2 + buf;
    }
	return str2;
}
void sha256(unsigned char input[], int len, unsigned char output[])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, len);
    SHA256_Final(output, &sha256);
}
void merkleTree::output_hash(unsigned char output[]){
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
		output[i] = out_hash[i];
	}
}
string merkleTree::output_hash(){
	return out_str;
}
vector<string> merkleTree::quary_path(truple trp){
	vector<string> ans;
	string now_pos = sha256(trp.operand1 + to_string(op_form(trp.op)) + trp.operand2);
	ans.push_back(brother_map[now_pos]);
	while(father_map.count(now_pos) > 0){
		now_pos = father_map[now_pos];
		ans.push_back(brother_map[now_pos]);
	}
	return ans;
}
vector<string> merkleTree::quary_path(string label, unsigned char cipher[]){
	vector<string> ans;
	char buf[2];
	for(int i = 0; i < cipher_len; i++) {
		sprintf(buf,"%x",cipher[i]);
        label = label + buf;
	}
	string now_pos = sha256(label);
	ans.push_back(brother_map[now_pos]);
	while(father_map.count(now_pos) > 0){
		now_pos = father_map[now_pos];
		ans.push_back(brother_map[now_pos]);
	}
	return ans;
}
queue<string> merkleTree::init_hash(vector<truple> cmds){
	queue<string> ans;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	for(int i = 0; i < cmds.size(); i++){
		ans.push(sha256(cmds[i].operand1 + to_string(op_form(cmds[i].op)) + cmds[i].operand2));
	}
	return ans;
}
queue<string> merkleTree::init_hash(map<std::string, unsigned char[16]> datas){
	char buf[2];
	queue<string> ans;
	string mid;
	for( auto &v : datas ){
		mid = v.first;
		for(int i = 0; i < cipher_len; i++) {
			sprintf(buf,"%x",v.second[i]);
        	mid = mid + buf;
		}
		ans.push(sha256(mid));
	}
	return ans;
}
void merkleTree::make_brother(queue<string> tree){
	string left;
	string right;
	string top;
	while(tree.size() >= 2){
		left = tree.front();
		tree.pop();
		right = tree.front();
		tree.pop();
		top = sha256(left + right);
		if(tree.size() == 2){
			sha256(left + right, out_hash);
		}
		father_map[left] = top;
		father_map[right] = top;
		brother_map[left] = right;
		brother_map[right] = left;
		tree.push(top);
	}
	out_str = tree.front();

}
merkleTree::merkleTree(vector<truple> cmds){
	queue<string> mid = init_hash(cmds);
	make_brother(mid);
}
merkleTree::merkleTree(map<std::string, unsigned char[16]> datas){
	queue<string> mid= init_hash(datas);
	make_brother(mid);
}