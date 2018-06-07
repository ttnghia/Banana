//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//    ____________________________.""`-------`"".___________________________
//    /                                                                      \
//    \    This file is part of Banana - a general programming framework     /
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

#include <Banana/Utils/Timer.h>
#include <Banana/Utils/Formatters.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

using namespace Banana;

#define ARRAY_SIZE (1'000'000)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
String formatWithCommas(T value, Int precision = 2)
{
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

    std::stringstream ss;
    ss.str("");
    ss.imbue({ std::locale(), new Numpunct });
    ss << std::setprecision(precision) << std::fixed << value;
    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test formatWithCommas")
{
    ScopeTimer timer("Test NumberHelpers::formatWithCommas");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    // for(int i = 0; i < ARRAY_SIZE; ++i) {
    //     s = formatWithCommas(i);
    // }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Formatters::formatWithCommas with manually created static object")
{
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

    std::stringstream ss;
    ss.str("");
    ss.imbue({ std::locale(), new Numpunct });
    ss << std::setprecision(2) << std::fixed;

    ScopeTimer timer("Test NumberHelpers::formatWithCommas with manually created static object");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        ss.str("");
        ss << i;
        s = ss.str();;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Formatters::formatWithCommas with library created static object")
{
    ScopeTimer timer("Test NumberHelpers::formatWithCommas with library created static object");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        s = Formatters::Formatter<Formatters::FormatType::Scientific, 5, false>::format(i);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Formatters::formatWithCommas with library created static, multi-threaded object")
{
    ScopeTimer timer("Test NumberHelpers::formatWithCommas with library created static, multi-threaded object");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        s = Formatters::Formatter<Formatters::FormatType::Scientific, 5, true>::format(i);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Formatters::formatWithCommas with library created static, multi-threaded object, inline func")
{
    ScopeTimer timer("Test NumberHelpers::formatWithCommas with library created static, multi-threaded object, inline func");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        s = Formatters::toSciString(i);
    }
}

TEST_CASE("Print format")
{
    {
        std::cout << Formatters::toString(166146874614646) << std::endl;
        std::cout << Formatters::toString(166146874614646.615846416416100) << std::endl;

        std::cout << Formatters::toSciString(166146874614646) << std::endl;
        std::cout << Formatters::toSciString(166146874614646.615846416416100) << std::endl;
    }

    {
        size_t x = 12421352515151;
        std::cout << Formatters::toString(x) << std::endl;
        std::cout << Formatters::toSciString(x) << std::endl;
    }

    {
        Vec3f v = MathHelpers::vrand<Vec3f>();
        std::cout << Formatters::toString(v) << std::endl;
        std::cout << Formatters::toSciString(v) << std::endl;
    }

    {
        Mat3x3f m = LinaHelpers::randMatrix<3, float>();
        std::cout << Formatters::toString(m) << std::endl;
        std::cout << Formatters::toSciString(m) << std::endl;
    }
}
