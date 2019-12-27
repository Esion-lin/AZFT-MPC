#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <vector>
#include <string>
#include <openssl/sha.h>
#include <stdint.h>
#include "SG_API.h"
// Auther: Tainpei Lu
// Creation: 11/02 2019 
// this page defines interface functions that simulate trusted hardware
/*
There are some definitions about function args 
*/
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

#define label_len 17
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
	void operation(unsigned char label1[], unsigned char tru_in1[],unsigned int in1_len, int swi_1, unsigned char mac1[], unsigned int mac1_len, unsigned char label2[], unsigned char tru_in2[],unsigned int in2_len, int swi_2, unsigned char mac2[], unsigned int mac2_len, unsigned char tru_out[],unsigned int &out_len, unsigned char mac_op[], unsigned int macop_len, int op);
	//just for test
	int test_and_op(unsigned char tru_out[], int &out_len);
	/*
	thinks to hybrid encryption
	encrypt symmetric key with encrypto_key(), encrypt data with encrypto()
	*/

	void encrypto_key(unsigned char tru_key_out[],unsigned int &key_len_out);
	void encrypto(unsigned char tru_in[], unsigned int len, unsigned char tru_data_out[],unsigned int &data_len_out);
	//using AE instead Encryption
	void encrypt_MAC(unsigned char label[], unsigned char tru_in[], unsigned int len, unsigned char tru_data_out[],unsigned int &data_len_out, unsigned char tru_mac_out[], unsigned int &mac_len_out);
	//using AE check data and transform data
	bool verify_data(unsigned char label[], unsigned char tru_in[], unsigned int len, unsigned char mac_in[], unsigned int mac_len, unsigned char tru_data_out[], unsigned int &data_len_out, unsigned char tru_mac_out[], unsigned int &mac_len_out);
	//pair-wise, corresponding decryption function
	void decrypto_key(unsigned char tru_key_in[],unsigned int key_in_len);
	void decrypto(unsigned char tru_data_in[],unsigned int data_in_len, unsigned char tru_out[],unsigned int &out_len);
	//just for test
	void test_ch_data(unsigned char tru_in[]);
	//sign operation
	void sign_key(unsigned char tru_out[]);
	//verify key using remote_key
	bool sign_verify(unsigned char tru_in[]);
	//caculate the protocol cmd MAC with counter
	void sign_cmd(unsigned char label1[], unsigned char label2[], unsigned int op, unsigned char tru_out[], unsigned int &data_len_out);
	//caculate the protocol cmd MAC
	void sign_cmd_without_counter(unsigned char label1[], unsigned char label2[], unsigned int op, unsigned char tru_out[], unsigned int &data_len_out);
	//verify MAC for cmd counter
	/*
	Trusted hardware will matain a cmd counter.
	All the user can do is reset the counter or just enter commands
	When reset the counter, user can reinput commands.
	The interface of commands entry, see operation function.
	*/

	void reset_counter();
	/*
	Since only binary stream available, need decode from stream
	*/
	//from stream decode keys
	void stream_to_key(unsigned char tru_in[]);
private:

	/*
	There are some temporary functions
	*/
	//This is command counter
	unsigned int cmd_counter;
	//Generate a symmetric key
	void gen_sym_key(unsigned char tru_out[], unsigned int key_len);
	
	//This function do temporary decryption and encryption operations, in operation 
	void transfer_data(unsigned char tru_in[],unsigned int in_len, unsigned char tru_out[],unsigned int &out_len, bool tr, int signal);
	/*
	When doing operations, the byte stream is first converted to 64 digits with to_ll().
	And after that, turn back to stream with to_byte16().
	*/
	void to_ll(unsigned char input[], uint64_t &output);
	void to_byte16(uint64_t org, unsigned char output[]);

	//temporary function used to generate key stream
	void serialize(ECCPUBLICKEYBLOB pu_key, unsigned char tru_out[]);
	void serialize_signature(ECCSIGNATUREBLOB sign, unsigned char tru_out[]);
	//check if the mac is legal
	bool mac_verification(unsigned char text[], unsigned int text_len, unsigned char mac[], unsigned int mac_len);
	
	void query_signature(unsigned char tru_out[], bool verify);
	/*
	Next function and three variables are designed for merkle tree, remove it with AE used.
	*/
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
	bool key_ex_suc = false;
	bool key_verify_suc = false;
	//middle variable
	bool is_check_counter = false;
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