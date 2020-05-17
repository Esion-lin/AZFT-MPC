// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "tool.h"
#include "keccak.h"
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
        return END_OP;
    }else{
        return 0;
    }
}
int64_t tran_op(int64_t a, int64_t b, std::string op){
    int64_t ans;
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
    //printf("%ld %s %ld = %ld", a,op.c_str(),b,ans);
    return ans;
}
//check if the operand is number 
bool is_num(std::string data , int64_t &num){
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
std::string get_item(std::map<std::string,int64_t> dic, std::string str){
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
            sprintf(ans, "%ld", dic[num]);
            arr = arr + "_" + ans;
        }else{
            printf("index error(%s):array index is cipher text.\n", num.c_str());
        }
        return arr;

    }
}
void to_byte16(uint64_t org, uint8_t output[]){
    for(int i = 0; i < 8; i++){
        output[i] = 0;
        output[i+8] = org>>((7-i)*8);
    }
}
void to_byte16(int64_t org, uint8_t output[]){
    for(int i = 0; i < 8; i++){
        output[i] = 0;
        output[i+8] = org>>((7-i)*8);
    }
}
void to_byte16(uint64_t org1, uint64_t org2, uint8_t output[]){
    for(int i = 0; i < 8; i++){
        output[i] = org1>>((7-i)*8);
        output[i+8] = org2>>((7-i)*8);
    }
}
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, uint8_t output[]){
    for(int i = 0; i < 4; i++){
        output[i] = org1>>((3-i)*8);
        output[i+4] = org2>>((3-i)*8);
        output[i+8] = org3>>((3-i)*8);
        output[i+12] = org4>>((3-i)*8);
    }
}

//make sure the length of array is the multiple of 4
void to_byte(uint32_t org[], uint32_t len, uint8_t output[], uint32_t &out_len){
    for(int i = 0; i < len; i++){
        for(int j = 0; j < 4; j++){
            output[i*4 + j] = org[i]>>((3-j)*8);    
        }
    }
}
void to_ll(uint8_t input[], uint64_t &output){
    for(int i = 0; i < 8; i++){
        output *= 256;
        output += input[i+8];
    }
}
void to_ll(uint8_t input[], int64_t &output){
    for(int i = 0; i < 8; i++){
        output *= 256;
        output += input[i+8];
    }
}
void to_ll(uint8_t input[], uint64_t &output1, uint64_t &output2){
    for(int i = 0; i < 8; i++){
        output1 *= 256;
        output1 += input[i];
        output2 *= 256;
        output2 += input[i+8];
    }
}
void to_int(uint8_t input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4){
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
void to_int(uint8_t input[], uint32_t arr_len, uint32_t arr[]){
    for(int i = 0; i < arr_len/4; i++){
        arr[i] = 0;
        for(int j = 0; j < 4; j++){
            arr[i] *= 256;
            arr[i] += input[j + i*4];
        }
    }
}
std::vector<std::string> getFiles(std::string path){
    std::vector<std::string> files;//存放文件名
 
#ifdef WIN32
    _finddata_t file;
    long lf;
    if ((lf=_findfirst(path.c_str(), &file)) == -1) {
        cout<<path<<" not found!!!"<<endl;
    } else {
        while(_findnext(lf, &file) == 0) {
            if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
                continue;
            files.push_back(file.name);
        }
    }
    _findclose(lf);
#endif
 
#ifdef linux
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
 
    if ((dir=opendir(path.c_str())) == NULL)
        {
        perror("Open dir error...");
                exit(1);
        }
 
    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
        else if(ptr->d_type == 8)    ///file
            files.push_back(ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            continue;
        else if(ptr->d_type == 4)    ///dir
        {
            files.push_back(ptr->d_name);
        }
    }
    closedir(dir);
#endif
    sort(files.begin(), files.end());
    return files;
}
/*void sha3(uint8_t *digest, const uint8_t *message, size_t message_len){
    uint32_t SHALEN=32;
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha3_256(), NULL);
    EVP_DigestUpdate(mdctx, message, message_len);
    EVP_DigestFinal_ex(mdctx, digest, &SHALEN);
    EVP_MD_CTX_destroy(mdctx);
}*/
std::string sha3_k(uint8_t *message, int message_len){
    Keccak digestKeccak(Keccak::Keccak256);
    digestKeccak.add(message, message_len);
    return digestKeccak.getHash();
}
void store_data_to_file(uint8_t * data, int data_len, std::string filename){
    FILE * pFile;
    if((pFile = fopen (filename.c_str(), "wb"))==NULL){
        printf("cant open the file\n");
        exit(0);
    }
    fwrite (data , 1, data_len, pFile);
    fclose (pFile);
};
void load_data_frome_file(uint8_t * output, int data_len, std::string filename){
    FILE * pFile;
    if((pFile = fopen (filename.c_str(), "r"))==NULL){
        printf("cant open the file\n");
        exit(0);
    }
    fread(output, 1, data_len, pFile);
    fclose (pFile);
};
void store_file_with_string(std::string data, std::string filename){
    std::ofstream file(filename);
    file<<data;
    file.close();
    
}


