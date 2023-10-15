#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void luslog(const char* file, int32_t line, int32_t logLevel, const char* msg);
void lusprintf(const char* file, int32_t line, int32_t logLevel, const char* fmt, ...);
#ifdef __cplusplus
}
#endif

#define LUSLOG_LEVEL_TRACE 0
#define LUSLOG_LEVEL_DEBUG 1
#define LUSLOG_LEVEL_INFO 2
#define LUSLOG_LEVEL_WARN 3
#define LUSLOG_LEVEL_ERROR 4
#define LUSLOG_LEVEL_CRITICAL 5
#define LUSLOG_LEVEL_OFF 6

#define LUSLOG(level, msg, ...) lusprintf(__FILE__, __LINE__, level, msg, __VA_ARGS__)
#define LUSLOG_TRACE(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_TRACE, msg, __VA_ARGS__)
#define LUSLOG_DEBUG(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_DEBUG, msg, __VA_ARGS__)
#define LUSLOG_INFO(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_INFO, msg, __VA_ARGS__)
#define LUSLOG_WARN(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_WARN, msg, __VA_ARGS__)
#define LUSLOG_ERROR(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_ERROR, msg, __VA_ARGS__)
#define LUSLOG_CRITICAL(msg, ...) lusprintf(__FILE__, __LINE__, LUSLOG_LEVEL_CRITICAL, msg, __VA_ARGS__)
