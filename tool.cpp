// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "tool.h"

int tran_op(std::string op){
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
    }else if(op == "out"){
        return END_CMD;
    }else{
        return 0;
    }
}
int tran_op(int a, int b, std::string op){
    int ans;
    if(op == ">>>" || op == ">>"){
        ans = a >> b;
    }else if(op == "<<<" || op == "<<"){
        ans = a << b;
    }else if(op == "&"){
        ans = a & b;
    }else if(op == "|"){
        ans = a | b;
    }else if(op == "+"){
        ans = a + b;
    }else if(op == "-"){
        ans = a - b;
    }else if(op == "*"){
        ans = a * b;
    }else if(op == "/"){
        ans = a / b;
    }else if(op == "xor"){
        ans = a ^ b;
    }else if(op == ">"){
        ans = a > b;
    }else if(op == ">="){
        ans = a >= b;
    }else if(op == "<"){
        ans = a < b;
    }else if(op == ">="){
        ans = a >= b;
    }else if(op == "=="){
        ans = a == b;
    }
    return ans;
}
//check if the operand is number 
bool is_num(std::string data , int &num){
    if((data[0]>='0' && data[0]<='9' )||data[0]=='-' ){
        num = atoi(data.c_str());
        return true;
    }else{
        return false;
    }
}
/*
//This function is designed for conversion of array index items
    Because array index needs to be guaranteed to be plaintext, search from plaintext dictionary(org_dir)
*/
std::string get_item(std::map<std::string,int> dic, std::string str){
    if(str.find("[") == str.npos){
        return str;
    }else{
        std::string arr = str.substr(0,str.find("["));
        std::string num = str.substr(str.find("[")+1, str.find("]") - str.find("[")-1);
        //std::cout<<"num is "<<num<<"|"<<std::endl;
        int now_num;char ans[10];
        if(num[0]>='0' && num[0]<='9'){
            arr = arr + "_" + num;
        }else if(dic.find(num) != dic.end()){
            sprintf(ans, "%d", dic[num]);
            arr = arr + "_" + ans;
        }else{
            printf("index error(%s):array index is cipher text.\n", num.c_str());
        }
        return arr;

    }
}
void to_byte16(uint64_t org, unsigned char output[]){
    for(int i = 0; i < 8; i++){
        output[i] = 0;
        output[i+8] = org>>((7-i)*8);
    }
}
void to_byte16(uint64_t org1, uint64_t org2, unsigned char output[]){
    for(int i = 0; i < 8; i++){
        output[i] = org1>>((7-i)*8);
        output[i+8] = org2>>((7-i)*8);
    }
}
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, unsigned char output[]){
    for(int i = 0; i < 4; i++){
        output[i] = org1>>((3-i)*8);
        output[i+4] = org2>>((3-i)*8);
        output[i+8] = org3>>((3-i)*8);
        output[i+12] = org4>>((3-i)*8);
    }
}

//make sure the length of array is the multiple of 4
void to_byte(uint32_t org[], unsigned int len, unsigned char output[], unsigned int &out_len){
    for(int i = 0; i < len; i++){
        for(int j = 0; j < 4; j++){
            output[i*4 + j] = org[i]>>((3-j)*8);    
        }
    }
}
void to_ll(unsigned char input[], uint64_t &output){
    for(int i = 0; i < 8; i++){
        output *= 256;
        output += input[i+8];
    }
}
void to_ll(unsigned char input[], uint64_t &output1, uint64_t &output2){
    for(int i = 0; i < 8; i++){
        output1 *= 256;
        output1 += input[i];
        output2 *= 256;
        output2 += input[i+8];
    }
}
void to_int(unsigned char input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4){
    for(int i = 0; i < 4; i++){
        output1 *= 256;
        output1 += input[i];
        output2 *= 256;
        output2 += input[i+4];
        output3 *= 256;
        output3 += input[i+8];
        output4 *= 256;
        output4 += input[i+12];

    }
}
void to_int(unsigned char input[], unsigned int arr_len, uint32_t arr[]){
    for(int i = 0; i < arr_len/4; i++){
        arr[i] = 0;
        for(int j = 0; j < 4; j++){
            arr[i] *= 256;
            arr[i] += input[j + i*4];
        }
    }
}