#pragma once

#ifndef AUDIOBRIDGE_H
#define AUDIOBRIDGE_H

#include "stdint.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t AudioPlayerBuffered(void);
int32_t AudioPlayerGetDesiredBuffered(void);
void AudioPlayerPlayFrame(const uint8_t* buf, size_t len);

#ifdef __cplusplus
};
#endif

#endif
