/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_internal_api.h"
#include "tee_internal_api_ext.h"
#include "tee_srv.h"
#include <string.h>
#include <stdlib.h>
#include <element.h>
#include <tuple_c.h>
#define UUID_XOR    { 0x13245768, 0xacbd, 0xcedf,   \
                        { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 } }

#define CMD_XOR     (0x01234567)
#define CMD_INIT            (0x000)
#define CMD_ENCRYPT         (0x001)
#define CMD_DECRYPT         (0x002)
#define CMD_KEY_GET         (0x003)
#define CMD_KEY_STORE       (0x004)
#define CMD_TEST_MAC        (0x009)
#define CMD_KEY_SIGN        (0x005)
#define CMD_KEY_VERIFY      (0x006)
#define CMD_ENCRYPT_MAC     (0x007)
#define CMD_DECRYPT_MAC     (0x008)


#define MAX_LABEL_SIZE  1024
#define MAX_DATA_SIZE   1024*1024
#define SYM_KEY_SIZE    256
#define ASYM_KEY_SIZE   256
#define DIGEST_SIZE     256
#define ASYM_KEY_FOR_SIGN_SIZE   512

#define MAC_KEY_STYLE       TEE_TYPE_HMAC_SHA256
#define CIPHER_KEY_STYLE    TEE_TYPE_AES
#define PUBLIC_KEY_STYLE    TEE_TYPE_RSA_KEYPAIR

#define PUBLIC_TYPE_SIGN    (0x0000)
#define PUBLIC_TYPE_CIPHER  (0x0001)
#define PUBLIC_TYPE_SYM     (0x0003)
#define TA_DBG(format, a...)        \
    do {                            \
        TEE_Print(format, ##a);     \
    } while(0)

static TEE_UUID xor_uuid = UUID_XOR;

static TEE_Result _TA_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _TA_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _TA_OpenSessionEntryPoint(
        uint32_t paramTypes, TEE_Param params[4], void **sessionContext)
{
    return TEE_SUCCESS;
}

static void _TA_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}
/*store data*/
static TEE_Result sst_write_data(char* name, uint8_t name_len, 
                                 uint8_t* data, uint32_t data_len) {
    TEE_Result ret = TEE_SUCCESS;
    TEE_ObjectHandle ptObject = TEE_HANDLE_NULL;
    TA_DBG("sst_write_data: call TEE_CreatePersistentObject name: %s len: %d\n", name, name_len);
    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                     TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE,
                                     NULL, NULL, 0, &ptObject);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sst_write_data: TEE_CreatePersistentObject err: 0x%x\n", ret);
        goto cleanup1;
    }
    TA_DBG("start writing\n");
    ret = TEE_WriteObjectData(ptObject, data, data_len);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sst_write_data: TEE_WriteObjectData err: 0x%x\n",ret);
    }
    TA_DBG("finish writing\n");
    TEE_CloseObject(ptObject);
cleanup1:
    return ret;
}
/*load data*/
static TEE_Result sst_read_data(char* name, uint8_t name_len,
                                uint8_t* data, uint32_t* data_len) {
    TEE_Result ret = 0;
    TEE_ObjectHandle ptObject = TEE_HANDLE_NULL;
    uint32_t read_sz = 0;

    TA_DBG("sst_read_data: call TEE_OpenPersistentObject name: %s len: %d\n", name, name_len);

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &ptObject);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sst_read_data: TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup1;
    }

    ret = TEE_ReadObjectData(ptObject, data, *data_len, &read_sz);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sst_read_data: TEE_ReadObjectData err: 0x%x\n", ret);
        goto cleanup;
    }

    *data_len = read_sz;

cleanup:
    TEE_CloseObject(ptObject);

cleanup1:
    return ret;

}
/*generate key pair

*/
static TEE_Result persistent_transient(TEE_ObjectHandle object, char* name, uint8_t name_len){
    TEE_Result ret = 0;
    TEE_ObjectHandle objectPersistence = TEE_HANDLE_NULL;
    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                     TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE,
                                     object, NULL, 0, &objectPersistence);
    if (TEE_SUCCESS != ret) {
        TA_DBG("persistent_transient: TEE_CreatePersistentObject err: 0x%x\n", ret);
    }
    TEE_CloseObject(objectPersistence);
    return ret;
}
static TEE_Result gen_public_key(char* name, uint8_t name_len, uint32_t key_len){
    TEE_ObjectHandle keyObject = TEE_HANDLE_NULL;
    TEE_ObjectHandle keyObjectPersistence = TEE_HANDLE_NULL;
    TEE_Result ret = 0;
    ret = TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR,key_len,&keyObject);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_public_key: TEE_AllocateTransientObject err: 0x%x\n", ret);
        return ret;
    }
    ret = TEE_GenerateKey(keyObject, key_len, NULL, 0);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_public_key: TEE_GenerateKey err: 0x%x\n", ret);
        goto cleanup;
    }
    //return ref of public key data
    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                     TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE,
                                     keyObject, NULL, 0, &keyObjectPersistence);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_public_key: TEE_CreatePersistentObject err: 0x%x\n", ret);
        goto cleanup1;
    }
cleanup1:
    TEE_CloseObject(keyObjectPersistence);
cleanup:
    TEE_CloseObject(keyObject);
    return ret;
}

/*generate key for data
    name: org_key
*/
static TEE_Result gen_sym_key(char* name, uint8_t name_len, uint32_t type){
    TEE_ObjectHandle keyObject = TEE_HANDLE_NULL;
    TEE_ObjectHandle key_Object = TEE_HANDLE_NULL;
    TEE_Result ret = 0;
    ret = TEE_AllocateTransientObject(type,SYM_KEY_SIZE,&keyObject);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_sym_key: TEE_AllocateTransientObject err: 0x%x\n", ret);
    }
    ret = TEE_GenerateKey(keyObject, SYM_KEY_SIZE, NULL, 0);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_sym_key: TEE_GenerateKey err: 0x%x\n", ret);
        goto cleanup;
    }
    

    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                     TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE,
                                     keyObject, NULL, 0, &key_Object);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_sym_key: TEE_CreatePersistentObject err: 0x%x\n", ret);
        goto cleanup1;
    }
    
    /*
    uint32_t objectType;
    uint32_t keySize;
    uint32_t maxKeySize;
    uint32_t objectUsage;
    uint32_t dataSize;
    uint32_t dataPosition;
    uint32_t handleFlags;
    */
    
cleanup1:
    TEE_CloseObject(key_Object);
cleanup:
    TEE_CloseObject(keyObject);
    return ret;
}

/*generate MAC code*/
static TEE_Result gen_mac(uint8_t* data, uint32_t data_len,  char* name, uint8_t name_len, uint8_t * data_out, uint32_t * data_out_len, bool check){
    TEE_Result ret = 0;
    TEE_OperationHandle macOp = TEE_HANDLE_NULL;
    TEE_ObjectHandle key = TEE_HANDLE_NULL;
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_mac: Key not found. TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup;
    }
    ret = TEE_AllocateOperation(&macOp, TEE_ALG_HMAC_SHA256, TEE_MODE_MAC, SYM_KEY_SIZE);
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_mac: allocate op error. TEE_AllocateOperation err: 0x%x\n", ret);
        goto cleanup;
    }
    TA_DBG("gen_mac: start TEE_SetOperationKey\n");
    ret = TEE_SetOperationKey(macOp, key);
    TA_DBG("gen_mac: start TEE_MACComputeFinal\n");
    TEE_MACInit(macOp, NULL, 0);
    if(check){
        ret = TEE_MACCompareFinal(macOp, data, data_len, data_out, *data_out_len);
    }else{
        ret = TEE_MACComputeFinal(macOp, data, data_len, data_out, data_out_len);   
    }
    if (TEE_SUCCESS != ret) {
        TA_DBG("gen_mac: caculate MAC error. TEE_SetOperationKey err: 0x%x\n", ret);
        goto cleanup;
    }
cleanup:
    TEE_CloseObject(key);
    return ret;
}
/*store data key*/
static TEE_Result serialize_key(char* name, uint8_t name_len, uint8_t *data, uint32_t data_len,uint32_t key_len, bool public, uint32_t type){
    //from data to initiate attribute
    TEE_Result ret = 0;
    TEE_ObjectHandle keyObject = TEE_HANDLE_NULL;
    TEE_Attribute crypto_att[2];
    /* judge if it is asymmetric*/
    if(public){
        TEE_InitRefAttribute(&crypto_att[0], TEE_ATTR_RSA_MODULUS, data, (uint32_t) (key_len / 8));
        TEE_InitRefAttribute(&crypto_att[1], TEE_ATTR_RSA_PUBLIC_EXPONENT, data + (uint32_t) (key_len / 8), 3);
        ret = TEE_AllocateTransientObject(TEE_TYPE_RSA_PUBLIC_KEY,key_len,&keyObject);
        if (TEE_SUCCESS != ret) {
            TA_DBG("store_public_key: TEE_AllocateTransientObject err: 0x%x\n", ret);
            goto cleanup;
        }
        ret = TEE_PopulateTransientObject(keyObject, crypto_att, 2);
        if (TEE_SUCCESS != ret) {
            TA_DBG("store_public_key: TEE_PopulateTransientObject err: 0x%x\n", ret);
            goto cleanup;
        }
    }else{
        TEE_InitRefAttribute(&crypto_att[0], TEE_ATTR_SECRET_VALUE, data, data_len);
        ret = TEE_AllocateTransientObject(type,key_len,&keyObject);
        if (TEE_SUCCESS != ret) {
            TA_DBG("store_sym_key: TEE_AllocateTransientObject err: 0x%x\n", ret);
            goto cleanup;
        }
        ret = TEE_PopulateTransientObject(keyObject, crypto_att, 1);
        if (TEE_SUCCESS != ret) {
            TA_DBG("store_sym_key: TEE_PopulateTransientObject err: 0x%x\n", ret);
            goto cleanup;
        }
    }
    
    persistent_transient(keyObject, name, name_len);
cleanup:
    TEE_CloseObject(keyObject);
    return ret;
}
static TEE_Result gen_mac_for_protocol_key(){
    uint8_t key[32] = {32,43,15,6,98,123,45,52,99,10,65,99,244,98,0,85,219,187,176,153,63,188,3,2,25,6,227,128,98,87,131,32};
    return serialize_key("mac_for_protocol_key", 20, key, 32, SYM_KEY_SIZE,false, MAC_KEY_STYLE);
}
static TEE_Result deserialize_key(char* name, uint8_t name_len, uint8_t *out_data, uint32_t *out_data_len, bool public){
    TEE_ObjectHandle key = TEE_HANDLE_NULL;
    TEE_Result ret;
    uint8_t ans[1000]={0};
    uint32_t ans_size = 1000;
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("deserialize_key: Key not found. TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup;
    }
    if(public){
        TA_DBG("deserialize_key: start public key extract\n");
        ret = TEE_GetObjectBufferAttribute(key, TEE_ATTR_RSA_MODULUS, ans, &ans_size);
        if (TEE_SUCCESS != ret) {
            TA_DBG("deserialize_key: TEE_GetObjectBufferAttribute err: 0x%x\n", ret);
            goto cleanup;
        }
        memcpy(out_data, ans, ans_size);
        *out_data_len = ans_size;
        ret = TEE_GetObjectBufferAttribute(key, TEE_ATTR_RSA_PUBLIC_EXPONENT, ans, &ans_size);
        if (TEE_SUCCESS != ret) {
            TA_DBG("deserialize_key: TEE_GetObjectBufferAttribute err: 0x%x\n", ret);
            goto cleanup;
        }
        memcpy(out_data + *out_data_len, ans, ans_size);
        *out_data_len += ans_size;
    }else{
        ret = TEE_GetObjectBufferAttribute(key, TEE_ATTR_SECRET_VALUE, ans, &ans_size);
        if (TEE_SUCCESS != ret) {
            TA_DBG("deserialize_key: TEE_GetObjectBufferAttribute err: 0x%x\n", ret);
            goto cleanup;
        }
        memcpy(out_data, ans, ans_size);
        *out_data_len = ans_size;
    }
cleanup:
    TEE_CloseObject(key);
    return ret;
}
static TEE_Result create_digest( uint8_t *msg, uint32_t msg_len, uint8_t* digest, uint32_t *digest_len){
    TEE_ObjectHandle digObject = TEE_HANDLE_NULL;
    TEE_Result ret = 0;
    TEE_OperationHandle digOp = TEE_HANDLE_NULL;
    ret = TEE_AllocateOperation(&digOp, TEE_ALG_SHA256, TEE_MODE_DIGEST, 0);
    if (TEE_SUCCESS != ret) {
        TA_DBG("create_digest: allocate op error. TEE_AllocateOperation err: 0x%x\n", ret);
        goto cleanup;
    }
    ret = TEE_DigestDoFinal(digOp, msg, msg_len, digest, digest_len);
    if (TEE_SUCCESS != ret) {
        TA_DBG("create_digest: create digest error. TEE_DigestDoFinal err: 0x%x\n", ret);
        goto cleanup;
    }
cleanup:
    TEE_CloseObject(digObject);
    return ret;
}
static TEE_Result transf_data(uint8_t* data, uint32_t data_len, char* name, uint8_t name_len, uint8_t* out_data, uint32_t* out_data_len, bool encrypt){
    //TODO: use name to open persistent key, then encrypt the data
    TEE_Result ret = 0;
    TEE_OperationHandle trOp = TEE_HANDLE_NULL;
    //get key
    TEE_ObjectHandle key = TEE_HANDLE_NULL;
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("transf_data: Key not found. TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup;
    }
    //start encryption
    if(encrypt){
        ret = TEE_AllocateOperation(&trOp, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_ENCRYPT, SYM_KEY_SIZE);
    }else{
        ret = TEE_AllocateOperation(&trOp, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_DECRYPT, SYM_KEY_SIZE);
    }
    if (TEE_SUCCESS != ret) {
        TA_DBG("transf_data: allocate op error. TEE_AllocateOperation err: 0x%x\n", ret);
        goto cleanup;
    }
    ret = TEE_SetOperationKey(trOp, key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("transf_data: set key error. TEE_SetOperationKey err: 0x%x\n", ret);
        goto cleanup;
    }
    TEE_CipherInit(trOp,NULL,0);
    ret = TEE_CipherDoFinal(trOp, data, data_len, out_data, out_data_len);
    if (TEE_SUCCESS != ret) {
        TA_DBG("transf_data: transfer data error. TEE_CipherDoFinal err: 0x%x\n", ret);
        goto cleanup;
    }
cleanup:
    TEE_CloseObject(key);
    return ret;
}
/*encrypt txt*/
static TEE_Result encrypt_data(uint8_t* data, uint32_t data_len, char* name, uint8_t name_len, uint8_t* out_data, uint32_t* out_data_len, bool encrypt){
    TEE_Result ret = 0;
    TEE_OperationHandle enOp = TEE_HANDLE_NULL;
    TEE_ObjectHandle key = TEE_HANDLE_NULL;
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("encrypt_data: Key not found. TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup;
    }
    if(encrypt){
        ret = TEE_AllocateOperation(&enOp, TEE_ALG_RSA_NOPAD, TEE_MODE_ENCRYPT, ASYM_KEY_SIZE);
    }else{
        ret = TEE_AllocateOperation(&enOp, TEE_ALG_RSA_NOPAD, TEE_MODE_DECRYPT, ASYM_KEY_SIZE);
    }
    if (TEE_SUCCESS != ret) {
        TA_DBG("encrypt_data: allocate op error. TEE_AllocateOperation err: 0x%x\n", ret);
        goto cleanup;
    }
    ret = TEE_SetOperationKey(enOp, key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("encrypt_data: set key error. TEE_SetOperationKey err: 0x%x\n", ret);
        goto cleanup;
    }
    
    if(encrypt){
        if((ret = TEE_AsymmetricEncrypt(enOp,NULL, 0, data, data_len,out_data, out_data_len)) != TEE_SUCCESS){
            TA_DBG("encrypt_data: encrypt data error. TEE_AsymmetricEncrypt err: 0x%x\n", ret);
            goto cleanup;
        }
    }else{
        if((ret = TEE_AsymmetricDecrypt(enOp,NULL, 0, data, data_len,out_data, out_data_len)) != TEE_SUCCESS){
            TA_DBG("encrypt_data: decrypt data error. TEE_AsymmetricEncrypt err: 0x%x\n", ret);
            goto cleanup;
        }
    }
cleanup:
    TEE_CloseObject(key);
    return ret;
}
/*sign*/
static TEE_Result sign_data(uint8_t* digest, uint32_t digest_len, char* name, uint8_t name_len, uint8_t* signature, uint32_t* signature_len, bool sign){
    TEE_Result ret = 0;
    TEE_OperationHandle enOp = TEE_HANDLE_NULL;
    TEE_ObjectHandle key = TEE_HANDLE_NULL;
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
                                   TEE_DATA_FLAG_ACCESS_READ |
                                   TEE_DATA_FLAG_ACCESS_WRITE |
                                   TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sign_data: Key not found. TEE_OpenPersistentObject err: 0x%x\n", ret);
        goto cleanup;
    }
    if(sign){
        ret = TEE_AllocateOperation(&enOp, TEE_ALG_RSASSA_PKCS1_V1_5_SHA256, TEE_MODE_SIGN, ASYM_KEY_FOR_SIGN_SIZE);
    }else{
        ret = TEE_AllocateOperation(&enOp, TEE_ALG_RSASSA_PKCS1_V1_5_SHA256, TEE_MODE_VERIFY, ASYM_KEY_FOR_SIGN_SIZE);
    }
    if (TEE_SUCCESS != ret) {
        TA_DBG("sign_data: allocate op error. TEE_AllocateOperation err: 0x%x\n", ret);
        goto cleanup;
    }
    ret = TEE_SetOperationKey(enOp, key);
    if (TEE_SUCCESS != ret) {
        TA_DBG("sign_data: set key error. TEE_SetOperationKey err: 0x%x\n", ret);
        goto cleanup;
    }

    if(sign){
        ret = TEE_AsymmetricSignDigest(enOp,NULL, 0, digest, digest_len,signature, signature_len);
    }else{
        ret = TEE_AsymmetricVerifyDigest(enOp,NULL, 0, digest, digest_len,signature, *signature_len);
        if(ret != TEE_SUCCESS){
            goto cleanup;
        }
    }
    if (TEE_SUCCESS != ret) {
        TA_DBG("sign_data: sign data error. TEE_AsymmetricSignDigest err: 0x%x\n", ret);
        goto cleanup;
    }
cleanup:
    TEE_CloseObject(key);
    return ret;
}

static TEE_Result sign_ins(uint8_t* S, uint32_t S_len, uint8_t* W, uint32_t W_len, uint8_t* D, uint32_t D_len, uint8_t* mac, uint32_t*mac_len){
    TEE_Result ret = 0;
    uint32_t len = 0;
    uint8_t *tmp = NULL;
    uint32_t tmp_size = S_len + W_len;
    //gen the data mac

    ret = gen_mac(D, D_len, "org_key_mac", 11, mac, &len, false);
    if (TEE_SUCCESS != ret) {
        TA_DBG("call gen_mac for data err\n");
        goto cleanup;
    }
    //gen mac of S and W
    
    tmp = (uint8_t *)malloc(tmp_size);
    memcpy(tmp, S, S_len);
    memcpy(tmp + S_len, W, W_len);
    ret = gen_mac(tmp, tmp_size, "org_key_mac", 11, mac+len, &len, false);
    if (TEE_SUCCESS != ret) {
        TA_DBG("call gen_mac for Weight err\n");
        goto cleanup;
    }
    *mac_len = len * 2;
    free(tmp);
    tmp = NULL;
cleanup:
    return ret;
}
/* sign the table of all public key*/
static TEE_Result sign_verify_public_key(uint8_t* out_data, uint32_t* out_data_len, bool sign){
    TEE_Result ret = 0;
    uint32_t key_len = ASYM_KEY_SIZE/2;
    uint8_t key_stream[key_len]; 
    uint32_t digest_len = (uint32_t)(DIGEST_SIZE/8);
    uint8_t digest[digest_len];
    uint32_t outdata_len = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8);
    uint8_t outdata[outdata_len];
    if(sign){
        if((ret = deserialize_key("cipher_key", 10, key_stream, &key_len, true)) != TEE_SUCCESS){
            goto cleanup;
        }
        if((ret = deserialize_key("remote_cipher_key", 17, key_stream + key_len, &key_len, true)) != TEE_SUCCESS){
            goto cleanup;   
        }
    }else{
        if((ret = deserialize_key("remote_cipher_key", 17, key_stream, &key_len, true)) != TEE_SUCCESS){
            goto cleanup;
        }
        if((ret = deserialize_key("cipher_key", 10, key_stream + key_len, &key_len, true)) != TEE_SUCCESS){
            goto cleanup;   
        }   
    }

    if((ret = create_digest(key_stream, key_len, digest, &digest_len)) != TEE_SUCCESS){
        TA_DBG("sign_verify_public_key: create_digest err\n");
        goto cleanup;
    }
    

    if(sign){
        if((ret = sign_data(digest,digest_len,"sign_key",8,outdata,&outdata_len,sign)) != TEE_SUCCESS){
            TA_DBG("sign_verify_public_key: sign_digest err\n");
            goto cleanup;
        }
        memcpy(out_data, outdata, outdata_len);
        * out_data_len = outdata_len;
    }else{
        if((ret = sign_data(digest,digest_len,"remote_sign_key",15,out_data,out_data_len,sign)) != TEE_SUCCESS){
            TA_DBG("sign_verify_public_key: verify err\n");
            goto cleanup;
        }
    }
cleanup:
    return ret;
}
static bool check_ins(uint8_t* S, uint32_t S_len, uint8_t* W, uint32_t W_len, uint8_t* D, uint32_t D_len, uint8_t* mac, uint32_t*mac_len){
    TEE_Result ret = 0;
    uint32_t len = 0;
    uint8_t *tmp = NULL;
    uint32_t tmp_size = S_len + W_len;
    //check the data mac

    ret = gen_mac(D, D_len, "remote_key_mac", 11, mac, &len, true);
    if (TEE_SUCCESS != ret) {
        TA_DBG("call gen_mac for data err\n");
        goto cleanup;
    }
    //gen mac of S and W
    
    tmp = (uint8_t *)malloc(tmp_size);
    memcpy(tmp, S, S_len);
    memcpy(tmp + S_len, W, W_len);
    ret = gen_mac(tmp, tmp_size, "remote_key_mac", 11, mac+len, &len, true);
    if (TEE_SUCCESS != ret) {
        TA_DBG("call gen_mac for Weight err\n");
        goto cleanup;
    }
    *mac_len = len * 2;
    free(tmp);
    tmp = NULL;
cleanup:
    if(TEE_SUCCESS != ret){
        return false;
    }else{
        return true;    
    }
}
static TEE_Result run_protocol(uint8_t* protocol, uint32_t protocol_len, uint8_t* mac, uint32_t mac_len){
    TEE_Result ret = 0;
    uint8_t * data = (uint8_t*)malloc(MAX_DATA_SIZE);
    uint8_t * label = (uint8_t*)malloc(MAX_LABEL_SIZE);
    uint32_t data_len = MAX_DATA_SIZE;
    uint32_t label_len = MAX_LABEL_SIZE;
    struct Tuple tmp;
    tmp.data = NULL;
    uint8_t tmp_label[5]="";
    struct Data data_s;
    struct Code code;
    if((ret = gen_mac(protocol,protocol_len,"mac_for_protocol_key",20, mac, &mac_len, true)) != TEE_SUCCESS){
        return TEE_ERROR_BAD_STATE;
    }
    /*recover data*/
    if( (ret = sst_read_data("label",5,label,&label_len)) != TEE_SUCCESS ){
        TA_DBG("error: label store Error.\n");
        goto cleanup1;
    }
    if( (ret = sst_read_data("data",4,data,&data_len)) != TEE_SUCCESS ){
        TA_DBG("error: data store Error.\n");
        goto cleanup1;
    }
    data_s = serialize(label, label_len, data, data_len);
    code = code_serialize(protocol,protocol_len);

    /*expend protocol*/
    
    while(code.now_pos < code.code_size){
        run_code(&data_s, &code, &tmp, (uint8_t*)tmp_label);
    }

    /*re store data*/
cleanup1:
    free(data);
    free(label);
    
    return ret;
}
/*static TEE_Result hybrid_encrypt(uint8_t* data, uint32_t data_len, char* name, uint8_t name_len){
    //TODO: like encrypt_data, use asymmetric key to encrypt a symmetric key(may be random or fixed)
}*/
static TEE_Result fixed_hybrid_encrypt(uint8_t* data, uint32_t data_len, uint8_t* out_data, uint32_t* out_data_len){
    //use public to encrypto symmetric key
    TEE_Result ret = 0;
    uint32_t symKey_size = (uint32_t)(SYM_KEY_SIZE/8);
    uint8_t symKey[symKey_size]; 
    uint32_t symOut_size = (uint32_t)(ASYM_KEY_SIZE/8);
    uint8_t symOut[symOut_size]; 
    uint32_t out_size = data_len;
    uint8_t *out_arr = (uint8_t *)malloc(out_size); 
    if((ret = deserialize_key("org_key", 7, symKey, &symKey_size, false)) != TEE_SUCCESS){
        goto cleanup;
    }
    TA_DBG("start encrypt key\n");
    if((ret = encrypt_data(symKey, symKey_size, "remote_cipher_key", 17, symOut, &symOut_size, true)) != TEE_SUCCESS){
        goto cleanup;
    }
    //use symetric key encrypt data
    TA_DBG("start encrypt data\n");
    if((ret = transf_data(data, data_len, "org_key", 7, out_arr, &out_size,true)) != TEE_SUCCESS){
        goto cleanup;
    }
    memcpy(out_data, symOut, symOut_size);
    memcpy(out_data + symOut_size, out_arr, out_size);
    *out_data_len = symOut_size + out_size;
cleanup:
    free(out_arr);
    return ret;
}
static TEE_Result fixed_hybrid_decrypt(uint8_t* data, uint32_t data_len, uint8_t* out_data, uint32_t* out_data_len){
    TEE_Result ret = 0;
    uint32_t symKey_size = (uint32_t)(SYM_KEY_SIZE/8);
    uint8_t symKey[symKey_size]; 
    if((ret = encrypt_data(data, (uint32_t)(ASYM_KEY_SIZE/8), "cipher_key", 10, symKey, &symKey_size, false)) != TEE_SUCCESS){
        goto cleanup;
    }
    if((ret = serialize_key("remote_org_key", 14, symKey, (uint32_t)(SYM_KEY_SIZE/8), SYM_KEY_SIZE, false, CIPHER_KEY_STYLE)) != TEE_SUCCESS){
        goto cleanup;
    }
    if((ret = transf_data(data + (uint32_t)(ASYM_KEY_SIZE/8), data_len - (uint32_t)(ASYM_KEY_SIZE/8), "remote_org_key", 14, out_data, out_data_len,false)) != TEE_SUCCESS){
        goto cleanup;
    }
cleanup:
    return ret;
}
static TEE_Result fixed_hybrid_MAC(uint8_t* data, uint32_t data_len, uint8_t* out_data, uint32_t* out_data_len){
    //use public to encrypto symmetric key
    TEE_Result ret = 0;
    uint32_t symKey_size = (uint32_t)(SYM_KEY_SIZE/8);
    uint8_t symKey[symKey_size]; 
    uint32_t symOut_size = (uint32_t)(ASYM_KEY_SIZE/8);
    uint8_t symOut[symOut_size]; 
    uint32_t out_size = data_len;
    uint8_t out_arr[out_size]; 
    if((ret = deserialize_key("org_key_mac", 11, symKey, &symKey_size, false)) != TEE_SUCCESS){
        goto cleanup;
    }
    if((ret = encrypt_data(symKey, symKey_size, "remote_cipher_key", 17, symOut, &symOut_size, true)) != TEE_SUCCESS){
        goto cleanup;
    }
    //use symetric key encrypt data
    if((ret = gen_mac(data, data_len, "org_key_mac", 11, out_arr, &out_size,false)) != TEE_SUCCESS){
        goto cleanup;
    }
    memcpy(out_data, symOut, symOut_size);
    memcpy(out_data + symOut_size, out_arr, out_size);
    *out_data_len = symOut_size + out_size;
cleanup:
    return ret;
}
static TEE_Result fixed_hybrid_MAC_check(uint8_t* data, uint32_t data_len, uint8_t* out_data, uint32_t out_data_len){
    TEE_Result ret = 0;
    uint32_t symKey_size = (uint32_t)(SYM_KEY_SIZE/8);
    uint8_t symKey[symKey_size]; 
    uint32_t out_len;
    if((ret = encrypt_data(out_data, (uint32_t)(ASYM_KEY_SIZE/8), "cipher_key", 10, symKey, &symKey_size, false)) != TEE_SUCCESS){
        goto cleanup;
    }
    TA_DBG("check serialize_key\n");
    if((ret = serialize_key("remote_key_mac", 14, symKey, (uint32_t)(SYM_KEY_SIZE/8), SYM_KEY_SIZE, false, MAC_KEY_STYLE)) != TEE_SUCCESS){
        goto cleanup;
    }
    out_len = out_data_len - (uint32_t)(ASYM_KEY_SIZE/8);
    TA_DBG("check gen_mac\n");
    if((ret = gen_mac(data, data_len, "remote_key_mac", 14, out_data + (uint32_t)(ASYM_KEY_SIZE/8), &out_len ,true)) != TEE_SUCCESS){
        goto cleanup;
    }
cleanup:
    return ret;
}

static TEE_Result ae_encrypto(uint8_t* label, uint32_t label_len, uint8_t* data, uint32_t data_len, uint8_t* data_out, uint32_t* out_len, uint8_t* mac, uint32_t* mac_len){
    TEE_Result ret = 0;
    if((ret = fixed_hybrid_encrypt(data, data_len, data_out, out_len)) == TEE_SUCCESS){
        uint32_t mac_in_len = *out_len + label_len;
        uint8_t * mac_in = (uint8_t *)malloc(mac_in_len);
        memcpy(mac_in, label, label_len);
        memcpy(mac_in + label_len, data_out, *out_len);
        return fixed_hybrid_MAC(mac_in, mac_in_len, mac, mac_len);
    }else{
        return ret;
    }
}

static TEE_Result ae_decrypto(uint8_t* label, uint32_t label_len, uint8_t* data, uint32_t data_len, uint8_t* mac, uint32_t mac_len){
    TEE_Result ret = 0;
    uint32_t mac_in_len = data_len + label_len;
    uint8_t * mac_in = (uint8_t *)malloc(mac_in_len);
    uint32_t data_plain_len;
    uint8_t * data_plain = NULL;
    memcpy(mac_in, label, label_len);
    memcpy(mac_in + label_len, data, data_len);
    TA_DBG("start encrypte. mac_in_len: %u, mac_len: %u\n",mac_in_len,mac_len);
    if( (ret = fixed_hybrid_MAC_check(mac_in, mac_in_len,mac,mac_len)) != TEE_SUCCESS ){
        TA_DBG("error: data mac illegal.\n");
        goto cleanup;
    }
    TA_DBG("start encrypto end.\n");
    
    data_plain_len = data_len - (uint32_t)(ASYM_KEY_SIZE/8);
    data_plain = (uint8_t *)malloc(data_plain_len);
    if( (ret = fixed_hybrid_decrypt(data, data_len, data_plain, &data_plain_len)) != TEE_SUCCESS ){
        TA_DBG("error: data decrypt illegal.\n");
        goto cleanup1;
    }
    if( (ret = sst_write_data("label",5,label,label_len)) != TEE_SUCCESS ){
        TA_DBG("error: label store Error.\n");
        goto cleanup1;
    }
    if( (ret = sst_write_data("data",4,data_plain,data_plain_len)) != TEE_SUCCESS ){
        TA_DBG("error: data store Error.\n");
        goto cleanup1;
    }
    
cleanup1:
    free(data_plain);
cleanup:
    free(mac_in);
    return ret;
}
static TEE_Result _TA_InvokeCommandEntryPoint(
        void *sessionContext, uint32_t commandID,
        uint32_t paramTypes, TEE_Param params[4])
{
    TEE_Result ret = 0;
    if(ret != TEE_SUCCESS){
        return ret;
    }
    if (commandID == CMD_ENCRYPT){
        
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        ret = fixed_hybrid_encrypt(
                    params[0].memref.buffer, 
                    params[0].memref.size,
                    params[1].memref.buffer,
                    &(params[1].memref.size));
    } else if (commandID == CMD_DECRYPT){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        uint32_t data_plain_len = params[0].memref.size - (uint32_t)(ASYM_KEY_SIZE/8);;
        uint8_t * data_plain = (uint8_t *)malloc(data_plain_len);

        ret = fixed_hybrid_decrypt(
                    params[0].memref.buffer, 
                    params[0].memref.size,
                    params[1].memref.buffer,
                    &(params[1].memref.size));
        return TEE_SUCCESS;
    } else if (commandID == CMD_KEY_GET){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_VALUE_INPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        if(params[0].value.a == PUBLIC_TYPE_SIGN){
            ret = deserialize_key(
                    "sign_key", 
                    8,
                    params[1].memref.buffer,
                    &(params[1].memref.size), true);
        }else if(params[0].value.a == PUBLIC_TYPE_CIPHER){
            ret = deserialize_key(
                    "cipher_key", 
                    10,
                    params[1].memref.buffer,
                    &(params[1].memref.size), true);
        }
    } else if (commandID == CMD_INIT){
        ret = gen_sym_key("org_key", 7, CIPHER_KEY_STYLE);
        ret = gen_sym_key("org_key_mac", 11, MAC_KEY_STYLE);
        ret = gen_public_key("sign_key", 8, ASYM_KEY_FOR_SIGN_SIZE);
        ret = gen_public_key("cipher_key", 10, ASYM_KEY_SIZE);
        ret = gen_mac_for_protocol_key();
        //test();
    }else if (commandID == CMD_TEST_MAC){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        ret = gen_mac(
            params[0].memref.buffer, 
            params[0].memref.size,
            "org_key_mac", 
            11,
            params[1].memref.buffer,
            &(params[1].memref.size),false);
    }else if (commandID == CMD_KEY_STORE){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_VALUE_INPUT,
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        if(params[0].value.a == PUBLIC_TYPE_SIGN){
            ret = serialize_key(
                    "remote_sign_key", 
                    15,
                    params[1].memref.buffer,
                    params[1].memref.size,
                    ASYM_KEY_FOR_SIGN_SIZE, true, PUBLIC_KEY_STYLE);
        }else if(params[0].value.a == PUBLIC_TYPE_CIPHER){
            ret = serialize_key(
                    "remote_cipher_key", 
                    17,
                    params[1].memref.buffer,
                    params[1].memref.size, 
                    ASYM_KEY_SIZE, true, PUBLIC_KEY_STYLE);
        }
    }else if(commandID == CMD_KEY_SIGN){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        sign_verify_public_key(
            params[0].memref.buffer,
            &(params[0].memref.size), true);
    }else if(commandID == CMD_KEY_VERIFY){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_VALUE_OUTPUT,
                    TEE_PARAM_TYPE_NONE,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        if(sign_verify_public_key(
            params[0].memref.buffer,
             &(params[0].memref.size), false) == TEE_SUCCESS){
            params[1].value.a = 1;
        }else{
            params[1].value.a = 0;
        }
    }else if (commandID == CMD_ENCRYPT_MAC){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT,
                    TEE_PARAM_TYPE_MEMREF_OUTPUT) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        ret = ae_encrypto(
                    params[0].memref.buffer, 
                    params[0].memref.size,
                    params[1].memref.buffer, 
                    params[1].memref.size,
                    params[2].memref.buffer,
                    &(params[2].memref.size),
                    params[3].memref.buffer,
                    &(params[3].memref.size));
    } else if (commandID == CMD_DECRYPT_MAC){
        if (TEE_PARAM_TYPES(
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_MEMREF_INPUT,
                    TEE_PARAM_TYPE_NONE) != paramTypes) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        ret = ae_decrypto(
                    params[0].memref.buffer, 
                    params[0].memref.size,
                    params[1].memref.buffer, 
                    params[1].memref.size,
                    params[2].memref.buffer,
                    params[2].memref.size);
    }

    return TEE_SUCCESS;
}

tee_srv_head_t xor_srv_head = {
    .uuid = &xor_uuid,
    .type = SRV_TYPE_1_I_M_S_NKA,
    .stack_size = 0x8000,
    .TA_CreateEntryPoint = _TA_CreateEntryPoint,
    .TA_DestroyEntryPoint = _TA_DestroyEntryPoint,
    .TA_OpenSessionEntryPoint = _TA_OpenSessionEntryPoint,
    .TA_CloseSessionEntryPoint = _TA_CloseSessionEntryPoint,
    .TA_InvokeCommandEntryPoint = _TA_InvokeCommandEntryPoint,
};

