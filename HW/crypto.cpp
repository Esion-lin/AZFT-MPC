#include "crypto.h"
#include <iostream>
#include <algorithm>
bool test = false;
//just for test
void test_pub(ECCPUBLICKEYBLOB pub_key){
	printf("BitLen: %d\n",pub_key.BitLen);
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i++){
		std::cout<<pub_key.XCoordinate[i]<<" "<<pub_key.YCoordinate[i]<<" ";
	}
	std::cout<<std::endl;

}
//just for test
void test_pri(ECCPRIVATEKEYBLOB pri_key){
	printf("BitLen: %d\n",pri_key.BitLen);
	for(int i = 0; i < ECC_MAX_MODULUS_BITS_LEN/8; i++){
		std::cout<<pri_key.PrivateKey[i]<<" ";
	}
	std::cout<<std::endl;

}
truthtee::truthtee(){
	int ret = SG_GenKeyPair(SGD_SM2,&pub_key1,&pri_key1);
	if (ret!=SAR_OK)
	{
		printf("init error\n");
	}
	ret = SG_GenKeyPair(SGD_SM2,&pub_key2,&pri_key2);
	if (ret!=SAR_OK)
	{
		printf("init error\n");
	}
}
void truthtee::serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]){
	int ptr = 0;
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/8; i ++ ){
		tru_out[ptr++] = pu_key.XCoordinate[i];
		tru_out[ptr++] = pu_key.YCoordinate[i];
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
	for(int i = 0; i < ECC_MAX_XCOORDINATE_BITS_LEN/2; i ++){
		serial[i] = tru_out[i];
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
void truthtee::en_date(unsigned char tru_in[], int len, unsigned char tru_out[], int &len_out){
	unsigned char c[0x100];
	unsigned int tmplen;
	memcpy(dataA,tru_in,len);
	A_len = len;
	A_get = true;
	if(SG_SM2Enc(&remote_pub_key1,tru_in,len,c,&tmplen)==SAR_OK){
		len_out = tmplen;
		memcpy(tru_out,c,tmplen);
	}else{
		perror("encrypto error\n");
	}
}
void truthtee::de_date(unsigned char tru_in[], int len){
	unsigned char c[0x100];
	unsigned int tmplen;
	
	if(SG_SM2Dec(&pri_key1,tru_in,len,c,&tmplen)==SAR_OK){
		memcpy(dataB,c,tmplen);
		B_len = tmplen;
		B_get = true;
	}else{
		perror("decrypto error\n");
	}
}
int truthtee::add_op(unsigned char tru_out[], int &out_len){
	if(!A_get){
		return 0;
	}
	if(!B_get){
		return -1;
	}
	int len = std::min(A_len,B_len);
	out_len = len;
	for(int i = 0; i < len; i++){
		tru_out[i] = dataA[i] + dataB[i];
	}
	return 1;

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
//sign operation
void truthtee::sign_key(unsigned char tru_out[]){

}
	//verify key using remote_key
bool truthtee::sign_verify(unsigned char tru_in[]){
	return true;
}
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