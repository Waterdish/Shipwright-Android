#pragma once

#ifndef _CONSOLEVARIABLEBRIDGE_H
#define _CONSOLEVARIABLEBRIDGE_H

#include "stdint.h"
#include "libultraship/color.h"

#ifdef __cplusplus
#include <memory>
#include <config/ConsoleVariable.h>
std::shared_ptr<LUS::CVar> CVarGet(const char* name);

extern "C" {
#endif

int32_t CVarGetInteger(const char* name, int32_t defaultValue);
float CVarGetFloat(const char* name, float defaultValue);
const char* CVarGetString(const char* name, const char* defaultValue);
Color_RGBA8 CVarGetColor(const char* name, Color_RGBA8 defaultValue);
Color_RGB8 CVarGetColor24(const char* name, Color_RGB8 defaultValue);

void CVarSetInteger(const char* name, int32_t value);
void CVarSetFloat(const char* name, float value);
void CVarSetString(const char* name, const char* value);
void CVarSetColor(const char* name, Color_RGBA8 value);
void CVarSetColor24(const char* name, Color_RGB8 value);

void CVarRegisterInteger(const char* name, int32_t defaultValue);
void CVarRegisterFloat(const char* name, float defaultValue);
void CVarRegisterString(const char* name, const char* defaultValue);
void CVarRegisterColor(const char* name, Color_RGBA8 defaultValue);
void CVarRegisterColor24(const char* name, Color_RGB8 defaultValue);

void CVarClear(const char* name);

void CVarLoad();
void CVarSave();

#ifdef __cplusplus
};
#endif

#endif