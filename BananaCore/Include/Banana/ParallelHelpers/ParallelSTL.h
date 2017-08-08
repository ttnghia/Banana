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
template<class RealType>
inline RealType max_abs(const std::vector<RealType>& x)
{
    ParallelObjects::VectorMaxAbs<RealType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

template<class RealType, class VectorType>
inline RealType max_abs(const std::vector<VectorType>& x)
{
    ParallelObjects::VectorMaxAbs<RealType, VectorType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.getResult();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class RealType>
inline RealType min(const std::vector<RealType>& x)
{
    ParallelObjects::VectorMinElement<RealType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class RealType>
inline RealType max(const std::vector<RealType>& x)
{
    ParallelObjects::VectorMaxElement<RealType> vm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vm);

    return vm.getResult();
}

template<class RealType>
inline void min_max(const std::vector<RealType>& x, RealType& minElement, RealType& maxElement)
{
    ParallelObjects::VectorMinMaxElements<RealType> vmm(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vmm);

    minElement = vmm.getMin();
    maxElement = vmm.getMax();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline RealType sum(const std::vector<RealType>& x)
{
    ParallelObjects::VectorSum<RealType> vSum(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vSum);

    return vSum.getSum();
}

template<class RealType>
inline RealType average(const std::vector<RealType>& x)
{
    return ParallelSTL::sum<RealType>(x) / static_cast<RealType>(x.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class RealType>
inline void sort(std::vector<RealType>& v)
{
    tbb::parallel_sort(v);
}

template<class RealType>
inline void sort_dsd(std::vector<RealType>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<RealType> ());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParallelSTL

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana