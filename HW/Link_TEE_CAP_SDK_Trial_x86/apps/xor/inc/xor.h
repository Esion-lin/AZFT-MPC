/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _XOR_H_
#define _XOR_H_

#define UUID_XOR    { 0x13245768, 0xacbd, 0xcedf,   \
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


#define CMD_TEST_MAC        (0x009)

#define SYM_KEY_SIZE    256
#define ASYM_KEY_SIZE   256
#define ASYM_KEY_FOR_SIGN_SIZE   512

#define PUBLIC_TYPE_SIGN 	(0x0000)
#define PUBLIC_TYPE_CIPHER 	(0x0001)  
#define PUBLIC_TYPE_SYM     (0x0003)                       
#endif /* _XOR_H_ */
