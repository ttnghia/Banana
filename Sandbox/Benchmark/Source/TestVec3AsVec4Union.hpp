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
union VEC4
{
    struct VEC3
    {
        Vec3f value;
        float dummy = 0;
    } v3;
    Vec4f v4;
};

union MAT4
{
    struct
    {
        VEC4 c0;
        VEC4 c1;
        VEC4 c2;
        VEC4 c3;
    } m3;
    Mat4x4f m4;
};

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
        VEC4 x4, y4, result;
        x4.v3.value = x;
        y4.v3.value = y;
        result.v4   = x4.v4 + y4.v4;
        return result.v3.value;
    } else {
        VEC4 x4, result;
        x4.v3.value = x;
        result.v4   = x4.v4 * static_cast<T>(rand() % 100);
        return result.v3.value;
    }
}

template<class T>
Vec3<T> doOperationSIMD(const Mat3x3<T>& M, const Vec3<T>& x)
{
    MAT4 m4;
    VEC4 x4, result;
    m4.m3.c0.v3.value = M[0];
    m4.m3.c1.v3.value = M[1];
    m4.m3.c2.v3.value = M[2];
    x4.v3.value       = x;

    int op = rand() % 2;
    if(op == 0) {
        result.v4 = m4.m4 * x4.v4;
    } else {
        result.v4 = (m4.m4 * static_cast<T>(rand() % 100)) * x4.v4;
    }
    return result.v3.value;
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