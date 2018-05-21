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

#include <cassert>
#include <vector>
#include <cmath>

#include <tbb/tbb.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelSTL
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class T>
inline T min(const Vector<T>& x)
{
    ParallelObjects::MinElement<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<Int N, class T>
inline T min(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::MinElement<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T max(const Vector<T>& x)
{
    ParallelObjects::MaxElement<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<Int N, class T>
inline T max(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::MaxElement<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T maxAbs(const Vector<T>& x)
{
    ParallelObjects::MaxAbs<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<Int N, class T>
inline T maxAbs(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::MaxAbs<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
inline T maxNorm2(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::MaxNorm2<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void min_max(const Vector<T>& x, T& minElement, T& maxElement)
{
    ParallelObjects::MinMaxElements<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);

    minElement = pObj.getMin();
    maxElement = pObj.getMax();
}

template<Int N, class T>
inline void min_max(const Vector<VecX<N, T>>& x, VecX<N, T>& minElement, VecX<N, T>& maxElement)
{
    ParallelObjects::MinMaxElements<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);

    minElement = pObj.getMin();
    maxElement = pObj.getMax();
}

template<Int N, class T>
inline void min_max(const Vector<MatXxX<N, T>>& x, T& minElement, T& maxElement)
{
    ParallelObjects::MinMaxElements<0, T> pObj(reinterpret_cast<const T*>(x.data()));
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size() * static_cast<size_t>(N * N)), pObj);

    minElement = pObj.getMin();
    maxElement = pObj.getMax();
}

template<Int N, class T>
inline void min_max_norm2(const Vector<VecX<N, T>>& x, T& minVal, T& maxVal)
{
    ParallelObjects::MinMaxNorm2<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);

    minVal = pObj.getMin();
    maxVal = pObj.getMax();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T sum(const Vector<T>& x)
{
    ParallelObjects::VectorSum<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<Int N, class T>
inline VecX<N, T> sum(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::VectorSum<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<class T>
inline T sum_sqr(const Vector<T>& x)
{
    ParallelObjects::VectorSumSqr<1, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<Int N, class T>
inline T sum_sqr(const Vector<VecX<N, T>>& x)
{
    ParallelObjects::VectorSumSqr<N, T> pObj(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), pObj);
    return pObj.getResult();
}

template<class T>
inline T average(const Vector<T>& x)
{
    return ParallelSTL::sum<T>(x) / static_cast<T>(x.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class T>
inline void sort(Vector<T>& v)
{
    tbb::parallel_sort(v);
}

template<class T>
inline void sort_dsd(Vector<T>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<T> ());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParallelSTL

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana