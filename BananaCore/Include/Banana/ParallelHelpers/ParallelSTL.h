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
template<class Real>
inline Real maxAbs(const std::vector<Real>& x)
{
    ParallelObjects::VectorMaxAbs<Real> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

template<class Real, class VectorType>
inline Real maxVecAbs(const std::vector<VectorType>& x)
{
    ParallelObjects::VectorMaxAbs<Real, VectorType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class Real>
inline Real min(const std::vector<Real>& x)
{
    ParallelObjects::VectorMinElement<Real> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class Real>
inline Real max(const std::vector<Real>& x)
{
    ParallelObjects::VectorMaxElement<Real> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class Real, class VectorType>
inline Real maxNorm2(const std::vector<VectorType>& x)
{
    ParallelObjects::VectorMaxNorm2<Real, VectorType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class Real>
inline void min_max(const std::vector<Real>& x, Real& minElement, Real& maxElement)
{
    ParallelObjects::VectorMinMaxElements<Real> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minElement = vmm.getMin();
    maxElement = vmm.getMax();
}

template<class Real, class VectorType>
inline void min_max_norm2(const std::vector<VectorType>& x, Real& minNorm2, Real& maxNorm2)
{
    ParallelObjects::VectorMinMaxNorm2<Real, VectorType> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minNorm2 = vmm.getMin();
    maxNorm2 = vmm.getMax();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
inline Real sum(const std::vector<Real>& x)
{
    ParallelObjects::VectorSum<Real> vSum(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vSum);

    return vSum.getSum();
}

template<class Real>
inline Real average(const std::vector<Real>& x)
{
    return ParallelSTL::sum<Real>(x) / static_cast<Real>(x.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class Real>
inline void sort(std::vector<Real>& v)
{
    tbb::parallel_sort(v);
}

template<class Real>
inline void sort_dsd(std::vector<Real>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<Real> ());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParallelSTL

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana