//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/12/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(_WIN32) || defined(_WIN64)
#   define __Banana_Windows__
#elif defined(__APPLE__)
#   define __Banana_Apple__
#   define __Banana_MacOS__
#elif defined(linux) || defined(__linux__)
#   define __Banana_Linux__
#endif

#ifndef __BNN_UnUsed_Variable
#   define __BNN_UnUsed_Variable(x) ((void)x)
#endif

#ifdef __Banana_Windows__
#   define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#   define NOMINMAX
#   define __func__ __FUNCTION__
#include <Windows.h>
#include <exception>
#include <string>

inline void throwIfFailed(HRESULT hr)
{
    if(FAILED(hr))
    {
        throw std::exception(std::to_string(hr).c_str());
    }
}

#endif  // __Banana_Windows__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PrintLine \
{ \
    printf("%d: %s\n", __LINE__, __FILE__); \
    fflush(stdout); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PrintExp(x) \
{ \
    std::stringstream ss; \
    ss << "Printing at line: " << __LINE__ << ", file: " << __FILE__ << ":" << std::endl; \
    ss << "    " << #x << ": " << x; \
    printf("%s\n", ss.str().c_str()); \
    fflush(stdout); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_PrintLocation \
{ \
   std::stringstream ss; \
    ss << "Function: " << __func__ << std::endl; \
    ss << "Line: " << __LINE__ << ", file: " << __FILE__; \
    printf("%s\n", ss.str().c_str()); \
    fflush(stdout); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef __BNN_Info
#   define __BNN_Info(info) \
    { \
        fprintf(stderr, info); \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef __BNN_Err
#   define __BNN_Err(err) \
    { \
        fprintf(stderr, err); \
        __BNN_PrintLocation \
    }
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_Die(err) \
{ \
    __BNN_Err(err) \
    exit(EXIT_FAILURE); \
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_Assert(condition) \
{ \
    if(!(condition)) \
    { \
        std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
        printf("%s\n", erMsg.c_str()); \
        __BNN_PrintLocation \
        exit(EXIT_FAILURE); \
    } \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_AssertMsg(condition, msg) \
{ \
    if(!(condition)) \
    { \
        std::string erMsg = std::string("Assertion failed: ") + std::string(#condition); \
        std::string rsMsg = std::string("Reason: ") + std::string(msg); \
        printf("%s\n%s\n", erMsg.c_str(), rsMsg.c_str()); \
        __BNN_PrintLocation \
        exit(EXIT_FAILURE); \
    } \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_CalledToUmimplementedFunc \
{ \
    __BNN_Die("Called function is unimplemented.") \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_CalledToWrongPlace \
{ \
    __BNN_Die("This function should not be reached.") \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_DeniedCallToBaseFunc \
{ \
    __BNN_Die("This function should not be called from base class.") \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_Die_UnknownErr \
{ \
    __BNN_Die("An unknown error has occured...") \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// test
#define __BNN_PerformanceTest_Begin(funcName) \
{ \
        Timer test_timer; \
        test_timer.tick(); \
        std::string strName(funcName);


#define __BNN_PerformanceTest_End \
    test_timer.tock(); \
    printf("Test %s finished. Time: %s\n", strName.c_str(), \
           test_timer.get_run_time().c_str()); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// data macros
#define __BNN_AssertAprx(a, b, threshold) \
{ \
    __BNN_AssertMsg(fabs(a-b) < threshold,  "Numbers are not equal."); \
}
