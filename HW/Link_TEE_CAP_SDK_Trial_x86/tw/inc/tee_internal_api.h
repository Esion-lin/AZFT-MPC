/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_INTERNAL_API_H_
#define _TEE_INTERNAL_API_H_

#include "tee_types.h"

/******************************************************************************/
/* GENERIC                                                                    */
/******************************************************************************/

#define TEE_SUCCESS                         (0x00000000)
#define TEE_TEE_ENABLE                      (0x0000000C)
#define TEE_ERROR_CORRUPT_OBJECT            (0xF0100001)
#define TEE_ERROR_CORRUPT_OBJECT_2          (0xF0100002)
#define TEE_ERROR_STORAGE_NOT_AVAILABLE     (0xF0100003)
#define TEE_ERROR_STORAGE_NOT_AVAILABLE_2   (0xF0100004)
#define TEE_ERROR_GENERIC                   (0xFFFF0000)
#define TEE_ERROR_ACCESS_DENIED             (0xFFFF0001)
#define TEE_ERROR_CANCEL                    (0xFFFF0002)
#define TEE_ERROR_ACCESS_CONFLICT           (0xFFFF0003)
#define TEE_ERROR_EXCESS_DATA               (0xFFFF0004)
#define TEE_ERROR_BAD_FORMAT                (0xFFFF0005)
#define TEE_ERROR_BAD_PARAMETERS            (0xFFFF0006)
#define TEE_ERROR_BAD_STATE                 (0xFFFF0007)
#define TEE_ERROR_ITEM_NOT_FOUND            (0xFFFF0008)
#define TEE_ERROR_NOT_IMPLEMENTED           (0xFFFF0009)
#define TEE_ERROR_NOT_SUPPORTED             (0xFFFF000A)
#define TEE_ERROR_NO_DATA                   (0xFFFF000B)
#define TEE_ERROR_OUT_OF_MEMORY             (0xFFFF000C)
#define TEE_ERROR_BUSY                      (0xFFFF000D)
#define TEE_ERROR_COMMUNICATION             (0xFFFF000E)
#define TEE_ERROR_SECURITY                  (0xFFFF000F)
#define TEE_ERROR_SHORT_BUFFER              (0xFFFF0010)
#define TEE_PENDING                         (0xFFFF2000)
#define TEE_ERROR_TIMEOUT                   (0xFFFF3001)
#define TEE_ERROR_OVERFLOW                  (0xFFFF300F)
#define TEE_ERROR_TARGET_DEAD               (0xFFFF3024)
#define TEE_ERROR_STORAGE_NO_SPACE          (0xFFFF3041)
#define TEE_ERROR_MAC_INVALID               (0xFFFF3071)
#define TEE_ERROR_SIGNATURE_INVALID         (0xFFFF3072)
#define TEE_ERROR_TIME_NOT_SET              (0xFFFF5000)
#define TEE_ERROR_TIME_NEEDS_RESET          (0xFFFF5001)
#define TEE_ERROR_TEE_NOT_ENABLE            (0xFFFFC00C)

#define TEE_HANDLE_NULL                     (0x00000000)

#ifndef __ASSEMBLY__

typedef uint32_t TEE_Result;

typedef struct
{
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint8_t clockSeqAndNode[8];
} TEE_UUID;

#endif /* __ASSEMBLY__ */

/******************************************************************************/
/* CORE FRAMEWORK                                                             */
/******************************************************************************/

#define TEE_PARAM_TYPE_NONE             (0)
#define TEE_PARAM_TYPE_VALUE_INPUT      (1)
#define TEE_PARAM_TYPE_VALUE_OUTPUT     (2)
#define TEE_PARAM_TYPE_VALUE_INOUT      (3)
#define TEE_PARAM_TYPE_MEMREF_INPUT     (5)
#define TEE_PARAM_TYPE_MEMREF_OUTPUT    (6)
#define TEE_PARAM_TYPE_MEMREF_INOUT     (7)

#define TEE_LOGIN_PUBLIC                (0x00000000)
#define TEE_LOGIN_USER                  (0x00000001)
#define TEE_LOGIN_GROUP                 (0x00000002)
#define TEE_LOGIN_APPLICATION           (0x00000004)
#define TEE_LOGIN_APPLICATION_USER      (0x00000005)
#define TEE_LOGIN_APPLICATION_GROUP     (0x00000006)
#define TEE_LOGIN_TRUSTED_APP           (0xF0000000)

#define TEE_ORIGIN_API                  (0x00000001)
#define TEE_ORIGIN_COMMS                (0x00000002)
#define TEE_ORIGIN_TEE                  (0x00000003)
#define TEE_ORIGIN_TRUSTED_APP          (0x00000004)

#define TEE_PROPSET_CURRENT_TA          ((TEE_PropSetHandle)0xFFFFFFFF)
#define TEE_PROPSET_CURRENT_CLIENT      ((TEE_PropSetHandle)0xFFFFFFFE)
#define TEE_PROPSET_TEE_IMPLEMENTATION  ((TEE_PropSetHandle)0xFFFFFFFD)

#define TEE_MEMORY_ACCESS_READ          (0x00000001)
#define TEE_MEMORY_ACCESS_WRITE         (0x00000002)
#define TEE_MEMORY_ACCESS_ANY_OWNER     (0x00000004)

#define TEE_TIMEOUT_INFINITE            (0xFFFFFFFF)

#define TEE_PARAM_TYPES(t0, t1, t2, t3) \
        ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))
#define TEE_PARAM_TYPE_GET(t, i) (((t) >> (i*4)) & 0xF)

#ifndef __ASSEMBLY__

typedef struct
{
    uint32_t login;
    TEE_UUID uuid;
} TEE_Identity;

typedef union
{
    struct
    {
        void *buffer;
        uint32_t size;
    } memref;
    struct
    {
        uint32_t a, b;
    } value;
} TEE_Param;

typedef struct __TEE_TASessionHandle *TEE_TASessionHandle;

typedef struct __TEE_PropSetHandle *TEE_PropSetHandle;

void TEE_Panic(TEE_Result panicCode);

TEE_Result TEE_OpenTASession(
        TEE_UUID            *destination,
        uint32_t            cancellationRequestTimeout,
        uint32_t            paramTypes,
        TEE_Param           params[4],
        TEE_TASessionHandle *session,
        uint32_t            *returnOrigin);

void TEE_CloseTASession(TEE_TASessionHandle   session);

TEE_Result TEE_InvokeTACommand(
        TEE_TASessionHandle session,
        uint32_t            cancellationRequestTimeout,
        uint32_t            commandID,
        uint32_t            paramTypes,
        TEE_Param           params[4],
        uint32_t            *returnOrigin);

TEE_Result TEE_CheckMemoryAccessRights(
        uint32_t accessFlags,
        void     *buffer,
        uint32_t size);

void TEE_SetInstanceData(void *instanceData);

void *TEE_GetInstanceData(void);

void *TEE_Malloc(uint32_t size, uint32_t hint);

void *TEE_Realloc(void *buffer, uint32_t newSize);

void TEE_Free(void *buffer);

void TEE_MemMove(void *dest, void *src, uint32_t size);

int32_t TEE_MemCompare(void *buffer1, void *buffer2, uint32_t size);

void TEE_MemFill(void *buffer, uint32_t x, uint32_t size);

#endif /* __ASSEMBLY__ */

/******************************************************************************/
/* Trust Storage                                                              */
/******************************************************************************/

#define TEE_STORAGE_PRIVATE             (0x00000001)

#define TEE_DATA_FLAG_ACCESS_READ       (0x00000001)
#define TEE_DATA_FLAG_ACCESS_WRITE      (0x00000002)
#define TEE_DATA_FLAG_ACCESS_WRITE_META (0x00000004)
#define TEE_DATA_FLAG_SHARE_READ        (0x00000010)
#define TEE_DATA_FLAG_SHARE_WRITE       (0x00000020)
#define TEE_DATA_FLAG_OVERWRITE         (0x00000400)

#define TEE_ATTR_SECRET_VALUE           (0xC0000000)
#define TEE_ATTR_RSA_MODULUS            (0xD0000130)
#define TEE_ATTR_RSA_PUBLIC_EXPONENT    (0xD0000230)
#define TEE_ATTR_RSA_PRIVATE_EXPONENT   (0xC0000330)
#define TEE_ATTR_RSA_PRIME1             (0xC0000430)
#define TEE_ATTR_RSA_PRIME2             (0xC0000530)
#define TEE_ATTR_RSA_EXPONENT1          (0xC0000630)
#define TEE_ATTR_RSA_EXPONENT2          (0xC0000730)
#define TEE_ATTR_RSA_COEFFICIENT        (0xC0000830)
#define TEE_ATTR_DSA_PRIME              (0xD0001031)
#define TEE_ATTR_DSA_SUBPRIME           (0xD0001131)
#define TEE_ATTR_DSA_BASE               (0xD0001231)
#define TEE_ATTR_DSA_PUBLIC_VALUE       (0xD0000131)
#define TEE_ATTR_DSA_PRIVATE_VALUE      (0xC0000231)
#define TEE_ATTR_DH_PRIME               (0xD0001032)
#define TEE_ATTR_DH_SUBPRIME            (0xD0001132)
#define TEE_ATTR_DH_BASE                (0xD0001232)
#define TEE_ATTR_DH_X_BITS              (0xF0001332)
#define TEE_ATTR_DH_PUBLIC_VALUE        (0xD0000132)
#define TEE_ATTR_DH_PRIVATE_VALUE       (0xC0000232)
#define TEE_ATTR_RSA_OAEP_LABEL         (0xD0000930)
#define TEE_ATTR_RSA_PSS_SALT_LENGTH    (0xF0000A30)
#define TEE_ATTR_ECC_PUBLIC_VALUE_X     (0xD0000141)
#define TEE_ATTR_ECC_PUBLIC_VALUE_Y     (0xD0000241)
#define TEE_ATTR_ECC_PRIVATE_VALUE      (0xC0000341)
#define TEE_ATTR_ECC_CURVE              (0xF0000441)

#define TEE_ATTR_FLAG_VALUE             (0x20000000)
#define TEE_ATTR_FLAG_PUBLIC            (0x10000000)

#define TEE_TYPE_AES                    (0xA0000010)
#define TEE_TYPE_DES                    (0xA0000011)
#define TEE_TYPE_DES3                   (0xA0000013)
#define TEE_TYPE_HMAC_MD5               (0xA0000001)
#define TEE_TYPE_HMAC_SHA1              (0xA0000002)
#define TEE_TYPE_HMAC_SHA224            (0xA0000003)
#define TEE_TYPE_HMAC_SHA256            (0xA0000004)
#define TEE_TYPE_HMAC_SHA384            (0xA0000005)
#define TEE_TYPE_HMAC_SHA512            (0xA0000006)
#define TEE_TYPE_RSA_PUBLIC_KEY         (0xA0000030)
#define TEE_TYPE_RSA_KEYPAIR            (0xA1000030)
#define TEE_TYPE_DSA_PUBLIC_KEY         (0xA0000031)
#define TEE_TYPE_DSA_KEYPAIR            (0xA1000031)
#define TEE_TYPE_DH_KEYPAIR             (0xA1000032)
#define TEE_TYPE_ECDSA_PUBLIC_KEY       (0xA0000041)
#define TEE_TYPE_ECDSA_KEYPAIR          (0xA1000041)
#define TEE_TYPE_ECDH_PUBLIC_KEY        (0xA0000042)
#define TEE_TYPE_ECDH_KEYPAIR           (0xA1000042)
#define TEE_TYPE_SM2_PUBLIC_KEY         (0xA0000043)
#define TEE_TYPE_SM2_KEYPAIR            (0xA1000043)
#define TEE_TYPE_GENERIC_SECRET         (0xA0000000)
#define TEE_TYPE_CORRUPTED_OBJECT       (0xA00000BE)
#define TEE_TYPE_DATA                   (0xA00000BF)

#define TEE_USAGE_EXTRACTABLE           (0x00000001)
#define TEE_USAGE_ENCRYPT               (0x00000002)
#define TEE_USAGE_DECRYPT               (0x00000004)
#define TEE_USAGE_MAC                   (0x00000008)
#define TEE_USAGE_SIGN                  (0x00000010)
#define TEE_USAGE_VERIFY                (0x00000020)
#define TEE_USAGE_DERIVE                (0x00000040)

#define TEE_HANDLE_FLAG_PERSISTENT      (0x00010000)
#define TEE_HANDLE_FLAG_INITIALIZED     (0x00020000)
#define TEE_HANDLE_FLAG_KEY_SET         (0x00040000)
#define TEE_HANDLE_FLAG_EXPECT_TWO_KEYS (0x00080000)

#define TEE_DATA_MAX_POSITION           (0xFFFFFFFF)
#define TEE_OBJECT_ID_MAX_LEN           (64)

#ifndef __ASSEMBLY__

typedef struct
{
    uint32_t attributeID;
    union
    {
        struct
        {
            void *buffer; uint32_t length;
        } ref;
        struct
        {
            uint32_t a, b;
        } value;
    } content;
} TEE_Attribute;

typedef struct {
    uint32_t objectType;
    uint32_t keySize;
    uint32_t maxKeySize;
    uint32_t objectUsage;
    uint32_t dataSize;
    uint32_t dataPosition;
    uint32_t handleFlags;
} TEE_ObjectInfo;

typedef enum
{
    TEE_DATA_SEEK_SET = 0,
    TEE_DATA_SEEK_CUR = 1,
    TEE_DATA_SEEK_END = 2
} TEE_Whence;

typedef struct __TEE_ObjectHandle *TEE_ObjectHandle;

typedef struct __TEE_ObjectEnumHandle *TEE_ObjectEnumHandle;

TEE_Result TEE_GetObjectInfo1(
        TEE_ObjectHandle    object,
        TEE_ObjectInfo      *objectInfo);

TEE_Result TEE_RestrictObjectUsage1(
        TEE_ObjectHandle    object,
        uint32_t            objectUsage);

TEE_Result TEE_GetObjectBufferAttribute(
        TEE_ObjectHandle    object,
        uint32_t            attributeID,
        void                *buffer, uint32_t *size);

TEE_Result TEE_GetObjectValueAttribute(
        TEE_ObjectHandle    object,
        uint32_t            attributeID,
        uint32_t            *a,
        uint32_t            *b);

void TEE_CloseObject(TEE_ObjectHandle object);

TEE_Result TEE_AllocateTransientObject(
        uint32_t            objectType,
        uint32_t            maxKeySize,
        TEE_ObjectHandle    *object);

void TEE_FreeTransientObject(
        TEE_ObjectHandle    object);

void TEE_ResetTransientObject(
        TEE_ObjectHandle    object);

TEE_Result TEE_PopulateTransientObject(
        TEE_ObjectHandle    object,
        TEE_Attribute       *attrs,
        uint32_t            attrCount);

void TEE_InitRefAttribute(
        TEE_Attribute       *attr,
        uint32_t            attributeID,
        void                *buffer,
        uint32_t            length);

void TEE_InitValueAttribute(
        TEE_Attribute       *attr,
        uint32_t            attributeID,
        uint32_t            a,
        uint32_t            b);

TEE_Result TEE_CopyObjectAttributes1(
        TEE_ObjectHandle    destObject,
        TEE_ObjectHandle    srcObject);

TEE_Result TEE_GenerateKey(
        TEE_ObjectHandle    object,
        uint32_t            keySize,
        TEE_Attribute       *params, uint32_t paramCount);

TEE_Result TEE_OpenPersistentObject(
        uint32_t            storageID,
        void                *objectID, uint32_t objectIDLen,
        uint32_t            flags,
        TEE_ObjectHandle    *object);

TEE_Result TEE_CreatePersistentObject(
        uint32_t            storageID,
        void                *objectID, uint32_t objectIDLen,
        uint32_t            flags,
        TEE_ObjectHandle    attributes,
        void                *initialData, uint32_t initialDataLen,
        TEE_ObjectHandle    *object);

TEE_Result TEE_CloseAndDeletePersistentObject1(
        TEE_ObjectHandle    object);

TEE_Result TEE_RenamePersistentObject(
        TEE_ObjectHandle    object,
        void                *newObjectID, uint32_t newObjectIDLen);

TEE_Result TEE_AllocatePersistentObjectEnumerator(
        TEE_ObjectEnumHandle    *objectEnumerator);

void TEE_FreePersistentObjectEnumerator(
        TEE_ObjectEnumHandle    objectEnumerator);

void TEE_ResetPersistentObjectEnumerator(
        TEE_ObjectEnumHandle    objectEnumerator);

TEE_Result TEE_StartPersistentObjectEnumerator(
        TEE_ObjectEnumHandle    objectEnumerator,
        uint32_t                storageID);

TEE_Result TEE_GetNextPersistentObject(
        TEE_ObjectEnumHandle    objectEnumerator,
        TEE_ObjectInfo          *objectInfo,
        void                    *objectID,
        uint32_t                *objectIDLen);

TEE_Result TEE_ReadObjectData(
        TEE_ObjectHandle    object,
        void                *buffer,
        uint32_t            size,
        uint32_t            *count);

TEE_Result TEE_WriteObjectData(
        TEE_ObjectHandle    object,
        void                *buffer, uint32_t size);

TEE_Result TEE_TruncateObjectData(
        TEE_ObjectHandle    object,
        uint32_t            size);

TEE_Result TEE_SeekObjectData(
        TEE_ObjectHandle    object,
        int32_t             offset,
        TEE_Whence          whence);

/* deprecated functions */
void TEE_GetObjectInfo(
        TEE_ObjectHandle object,
        TEE_ObjectInfo   *objectInfo);

void TEE_RestrictObjectUsage(
        TEE_ObjectHandle    object,
        uint32_t            objectUsage);

void TEE_CopyObjectAttributes(
        TEE_ObjectHandle    destObject,
        TEE_ObjectHandle    srcObject);

void TEE_CloseAndDeletePersistentObject(
        TEE_ObjectHandle    object);

/* Alibaba implementation */
TEE_Result TEE_OpenPersistentObjectByUUID(
        TEE_UUID            *UUID,
        uint32_t            storageID,
        void                *objectID, uint32_t objectIDLen,
        uint32_t            flags,
        TEE_ObjectHandle    *object);

TEE_Result TEE_AllocatePersistentObjectEnumeratorByUUID(
        TEE_UUID                *uuid,
        TEE_ObjectEnumHandle    *objectEnumerator);
#endif /* __ASSEMBLY__ */

/******************************************************************************/
/* Cryptographic                                                              */
/******************************************************************************/

#define TEE_ALG_AES_ECB_NOPAD                   (0x10000010)
#define TEE_ALG_AES_CBC_NOPAD                   (0x10000110)
#define TEE_ALG_AES_CTR                         (0x10000210)
#define TEE_ALG_AES_CTS                         (0x10000310)
#define TEE_ALG_AES_XTS                         (0x10000410)
#define TEE_ALG_AES_CBC_MAC_NOPAD               (0x30000110)
#define TEE_ALG_AES_CBC_MAC_PKCS5               (0x30000510)
#define TEE_ALG_AES_CMAC                        (0x30000610)
#define TEE_ALG_AES_CCM                         (0x40000710)
#define TEE_ALG_AES_GCM                         (0x40000810)
#define TEE_ALG_DES_ECB_NOPAD                   (0x10000011)
#define TEE_ALG_DES_CBC_NOPAD                   (0x10000111)
#define TEE_ALG_DES_CBC_MAC_NOPAD               (0x30000111)
#define TEE_ALG_DES_CBC_MAC_PKCS5               (0x30000511)
#define TEE_ALG_DES3_ECB_NOPAD                  (0x10000013)
#define TEE_ALG_DES3_CBC_NOPAD                  (0x10000113)
#define TEE_ALG_DES3_CBC_MAC_NOPAD              (0x30000113)
#define TEE_ALG_DES3_CBC_MAC_PKCS5              (0x30000513)
#define TEE_ALG_RSASSA_PKCS1_V1_5_MD5           (0x70001830)
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA1          (0x70002830)
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA224        (0x70003830)
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA256        (0x70004830)
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA384        (0x70005830)
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA512        (0x70006830)
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1      (0x70212930)
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224    (0x70313930)
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256    (0x70414930)
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384    (0x70515930)
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512    (0x70616930)
#define TEE_ALG_RSAES_PKCS1_V1_5                (0x60000130)
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1      (0x60210230)
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224    (0x60310230)
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256    (0x60410230)
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384    (0x60510230)
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512    (0x60610230)
#define TEE_ALG_RSA_NOPAD                       (0x60000030)
#define TEE_ALG_DSA_SHA1                        (0x70002131)
#define TEE_ALG_DSA_SHA224                      (0x70003131)
#define TEE_ALG_DSA_SHA256                      (0x70004131)
#define TEE_ALG_DH_DERIVE_SHARED_SECRET         (0x80000032)
#define TEE_ALG_MD5                             (0x50000001)
#define TEE_ALG_SHA1                            (0x50000002)
#define TEE_ALG_SHA224                          (0x50000003)
#define TEE_ALG_SHA256                          (0x50000004)
#define TEE_ALG_SHA384                          (0x50000005)
#define TEE_ALG_SHA512                          (0x50000006)
#define TEE_ALG_SM3                             (0x50000007)
#define TEE_ALG_HMAC_MD5                        (0x30000001)
#define TEE_ALG_HMAC_SHA1                       (0x30000002)
#define TEE_ALG_HMAC_SHA224                     (0x30000003)
#define TEE_ALG_HMAC_SHA256                     (0x30000004)
#define TEE_ALG_HMAC_SHA384                     (0x30000005)
#define TEE_ALG_HMAC_SHA512                     (0x30000006)
#define TEE_ALG_ECDSA_P192                      (0x70001041)
#define TEE_ALG_ECDSA_P224                      (0x70002041)
#define TEE_ALG_ECDSA_P256                      (0x70003041)
#define TEE_ALG_ECDSA_P384                      (0x70004041)
#define TEE_ALG_ECDSA_P521                      (0x70005041)
#define TEE_ALG_ECDH_P192                       (0x80001042)
#define TEE_ALG_ECDH_P224                       (0x80002042)
#define TEE_ALG_ECDH_P256                       (0x80003042)
#define TEE_ALG_ECDH_P384                       (0x80004042)
#define TEE_ALG_ECDH_P521                       (0x80005042)
#define TEE_ALG_SM2_P256                        (0x70006043)

#define TEE_OPERATION_CIPHER                    (1)
#define TEE_OPERATION_MAC                       (3)
#define TEE_OPERATION_AE                        (4)
#define TEE_OPERATION_DIGEST                    (5)
#define TEE_OPERATION_ASYMMETRIC_CIPHER         (6)
#define TEE_OPERATION_ASYMMETRIC_SIGNATURE      (7)
#define TEE_OPERATION_KEY_DERIVATION            (8)

#define TEE_USAGE_EXTRACTABLE                 (0x00000001)
#define TEE_USAGE_ENCRYPT                     (0x00000002)
#define TEE_USAGE_DECRYPT                     (0x00000004)
#define TEE_USAGE_MAC                         (0x00000008)
#define TEE_USAGE_SIGN                        (0x00000010)
#define TEE_USAGE_VERIFY                      (0x00000020)
#define TEE_USAGE_DERIVE                      (0x00000040)

#define TEE_HANDLE_FLAG_PERSISTENT            (0x00010000)
#define TEE_HANDLE_FLAG_INITIALIZED           (0x00020000)
#define TEE_HANDLE_FLAG_KEY_SET               (0x00040000)
#define TEE_HANDLE_FLAG_EXPECT_TWO_KEYS       (0x00080000)

#define TEE_OPERATION_STATE_INITIAL           (0x00000000)
#define TEE_OPERATION_STATE_ACTIVE            (0x00000001)
#ifndef __ASSEMBLY__

typedef enum
{
    TEE_MODE_ENCRYPT    = 0,
    TEE_MODE_DECRYPT    = 1,
    TEE_MODE_SIGN       = 2,
    TEE_MODE_VERIFY     = 3,
    TEE_MODE_MAC        = 4,
    TEE_MODE_DIGEST     = 5,
    TEE_MODE_DERIVE     = 6,
} TEE_OperationMode;

typedef struct
{
    uint32_t algorithm;
    uint32_t operationClass;
    uint32_t mode;
    uint32_t digestLength;
    uint32_t maxKeySize;
    uint32_t keySize;
    uint32_t requiredKeyUsage;
    uint32_t handleState;
} TEE_OperationInfo;

typedef struct
{
    uint32_t keySize;
    uint32_t requiredKeyUsage;
} TEE_OperationInfoKey;

typedef struct {
    uint32_t algorithm;
    uint32_t operationClass;
    uint32_t mode;
    uint32_t digestLength;
    uint32_t maxKeySize;
    uint32_t handleState;
    uint32_t operationState;
    uint32_t numberOfKeys;
    TEE_OperationInfoKey keyInformation[];
} TEE_OperationInfoMultiple;

typedef struct __TEE_OperationHandle *TEE_OperationHandle;

TEE_Result TEE_AllocateOperation(
        TEE_OperationHandle     *operation,
        uint32_t                algorithm,
        uint32_t                mode,
        uint32_t                maxKeySize);

void TEE_FreeOperation(TEE_OperationHandle operation);

void TEE_GetOperationInfo(
        TEE_OperationHandle     operation,
        TEE_OperationInfo       *operationInfo);

TEE_Result TEE_GetOperationInfoMultiple(
        TEE_OperationHandle       operation,
        TEE_OperationInfoMultiple *operationInfoMultiple, uint32_t *operationSize);

void TEE_ResetOperation(TEE_OperationHandle operation);

TEE_Result TEE_SetOperationKey(
        TEE_OperationHandle     operation,
        TEE_ObjectHandle        key);

TEE_Result TEE_SetOperationKey2(
        TEE_OperationHandle     operation,
        TEE_ObjectHandle        key1,
        TEE_ObjectHandle        key2);

void TEE_CopyOperation(
        TEE_OperationHandle     dstOperation,
        TEE_OperationHandle     srcOperation);

void TEE_DigestUpdate(
        TEE_OperationHandle     operation,
        void                    *chunk, uint32_t chunkSize);

TEE_Result TEE_DigestDoFinal(
        TEE_OperationHandle     operation,
        void                    *chunk, uint32_t chunkLen,
        void                    *hash, uint32_t *hashLen);

void TEE_CipherInit(
       TEE_OperationHandle      operation,
       void                     *IV, uint32_t IVLen);

TEE_Result TEE_CipherUpdate(
       TEE_OperationHandle      operation,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen);

TEE_Result TEE_CipherDoFinal(
       TEE_OperationHandle      operation,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen);

void TEE_MACInit(
       TEE_OperationHandle      operation,
       void                     *IV, uint32_t IVLen);

void TEE_MACUpdate(
       TEE_OperationHandle      operation,
       void                     *chunk, uint32_t chunkSize);

TEE_Result TEE_MACComputeFinal(
       TEE_OperationHandle      operation,
       void                     *message, uint32_t messageLen,
       void                     *mac, uint32_t *macLen);

TEE_Result TEE_MACCompareFinal(
       TEE_OperationHandle      operation,
       void                     *message, uint32_t messageLen,
       void                     *mac, uint32_t macLen);

TEE_Result TEE_AEInit(
       TEE_OperationHandle      operation,
       void                     *nonce, uint32_t nonceLen,
       uint32_t                 tagLen,
       uint32_t                 AADLen,
       uint32_t                 payloadLen);

void TEE_AEUpdateAAD(
       TEE_OperationHandle      operation,
       void                     *AADdata, uint32_t AADdataLen);

TEE_Result TEE_AEUpdate(
       TEE_OperationHandle      operation,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen);

TEE_Result TEE_AEEncryptFinal(
       TEE_OperationHandle      operation,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen,
       void                     *tag, uint32_t *tagLen);

TEE_Result TEE_AEDecryptFinal(
       TEE_OperationHandle      operation,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen,
       void                     *tag, uint32_t tagLen);

TEE_Result TEE_AsymmetricEncrypt(
       TEE_OperationHandle      operation,
       TEE_Attribute            *params, uint32_t paramCount,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen);

TEE_Result TEE_AsymmetricDecrypt(
       TEE_OperationHandle      operation,
       TEE_Attribute            *params, uint32_t paramCount,
       void                     *srcData, uint32_t srcLen,
       void                     *destData, uint32_t *destLen);

TEE_Result TEE_AsymmetricSignDigest(
       TEE_OperationHandle      operation,
       TEE_Attribute            *params, uint32_t paramCount,
       void                     *digest, uint32_t digestLen,
       void                     *signature, uint32_t *signatureLen);

TEE_Result TEE_AsymmetricVerifyDigest(
       TEE_OperationHandle      operation,
       TEE_Attribute            *params, uint32_t paramCount,
       void                     *digest, uint32_t digestLen,
       void                     *signature, uint32_t signatureLen);

void TEE_DeriveKey(
       TEE_OperationHandle      operation,
       TEE_Attribute*           params, uint32_t paramCount,
       TEE_ObjectHandle         derivedKey);

void TEE_GenerateRandom(
       void                     *randomBuffer,
       uint32_t                 randomBufferLen);

#endif /* __ASSEMBLY__ */

/******************************************************************************/
/* Time                                                                       */
/******************************************************************************/

#ifndef __ASSEMBLY__

typedef struct
{
    uint32_t seconds;
    uint32_t millis;
} TEE_Time;

void TEE_GetSystemTime(TEE_Time *time);
TEE_Result TEE_Wait(uint32_t timeout);
TEE_Result TEE_GetTAPersistentTime(TEE_Time *time);
TEE_Result TEE_SetTAPersistentTime(TEE_Time *time);
void TEE_GetREETime(TEE_Time *time);

#endif /* __ASSEMBLY__ */

typedef uint32_t TEE_BigInt;

typedef uint32_t TEE_BigIntFMMContext;

typedef uint32_t TEE_BigIntFMM;
#endif /* _TEE_INTERNAL_API_H_ */
