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

#include <Banana/Setup.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/Utils/Timer.h>
using namespace Banana;

#include <algorithm>
#include <execution>
#include <limits>

#define ARRAY_SIZE (1 << 28)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Banana::ParallelSTL vs C++17 parallel execution, float numbers")
{
    srand(static_cast<UInt>(time(0)));
    ////////////////////////////////////////////////////////////////////////////////
    Vector<float> A;
    {
        printf("\n\n");
        ScopeTimer timer("Init data");
        ////////////////////////////////////////////////////////////////////////////////
        A.resize(ARRAY_SIZE);
        for(auto& x : A) {
            x = MathHelpers::frand11<float>();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        printf("\n\n");
        ScopeTimer timer("Test sequential, float");
        float      minElement = std::numeric_limits<float>::max();;
        for(auto x : A) {
            if(minElement > x) { minElement = x; }
        }
        printf("    min: %f\n", minElement);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test Banana ParallelSTL, float");
        auto       minElement = ParallelSTL::min<float>(A);
        printf("    min: %f\n", minElement);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test std::min_element with par, float");
        auto       it = std::min_element(std::execution::par, A.begin(), A.end());
        printf("    min: %f\n", *it);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test std::min_element with par_unseq, float");
        auto       it = std::min_element(std::execution::par_unseq, A.begin(), A.end());
        printf("    min: %f\n", *it);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test Banana::ParallelSTL vs C++17 parallel execution, double numbers")
{
    srand(static_cast<UInt>(time(0)));
    ////////////////////////////////////////////////////////////////////////////////
    Vector<double> A;
    {
        printf("\n\n");
        ScopeTimer timer("Init data");
        ////////////////////////////////////////////////////////////////////////////////
        A.resize(ARRAY_SIZE);
        for(auto& x : A) {
            x = MathHelpers::frand11<double>();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        printf("\n\n");
        ScopeTimer timer("Test sequential, double");
        double     minElement = std::numeric_limits<double>::max();
        for(auto x : A) {
            if(minElement > x) { minElement = x; }
        }
        printf("    min: %f\n", minElement);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test Banana ParallelSTL, double");
        auto       minElement = ParallelSTL::min<double>(A);
        printf("    min: %f\n", minElement);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test std::min_element with par, double");
        auto       it = std::min_element(std::execution::par, A.begin(), A.end());
        printf("    min: %f\n", *it);
    }

    {
        printf("\n\n");
        ScopeTimer timer("Test std::min_element with par_unseq, double");
        auto       it = std::min_element(std::execution::par_unseq, A.begin(), A.end());
        printf("    min: %f\n", *it);
    }
}