#ifndef CPR_DEFINES_H
#define CPR_DEFINES_H

#include <utility>

#define CRL_FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

#if defined(__APPLE__)
    #include <unistd.h>
    #define CRL_SLEEP(x) sleep(x)
#elif defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
    #include <Windows.h>
    #define CRL_SLEEP(x) Sleep(x * 1000)
#endif

#endif
