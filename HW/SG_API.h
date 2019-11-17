
#ifndef _SG_API_H_
#define _SG_API_H_ 

#ifdef __cplusplus
	extern "C"{
#endif

/*ECC / SM2*/
#define ECC_MAX_XCOORDINATE_BITS_LEN 512
#define ECC_MAX_YCOORDINATE_BITS_LEN 512
#define ECC_MAX_MODULUS_BITS_LEN 512

//typedef unsigned int ULONG;
typedef unsigned char BYTE;

/*ECC/ SM2��Կ���ݽṹ*/
typedef struct Struct_ECCPUBLICKEYBLOB{
	unsigned int 		BitLen;
	BYTE	   XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	BYTE	   YCoordinate[ECC_MAX_YCOORDINATE_BITS_LEN/8];
}ECCPUBLICKEYBLOB, *PECCPUBLICKEYBLOB;

/*ECC/ SM2˽Կ���ݽṹ*/
typedef struct Struct_ECCPRIVATEKEYBLOB{
	unsigned int	   BitLen;
	BYTE	   PrivateKey[ECC_MAX_MODULUS_BITS_LEN/8];
}ECCPRIVATEKEYBLOB, *PECCPRIVATEKEYBLOB;

/*ECC /SM2 �������ݽṹ*/
typedef struct Struct_ECCCIPHERBLOB{
	BYTE  XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	BYTE  YCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	BYTE  Hash[32];
	unsigned int CipherLen;
	BYTE  Cipher[1];
}ECCCIPHERBLOB, *PECCCIPHERBLOB;

/*ECC /SM2 ǩ�����ݽṹ*/
typedef struct Struct_ECCSIGNATUREBLOB{
	BYTE r[ECC_MAX_XCOORDINATE_BITS_LEN/8];
	BYTE s[ECC_MAX_XCOORDINATE_BITS_LEN/8];
}ECCSIGNATUREBLOB, *PECCSIGNATUREBLOB;



/*��������*/
#define SGD_TRUE		0x00000001
#define SGD_FALSE		0x00000000

#define SGD_SMS4_ECB	0x00002001
#define SGD_SMS4_CBC	0x00002002

#define SGD_SM2         0x00020100

#define SGD_SM3			0x00000001

#define  EnDataMaxLen   0x1000
#define  SignDataMaxLen  0x200000

int SG_SymEnc(unsigned int algorithm ,
		unsigned char *pkey,
		unsigned int pkeyLen,
		unsigned char *pIV,
		unsigned int pIVLen,
		unsigned char *pDataIn,
		unsigned int pDataInLen,
		unsigned char *pDataOut,
		unsigned int* pDataOutLen);
int SG_SymDec(unsigned int algorithm ,
		unsigned char *pkey,
		unsigned int pkeyLen,
		unsigned char *pIV,
		unsigned int pIVLen,
		unsigned char *pDataIn,
		unsigned int pDataInLen,
		unsigned char *pDataOut,
		unsigned int* pDataOutLen);

int SG_GenKeyPair(unsigned int algorithm,
		ECCPUBLICKEYBLOB *PublicKey,
		ECCPRIVATEKEYBLOB *PrivateKey);
int SG_SM2Enc(ECCPUBLICKEYBLOB *pECCPubKeyBlob,
		unsigned char *pDataIn,
		unsigned int pDataInLen,
		unsigned char *pCipherText,
		unsigned int *CipherTextLen);
int SG_SM2Dec(ECCPRIVATEKEYBLOB*pECCPriKeyBlob,
		unsigned char* pCipherText,
		unsigned int pCipherTextLen,
		unsigned char *pbPlainText,
		unsigned int *pulPlainTextLen);
int SG_SM2Sign(unsigned char flag,
		ECCPUBLICKEYBLOB *pECCPubKeyBlob,
		ECCPRIVATEKEYBLOB *pECCPriKeyBlob,
		unsigned char *pID,
		unsigned int IDLen,
		unsigned char *pDataIn,
		unsigned int pDataInLen,
		PECCSIGNATUREBLOB pSignature);
int SG_SM2Verify(unsigned char flag,
		ECCPUBLICKEYBLOB *pECCPubKeyBlob,
		unsigned char *pID,
		unsigned int IDLen,
		unsigned char *pDataIn,
		unsigned int pDataInLen,
		PECCSIGNATUREBLOB pSignature);

int SG_Digest(ECCPUBLICKEYBLOB *pECCPubKeyBlob,unsigned char *pID,unsigned int pIDLen,unsigned char *pDataIn,unsigned int pDataInLen,unsigned char *pDataOut,unsigned int* pDataOutLen);

 int WhiteBoxInit(char *filename);
 int GetWhiteContext(char *filename,unsigned char* fileContext,int fileContextLen,int* retLen);
 int WhiteBoxEncrypt(char *filename,unsigned char *plaindata,unsigned int plaindataLen,unsigned char *cipherBuf,unsigned int cipheBufLen,unsigned int *cipherLen);
 int WhiteBoxDecrypt(char* filename,unsigned char *cipher,unsigned int cipherLen,unsigned char *plainBuf,unsigned int plainBufLen,unsigned int *plainLen);
 
#define SAR_OK                      0x00000000	/*�ɹ�*/
#define SAR_FAIL                    0x0A000001	/*ʧ��*/
#define SAR_NOTSUPPORTYETERR        0x0A000003  /*��֧��*/
#define SAR_INVALIDPARAMERR         0x0A000006  /*��Ч�Ĳ���*/
#define SAR_MEMORYERR               0x0A00000E  /*�ڴ����*/
#define SAR_INDATALENERR            0x0A000010  /*�������ݳ��ȴ���*/
#define SAR_HASHERR                 0x0A000014  /*��ϣ�������*/
#define SAR_GENKEYERR               0x0A000015  /*������Կ�Դ���*/
#define SAR_MODULUSLENERR           0x0A000016  /*�ǶԳ���Կģ������*/
#define SAR_ENCERR                  0x0A000018  /*�ǶԳƼ��ܴ���*/
#define SAR_DECERR                  0x0A000019  /*�ǶԳƽ��ܴ���*/
#define SAR_HASHNOTEQUALERR         0x0A00001A  /*��ǩ��ͨ��*/
#define SAR_BUFFER_TOO_SMALL        0x0A000020  /*����������*/


#ifdef __cplusplus
}
#endif

#endif