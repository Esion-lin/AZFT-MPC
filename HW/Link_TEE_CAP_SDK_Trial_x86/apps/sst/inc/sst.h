/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TST_H_
#define _TST_H_

#define CMD_ADD_ITEM       (0x1)
#define CMD_READ_ITEM      (0x2)
#define CMD_DEL_ITEM       (0x3)

#define UUID_SST    { 0x11223344, 0xacbd, 0xcedf,   \
                    { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x79 } }

#define TA_DBG(format, a...)        \
    do {                            \
        TEE_Print(format, ##a);     \
        TEE_DumpMem(format, ##a);   \
    } while(0)

#endif /* _TST_H_ */
