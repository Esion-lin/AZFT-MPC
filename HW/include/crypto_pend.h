#ifndef _CRYPTO_PENDING_H
#define _CRYPTO_PENDING_H
#include "truthtee.h"
/*

This class is designed for ML.
Data need only be entered once, so use internal cache to store data
*/

class truthtee_pend: public truthtee{
public:
	int data_input(unsigned char data[], unsigned int data_len);
	int block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, unsigned char output[]);
private:
	unsigned char data_internal;
	unsigned int dtat_in_len;
};

#endif