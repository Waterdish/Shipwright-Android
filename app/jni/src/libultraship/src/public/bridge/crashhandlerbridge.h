#pragma once

#ifndef CRASHHANDLERBRIDGE_H
#define CRASHHANDLERBRIDGE_H

#include <stddef.h>

typedef void (*CrashHandlerCallback)(char*, size_t*);

#ifdef __cplusplus
extern "C" {
#endif

void CrashHandlerRegisterCallback(CrashHandlerCallback callback);

#ifdef __cplusplus
}
#endif

#endif
