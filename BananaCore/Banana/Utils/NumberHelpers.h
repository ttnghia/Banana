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
//    /                    Created: 2017 by Nghia Truong                     \
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
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NumberHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool isValidNumber(T x)
{
    return !std::isnan(x) && !std::isinf(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType, Int N, class RealType>
VecX<N, IndexType> createGrid(const VecX<N, RealType>& bmin, const VecX<N, RealType>& bmax, RealType spacing)
{
    VecX<N, RealType>  fgrid = (bmax - bmin) / spacing;
    VecX<N, IndexType> result;

    for(Int d = 0; d < N; ++d) {
        result[d] = static_cast<IndexType>(ceil(fgrid[d]));
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType, class Function>
void scan(VecX<N, IndexType> idx, const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0)
{
    if(dim == N - 1) {
        for(IndexType i = minIdx[dim]; i < maxIdx[dim]; ++i) {
            idx[dim] = i;
            f(idx);
        }
    } else {
        for(IndexType i = minIdx[dim]; i < maxIdx[dim]; ++i) {
            idx[dim] = i;
            scan(idx, minIdx, maxIdx, std::forward<Function>(f), dim + 1);
        }
    }
}

template<Int N, class IndexType, class Function>
void scan(const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0)
{
    scan(VecX<N, IndexType>(0), minIdx, maxIdx, std::forward<Function>(f), dim);
}

template<Int N, class IndexType, class Function>
void scan(const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0)
{
    scan(VecX<N, IndexType>(0), VecX<N, IndexType>(0), maxIdx, std::forward<Function>(f), dim);
}

template<Int N, class IndexType, class Function>
void scan11(Function&& f)
{
    scan(VecX<N, IndexType>(0), VecX<N, IndexType>(-1), VecX<N, IndexType>(2), std::forward<Function>(f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
IntType generateRandomInt(IntType start = 0, IntType end = std::numeric_limits<IntType>::max())
{
    if(end < start) {
        std::swap(end, start);
    }
    std::random_device                     rd;
    std::mt19937                           gen(rd());
    std::uniform_int_distribution<IntType> dis(start, end);

    return dis(gen);
}

template<class IntType, class SizeType>
Vector<IntType> generateRandomIntVector(SizeType size, IntType start = 0, IntType end = std::numeric_limits<IntType>::max())
{
    if(end < start) {
        std::swap(end, start);
    }
    std::random_device                     rd;
    std::mt19937                           gen(rd());
    std::uniform_int_distribution<IntType> dis(start, end);

    Vector<IntType> v(size);
    for(SizeType i = 0; i < size; ++i) {
        v[i] = dis(gen);
    }
    return v;
}

template<class RealType>
RealType generateRandomReal(RealType start = RealType(0), RealType end = std::numeric_limits<RealType>::max())
{
    if(end < start) {
        std::swap(end, start);
    }
    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis(start, end);

    return dis(gen);
}

template<class RealType, class SizeType>
Vector<RealType> generateRandomRealVector(SizeType size, RealType start = RealType(0), RealType end = std::numeric_limits<RealType>::max())
{
    if(end < start) {
        std::swap(end, start);
    }
    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis(start, end);

    Vector<RealType> v(size);
    for(SizeType i = 0; i < size; ++i) {
        v[i] = dis(gen);
    }
    return v;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FastRand
{
public:
    void seed(UInt seed_) { s_Seed = seed_; }

    /// Compute a pseudo-random integer
    /// Output value in range [0, 32767]
    __BNN_INLINE Int rand()
    {
        s_Seed = (214013u * s_Seed + 2531011u);
        return static_cast<Int>((s_Seed >> 16) & 0x7FFF);
    }

private:
    UInt s_Seed = 0u;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool isInside(const VecX<N, RealType>& ppos, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax)
{
    for(Int d = 0; d < N; ++d) {
        if(ppos[d] < bMin[d] || ppos[d] > bMax[d]) {
            return false;
        }
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType1, class RealType2>
void jitter(VecX<N, RealType1>& ppos, RealType2 maxJitter)
{
    for(Int j = 0; j < N; ++j) {
        ppos += MathHelpers::frand11<RealType1>() * static_cast<RealType1>(maxJitter);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void translate(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& translation)
{
    Scheduler::parallel_for(points.size(), [&](size_t i)
                            {
                                points[i] = points[i] + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void scale(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& scale)
{
    Scheduler::parallel_for(points.size(), [&](size_t i)
                            {
                                points[i] = points[i] * scale;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void rotate(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& rotation)
{
    RealType azimuth = rotation[0];
    RealType elevation = rotation[1];
    RealType roll = rotation[2];
    RealType sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3<RealType> R[3];

    sinA = static_cast<RealType>(sin(azimuth));
    cosA = static_cast<RealType>(cos(azimuth));
    sinE = static_cast<RealType>(sin(elevation));
    cosE = static_cast<RealType>(cos(elevation));
    sinR = static_cast<RealType>(sin(roll));
    cosR = static_cast<RealType>(cos(roll));

    R[0][0] = cosR * cosA - sinR * sinA * sinE;
    R[0][1] = sinR * cosA + cosR * sinA * sinE;
    R[0][2] = -sinA * cosE;

    R[1][0] = -sinR * cosE;
    R[1][1] = cosR * cosE;
    R[1][2] = sinE;

    R[2][0] = cosR * sinA + sinR * cosA * sinE;
    R[2][1] = sinR * sinA - cosR * cosA * sinE;
    R[2][2] = cosA * cosE;

    Scheduler::parallel_for(points.size(), [&](size_t i)
                            {
                                const auto& pi = points[i];
                                points[i]      = Vec3<RealType>(glm::dot(R[0], pi),
                                                                glm::dot(R[1], pi),
                                                                glm::dot(R[2], pi));
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void transform(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& translation, const VecX<N, RealType>& scale)
{
    Scheduler::parallel_for(points.size(), [&](size_t i)
                            {
                                points[i] = points[i] * scale + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void transform(Vec_Vec3<RealType>& points, const Vec3<RealType>& translation, const Vec3<RealType>& scale, const Vec3<RealType>& rotation)
{
    RealType azimuth = rotation[0];
    RealType elevation = rotation[1];
    RealType roll = rotation[2];
    RealType sinA, cosA, sinE, cosE, sinR, cosR;

    Vec3<RealType> R[3];

    sinA = static_cast<RealType>(sin(azimuth));
    cosA = static_cast<RealType>(cos(azimuth));
    sinE = static_cast<RealType>(sin(elevation));
    cosE = static_cast<RealType>(cos(elevation));
    sinR = static_cast<RealType>(sin(roll));
    cosR = static_cast<RealType>(cos(roll));

    R[0][0] = cosR * cosA - sinR * sinA * sinE;
    R[0][1] = sinR * cosA + cosR * sinA * sinE;
    R[0][2] = -sinA * cosE;

    R[1][0] = -sinR * cosE;
    R[1][1] = cosR * cosE;
    R[1][2] = sinE;

    R[2][0] = cosR * sinA + sinR * cosA * sinE;
    R[2][1] = sinR * sinA - cosR * cosA * sinE;
    R[2][2] = cosA * cosE;

    Scheduler::parallel_for(points.size(), [&](size_t i)
                            {
                                const auto& pi = points[i];
                                Vec3<RealType> tmp(glm::dot(R[0], pi),
                                                   glm::dot(R[1], pi),
                                                   glm::dot(R[2], pi));
                                points[i] = tmp * scale + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace NumberHelpers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana