#ifndef _CRYPTO_PENDING_H
#define _CRYPTO_PENDING_H
#include "crypto.h"
#include "tuple.h"
#define COV 0 
#define POOLING 1 
#define RELU 2 
#define BN_ID 3 
#define FC_ID 4
#define SHORTCUT 5
#define SIZE_COV 11 
#define SIZE_POOLING 8 
#define SIZE_RELU (3 + sizeof(baseInt))
#define SIZE_BN 3
#define SIZE_FC (3 + sizeof(int))
#define SIZE_SHORTCUT 4
//SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
/*

This class is designed for ML.
Data need only be entered once, so use internal cache to store data
*/

class truthtee_pend: public truthtee{
public:
	truthtee_pend(){};
	~truthtee_pend(){};
	int test_with_remote(uint8_t input_data[], uint32_t input_len, uint8_t output_data[], uint32_t &output_len);
	int data_input(uint8_t data[], uint32_t data_len, int l, int w, int h);
	int block(uint8_t W[], uint32_t W_len, uint8_t structure[], uint32_t struct_len, baseInt output[]);
	int block(baseInt* w_data, uint32_t W_len, uint8_t structure[], uint32_t struct_len, baseInt output[]);

	Image data_image;
};

#endif