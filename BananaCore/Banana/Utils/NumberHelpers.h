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
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::NumberHelpers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool isValidNumber(T x) {
    return !std::isnan(x) && !std::isinf(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType, Int N, class RealType>
VecX<N, IndexType> createGrid(const VecX<N, RealType>& bmin, const VecX<N, RealType>& bmax, RealType spacing) {
    VecX<N, RealType>  fgrid = (bmax - bmin) / spacing;
    VecX<N, IndexType> result;

    for(Int d = 0; d < N; ++d) {
        result[d] = static_cast<IndexType>(ceil(fgrid[d]));
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType, class Function>
void scan(VecX<N, IndexType> idx, const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0) {
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
void scan(const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0) {
    scan(VecX<N, IndexType>(0), minIdx, maxIdx, std::forward<Function>(f), dim);
}

template<Int N, class IndexType, class Function>
void scan(const VecX<N, IndexType>& maxIdx, Function&& f, Int dim = 0) {
    scan(VecX<N, IndexType>(0), VecX<N, IndexType>(0), maxIdx, std::forward<Function>(f), dim);
}

template<Int N, class IndexType, class Function>
void scan11(Function&& f) {
    scan(VecX<N, IndexType>(0), VecX<N, IndexType>(-1), VecX<N, IndexType>(2), std::forward<Function>(f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class MT_iRandom {
    using Distribution = std::uniform_int_distribution<T>;
    constexpr static UInt s_CacheSize = 1048576u;
public:
    MT_iRandom(T start = T(0), T end = std::numeric_limits<T>::max()) : m_Dist(Distribution(start, end)) { generateCache(); }

    T rnd() {
        m_Lock.lock();
        if(m_CacheIdx >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        T tmp = m_Cache[m_CacheIdx++];
        m_Lock.unlock();
        return tmp;
    }

    template<class Vector>
    Vector vrnd() {
        static_assert(sizeof(typename Vector::value_type) == sizeof(T));
        UInt N = static_cast<UInt>(Vector::length());
        m_Lock.lock();
        if(m_CacheIdx + N >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        auto oldIdx = m_CacheIdx;
        m_CacheIdx += N;
        m_Lock.unlock();
        Vector result;
        std::memcpy(&result, &m_Cache[oldIdx], sizeof(Vector));
        return result;
    }

    template<class Matrix>
    Matrix mrnd() {
        static_assert(sizeof(typename Matrix::value_type) == sizeof(T));
        UInt MxN = static_cast<UInt>(Matrix::length() * Matrix::col_type::length());
        m_Lock.lock();
        if(m_CacheIdx + MxN >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        auto oldIdx = m_CacheIdx;
        m_CacheIdx += MxN;
        m_Lock.unlock();
        Matrix result;
        std::memcpy(&result, &m_Cache[oldIdx], sizeof(Matrix));
        return result;
    }

private:
    void generateCache() {
        for(Int i = 0; i < s_CacheSize; ++i) {
            m_Cache[i] = m_Dist(m_Generator);
        }
    }

    ParallelObjects::SpinLock m_Lock {};
    std::mt19937              m_Generator { (std::random_device())() };
    Distribution              m_Dist;

    T    m_Cache[s_CacheSize];
    UInt m_CacheIdx = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class MT_fRandom {
    using Distribution = std::uniform_real_distribution<T>;
    constexpr static UInt s_CacheSize = 1048576u;
public:
    MT_fRandom(T start = T(0), T end = std::numeric_limits<T>::max()) : m_Dist(Distribution(start, end)) { generateCache(); }

    T rnd() {
        m_Lock.lock();
        if(m_CacheIdx >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        T tmp = m_Cache[m_CacheIdx++];
        m_Lock.unlock();
        return tmp;
    }

    template<class Vector>
    Vector vrnd() {
        static_assert(sizeof(typename Vector::value_type) == sizeof(T));
        UInt N = static_cast<UInt>(Vector::length());
        m_Lock.lock();
        if(m_CacheIdx + N >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        auto oldIdx = m_CacheIdx;
        m_CacheIdx += N;
        m_Lock.unlock();
        Vector result;
        std::memcpy(&result, &m_Cache[oldIdx], sizeof(Vector));
        return result;
    }

    template<class Matrix>
    Matrix mrnd() {
        static_assert(sizeof(typename Matrix::value_type) == sizeof(T));
        UInt MxN = static_cast<UInt>(Matrix::length() * Matrix::col_type::length());
        m_Lock.lock();
        if(m_CacheIdx + MxN >= s_CacheSize) {
            m_CacheIdx = 0;
        }
        auto oldIdx = m_CacheIdx;
        m_CacheIdx += MxN;
        m_Lock.unlock();
        Matrix result;
        std::memcpy(&result, &m_Cache[oldIdx], sizeof(Matrix));
        return result;
    }

private:
    void generateCache() {
        for(Int i = 0; i < s_CacheSize; ++i) {
            m_Cache[i] = m_Dist(m_Generator);
        }
    }

    ParallelObjects::SpinLock m_Lock {};
    std::mt19937              m_Generator { (std::random_device())() };
    Distribution              m_Dist;

    T    m_Cache[s_CacheSize];
    UInt m_CacheIdx = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class iRand {
public:
    static auto rnd() { return s_Rand.rnd(); }
    template<class Vector> static auto vrnd() { return s_Rand.template vrnd<Vector>(); }
    template<class Matrix> static auto mrnd() { return s_Rand.template mrnd<Matrix>(); }
private:
    static inline MT_iRandom<T> s_Rand = MT_iRandom<T>();
};

template<class T>
class fRand {
public:
    static auto rnd() { return s_Rand.rnd(); }
    template<class Vector> static auto vrnd() { return s_Rand.template vrnd<Vector>(); }
    template<class Matrix> static auto mrnd() { return s_Rand.template mrnd<Matrix>(); }
private:
    static inline MT_fRandom<T> s_Rand = MT_fRandom<T>();
};

template<class T>
class fRand01 {
public:
    static auto rnd() { return s_Rand.rnd(); }
    template<class Vector> static auto vrnd() { return s_Rand.template vrnd<Vector>(); }
    template<class Matrix> static auto mrnd() { return s_Rand.template mrnd<Matrix>(); }
private:
    static inline MT_fRandom<T> s_Rand = MT_fRandom<T>(T(0), T(1));
};

template<class T>
class fRand11 {
public:
    static auto rnd() { return s_Rand.rnd(); }
    template<class Vector> static auto vrnd() { return s_Rand.template vrnd<Vector>(); }
    template<class Matrix> static auto mrnd() { return s_Rand.template mrnd<Matrix>(); }
private:
    static inline MT_fRandom<T> s_Rand = MT_fRandom<T>(T(-1), T(1));
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType, class SizeType>
Vector<IntType> generateRandomIntVector(SizeType size, IntType start = 0, IntType end = std::numeric_limits<IntType>::max()) {
    Vector<IntType> v(size);
    for(SizeType i = 0; i < size; ++i) {
        v[i] = MT_iRandom<IntType>::rnd();
    }
    return v;
}

template<class RealType, class SizeType>
Vector<RealType> generateRandomRealVector(SizeType size, RealType start = RealType(0), RealType end = std::numeric_limits<RealType>::max()) {
    Vector<RealType> v(size);
    for(SizeType i = 0; i < size; ++i) {
        v[i] = MT_fRandom<RealType>::rnd();
    }
    return v;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Transforms even the sequence 0,1,2,3,... into reasonably good random numbers
// Challenge: improve on this in speed and "randomness"!
// This seems to pass several statistical tests, and is a bijective map (of 32-bit unsigned ints)
inline unsigned int randhash(unsigned int seed) {
    unsigned int i = (seed ^ 0xA3C59AC3u) * 2654435769u;
    i ^= (i >> 16);
    i *= 2654435769u;
    i ^= (i >> 16);
    i *= 2654435769u;
    return i;
}

// the inverse of randhash
inline unsigned int unhash(unsigned int h) {
    h *= 340573321u;
    h ^= (h >> 16);
    h *= 340573321u;
    h ^= (h >> 16);
    h *= 340573321u;
    h ^= 0xA3C59AC3u;
    return h;
}

// returns repeatable stateless pseudo-random number in [0,1]
template<class T>
inline T frandhash(unsigned int seed) {
    return T(randhash(seed)) / static_cast<T>(UINT_MAX);
}

// returns repeatable stateless pseudo-random number in [a,b]
template<class T>
inline T frandhash(T a, T b, unsigned int seed) {
    return (b - a) * (randhash(seed) / static_cast<T>(UINT_MAX)) + a;
}

template<class T>
inline T frandhash11(unsigned int seed) {
    return frandhash(T(-1.0), T(1.0), seed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FastRand {
public:
    void seed(UInt seed_) { s_Seed = seed_; }

    /// Compute a pseudo-random integer
    /// Output value in range [0, 32767]
    __BNN_INLINE Int rand() {
        s_Seed = (214013u * s_Seed + 2531011u);
        return static_cast<Int>((s_Seed >> 16) & 0x7FFF);
    }

private:
    UInt s_Seed = 0u;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool isInside(const VecX<N, RealType>& ppos, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) {
    for(Int d = 0; d < N; ++d) {
        if(ppos[d] < bMin[d] || ppos[d] > bMax[d]) {
            return false;
        }
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType1, class RealType2>
void jitter(VecX<N, RealType1>& ppos, RealType2 maxJitter) {
    for(Int j = 0; j < N; ++j) {
        ppos += fRand11<RealType1>::rnd() * static_cast<RealType1>(maxJitter);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void translate(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& translation) {
    Scheduler::parallel_for(points.size(), [&](size_t i) {
                                points[i] = points[i] + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void scale(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& scale) {
    Scheduler::parallel_for(points.size(), [&](size_t i) {
                                points[i] = points[i] * scale;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void rotate(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& rotation) {
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

    Scheduler::parallel_for(points.size(), [&](size_t i) {
                                const auto& pi = points[i];
                                points[i]      = Vec3<RealType>(glm::dot(R[0], pi),
                                                                glm::dot(R[1], pi),
                                                                glm::dot(R[2], pi));
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void transform(Vec_VecX<N, RealType>& points, const VecX<N, RealType>& translation, const VecX<N, RealType>& scale) {
    Scheduler::parallel_for(points.size(), [&](size_t i) {
                                points[i] = points[i] * scale + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void transform(Vec_Vec3<RealType>& points, const Vec3<RealType>& translation, const Vec3<RealType>& scale, const Vec3<RealType>& rotation) {
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

    Scheduler::parallel_for(points.size(), [&](size_t i) {
                                const auto& pi = points[i];
                                Vec3<RealType> tmp(glm::dot(R[0], pi),
                                                   glm::dot(R[1], pi),
                                                   glm::dot(R[2], pi));
                                points[i] = tmp * scale + translation;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::NumberHelpers
