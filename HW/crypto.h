#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdint.h>
#include "SG_API.h"
//Binary
#define AND_OP 1
#define NOT_OP 2
#define OR_OP 3
#define NAND_OP 4
#define SHF_LE 5
#define SHF_RI 6


//Arithmatic
#define ADD_OP 11
#define SUB_OP 12
#define MUL_OP 13
#define DIV_OP 14


#define GREAT_OP 21
#define GE_OP 22
#define LESS_OP 23
#define LE_OP 24
#define EQ_OP 25
//define transfer type

#define org_key 2
#define remote_key 3
#define DECRYPTO true
#define ENCRYPTO false

#define SWI_PLA 0
#define SWI_REM 1
#define SWI_ORG 2
#define cipher_len 16
#define sym_key_len 128
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
	void operation(unsigned char tru_in1[],unsigned int in1_len, int swi_1, unsigned char tru_in2[],unsigned int in2_len, int swi_2, unsigned char tru_out[],unsigned int &out_len, int op);
	int test_and_op(unsigned char tru_out[], int &out_len);
	void encrypto_key(unsigned char tru_key_out[],unsigned int &key_len_out);
	void encrypto(unsigned char tru_in[], unsigned int len, unsigned char tru_data_out[],unsigned int &data_len_out);
	void decrypto_key(unsigned char tru_key_in[],unsigned int key_in_len);
	void decrypto(unsigned char tru_data_in[],unsigned int data_in_len, unsigned char tru_out[],unsigned int &out_len);
	void ch_data(unsigned char tru_in[]);
	//sign operation
	void sign_key(unsigned char tru_out[]);
	//verify key using remote_key
	bool sign_verify(unsigned char tru_in[]);

	//from stream decode keys
	void stream_to_key(unsigned char tru_in[]);
private:
	void gen_sym_key();
	void transfer_data(unsigned char tru_in[],unsigned int in_len, unsigned char tru_out[],unsigned int &out_len, bool tr, int signal);
	void to_ll(unsigned char input[], uint64_t &output);
	void to_byte16(uint64_t org, unsigned char output[]);
	void serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]);
	void serialize_signature(ECCSIGNATUREBLOB sign, unsigned char tru_out[]);
	void query_signature(unsigned char tru_out[], bool verify);
	ECCSIGNATUREBLOB deserialize_signature(unsigned char tru_in[]);
	ECCPUBLICKEYBLOB deserialize(unsigned char tru_in[]);
	ECCPRIVATEKEYBLOB 	pri_key1;
	ECCPRIVATEKEYBLOB 	pri_key2;
	ECCSIGNATUREBLOB  	sig;
	ECCSIGNATUREBLOB	remote_sig;
	unsigned char sym_key_keep[sym_key_len/8];
	unsigned char sym_key_remote[sym_key_len/8];
	bool key_ex_suc;
	bool key_verify_suc;
	//middle variable
	unsigned char		dataA[0x100];
	int A_len;
	bool A_get;
	bool B_get;
	unsigned char		dataB[0x100];
	int B_len;
	unsigned char 		remote_a[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	unsigned char 		remote_b[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	unsigned char 		remote_serial[ECC_MAX_XCOORDINATE_BITS_LEN/2];
};