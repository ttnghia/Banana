//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NumberHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool isValidNumber(T x)
{
    return !std::isnan(x) && !std::isinf(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType, Int N, class RealType>
inline VecX<N, IndexType> createGrid(const VecX<N, RealType>& bmin, const VecX<N, RealType>& bmax, RealType spacing)
{
    VecX<N, RealType>  fgrid = (bmax - bmin) / spacing;
    VecX<N, IndexType> result;

    for(Int i = 0; i < N; ++i) {
        result[i] = static_cast<IndexType>(ceil(fgrid[i]));
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType, class Function>
inline void scan(VecX<N, IndexType> idx, const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, const Function& f, Int dim = 0)
{
    if(dim == N - 1) {
        for(IndexType i = minIdx[dim]; i < maxIdx[dim]; ++i) {
            idx[dim] = i;
            f(idx);
        }
    } else {
        for(IndexType i = minIdx[dim]; i < maxIdx[dim]; ++i) {
            idx[dim] = i;
            scan(idx, minIdx, maxIdx, f, dim + 1);
        }
    }
}

template<Int N, class IndexType, class Function>
inline void scan(const VecX<N, IndexType>& minIdx, const VecX<N, IndexType>& maxIdx, const Function& f, Int dim = 0)
{
    scan(VecX<N, IndexType>(0), minIdx, maxIdx, f, dim);
}

template<Int N, class IndexType, class Function>
inline void scan(const VecX<N, IndexType>& maxIdx, const Function& f, Int dim = 0)
{
    scan(VecX<N, IndexType>(0), VecX<N, IndexType>(0), maxIdx, f, dim);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
inline IntType generateRandomInt(IntType start = 0, IntType end = std::numeric_limits<IntType>::max())
{
    std::random_device                     rd;
    std::mt19937                           gen(rd());
    std::uniform_int_distribution<IntType> dis(start, end);

    return dis(gen);
}

template<class RealType>
inline RealType generateRandomReal(RealType start = RealType(0), RealType end = std::numeric_limits<RealType>::max())
{
    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis(start, end);

    return dis(gen);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
String formatWithCommas(RealType value, Int precision = 2)
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
template<class RealType>
String formatToScientific(RealType value, Int precision = 5)
{
    std::stringstream ss;
    ss.str("");
    ss << std::setprecision(precision) << std::fixed << std::scientific << value;
    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline String formatToScientific(const Vec3<RealType>& vec, Int precision = 5)
{
    std::stringstream ss;
    ss.str("");

    ss << String("[");
    ss << std::setprecision(precision) << std::fixed << std::scientific << vec[0] + String(", ");
    ss << std::setprecision(precision) << std::fixed << std::scientific << vec[1] + String(", ");
    ss << std::setprecision(precision) << std::fixed << std::scientific << vec[2] + String("]");

    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String byteToHex(const char* arrBytes, size_t arrSize, Int width /*= 2*/)
{
    if(width < 2) { width = 2; }
    std::stringstream ss;

    for(size_t i = 0; i < arrSize; ++i) {
        ss << std::hex << std::uppercase << std::setw(width) << static_cast<Int>(arrBytes[i]);
    }

    return ss.str();
}

template<class T>
inline String byteToHex(const Vector<T>& vecBytes, Int width /*= 2*/)
{
    return byteToHex((const char*)vecBytes.data(), vecBytes.size() * sizeof(T));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename RealType>
String toString(const RealType value, const Int precision = 5)
{
    std::ostringstream ss;
    ss << std::setprecision(precision) << value;
    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline String toString(const VecX<N, RealType>& vec, Int precision = 5)
{
    std::stringstream ss;
    ss.str("");

    ss << String("[");

    for(Int i = 0; i < N - 1; ++i) {
        ss << formatWithCommas(vec[i], precision) << String(", ");
    }
    ss << formatWithCommas(vec[N - 1], precision) << String("]");

    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline String toString(const MatXxX<N, RealType>& mat, bool breakLine = false, Int precision = 5)
{
    std::stringstream ss;
    ss.str("");

    ss << String("[");

    // N-1 line
    for(Int i = 0; i < N - 1; ++i) {
        for(Int j = 0; j < N - 1; ++j) {
            ss << formatWithCommas(mat[j][i], precision) << String(", ");
        }
        ss << formatWithCommas(mat[N - 1][i], precision);

        if(breakLine) {
            ss << String(",") << std::endl;
        } else {
            ss << String("; ");
        }
    }

    // last line
    {
        for(Int j = 0; j < N - 1; ++j) {
            ss << formatWithCommas(mat[j][N - 1], precision) << String(", ");
        }
        ss << formatWithCommas(mat[N - 1][N - 1], precision);
    }
    ss << String("]");

    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T, class S, Int N>
inline VecX<N, T> convert(const VecX<N, S>& vec)
{
    VecX<N, T> result;
    for(Int i = 0; i < N; ++i) {
        result[i] = static_cast<T>(vec[i]);
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool isInside(const VecX<N, RealType>& ppos, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax)
{
    for(Int i = 0; i < N; ++i) {
        if(ppos[i] < bMin[i] || ppos[i] > bMax[i]) {
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
void clamp(VecX<N, RealType>& ppos, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax, Real margin /*= 0*/)
{
    for(UInt i = 0; i < N; ++i) {
        ppos[i] = MathHelpers::clamp(ppos[i], bMin[i] + margin, bMax[i] - margin);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace NumberHelpers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana