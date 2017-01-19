//------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#   define __Banana_Windows__
#elif defined(__APPLE__)
#   define __Banana_Apple__
#   ifndef JET_IOS
#       define __Banana_MacOS__
#   endif
#elif defined(linux) || defined(__linux__)
#   define __Banana_Linux__
#endif

#ifndef __UnUsed_Variable
#   define __UnUsed_Variable(x) ((void)x)
#endif

#ifdef __Banana_Windows__

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
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
