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

// Libmorton - Common helper methods needed in Morton encoding/decoding

#include <stdint.h>
#if _MSC_VER
#include <intrin.h>
#endif

template<typename morton>
inline bool findFirstSetBit(const morton x, unsigned long* firstbit_location)
{
#if _MSC_VER && !_WIN64
    // 32 BIT on 32 BIT
    if(sizeof(morton) <= 4)
    {
        return _BitScanReverse(firstbit_location, x) != 0;
    }
    // 64 BIT on 32 BIT
    else
    {
        *firstbit_location = 0;
        if(_BitScanReverse(firstbit_location, (x >> 32)))            // check first part
        {
            firstbit_location += 32;
            return true;
        }
        return _BitScanReverse(firstbit_location, (x & 0xFFFFFFFF)) != 0;
    }
#elif  _MSC_VER && _WIN64
    // 32 or 64 BIT on 64 BIT
    return _BitScanReverse64(firstbit_location, x) != 0;
#elif __GNUC__
    if(x == 0)
    {
        return false;
    }
    else
    {
        *firstbit_location = static_cast<unsigned long>((sizeof(morton) * 8) - __builtin_clzll(x));
        return true;
    }
#endif
}