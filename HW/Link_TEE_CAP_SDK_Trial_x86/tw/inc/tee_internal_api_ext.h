/**
 * Copyright (C) 2018 The YunOS IoT Project. All rights reserved.
 */

#ifndef _TEE_INTERNAL_API_EXT_H_
#define _TEE_INTERNAL_API_EXT_H_

#include "tee_types.h"

int TEE_Print(const char *fmt, ...);
TEE_Result TEE_GetDeviceUID(uint8_t *uid, uint32_t *size);

#endif /* _TEE_INTERNAL_API_EXT_H_ */
