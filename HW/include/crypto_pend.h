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
#define SIZE_RELU 3 + sizeof(baseInt) 
#define SIZE_BN 3
#define SIZE_FC 7
#define SIZE_SHORTCUT 4

/*

This class is designed for ML.
Data need only be entered once, so use internal cache to store data
*/

class truthtee_pend: public truthtee{
public:
	truthtee_pend(){};
	~truthtee_pend(){};
	int test_with_remote(unsigned char input_data[], unsigned int input_len, unsigned char output_data[], unsigned int &output_len);
	int data_input(unsigned char data[], unsigned int data_len, int l, int w, int h);
	int block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, baseInt output[]);
	int block(baseInt w_data[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, baseInt output[]);
	Image data_image;
};

#endif