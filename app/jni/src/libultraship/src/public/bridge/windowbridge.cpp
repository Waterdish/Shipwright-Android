#include "public/bridge/windowbridge.h"
#include "window/Window.h"
#include "Context.h"

extern "C" {

uint32_t WindowGetWidth() {
    return LUS::Context::GetInstance()->GetWindow()->GetWidth();
}

uint32_t WindowGetHeight() {
    return LUS::Context::GetInstance()->GetWindow()->GetHeight();
}

float WindowGetAspectRatio() {
    return LUS::Context::GetInstance()->GetWindow()->GetCurrentAspectRatio();
}

void WindowGetPixelDepthPrepare(float x, float y) {
    return LUS::Context::GetInstance()->GetWindow()->GetPixelDepthPrepare(x, y);
}

uint16_t WindowGetPixelDepth(float x, float y) {
    return LUS::Context::GetInstance()->GetWindow()->GetPixelDepth(x, y);
}
}
