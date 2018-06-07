//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <random>

#include <Banana/Setup.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Formatters
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum class FormatType
{
    CommaSeparated,
    Scientific
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<FormatType formatType = FormatType::CommaSeparated, Int precision = 2, bool bMultiThreaded = false>
class Formatter
{
public:
    Formatter() = delete;

    template<class T> static String        format(T x);
    template<Int N, class T> static String format(const VecX<N, T>& vec);
    template<Int N, class T> static String format(const MatXxX<N, T>& mat, bool breakLine = false);

private:
    static void init();
    ////////////////////////////////////////////////////////////////////////////////
    struct Numpunct : public std::numpunct<char>
    {
protected:
        virtual char do_thousands_sep() const
        {
            return ',';
        }

        virtual String do_grouping() const
        {
            return "\03";
        }
    };

    static inline bool                      s_bInit { false };
    static inline std::stringstream         s_ss {};
    static inline ParallelObjects::SpinLock s_lock {};
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
String toString(T value)
{
    return Formatter<FormatType::CommaSeparated, 2, true>::format(value);
}

template<class T>
String toString7(T value)
{
    return Formatter<FormatType::CommaSeparated, 7, true>::format(value);
}

template<class T>
String toString5(T value)
{
    return Formatter<FormatType::CommaSeparated, 5, true>::format(value);
}

template<class T>
String toString10(T value)
{
    return Formatter<FormatType::CommaSeparated, 10, true>::format(value);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
String toSciString(T value)
{
    return Formatter<FormatType::Scientific, 5, true>::format(value);
}

template<class T>
String toSciString2(T value)
{
    return Formatter<FormatType::Scientific, 2, true>::format(value);
}

template<class T>
String toSciString7(T value)
{
    return Formatter<FormatType::Scientific, 7, true>::format(value);
}

template<class T>
String toSciString10(T value)
{
    return Formatter<FormatType::Scientific, 10, true>::format(value);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String byteToHex(const char* arrBytes, size_t arrSize, Int width /*= 2*/)
{
    if(width < 2) { width = 2; }
    std::stringstream s_ss;
    for(size_t i = 0; i < arrSize; ++i) {
        s_ss << std::hex << std::uppercase << std::setw(width) << static_cast<Int>(arrBytes[i]);
    }
    return s_ss.str();
}

template<class T>
inline String byteToHex(const Vector<T>& vecBytes, Int width /*= 2*/)
{
    return byteToHex((const char*)vecBytes.data(), vecBytes.size() * sizeof(T), width);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace namespace Banana::Formatters
