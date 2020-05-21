#ifndef _TEE_H_
#define _TEE_H_


#include <stdio.h>
#include <stdlib.h>
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
#define CMD_ENCRYPT_MAC     (0x007)
#define CMD_DECRYPT_MAC     (0x008)
#define CMD_RUN             (0x010)
#define CMD_QUERY			(0x011)
#define CMD_TEST_MAC        (0x009)
                            
#define DIGEST_SIZE     256
#define SYM_KEY_SIZE    256
#define ASYM_KEY_SIZE   256
#define ASYM_KEY_EXPONENT_SIZE   24
#define ASYM_KEY_FOR_SIGN_SIZE   512

#define PUBLIC_TYPE_SIGN 	(0x0000)
#define PUBLIC_TYPE_CIPHER 	(0x0001)  
#define PUBLIC_TYPE_SYM     (0x0003)    
                            
static const TEEC_UUID _g_uuid_hw_head = UUID_HW_HEAD;
class TEE{
public:
	TEE();
	~TEE();
	TEEC_Result ret;
	/*key_for_cipher + key_for_sign*/
	uint32_t query_pub_stream(uint8_t t_out[]);
	uint32_t stream_to_key(uint8_t t_in[]);
	uint32_t sign_key(uint8_t t_out[]);
	uint32_t sign_verify(uint8_t t_in[]);
	uint32_t encrypt_with_MAC(uint8_t* label, uint32_t lab_len, uint8_t* tru_in, uint32_t len, uint8_t* tru_data_out,uint32_t *data_len_out, uint8_t* tru_mac_out, uint32_t *mac_len_out);
	uint32_t input_data(uint8_t* label, uint32_t lab_len, uint8_t* tru_in, uint32_t len, uint8_t* tru_mac_in, uint32_t mac_len_in);
	/*next function is designed for local store protocol*/
	uint32_t sign_ins(uint8_t* protocol, uint32_t protocol_len, uint8_t* tru_mac_out, uint32_t &mac_len_out);
	/*run protocol*/
	uint32_t run_op(uint8_t* protocol, uint32_t protocol_len, uint8_t* mac, uint32_t mac_len);
	uint32_t query_data(uint8_t* label, uint32_t lab_len, uint8_t* tru_out, uint32_t* out_len);
private:
	TEEC_Context     context;
    TEEC_Session     session;
    TEEC_Operation   operation;
};


#endif