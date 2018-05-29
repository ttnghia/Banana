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
#include <Banana/Utils/MathHelpers.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
#include <Banana/Utils/Timer.h>

using namespace Banana;

#define ARRAY_SIZE (1 << 24)
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
auto add(const Vec3<T>& x, const Vec3<T>& y)
{
    Vec4<T> x4, y4;
    x4.w = 0;
    y4.w = 0;
    std::memcpy(&x4, &x, sizeof(Vec3<T>));
    std::memcpy(&y4, &y, sizeof(Vec3<T>));
    ////////////////////////////////////////////////////////////////////////////////
    auto    result4 = x4 + y4;
    Vec3<T> result;
    std::memcpy(&result, &result4, sizeof(Vec3<T>));
    return result;
}

template<class T>
auto mul(const Vec3<T>& x, T a)
{
    Vec4<T> x4;
    x4.w = 0;
    std::memcpy(&x4, &x, sizeof(Vec3<T>));
    ////////////////////////////////////////////////////////////////////////////////
    auto    result4 = x4 * a;
    Vec3<T> result;
    std::memcpy(&result, &result4, sizeof(Vec3<T>));
    return result;
}

template<class T>
auto mul(const Mat3x3<T>& M, const Vec3<T>& x)
{
    Vec4<T>   x4;
    Mat4x4<T> M4;
    x4.w    = 0;
    M4[0].w = 0;
    M4[1].w = 0;
    M4[2].w = 0;
    M4[3].w = 0;
    std::memcpy(&x4,    &x,    sizeof(Vec3<T>));
    std::memcpy(&M4[0], &M[0], sizeof(Vec3<T>));
    std::memcpy(&M4[1], &M[1], sizeof(Vec3<T>));
    std::memcpy(&M4[2], &M[2], sizeof(Vec3<T>));
    std::memcpy(&M4[3], &M[3], sizeof(Vec3<T>));
    ////////////////////////////////////////////////////////////////////////////////
    auto    result4 = M4 * x4;
    Vec3<T> result;
    std::memcpy(&result, &result4, sizeof(Vec3<T>));
    return result;
}

template<class T>
auto mul(const Mat3x3<T>& M, T a)
{
    Mat4x4<T> M4;
    M4[0].w = 0;
    M4[1].w = 0;
    M4[2].w = 0;
    M4[3].w = 0;
    std::memcpy(&M4[0], &M[0], sizeof(Vec3<T>));
    std::memcpy(&M4[1], &M[1], sizeof(Vec3<T>));
    std::memcpy(&M4[2], &M[2], sizeof(Vec3<T>));
    std::memcpy(&M4[3], &M[3], sizeof(Vec3<T>));
    ////////////////////////////////////////////////////////////////////////////////
    auto      result4 = M4 * a;
    Mat3x3<T> result;
    std::memcpy(&result[0], &result4[0], sizeof(Vec3<T>));
    std::memcpy(&result[1], &result4[1], sizeof(Vec3<T>));
    std::memcpy(&result[2], &result4[2], sizeof(Vec3<T>));
    std::memcpy(&result[3], &result4[3], sizeof(Vec3<T>));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
Vec3<T> doOperation(const Vec3<T>& x, const Vec3<T>& y)
{
    int op = rand() % 2;
    if(op == 0) {
        return x + y;
    } else {
        return x * static_cast<T>(rand() % 100);
    }
}

template<class T>
Vec3<T> doOperation(const Mat3x3<T>& M, const Vec3<T>& x)
{
    int op = rand() % 2;
    if(op == 0) {
        return M * x;
    } else {
        return (M * static_cast<T>(rand() % 100)) * x;
    }
}

template<class T>
Vec3<T> doOperationSIMD(const Vec3<T>& x, const Vec3<T>& y)
{
    int op = rand() % 2;
    if(op == 0) {
        return add(x, y);
    } else {
        return mul(x, static_cast<T>(rand() % 100));
    }
}

template<class T>
Vec3<T> doOperationSIMD(const Mat3x3<T>& M, const Vec3<T>& x)
{
    int op = rand() % 2;
    if(op == 0) {
        return mul(M, x);
    } else {
        return mul(mul(M,  static_cast<T>(rand() % 100)), x);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("TEST Vec3f as Vec3f")
{
    srand(static_cast<UInt>(time(0)));
    ////////////////////////////////////////////////////////////////////////////////
    Vector<Vec3f>   A;
    Vector<Vec3f>   B;
    Vector<Mat3x3f> M;
    {
        ScopeTimer timer("Init data");
        ////////////////////////////////////////////////////////////////////////////////
        A.resize(ARRAY_SIZE);
        B.resize(ARRAY_SIZE);
        for(auto& x : A) {
            x = MathHelpers::vrand<Vec3f>();
        }
        M = LinaHelpers::randVecMatrices<3, UInt, float>(ARRAY_SIZE);
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        ScopeTimer timer("Test Vec3f-Vec3f op as Vec3f");
        for(int i = 0; i < ARRAY_SIZE; ++i) {
            B[i] = doOperation(A[i], A[i]);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    {
        ScopeTimer timer("Test Vec3f-Vec3f op as Vec4f");
        for(int i = 0; i < ARRAY_SIZE; ++i) {
            B[i] = doOperationSIMD(A[i], A[i]);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        ScopeTimer timer("Test Mat3x3f-Vec3f op as Vec3f");
        for(int i = 0; i < ARRAY_SIZE; ++i) {
            B[i] = doOperation(M[i], A[i]);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    {
        ScopeTimer timer("Test Mat3x3f-Vec3f op as Vec4f");
        for(int i = 0; i < ARRAY_SIZE; ++i) {
            B[i] = doOperationSIMD(M[i], A[i]);
        }
    }
}