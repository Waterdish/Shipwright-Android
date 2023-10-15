#pragma once

#ifndef WINDOWBRIDGE_H
#define WINDOWBRIDGE_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t WindowGetWidth();
uint32_t WindowGetHeight();
float WindowGetAspectRatio();
void WindowGetPixelDepthPrepare(float x, float y);
uint16_t WindowGetPixelDepth(float x, float y);

#ifdef __cplusplus
};
#endif

#endif