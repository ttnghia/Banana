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

using namespace Banana;

#define ARRAY_SIZE (10'000'000)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test + operator")
{
    ScopeTimer timer("Test addition operator");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        s = String("This is a stringgggggggggggggggggggg 1") +
            String("This is a stringgggggggggggggggggggg 2") +
            String("This is a stringgggggggggggggggggggg 3") +
            String("This is a stringgggggggggggggggggggg 4") +
            String("This is a stringgggggggggggggggggggg 5");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test += operator")
{
    ScopeTimer timer("Test addition operator");
    ////////////////////////////////////////////////////////////////////////////////
    String s;
    for(int i = 0; i < ARRAY_SIZE; ++i) {
        s += ("This is a stringgggggggggggggggggggg 1");
        s += ("This is a stringgggggggggggggggggggg 2");
        s += ("This is a stringgggggggggggggggggggg 3");
        s += ("This is a stringgggggggggggggggggggg 4");
        s += ("This is a stringgggggggggggggggggggg 5");
    }
}
