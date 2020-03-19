/*
this file is just for testing code

*/

#include "tuple.h"
#include <time.h>
#include "crypto_pend.h"
int main(){
	//srand((unsigned)time(NULL));
	/*test 8*8*3 matric cov 3*3*3*/
	truthtee_pend tee;
	baseInt data[8*8*3];

	baseInt kernel_data[3*3*3];
	unsigned char structure[SIZE_COV];
	unsigned char W[27*2*sizeof(baseInt)];
	unsigned int W_len = 27*2*sizeof(baseInt);
	unsigned char data_of_img[8*8*3*sizeof(baseInt)];
	unsigned int data_of_img_len = 8*8*3*sizeof(baseInt);
	unsigned char data_of_img_en[2*8*8*3*sizeof(baseInt)];
	unsigned int data_of_img_en_len;
	unsigned char W_en[27*2*sizeof(baseInt)*2];
	unsigned int W_en_len;
	
	/*add cov*/
	int itr = 0;
	int layer = 0;
	structure[itr ++] = layer ++;
	structure[itr ++] = layer;
	structure[itr ++] = COV;
	structure[itr ++] = 2;
	structure[itr ++] = 1;
	structure[itr ++] = 1;
	structure[itr ++] = 3;
	structure[itr ++] = 3;
	structure[itr ++] = 3;
	
	
	itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 8; j++){
			for(int k = 0; k < 8; k++){
				data[itr ++] = 0.1*k;
			}
		}
	}
	memcpy(data_of_img,data,8*8*3*sizeof(baseInt));
	itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++){
				kernel_data[itr ++] = 0.5;
			}
		}
	}
	memcpy(W,kernel_data,3*3*3*sizeof(baseInt));
	memcpy(W+3*3*3*sizeof(baseInt),kernel_data,3*3*3*sizeof(baseInt));
	for(int i = 0; i < 27*2*sizeof(baseInt); i++){
		printf("%u ", W[i]);
	}
	tee.test_with_remote(W, W_len, W_en, W_en_len);
	tee.test_with_remote(data_of_img, data_of_img_len, data_of_img_en, data_of_img_en_len);
	tee.data_input(data_of_img_en, data_of_img_en_len, 8,8,3);
	baseInt arr[100];
	tee.block(W_en , W_en_len, structure, SIZE_COV,arr);

	/*std::vector<Tuple> tuples;
	Tuple kernel({3,3,3});
	kernel.unserialize(kernel_data);
	tuples.push_back(kernel);
	tuples.push_back(kernel);
	kernel.output_data();
	Image ans = tee.image.convolution(tuples, true, 1);
	Image ans_1 = ans.pooling({3,3,1},false,1,avg_pooling); 
	ans.output_data();
	ans_1.output_data();*/
	return 0;
}