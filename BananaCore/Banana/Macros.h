//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(_WIN32) || defined(_WIN64)
#   define __BANANA_WINDOWS__
#elif defined(__APPLE__)
#   define __BANANA_APPLE__
#   define __BANANA_MACX__
#elif defined(linux) || defined(__linux__)
#   define __BANANA_LINUX__
#endif

#ifndef __BNN_UNUSED
#   define __BNN_UNUSED(x) ((void)x)
#endif

#ifdef __BANANA_WINDOWS__
//  Exclude rarely-used stuff from Windows headers
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   define __func__ __FUNCTION__
#   include <Windows.h>
#   include <exception>
#   include <string>

inline void throwIfFailed(HRESULT hr)
{
    if(FAILED(hr))
    {
        throw std::exception(std::to_string(hr).c_str());
    }
}

#if defined(DEBUG) || defined(_DEBUG)
#  define __BANANA_DEBUG__
#endif

#endif  // __BANANA_WINDOWS__

#ifdef __BANANA_WINDOWS__
#   define __BNN_SPRINT sprintf_s
#else
#   define __BNN_SPRINT sprintf
#endif

#include <csignal>
#define __BANANA_EARLY_TERMINATION \
    {                              \
        std::raise(SIGTERM);       \
    }
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Constants
#ifndef M_PI
# define M_PI    3.1415926535897932384626433832795028841971694
#endif
# define M_PIr   Real(3.1415926535897932384626433832795028841971694)

#define MEpsilon std::numeric_limits<Real>::epsilon()
#define Tiny     std::numeric_limits<Real>::min()
#define Huge     std::numeric_limits<Real>::max()

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Utility
#define _STR(x)            #x
#define __BNN_TO_STRING(x) _STR(x)

#ifdef __BANANA_WINDOWS__
#define __BNN_FORCE_INLINE __forceinline
#else
#define __BNN_FORCE_INLINE __attribute__((always_inline))
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_COMPILER_MESSAGE(msg) \
    __pragma(message("+++>" msg))

#define __BNN_TODO_MSG \
    __pragma(message("+++>TODO: => " __FILE__ "(" __BNN_TO_STRING(__LINE__) ") "))

#define __BNN_TODO(msg) \
    __pragma(message("+++>TODO: " msg " => " __FILE__ "(" __BNN_TO_STRING(__LINE__) ") "))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PRINT_LINE                        \
    {                                           \
        printf("%d: %s\n", __LINE__, __FILE__); \
        fflush(stdout);                         \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PRINT_EXP(x)                                                                    \
    {                                                                                         \
        std::stringstream ss;                                                                 \
        ss << "Printing at line: " << __LINE__ << ", file: " << __FILE__ << ":" << std::endl; \
        ss << "    " << #x << ": " << x;                                                      \
        printf("%s\n", ss.str().c_str());                                                     \
        fflush(stdout);                                                                       \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PRINT_LOCATION                                  \
    {                                                         \
        std::stringstream ss;                                 \
        ss << "Function: " << __func__ << std::endl;          \
        ss << "Line: " << __LINE__ << ", file: " << __FILE__; \
        printf("%s\n", ss.str().c_str());                     \
        fflush(stdout);                                       \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef __BNN_INFO
#   define __BNN_INFO(info)    \
    {                          \
        fprintf(stderr, info); \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef __BNN_ERROR
#   define __BNN_ERROR(err)   \
    {                         \
        fprintf(stderr, err); \
        __BNN_PRINT_LOCATION  \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_DIE(err)      \
    {                       \
        __BNN_ERROR(err)    \
        exit(EXIT_FAILURE); \
    }
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __BANANA_DEBUG__
#  define __BNN_ASSERT(condition)                                                            \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
            printf("%s\n", erMsg.c_str());                                                   \
            __BNN_PRINT_LOCATION                                                             \
            DebugBreak();                                                                    \
        }                                                                                    \
    }
#else
#  define __BNN_ASSERT(condition)                                                            \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
            printf("%s\n", erMsg.c_str());                                                   \
            __BNN_PRINT_LOCATION                                                             \
            exit(EXIT_FAILURE);                                                              \
        }                                                                                    \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __BANANA_DEBUG__
#  define __BNN_ASSERT_MSG(condition, msg)                                                   \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
            std::string rsMsg = std::string("Reason: ") + std::string(msg);                  \
            printf("%s\n%s\n", erMsg.c_str(), rsMsg.c_str());                                \
            __BNN_PRINT_LOCATION                                                             \
            DebugBreak();                                                                    \
        }                                                                                    \
    }
#else
#  define __BNN_ASSERT_MSG(condition, msg)                                                   \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
            std::string rsMsg = std::string("Reason: ") + std::string(msg);                  \
            printf("%s\n%s\n", erMsg.c_str(), rsMsg.c_str());                                \
            __BNN_PRINT_LOCATION                                                             \
            exit(EXIT_FAILURE);                                                              \
        }                                                                                    \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_CHECK_ERROR(condition, msg)                                                    \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
            std::string rsMsg = std::string("Reason: ") + std::string(msg);                  \
            printf("%s\n%s\n", erMsg.c_str(), rsMsg.c_str());                                \
            __BNN_PRINT_LOCATION                                                             \
        }                                                                                    \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_UNIMPLEMENTED_FUNC          __BNN_DIE("Called function is unimplemented.")
#define __BNN_CALLED_TO_WRONG_PLACE       __BNN_DIE("This function should not be reached.")
#define __BNN_DENIED_CALL_TO_BASE_CLASS   __BNN_DIE("This function should not be called from base class.")
#define __BNN_DENIED_SWITCH_DEFAULT_VALUE __BNN_DIE("Invalid default value in switch statement.")
#define __BNN_DIE_UNKNOWN_ERROR           __BNN_DIE("An unknown error has occured...")

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// test
#define __BNN_PERORMANCE_TEST_BEGIN \
    Banana::Timer testTimer;        \
    testTimer.tick();


#define __BNN_PERORMANCE_TEST_END(funcName) \
    testTimer.tock();                       \
    printf("Test %s finished. Time: %s\n", funcName, testTimer.getRunTime().c_str());

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// data macros
#define __BNN_ASSERT_EQUAL(a, b)                     __BNN_ASSERT_MSG(a == b, "Numbers are not equal.");
#define __BNN_ASSERT_APPROX_NUMBERS(a, b, threshold) __BNN_ASSERT_MSG(threshold > fabs(a - b), "Numbers are not equal.");
