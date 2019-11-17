#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "SG_API.h"

class truthtee{
public:
	truthtee();
	~truthtee();
	ECCPUBLICKEYBLOB 	pub_key1;
	ECCPUBLICKEYBLOB 	pub_key2;
	ECCPUBLICKEYBLOB 	remote_pub_key1;
	ECCPUBLICKEYBLOB 	remote_pub_key2;	

	

	//generate key stream, store in serial[]
	void query_pub_stream(unsigned char t_out[]);
	
	void and_op(unsigned char tru_in1[], unsigned char tru_in2[], unsigned char tru_out[], int swi);
	void or_op(unsigned char tru_in1[], unsigned char tru_in2[], unsigned char tru_out[], int swi);
	void not_op(unsigned char t_in[], unsigned char tru_out[], int swi);
	void nand_op(unsigned char tru_in1[], unsigned char tru_in2[], unsigned char tru_out[], int swi);
	int add_op(unsigned char tru_out[], int &out_len);
	int test_and_op(unsigned char tru_out[], int &out_len);
	void en_date(unsigned char tru_in[], int len, unsigned char tru_out[], int &len_out);
	void de_date(unsigned char tru_in[], int len);
	void ch_data(unsigned char tru_in[]);
	//sign operation
	void sign_key(unsigned char tru_out[]);
	//verify key using remote_key
	bool sign_verify(unsigned char tru_in[]);

	//from stream decode keys
	void stream_to_key(unsigned char tru_in[]);
private:
	void serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]);
	ECCPUBLICKEYBLOB deserialize(unsigned char tru_in[]);
	ECCPRIVATEKEYBLOB 	pri_key1;
	ECCPRIVATEKEYBLOB 	pri_key2;
	ECCSIGNATUREBLOB  	sig;
	bool key_ex_suc;
	bool key_verify_suc;
	//middle variable
	unsigned char		dataA[0x100];
	int A_len;
	bool A_get;
	bool B_get;
	unsigned char		dataB[0x100];
	int B_len;
	unsigned char 		serial[ECC_MAX_XCOORDINATE_BITS_LEN/2];
	unsigned char 		remote_a[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	unsigned char 		remote_b[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	unsigned char 		remote_serial[ECC_MAX_XCOORDINATE_BITS_LEN/2];
};