#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <vector>
#include <string>
#include <openssl/sha.h>
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

	//public key for data encrytion(send to other party) 
	ECCPUBLICKEYBLOB 	pub_key1;
	//public key for key signature(send to other party) 
	ECCPUBLICKEYBLOB 	pub_key2;
	
	//remote public key(use this key to encrypt data sent)
	ECCPUBLICKEYBLOB 	remote_pub_key1;
	//remote public key2(use this key to sign both public keys used to encrypt the data)
	ECCPUBLICKEYBLOB 	remote_pub_key2;	

	//generate key stream, store in serial[]
	void query_pub_stream(unsigned char t_out[]);
	//without verification(will moved to private function)
	void operation(unsigned char tru_in1[],unsigned int in1_len, int swi_1, unsigned char tru_in2[],unsigned int in2_len, int swi_2, unsigned char tru_out[],unsigned int &out_len, int op);
	//use merkle tree to verify commend integrity
	void operation(std::string label1, unsigned char tru_in1[],unsigned int in1_len, int swi_1, std::vector<std::string>path1, std::string label2, unsigned char tru_in2[],unsigned int in2_len, int swi_2, std::vector<std::string>path2, unsigned char tru_out[],unsigned int &out_len, int op, std::vector<std::string>path_protocol);
	//verify MAC with count and verify data integrity with MAC
	void operation(std::string label1, unsigned char tru_in1[],unsigned int in1_len, int swi_1, std::string label2, unsigned char tru_in2[],unsigned int in2_len, int swi_2, unsigned char tru_out[],unsigned int &out_len, int op)
	//just for test
	int test_and_op(unsigned char tru_out[], int &out_len);
	/*
	thinks to hybrid encryption
	encrypt symmetric key with encrypto_key(), encrypt data with encrypto()
	*/
	void encrypto_key(unsigned char tru_key_out[],unsigned int &key_len_out);
	void encrypto(unsigned char tru_in[], unsigned int len, unsigned char tru_data_out[],unsigned int &data_len_out);
	//pair-wise, corresponding decryption function
	void decrypto_key(unsigned char tru_key_in[],unsigned int key_in_len);
	void decrypto(unsigned char tru_data_in[],unsigned int data_in_len, unsigned char tru_out[],unsigned int &out_len);
	//just for test
	void test_ch_data(unsigned char tru_in[]);
	//sign operation
	void sign_key(unsigned char tru_out[]);
	//verify key using remote_key
	bool sign_verify(unsigned char tru_in[]);
	/*
	Since only binary stream available, need decode from stream
	*/
	//from stream decode keys
	void stream_to_key(unsigned char tru_in[]);
private:

	/*
	there are some temporary functions
	*/
	void gen_sym_key();
	void transfer_data(unsigned char tru_in[],unsigned int in_len, unsigned char tru_out[],unsigned int &out_len, bool tr, int signal);
	void to_ll(unsigned char input[], uint64_t &output);
	void to_byte16(uint64_t org, unsigned char output[]);
	void serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]);
	void serialize_signature(ECCSIGNATUREBLOB sign, unsigned char tru_out[]);
	void query_signature(unsigned char tru_out[], bool verify);
	std::string check_mac(std::string hash, std::vector<std::string>hash_table);
	std::string cmd_hash;
	std::string data_hash;
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