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

#include <tbb/tbb.h>
#include "./ParallelObjects.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelBLAS
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// dot products
//
template<class ScalarType>
inline ScalarType dotProduct(const std::vector<ScalarType>& x, const std::vector<ScalarType>& y)
{
    assert(x.size() == y.size());

    ParallelObjects::VectorDotProduct<ScalarType> vdp(x, y);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), vdp);

    return vdp.result;
}

template<class ScalarType, class VectorType>
inline ScalarType dotProduct(const std::vector<VectorType>& x, const std::vector<VectorType>& y)
{
    assert(x.size() == y.size());

    ParallelObjects::VectorDotProduct<ScalarType, VectorType> v(x, y);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), v);

    return v.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// inf-norm (maximum absolute value)
//
template<class ScalarType, class VectorType>
inline ScalarType maxAbs(const std::vector<VectorType>& x)
{
    ParallelObjects::VectorMaxAbs<ScalarType, VectorType> mabs(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), mabs);

    return mabs.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// saxpy (y=alpha*x+y)
//
template<class ScalarType, class VectorType>
inline void addScaled(ScalarType alpha, const std::vector<VectorType>& x, std::vector<VectorType>& y)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
        [&, alpha](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            {
                y[i] += alpha * x[i];
            }
        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// y=x+beta*y
//
template<class ScalarType, class VectorType>
inline void scaledAdd(ScalarType beta, const std::vector<VectorType>& x, std::vector<VectorType>& y)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
        [&, beta](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            {
                y[i] = beta * y[i] + x[i];
            }
        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// x *= alpha
//
template<class ScalarType, class VectorType>
inline void scale(ScalarType alpha, const std::vector<VectorType>& x)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
        [&, alpha](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            {
                x[i] *= alpha;
            }
        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelBLAS
