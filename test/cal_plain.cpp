#include "crypto_pend.h"
#include "tool.h"
int main(){
	uint8_t key[16];// = {113,26,66,150,170,194,214,179,160,144,202,72,108,74,136,63};
	load_data_frome_file(key, 16, "./key.data.b");
	uint8_t cipher[4000];
	load_data_frome_file(cipher, 4000, "./result.data");
	uint8_t plain[4000];
	uint32_t len;
	float result[1000];
	if(SG_SymDec(SGD_SMS4_ECB ,key, 16, key, 16, cipher, 4000, plain, &len) != SAR_OK){
		perror("decrypto data error\n");
	}
	store_data_to_file(plain, 4000, "./plaintext2.data");
	memcpy(result, plain, 4000);
	for(int i = 0; i < 100 ; i++){
		printf("%f ", result[i]);
	}
}