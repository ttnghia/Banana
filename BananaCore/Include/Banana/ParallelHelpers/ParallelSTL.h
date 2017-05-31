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
namespace ParallelSTL
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// inf-norm (maximum absolute value)
//
template<class ScalarType>
inline ScalarType max_abs(const std::vector<ScalarType>& x)
{
    ParallelObjects::VectorMaxAbs<ScalarType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

template<class ScalarType, class VectorType>
inline ScalarType max_abs(const std::vector<VectorType>& x)
{
    ParallelObjects::VectorMaxAbs<ScalarType, VectorType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class ScalarType>
inline ScalarType min(const std::vector<ScalarType>& x)
{
    ParallelObjects::VectorMinElement<ScalarType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class ScalarType>
inline ScalarType max(const std::vector<ScalarType>& x)
{
    ParallelObjects::VectorMaxElement<ScalarType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class ScalarType>
inline void min_max(const std::vector<ScalarType>& x, ScalarType& minElement, ScalarType& maxElement)
{
    ParallelObjects::VectorMinMaxElements<ScalarType> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minElement = vmm.getMin();
    maxElement = vmm.getMax();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
inline ScalarType sum(const std::vector<ScalarType>& x)
{
    ParallelObjects::VectorSum<ScalarType> vSum(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vSum);

    return vSum.getSum();
}

template<class ScalarType>
inline ScalarType average(const std::vector<ScalarType>& x)
{
    return ParallelSTL::sum<ScalarType>(x) / static_cast<ScalarType>(x.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class ScalarType>
inline void sort(std::vector<ScalarType>& v)
{
    tbb::parallel_sort(v);
}

template<class ScalarType>
inline void sort_dsd(std::vector<ScalarType>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<ScalarType> ());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelSTL
