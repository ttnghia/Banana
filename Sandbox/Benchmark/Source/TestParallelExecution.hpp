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

#if 0
#include <cstdio>
#include <vector>
#include <chrono>
#include <string>
#include <algorithm>
#include <execution>

////////////////////////////////////////////////////////////////////////////////
class ScopeTimer
{
protected:
    using Clock = std::chrono::high_resolution_clock;

public:
    ScopeTimer(const std::string& caption) : m_Caption(caption) { tick(); }
    ~ScopeTimer() { printf("%s: %fms\n", m_Caption.c_str(), tock()); }

    void tick() { m_StartTime = Clock::now(); }
    float tock() { return std::chrono::duration<float, std::milli>(Clock::now() - m_StartTime).count(); }

private:
    Clock::time_point m_StartTime;
    std::string       m_Caption;
};

////////////////////////////////////////////////////////////////////////////////
#define NUM_TEST (1 << 28)

int main()
{
    ////////////////////////////////////////////////////////////////////////////////
    // init data
    srand(static_cast<unsigned int>(time(0)));
    std::vector<float> vec(NUM_TEST);
    for(unsigned int i = 0; i < NUM_TEST; ++i) {
        vec[i] = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
    }
    ////////////////////////////////////////////////////////////////////////////////

    {
        ScopeTimer timer("Test sequential finding");
        float      minNum = 1e10;
        for(unsigned int i = 0; i < NUM_TEST; ++i) {
            if(minNum > vec[i]) { minNum = vec[i]; } }
        printf("Min: %f\n", minNum);
    }

    {
        ScopeTimer timer("Test std::min_element with execution::par");
        auto       it = std::min_element(std::execution::par, vec.begin(), vec.end());
        printf("Min: %f\n", *it);
    }

    {
        ScopeTimer timer("Test std::min_element with execution::par_unseq");
        auto       it = std::min_element(std::execution::par_unseq, vec.begin(), vec.end());
        printf("Min: %f\n", *it);
    }
}

#endif