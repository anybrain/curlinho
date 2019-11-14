#ifndef CPR_DEFINES_H
#define CPR_DEFINES_H

#include <utility>
#include "plog/Log.h"

#define CRL_FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

/// change this define to use your logger!
#define CRL_LOG PLOG_DEBUG
#define CRL_LOG_ERROR PLOG_ERROR

#if defined(__APPLE__)
    #include <unistd.h>
    #define CRL_SLEEP(x) sleep(x)
#elif defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
    #include <Windows.h>
    #define CRL_SLEEP(x) Sleep(x * 1000)
#endif

#endif
