#include <spdlog/spdlog.h>
#include "Utils/StringHelper.h"
#include "CrashHandler.h"
#include "Context.h"

#ifdef _WIN32
#include <windows.h>
#include <DbgHelp.h>

#include <inttypes.h>
#include <excpt.h>

#pragma comment(lib, "Dbghelp.lib")
#endif

namespace LUS {

#define WRITE_VAR_LINE(handler, varName, varValue) \
    handler->AppendStr(varName);                   \
    handler->AppendLine(varValue);
#define WRITE_VAR(handler, varName, varValue) \
    handler->AppendStr(varName);              \
    handler->AppendStr(varValue);

#define WRITE_VAR_LINE_M(varName, varValue) \
    AppendStr(varName);                     \
    AppendLine(varValue);
#define WRITE_VAR_M(varName, varValue) \
    AppendStr(varName);                \
    AppendStr(varValue);

bool CrashHandler::CheckStrLen(const char* str) {
    if (strlen(str) + mOutBuffersize >= gMaxBufferSize) {
        return false;
    }
    return true;
}

void CrashHandler::AppendStrTrunc(const char* str) {
    while (mOutBuffersize < gMaxBufferSize - 1) {
        mOutBuffer[mOutBuffersize++] = *str++;
    }
    mOutBuffer[mOutBuffersize] = '\0';
}

void CrashHandler::AppendStr(const char* str) {
    if (!CheckStrLen(str)) {
        AppendStrTrunc(str);
        return;
    }

    while (*str != '\0') {
        mOutBuffer[mOutBuffersize++] = *str++;
    }
}

void CrashHandler::AppendLine(const char* str) {
    AppendStr(str);
    mOutBuffer[mOutBuffersize++] = '\n';
}

/**
 * @brief Prints common data relevant to the crash
 *
 * @param buffer
 */
void CrashHandler::PrintCommon() {
    if (mCallback != nullptr) {
        mCallback(mOutBuffer, &mOutBuffersize);
    }

    SPDLOG_CRITICAL(mOutBuffer);
}

#if false//(__linux__)
void CrashHandler::PrintRegisters(ucontext_t* ctx) {
    char regbuffer[30];
    AppendLine("Registers:");
#if defined(__x86_64__)
    snprintf(regbuffer, std::size(regbuffer), "RAX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RAX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RDI: 0x%016llX", ctx->uc_mcontext.gregs[REG_RDI]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RSI: 0x%016llX", ctx->uc_mcontext.gregs[REG_RSI]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RDX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RDX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RCX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RCX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R8:  0x%016llX", ctx->uc_mcontext.gregs[REG_R8]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R9:  0x%016llX", ctx->uc_mcontext.gregs[REG_R9]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R10: 0x%016llX", ctx->uc_mcontext.gregs[REG_R10]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R11: 0x%016llX", ctx->uc_mcontext.gregs[REG_R11]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RSP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RSP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RBX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RBX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RBP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RBP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R12: 0x%016llX", ctx->uc_mcontext.gregs[REG_R12]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R13: 0x%016llX", ctx->uc_mcontext.gregs[REG_R13]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R14: 0x%016llX", ctx->uc_mcontext.gregs[REG_R14]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R15: 0x%016llX", ctx->uc_mcontext.gregs[REG_R15]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RIP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RIP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EFL: 0x%016llX", ctx->uc_mcontext.gregs[REG_EFL]);
    AppendLine(regbuffer);
#elif defined(__i386__)
    snprintf(regbuffer, std::size(regbuffer), "EDI: 0x%08lX", ctx->uc_mcontext.gregs[REG_EDI]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "ESI: 0x%08lX", ctx->uc_mcontext.gregs[REG_ESI]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EBP: 0x%08lX", ctx->uc_mcontext.gregs[REG_EBP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "ESP: 0x%08lX", ctx->uc_mcontext.gregs[REG_ESP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EBX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EBX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EDX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EDX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "ECX: 0x%08lX", ctx->uc_mcontext.gregs[REG_ECX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EAX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EAX]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EIP: 0x%08lX", ctx->uc_mcontext.gregs[REG_EIP]);
    AppendLine(regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EFL: 0x%08lX", ctx->uc_mcontext.gregs[REG_EFL]);
    AppendLine(regbuffer);
#endif
}

static void ErrorHandler(int sig, siginfo_t* sigInfo, void* data) {
    std::shared_ptr<CrashHandler> crashHandler = LUS::Context::GetInstance()->GetCrashHandler();
    char intToCharBuffer[16];

    std::array<void*, 4096> arr;
    ucontext_t* ctx = static_cast<ucontext_t*>(data);
    constexpr size_t nMaxFrames = arr.size();
    size_t size = backtrace(arr.data(), nMaxFrames);
    char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

    snprintf(intToCharBuffer, sizeof(intToCharBuffer), "Signal: %i", sig);
    crashHandler->AppendLine(intToCharBuffer);

    switch (sig) {
        case SIGILL:
            crashHandler->AppendLine("ILLEGAL INSTRUCTION");
            break;
        case SIGABRT:
            crashHandler->AppendLine("ABORT");
            break;
        case SIGFPE:
            crashHandler->AppendLine("ERRONEUS ARITHEMETIC OPERATION");
            break;
        case SIGSEGV:
            crashHandler->AppendLine("INVALID ACCESS TO STORAGE");
            break;
    }

    crashHandler->PrintRegisters(ctx);

    crashHandler->AppendLine("Traceback:");
    for (size_t i = 1; i < size; i++) {
        Dl_info info;
        int gotAddress = dladdr(arr[i], &info);
        std::string functionName(symbols[i]);

        if (gotAddress != 0 && info.dli_sname != nullptr) {
            FILE* pipe;
            int32_t status;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            const char* nameFound = info.dli_sname;

            if (status == 0) {
                nameFound = demangled;
            }

            functionName = StringHelper::Sprintf("%s (+0x%X)", nameFound, (char*)arr[i] - (char*)info.dli_saddr);
            free(demangled);
        }
        snprintf(intToCharBuffer, sizeof(intToCharBuffer), "%i ", (int)i);
        WRITE_VAR_LINE(crashHandler, intToCharBuffer, functionName.c_str());
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (LUS::Context::GetInstance()->GetName() + " has crashed").c_str(),
                             (LUS::Context::GetInstance()->GetName() +
                              " has crashed. Please upload the logs to the support channel in discord.")
                                 .c_str(),
                             nullptr);
    free(symbols);
    crashHandler->PrintCommon();

    LUS::Context::GetInstance()->GetLogger()->flush();
    spdlog::shutdown();
    exit(1);
}

static void ShutdownHandler(int sig, siginfo_t* sigInfo, void* data) {
    exit(1);
}

#elif _WIN32

#if defined(_WIN32) && !defined(_WIN64)
#define WINDOWS_32_BIT
#endif

void CrashHandler::PrintRegisters(CONTEXT* ctx) {
    AppendLine("Registers: ");
    char regBuff[25];
#if defined(_M_AMD64)
    sprintf_s(regBuff, std::size(regBuff), "RAX: 0x%016llX", ctx->Rax);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RCX: 0x%016llX", ctx->Rcx);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RDX: 0x%016llX", ctx->Rdx);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RBX: 0x%016llX", ctx->Rbx);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RSP: 0x%016llX", ctx->Rsp);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RBP: 0x%016llX", ctx->Rbp);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RSI: 0x%016llX", ctx->Rsi);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RDI: 0x%016llX", ctx->Rdi);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R9:  0x%016llX", ctx->R9);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R10: 0x%016llX", ctx->R10);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R11: 0x%016llX", ctx->R11);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R12: 0x%016llX", ctx->R12);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R13: 0x%016llX", ctx->R13);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R14: 0x%016llX", ctx->R14);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "R15: 0x%016llX", ctx->R15);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "RIP: 0x%016llX", ctx->Rip);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EFLAGS: 0x%08lX", ctx->EFlags);
    AppendLine(regBuff);
#elif WINDOWS_32_BIT
    sprintf_s(regBuff, std::size(regBuff), "EDI: 0x%08lX", ctx->Edi);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "ESI: 0x%08lX", ctx->Esi);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EBX: 0x%08lX", ctx->Ebx);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "ECX: 0x%08lX", ctx->Ecx);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EAX: 0x%08lX", ctx->Eax);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EBP: 0x%08lX", ctx->Ebp);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "ESP: 0x%08lX", ctx->Esp);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EFLAGS: 0x%08lX", ctx->EFlags);
    AppendLine(regBuff);

    sprintf_s(regBuff, std::size(regBuff), "EIP: 0x%08lX", ctx->Eip);
    AppendLine(regBuff);
#endif
}

void CrashHandler::PrintStack(CONTEXT* ctx) {
    BOOL result;
    HANDLE process;
    HANDLE thread;
    HMODULE hModule;
    ULONG frame;
    DWORD64 displacement;
    DWORD disp;

#if defined(_M_AMD64)
    STACKFRAME64 stack;
    memset(&stack, 0, sizeof(STACKFRAME64));
#elif WINDOWS_32_BIT
    STACKFRAME stack;
    memset(&stack, 0, sizeof(STACKFRAME));
    stack.AddrPC.Offset = (*ctx).Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
#endif

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(TCHAR)];
    char module[512];

    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;

    CONTEXT ctx2;
    memcpy(&ctx2, ctx, sizeof(CONTEXT));

    PrintRegisters(&ctx2);

    process = GetCurrentProcess();
    thread = GetCurrentThread();

    SymSetOptions(SYMOPT_NO_IMAGE_SEARCH | SYMOPT_IGNORE_IMAGEDIR);
    SymInitialize(process, "debug", true);

    constexpr DWORD machineType =
#if defined(_M_AMD64)
        IMAGE_FILE_MACHINE_AMD64;
#elif WINDOWS_32_BIT
        IMAGE_FILE_MACHINE_I386;
#endif

    displacement = 0;
    for (frame = 0;; frame++) {
        result = StackWalk(machineType, process, thread, &stack, &ctx2, nullptr, SymFunctionTableAccess,
                           SymGetModuleBase, nullptr);
        if (!result) {
            break;
        }
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, symbol);
#if defined(_M_AMD64)
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
#elif WINDOWS_32_BIT
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
#endif
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, &line)) {
            char lineNumberStr[16];
            sprintf_s(lineNumberStr, sizeof(lineNumberStr), "Line: %d", line.LineNumber);
            AppendStr(symbol->Name);
            AppendStr(" in ");
            AppendStr(line.FileName);
            AppendStr(lineNumberStr);
            // SPDLOG_CRITICAL("{} in {}: line: {}: ", symbol->Name, line.FileName, line.LineNumber);
        } else {
            char addrString[20];
            sprintf_s(addrString, std::size(addrString), "0x%016llX", symbol->Address);
            WRITE_VAR_M("At ", symbol->Name);
            WRITE_VAR_LINE_M("Addr: ", addrString);
            // SPDLOG_CRITICAL("at {}, addr 0x{}", symbol->Name, addrString);
            hModule = nullptr;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR)(stack.AddrPC.Offset), &hModule);

            if (hModule != nullptr) {
                GetModuleFileNameA(hModule, module, sizeof(module));
            }
            WRITE_VAR_LINE_M("In: ", module);
            // SPDLOG_CRITICAL("In {}", module);
        }
    }
    PrintCommon();
    LUS::Context::GetInstance()->GetLogger()->flush();
    spdlog::shutdown();
}

extern "C" LONG seh_filter(struct _EXCEPTION_POINTERS* ex) {
    char exceptionString[20];
    std::shared_ptr<CrashHandler> crashHandler = LUS::Context::GetInstance()->GetCrashHandler();

    snprintf(exceptionString, std::size(exceptionString), "0x%x", ex->ExceptionRecord->ExceptionCode);

    WRITE_VAR_LINE(crashHandler, "Exception: ", exceptionString);
    crashHandler->PrintStack(ex->ContextRecord);
    MessageBoxA(nullptr,
                (LUS::Context::GetInstance()->GetName() +
                 " has crashed. Please upload the logs to the support channel in discord.")
                    .c_str(),
                "Crash", MB_OK | MB_ICONERROR);

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif

CrashHandler::CrashHandler() {
    mOutBuffer = new char[gMaxBufferSize];
#if false//defined(__linux__)
    struct sigaction action;
    struct sigaction shutdownAction;

    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = ErrorHandler;

    sigaction(SIGILL, &action, nullptr);
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGFPE, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);

    shutdownAction.sa_flags = SA_SIGINFO;
    shutdownAction.sa_sigaction = ShutdownHandler;
    sigaction(SIGINT, &shutdownAction, nullptr);
    sigaction(SIGTERM, &shutdownAction, nullptr);
    sigaction(SIGQUIT, &shutdownAction, nullptr);
    sigaction(SIGKILL, &shutdownAction, nullptr);
#elif _WIN32
    SetUnhandledExceptionFilter(seh_filter);
#endif
}

CrashHandler::CrashHandler(CrashHandlerCallback callback) {
    mCallback = callback;
    CrashHandler();
}

CrashHandler::~CrashHandler() {
    SPDLOG_TRACE("destruct crash handler");
}

void CrashHandler::RegisterCallback(CrashHandlerCallback callback) {
    mCallback = callback;
}
} // namespace LUS