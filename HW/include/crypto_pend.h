#ifndef _CRYPTO_PENDING_H
#define _CRYPTO_PENDING_H
#include "truthtee.h"
class truthtee_pend: public truthtee{
public:
	int cov(unsigned char W[], unsigned char data[], unsigned char structure[], unsigned char output[]);
};

#endif