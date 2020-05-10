#include "tee.h"

#define ERR_LOG(_fn, _res)  \
        printf("ERROR - failed to call %s, 0x%08x\n", (_fn), (_res))

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)
void init_hw(TEEC_Operation *operation){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE);
}
void get_public_key(TEEC_Operation *operation, uint8_t *out_data, uint32_t *out_data_len, uint32_t type){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_VALUE_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].value.a = type;
    (*operation).params[1].tmpref.buffer = out_data;
    (*operation).params[1].tmpref.size = *out_data_len;
}
void set_public_key(TEEC_Operation *operation, uint8_t *data, uint32_t data_len, uint32_t type){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_VALUE_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].value.a = type;
    (*operation).params[1].tmpref.buffer = data;
    (*operation).params[1].tmpref.size = data_len;
}
void get_sign(TEEC_Operation *operation, uint8_t *out_data, uint32_t *out_data_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].tmpref.buffer = out_data;
    (*operation).params[0].tmpref.size = *out_data_len;
}
void check_sign(TEEC_Operation *operation, uint8_t *data, uint32_t data_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_VALUE_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].tmpref.buffer = data;
    (*operation).params[0].tmpref.size = data_len;
}
TEE::TEE(){
	ret = TEEC_SUCCESS;
	ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS) {
        printf("ERR failed to initial context\n");
        return;
    }
    ret = TEEC_OpenSession(&context, &session, &_g_uuid_hw_head, TEEC_LOGIN_PUBLIC,
                NULL, NULL, NULL);
    if (ret != TEEC_SUCCESS) {
        printf("open ss fail(0x%08x)\n", ret);
        goto cleanup1;
    }
    init_hw(&operation);
    ret = TEEC_InvokeCommand(&session, CMD_INIT, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("generate key error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup2;
    }
    return;
cleanup2:
    TEEC_CloseSession(&session);
cleanup1:
    TEEC_FinalizeContext(&context);


}
TEE::~TEE(){
	TEEC_CloseSession(&session);
	TEEC_FinalizeContext(&context);
}
uint32_t TEE::query_pub_stream(uint8_t out[]){
	uint32_t out_len = (uint32_t)(ASYM_KEY_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8);
 	uint8_t *out_data_buf = (uint8_t *)malloc(out_len);
	get_public_key(&operation, out_data_buf, &out_len, PUBLIC_TYPE_CIPHER);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_GET, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_public_key. PUBLIC_TYPE_CIPHER inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    memcpy(out, out_data_buf, out_len);
    free(out_data_buf);
    uint32_t out_len_sign = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8);
    out_data_buf = (uint8_t *)malloc(out_len_sign);
    get_public_key(&operation, out_data_buf, &out_len_sign, PUBLIC_TYPE_SIGN);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_GET, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_public_key. PUBLIC_TYPE_SIGN inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    memcpy(out + out_len, out_data_buf, out_len_sign);
    free(out_data_buf);
    out_data_buf = NULL;
    return ret;
}
uint32_t TEE::stream_to_key(uint8_t t_in[]){
	uint32_t out_len = (uint32_t)(ASYM_KEY_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8);
 	uint8_t *out_data_buf = (uint8_t *)malloc(out_len);
 	memcpy(out_data_buf, t_in, out_len);
 	set_public_key(&operation, out_data_buf, out_len, PUBLIC_TYPE_CIPHER);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_STORE, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("set_public_key. PUBLIC_TYPE_CIPHER inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    free(out_data_buf);
    uint32_t out_len_sign = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8);
    out_data_buf = (uint8_t *)malloc(out_len_sign);
    memcpy(out_data_buf, t_in + out_len, out_len_sign);
    set_public_key(&operation, out_data_buf, out_len, PUBLIC_TYPE_SIGN);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_STORE, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("set_public_key. PUBLIC_TYPE_SIGN inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    free(out_data_buf);
    out_data_buf = NULL;
    return ret;
}
uint32_t TEE::sign_key(uint8_t t_out[]){
	uint32_t out_len = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8);
    uint8_t *out_data_buf = (uint8_t *)malloc(out_len);
    get_sign(&operation, out_data_buf, &out_len);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_SIGN, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_sign. inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    memcpy(t_out, out_data_buf, out_len);
    free(out_data_buf);
    out_data_buf = NULL;
    return ret;
}
uint32_t TEE::sign_verify(uint8_t t_in[]){
	uint32_t out_len = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8);
    uint8_t *out_data_buf = (uint8_t *)malloc(out_len);
    check_sign(&operation, out_data_buf, out_len);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_VERIFY, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("check_sign. inv cmd failed(0x%08x)\n", ret);
        free(out_data_buf);
        return ret;
    }
    free(out_data_buf);
    return ret;
}
uint32_t TEE::encrypt_with_MAC(uint8_t* label, uint32_t lab_len, uint8_t* tru_in, uint32_t len, uint8_t* tru_data_out,uint32_t &data_len_out, uint8_t* tru_mac_out, uint32_t &mac_len_out){

}
uint32_t TEE::input_data(uint8_t* label, uint32_t lab_len, uint8_t* tru_in, uint32_t len, uint8_t* tru_data_in,uint32_t &data_len_in, uint8_t* tru_mac_in, uint32_t &mac_len_in){

}