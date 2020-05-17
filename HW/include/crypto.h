#ifndef _TRUTHTEE_H_
#define _TRUTHTEE_H_
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <vector>
#include <string>
#include <openssl/sha.h>
#include <stdint.h>
extern "C" {
#include "tee_client_api.h"
}
#define UUID_HW_HEAD    { 0x13245768, 0xacbd, 0xcedf,   \
                            { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 } }

#define CMD_XOR     (0x01234567)
#define CMD_INIT            (0x000)
#define CMD_ENCRYPT         (0x001)
#define CMD_DECRYPT         (0x002)
#define CMD_KEY_GET         (0x003)
#define CMD_KEY_STORE       (0x004)
#define CMD_KEY_SIGN        (0x005)
#define CMD_KEY_VERIFY      (0x006)



#define CMD_TEST_MAC        (0x009)

#define SYM_KEY_SIZE    256
#define ASYM_KEY_SIZE   256
#define ASYM_KEY_FOR_SIGN_SIZE   512

#define PUBLIC_TYPE_SIGN 	(0x0000)
#define PUBLIC_TYPE_CIPHER 	(0x0001)  
#define PUBLIC_TYPE_SYM     (0x0003)     
// Auther: Tainpei Lu
// Creation: 11/02 2019 
// this page defines interface functions that simulate trusted hardware
/*
There are some definitions about function args 
*/

// output
#define END_CMD 100
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

#define LABEL_LEN 17
#define MAC_LEN 32
#define CIPHER_LEN 16
class truthtee{

public:
	truthtee();
	~truthtee(){};

	//public key for data encrytion(send to other party) 
	ECCPUBLICKEYBLOB 	pub_key1;
	//public key for key signature(send to other party) 
	ECCPUBLICKEYBLOB 	pub_key2;
	
	//remote public key(use this key to encrypt data sent)
	ECCPUBLICKEYBLOB 	remote_pub_key1;
	//remote public key2(use this key to sign both public keys used to encrypt the data)
	ECCPUBLICKEYBLOB 	remote_pub_key2;	

	//generate key stream, store in serial[]
	void query_pub_stream(uint8_t t_out[]);
	//sign operation
	void sign_key(uint8_t tru_out[]);
	//verify key using remote_key
	bool sign_verify(uint8_t tru_in[]);
	//from stream decode keys
	/*
	Since only binary stream available, need decode from stream
	*/
	void stream_to_key(uint8_t tru_in[]);
	//verify MAC with count and verify data integrity with MAC
	void operation(uint8_t label1[], uint32_t lab_len1, uint8_t tru_in1[],uint32_t in1_len, int swi_1, uint8_t mac1[], uint32_t mac1_len, uint8_t label2[], uint32_t lab_len2, uint8_t tru_in2[],uint32_t in2_len, int swi_2, uint8_t mac2[], uint32_t mac2_len, uint8_t out_label[], uint32_t outlabel_len, uint8_t tru_out[],uint32_t &out_len, uint8_t tru_mac_out[],uint32_t &mac_out_len, uint8_t mac_op[], uint32_t macop_len, int op);
	//just for test
	int test_and_op(uint8_t tru_out[], int &out_len);
	/*
	thinks to hybrid encryption
	encrypt symmetric key with encrypto_key(), encrypt data with encrypto()
	*/
	//Generate a symmetric key
	void gen_sym_key(uint8_t tru_out[], uint32_t key_len);
	void encrypto_key(uint8_t tru_key_out[],uint32_t &key_len_out);
	void encrypto(uint8_t tru_in[], uint32_t len, uint8_t tru_data_out[],uint32_t &data_len_out);
	//using AE instead Encryption
	void encrypt_MAC(uint8_t label[], uint32_t lab_len, uint8_t tru_in[], uint32_t len, uint8_t tru_data_out[],uint32_t &data_len_out, uint8_t tru_mac_out[], uint32_t &mac_len_out);
	//using AE check data and transform data
	bool verify_data(uint8_t label[], uint32_t lab_len, uint8_t tru_in[], uint32_t len, uint8_t mac_in[], uint32_t mac_len, uint8_t tru_data_out[], uint32_t &data_len_out, uint8_t tru_mac_out[], uint32_t &mac_len_out);
	//pair-wise, corresponding decryption function
	void decrypto_key(uint8_t tru_key_in[],uint32_t key_in_len);
	
	//check ans
	bool decrypto(uint8_t label[], uint32_t lab_len, uint8_t tru_data_in[],uint32_t data_in_len, uint8_t mac_in_data[], uint32_t mac_data_len, uint8_t mac_in_cmd[], uint32_t mac_cmd_len, uint8_t tru_out[],uint32_t &out_len);
	//just for test
	void test_ch_data(uint8_t tru_in[]);
	
	//caculate the protocol cmd MAC with counter
	void sign_cmd(uint8_t label1[], uint32_t lab_len1, uint8_t label2[], uint32_t lab_len2, uint32_t op, uint8_t tru_out[], uint32_t &data_len_out);
	//caculate the protocol cmd MAC
	void sign_cmd_without_counter(uint8_t label1[], uint32_t lab_len1, uint8_t label2[], uint32_t lab_len2, uint32_t op, uint8_t tru_out[], uint32_t &data_len_out);
	void sign_cmd_accu(uint8_t label1[], uint32_t lab_len1, uint8_t label2[], uint32_t lab_len2, uint32_t op, uint8_t tru_out[], uint32_t &data_len_out);
	//verify MAC for cmd counter
	/*
	Trusted hardware will matain a cmd counter.
	All the user can do is reset the counter or just enter commands
	When reset the counter, user can reinput commands.
	The interface of commands entry, see operation function.
	*/

	void reset_counter();

	
protected:	
	void transfer_data(uint8_t tru_in[],uint32_t in_len, uint8_t tru_out[],uint32_t &out_len, bool tr, int signal);
private:
	TEEC_Context     context;
    TEEC_Session     session;
    TEEC_Operation   operation;
	/*
	There are some temporary functions
	*/
	//This is command counter
	uint32_t cmd_counter;
	
	
	//This function do temporary decryption and encryption operations, in operation 
	
	/*
	When doing operations, the byte stream is first converted to 64 digits with to_ll().
	And after that, turn back to stream with to_byte16().
	*/
	void to_ll(uint8_t input[], uint64_t &output);
	void to_byte16(uint64_t org, uint8_t output[]);
	void to_ll(uint8_t input[], int64_t &output);
	void to_byte16(int64_t org, uint8_t output[]);
	//without verification
	void operation(uint8_t tru_in1[],uint32_t in1_len, int swi_1, uint8_t tru_in2[],uint32_t in2_len, int swi_2, uint8_t tru_out[],uint32_t &out_len, int op);
	//temporary function used to generate key stream
	void serialize(ECCPUBLICKEYBLOB pu_key, uint8_t tru_out[]);
	void serialize_signature(ECCSIGNATUREBLOB sign, uint8_t tru_out[]);
	//check if the mac is legal
	bool mac_verification(uint8_t text[], uint32_t text_len, uint8_t mac[], uint32_t mac_len, bool remote);
	//check instruction accumulative
	void check_cmd_accu(uint8_t label1[], uint32_t lab_len1, uint8_t label2[], uint32_t lab_len2, uint32_t op);
	void query_signature(uint8_t tru_out[], bool verify);
	/*
	Next function and three variables are designed for merkle tree, remove it with AE used.
	*/
	uint8_t now_mac[MAC_LEN];
	uint8_t remote_mac[MAC_LEN];
	std::string check_mac(std::string hash, std::vector<std::string>hash_table);
	std::string cmd_hash;
	std::string data_hash;
	ECCSIGNATUREBLOB deserialize_signature(uint8_t tru_in[]);
	ECCPUBLICKEYBLOB deserialize(uint8_t tru_in[]);
	ECCPRIVATEKEYBLOB 	pri_key1;
	ECCPRIVATEKEYBLOB 	pri_key2;
	ECCSIGNATUREBLOB  	sig;
	ECCSIGNATUREBLOB	remote_sig;
	uint8_t sym_key_keep[sym_key_len/8];
	uint8_t sym_key_remote[sym_key_len/8];
	bool key_ex_suc = false;
	bool key_verify_suc = false;
	//middle variable
	bool is_check_counter = false;
	uint8_t		dataA[0x100];
	int A_len;
	bool A_get;
	bool B_get;
	uint8_t		dataB[0x100];
	int B_len;
	//the trap is designed for selection of MAC-checked model
	bool is_accumulative = true;

	uint8_t 		remote_a[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	uint8_t 		remote_b[ECC_MAX_XCOORDINATE_BITS_LEN/4];
	uint8_t 		remote_serial[ECC_MAX_XCOORDINATE_BITS_LEN/2];
};

#endif