#include "crashhandlerbridge.h"
#include "Context.h"

void CrashHandlerRegisterCallback(CrashHandlerCallback callback) {
    LUS::Context::GetInstance()->GetCrashHandler()->RegisterCallback(callback);
}
