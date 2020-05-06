#include "tee_internal_api.h"
#include "tee_internal_api_ext.h"
#include "tee_srv.h"
#define UUID_HARDWARE    { 0x13245768, 0xacbd, 0xcedf,   \
                        { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 } }

#define CMD_GEN     (0x00000000)
#define TA_DBG(format, a...)        \
    do {                            \
        TEE_Print(format, ##a);     \
    } while(0)
static const TEEC_UUID _g_uuid_hardware = UUID_HARDWARE;

static TEE_Result _TA_CreateEntryPoint(void){
    return TEE_SUCCESS;
}

static void _TA_DestroyEntryPoint(void){
    return;
}
static TEE_Result _TA_OpenSessionEntryPoint(
        uint32_t paramTypes, TEE_Param params[4], void **sessionContext){
    return TEE_SUCCESS;
}
static void _TA_CloseSessionEntryPoint(void *sessionContext){
    return;
}

static TEE_Result key_check(){
	//char* name = "key";
	//uint8_t name_len = 3;
	//check if the key is storage.
	TEE_ObjectHandle keyObject = TEE_HANDLE_NULL;
	TEE_Result ret = 0;
	ret = TEE_AllocateTransientObject(TEE_TYPE_AES,128,&keyObject);
	if (TEE_SUCCESS != ret) {
		TA_DBG("key_check: TEE_AllocateTransientObject err: 0x%x\n", ret);
	}
	ret = TEE_GenerateKey(keyObject, 128, NULL, 0);
	if (TEE_SUCCESS != ret) {
		TA_DBG("key_check: TEE_GenerateKey err: 0x%x\n", ret);
	}
cleanup:
	TEE_CloseObject(keyObject);
	return ret;
}
static TEE_Result _TA_InvokeCommandEntryPoint(
        void *sessionContext, uint32_t commandID,
        uint32_t param_types, TEE_Param params[4]){
	


}