#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <stddef.h>

#ifndef __linux__ // I changed this because it was giving errors for android
#include <csignal>
#include <cstdio>
#include <cxxabi.h> // for __cxa_demangle
#include <dlfcn.h>  // for dladdr
#include <execinfo.h>
#include <unistd.h>
#include <SDL.h>
#endif

namespace LUS {
typedef void (*CrashHandlerCallback)(char*, size_t*);

class CrashHandler {
  public:
    CrashHandler();
    ~CrashHandler();
    CrashHandler(CrashHandlerCallback callback);

    void RegisterCallback(CrashHandlerCallback callback);
    void AppendLine(const char* str);
    void AppendStr(const char* str);
    void PrintCommon();

#ifdef __linux__
    void PrintRegisters(ucontext_t* ctx);
#elif _WIN32
    void PrintRegisters(CONTEXT* ctx);
    void PrintStack(CONTEXT* ctx);
#endif

  private:
    CrashHandlerCallback mCallback = nullptr;
    char* mOutBuffer = nullptr;
    static constexpr size_t gMaxBufferSize = 32768;
    size_t mOutBuffersize = 0;

    void AppendStrTrunc(const char* str);

    bool CheckStrLen(const char* str);
};
} // namespace LUS

#endif // CRASH_HANDLER_H
