//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
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
// inf-norm (maximum absolute value)
//
template<class T>
inline T maxAbs(const Vector<T>& x)
{
    ParallelObjects::VectorMaxAbs<T> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

template<class T, class VectorType>
inline T maxVecAbs(const Vector<VectorType>& x)
{
    ParallelObjects::VectorMaxAbs<T, VectorType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class T>
inline T min(const Vector<T>& x)
{
    ParallelObjects::VectorMinElement<T> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class T>
inline T max(const Vector<T>& x)
{
    ParallelObjects::VectorMaxElement<T> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class T, class VectorType>
inline T maxNorm2(const Vector<VectorType>& x)
{
    ParallelObjects::VectorMaxNorm2<T, VectorType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<Int N, class RealType>
inline void min_max_vector(const Vector<VecX<N, RealType>>& x, VecX<N, RealType>& minElement, VecX<N, RealType>& maxElement)
{
    ParallelObjects::VectorMinMaxVectorElements<N, RealType> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minElement = vmm.getMin();
    maxElement = vmm.getMax();
}

template<class T>
inline void min_max(const Vector<T>& x, T& minElement, T& maxElement)
{
    ParallelObjects::VectorMinMaxElements<T> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minElement = vmm.getMin();
    maxElement = vmm.getMax();
}

template<class T, class VectorType>
inline void min_max_norm2(const Vector<VectorType>& x, T& minNorm2, T& maxNorm2)
{
    ParallelObjects::VectorMinMaxNorm2<T, VectorType> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minNorm2 = vmm.getMin();
    maxNorm2 = vmm.getMax();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T sum(const Vector<T>& x)
{
    ParallelObjects::VectorSum<T> vSum(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vSum);

    return vSum.getSum();
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