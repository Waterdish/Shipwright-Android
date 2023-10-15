#pragma once

#ifndef OS_H
#define OS_H

#include "stdint.h"
#include "libultra/controller.h"
#include "libultra/message.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t osContInit(OSMesgQueue* mq, uint8_t* controllerBits, OSContStatus* status);
int32_t osContStartReadData(OSMesgQueue* mesg);
void osContGetReadData(OSContPad* pad);

uint64_t osGetTime(void);
uint32_t osGetCount(void);

#ifdef __cplusplus
};
#endif

#endif