/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _CA_SST_H_
#define _CA_SST_H_

#define UUID_SST    { 0x11223344, 0xacbd, 0xcedf,   \
                    { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 } }

uint32_t sst_init(void);
uint32_t sst_add_item(const char *name, const uint8_t *data,
                      uint32_t data_len, uint32_t type, uint32_t flag);
uint32_t sst_get_item(const char *name, uint8_t *data, uint32_t *data_len, uint32_t *type);
uint32_t sst_destroy(void);

#endif /* _CA_SST_H_ */
