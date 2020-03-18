#ifndef _CRYPTO_PENDING_H
#define _CRYPTO_PENDING_H
#include "truthtee.h"
#include "tuple.h"
#define cov 0 
#define pooling 1 
#define ReLU 2 
#define BN 3 
#define FC 4
#define shortcut 5


/*

This class is designed for ML.
Data need only be entered once, so use internal cache to store data
*/

class truthtee_pend: public truthtee{
public:

	truthtee_pend():truthtee(){};
	int data_input(unsigned char data[], unsigned int data_len, int l, int w, int h);
	int block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, unsigned char output[]);
private:
	Image data_image;
};

#endif