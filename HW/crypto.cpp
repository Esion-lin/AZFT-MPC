#include "crypto.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
bool test = false;
//test, print public key content
void test_pub(ECCPUBLICKEYBLOB pub_key){
	printf("BitLen: %d\n",pub_key.BitLen);
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i++){
		std::cout<<pub_key.XCoordinate[i]<<" "<<pub_key.YCoordinate[i]<<" ";
	}
	std::cout<<std::endl;

}
//test, print private key content
void test_pri(ECCPRIVATEKEYBLOB pri_key){
	printf("BitLen: %d\n",pri_key.BitLen);
	for(int i = 0; i < ECC_MAX_MODULUS_BITS_LEN/8; i++){
		std::cout<<pri_key.PrivateKey[i]<<" ";
	}
	std::cout<<std::endl;

}
//test, print signature content
void test_sign(ECCSIGNATUREBLOB sign){
	printf("sign messsage is :\n");
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i++){
		std::cout<<sign.r[i]<<" "<<sign.s[i];
	}
	std::cout<<std::endl;

}
/*
when use Merkle tree to verify MAC, remain SHA, remove otherwise. 
*/
void sha256(const std::string str, unsigned char output[])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(output, &sha256);
}
std::string sha256(const std::string str)
{
	char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string str2 = "";
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

truthtee::truthtee(){
	int ret = SG_GenKeyPair(SGD_SM2,&pub_key1,&pri_key1);
	if (ret!=SAR_OK)
	{
		printf("init error\n");
	}
	usleep(200);
	//sleep 200 ms for a new random seed
	ret = SG_GenKeyPair(SGD_SM2,&pub_key2,&pri_key2);
	if (ret!=SAR_OK)
	{
		printf("init error\n");
	}
	gen_sym_key();
}
void truthtee::gen_sym_key(){
	srand((unsigned)time(NULL));
	for(int i = 0; i < sym_key_len/8; i++){
		sym_key_keep[i] = rand()%256;
	}
}
void truthtee::serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]){
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i ++ ){
		tru_out[ptr++] = pu_key.XCoordinate[i];
		tru_out[ptr++] = pu_key.YCoordinate[i];
	}

}
void truthtee::serialize_signature(ECCSIGNATUREBLOB sign, unsigned char tru_out[]){
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i ++ ){
		tru_out[ptr++] = sign.r[i];
		tru_out[ptr++] = sign.s[i];
	}

}
ECCPUBLICKEYBLOB truthtee::deserialize(unsigned char tru_in[]){
	ECCPUBLICKEYBLOB pub_key;
	pub_key.BitLen = 256;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i ++ ){
		pub_key.XCoordinate[i] = tru_in[i*2];
		pub_key.YCoordinate[i] = tru_in[i*2+1];
	}
	return pub_key;
}
ECCSIGNATUREBLOB truthtee::deserialize_signature(unsigned char tru_in[]){
	ECCSIGNATUREBLOB sign;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i ++ ){
		sign.r[i] = tru_in[i*2];
		sign.s[i] = tru_in[i*2+1];
	}
	return sign;
}
void truthtee::query_pub_stream(unsigned char tru_out[]){
	unsigned char 		a[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	unsigned char 		b[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	serialize(pub_key1,a);
	serialize(pub_key2,b);
	if(test){
				
	}
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/4; i ++){
		tru_out[ptr++] = a[i];
		tru_out[ptr++] = b[i];
	}
	

}
void truthtee::query_signature(unsigned char tru_out[], bool verify){
	unsigned char 		a[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	serialize(pub_key1,a);
	if(test){
				
	}
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/4; i ++){
		if(!verify){
			tru_out[ptr++] = a[i];
			tru_out[ptr++] = remote_a[i];
		}else{
			tru_out[ptr++] = remote_a[i];
			tru_out[ptr++] = a[i];
		}
		
	}
}
void truthtee::stream_to_key(unsigned char tru_in[]){
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/4; i ++){
		remote_a[i] = tru_in[ptr++];
		remote_b[i] = tru_in[ptr++];
	}
	remote_pub_key1 = deserialize(remote_a);
	remote_pub_key2 = deserialize(remote_b);
	key_ex_suc = true;
	if(test){
		test_pub(remote_pub_key1);
		test_pub(remote_pub_key2);	
	}
	
}
void truthtee::encrypto_key(unsigned char tru_key_out[],unsigned int &key_len_out){
	if(SG_SM2Enc(&remote_pub_key1,sym_key_keep,sym_key_len/8,tru_key_out,&key_len_out) != SAR_OK){
		perror("encrypto key error\n");
	}
}
void truthtee::encrypto(unsigned char tru_in[],unsigned int len, unsigned char tru_data_out[],unsigned int &data_len_out){

	
	if(SG_SymEnc(SGD_SMS4_ECB ,sym_key_keep,sym_key_len/8,sym_key_keep,sym_key_len/8,tru_in,len,tru_data_out,&data_len_out) != SAR_OK){
		perror("encrypto data error\n");
	}

}
void truthtee::decrypto_key(unsigned char tru_key_in[],unsigned int key_in_len){
	unsigned int key_check = 0;
	if(SG_SM2Dec(&pri_key1,tru_key_in,key_in_len,sym_key_remote,&key_check) != SAR_OK){
		perror("decrypto key error\n");
	}
	if(key_check != sym_key_len/8){
		perror("decrypto key error\n");
	}
}
void truthtee::decrypto(unsigned char tru_data_in[],unsigned int data_in_len, unsigned char tru_out[],unsigned int &out_len){

	
	if(SG_SymDec(SGD_SMS4_ECB ,sym_key_keep,sym_key_len/8,sym_key_keep,sym_key_len/8,tru_data_in,data_in_len,tru_out,&out_len) != SAR_OK){
		perror("decrypto data error\n");
	}

}
void truthtee::transfer_data(unsigned char tru_in[],unsigned int in_len, unsigned char tru_out[],unsigned int &out_len, bool tr, int signal){
	if(tr){
		if(signal == org_key){
			if(SG_SymDec(SGD_SMS4_ECB ,sym_key_keep,sym_key_len/8,sym_key_keep,sym_key_len/8,tru_in,in_len,tru_out,&out_len) != SAR_OK){
				perror("decrypto data error\n");
			}
		}else if(signal == remote_key){
			if(SG_SymDec(SGD_SMS4_ECB ,sym_key_remote,sym_key_len/8,sym_key_remote,sym_key_len/8,tru_in,in_len,tru_out,&out_len) != SAR_OK){
				perror("decrypto data error\n");
			}
		}
		
		
	}else{
		if(SG_SymEnc(SGD_SMS4_ECB ,sym_key_keep,sym_key_len/8,sym_key_keep,sym_key_len/8,tru_in,in_len,tru_out,&out_len) != SAR_OK){
			perror("decrypto data error\n");
		}

	}
}
int truthtee::test_and_op(unsigned char tru_out[], int &out_len){
	if(!A_get){
		return 0;
	}
	if(!B_get){
		return -1;
	}
	int len = std::min(A_len,B_len);
	out_len = len;
	for(int i = 0; i < len; i++){
		tru_out[i] = dataA[i] ^ dataB[i];
	}
	return 1;

}
void truthtee::to_ll(unsigned char input[], uint64_t &output){
    for(int i = 0; i < 8; i++){
        output *= 256;
        output += input[i+8];
    }
}
void truthtee::to_byte16(uint64_t org, unsigned char output[]){
    for(int i = 0; i < 8; i++){
        output[i] = 0;
        output[i+8] = org>>((7-i)*8);
    }
}
//sign operation
void truthtee::sign_key(unsigned char tru_out[]){
	//use sign key encrypt public key
	unsigned char allover_public_key[ECC_MAX_XCOORDINATE_BITS_LEN/2];
	query_signature(allover_public_key, false);
	if(SG_SM2Sign(1,&pub_key2,&pri_key2,NULL,0,allover_public_key,ECC_MAX_XCOORDINATE_BITS_LEN/2,&sig) != SAR_OK){
		perror("signature data error\n");
	}
	serialize_signature(sig,tru_out);

}
bool truthtee::sign_verify(unsigned char tru_in[]){
	remote_sig = deserialize_signature(tru_in);
	if(test){
		test_sign(remote_sig);
	}
	unsigned char allover_public_key[ECC_MAX_XCOORDINATE_BITS_LEN/2];
	query_signature(allover_public_key, false);
	if(SG_SM2Verify(1,&remote_pub_key2,NULL,0,allover_public_key,ECC_MAX_XCOORDINATE_BITS_LEN/2,&remote_sig) != SAR_OK){
		perror("verify signature error\n");
	}else{
		key_verify_suc = true;
		return true;
	}
	return false;

}
std::string truthtee::check_mac(std::string hash, std::vector<std::string>hash_table){
	/*
	recursively caculate hash
	*/
	std::string ans_hash = hash;
 	for(int i = 0; i < hash_table.size(); i ++){
 		ans_hash = sha256(ans_hash + hash_table[i]);
	}
}
void truthtee::operation(std::string label1, unsigned char tru_in1[],unsigned int in1_len, int swi_1, std::vector<std::string>path1, std::string label2, unsigned char tru_in2[],unsigned int in2_len, int swi_2, std::vector<std::string>path2, unsigned char tru_out[],unsigned int &out_len, int op, std::vector<std::string>path_protocol){
	/*
	check MAC:
		1. check label and data
		2. check cmd
	*/
	std::string hash1 = sha256(label1 + std::to_string(op) + label2);
	if(check_mac(hash1, path_protocol) != cmd_hash){
		printf("Illegal cmd\n");
		return;
	}
	char buf[2];
	for(int i = 0; i < cipher_len; i++) {
		sprintf(buf,"%x",tru_in1[i]);
        label1 = label1 + buf;
	}
	std::string hash2 = sha256(label1);
	if(check_mac(hash2, path1) != data_hash){
		printf("Illegal cmd\n");
		return;
	}
	for(int i = 0; i < cipher_len; i++) {
		sprintf(buf,"%x",tru_in2[i]);
        label2 = label2 + buf;
	}
	hash2 = sha256(label1);
	if(check_mac(hash2, path2) != data_hash){
		printf("Illegal cmd\n");
		return;
	}
	//if MAC checked, run operation
	operation(tru_in1, in1_len, swi_1, tru_in2, in2_len, swi_2, tru_out,out_len, op);

}
void truthtee::operation(unsigned char tru_in1[],unsigned int in1_len, int swi_1, unsigned char tru_in2[],unsigned int in2_len, int swi_2, unsigned char tru_out[],unsigned int &out_len, int op){
	unsigned char d1[0x100];
	unsigned int d1_len;
	unsigned char d2[0x100];
	unsigned int d2_len;
	unsigned char ans[0x100];
	unsigned int ans_len;
	if(swi_1 == SWI_ORG && in1_len != 0){
		transfer_data(tru_in1, in1_len, d1, d1_len, DECRYPTO, org_key);
	}else if(swi_1 == SWI_REM && in1_len != 0){
		transfer_data(tru_in1, in1_len, d1, d1_len, DECRYPTO, remote_key);
	}else if(swi_1 == SWI_PLA){
		memcpy(d1, tru_in1, 16);;
	}else{
		printf("Illegal swi \n");
	}
	if(swi_2 == SWI_ORG && in2_len != 0){
		transfer_data(tru_in2, in2_len, d2, d2_len, DECRYPTO, org_key);
	}else if(swi_2 == SWI_REM && in2_len != 0){

		transfer_data(tru_in2, in2_len, d2, d2_len, DECRYPTO, remote_key);
	}else if(swi_2 == SWI_PLA){
		memcpy(d2, tru_in2, 16);
	}else{
		printf("Illegal swi \n");
	}
	//turn to Int
	uint64_t ud1 = 0;
	uint64_t ud2 = 0;
	uint64_t uan = 0;
	if(in1_len != 0){
		to_ll(d1,ud1);
	}
	if(in2_len != 0){
		to_ll(d2,ud2);
	}
	
	switch(op){
		case AND_OP:
			uan = ud1 & ud2;
			break;
		case OR_OP:
			uan = ud1 | ud2;
			break;
		case NAND_OP:
			uan = ud1 ^ ud2;
			break;
		case ADD_OP:
			uan = ud1 + ud2;
			break;
		case SUB_OP:
			uan = ud1 - ud2;
			break;
		case MUL_OP:
			uan = ud1 * ud2;
			break;
		case DIV_OP:
			uan = ud1 / ud2;
			break;
		case SHF_LE:
			uan = ud1 << ud2;
			break;
		case SHF_RI:
			uan = ud1 >> ud2;
			break;
		case NOT_OP:
			uan = ~ud1;
			break;
		case GREAT_OP:
			uan = ud1 > ud2;
			break;
		case GE_OP:
			uan = ud1 >= ud2;
			break;
		case LESS_OP:
			uan = ud1 < ud2;
			break;
		case LE_OP:
			uan = ud1 <= ud2;
			break;
		case EQ_OP:
			uan = ud1 == ud2;
			break;
	}
	if(test)
		std::cout<<ud1<<" "<<op<<" "<<ud2<<" "<<uan<<std::endl;
	to_byte16(uan,ans);
	/*
	//Do not turn to Int
	int len = std::min(d1_len,d2_len);
	unsigned int carry = 0; 
	for(int i = len-1; i >= 0; i--){
		if(op == NOT_OP){
			if(in1_len != 0){
				ans[i] = ~d1[i];
			}else{
				ans[i] = ~d2[i];
			}
			continue;
		}
		switch(op){
			case AND_OP:
				ans[i] = d1[i] & d2[i];
				break;
			case OR_OP:
				ans[i] = d1[i] | d2[i];
				break;
			case NAND_OP:
				ans[i] = d1[i] ^ d2[i];
				break;
			case ADD_OP:
				ans[i] = (d1[i] + d2[i] + carry)%256;
				carry = (d1[i] + d2[i] + carry)/256;
				break;
			case SUB_OP:
				if(d1[i] + carry < d2[i]){
					ans[i] = d1[i] + carry + 256 - d2[i];
					carry = -1;
				}else{
					ans[i] = d1[i] - d2[i];
					carry = 0;
				}
				break;
			case MUL_OP:
				ans[i] = d1[i] * d2[i];
				break;
			case DIV_OP:
				ans[i] = d1[i] / d2[i];
				break;
			
		}
	}
	*/
	transfer_data(ans, 16, tru_out, out_len, ENCRYPTO, org_key);

}
	//verify key using remote_key


//for test
/*int main(){
	truthtee * tru = new truthtee();
	test_pri(tru->pri_key1);
	test_pri(tru->pri_key2);
	test_pub(tru->pub_key1);
	test_pub(tru->pub_key2);
	std::cout<<tru->query_pub();
	tru->stream_to_key(tru->serial);
	test_pub(tru->remote_pub_key1);
	test_pub(tru->remote_pub_key2);

}*/