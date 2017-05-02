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

#include <tbb/tbb.h>

#include <algorithm>
#include <functional>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelFuncs
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for
template<class IndexType, class Function>
void parallel_for(IndexType begin, IndexType end, const Function& function)
{
    size_t size = static_cast<size_t>(diff);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, size),
        [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            {
                function(i);
            }
        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 2D
template<class IndexType, class Function>
void parallel_for(IndexType beginIndexX, IndexType endIndexX,
                  IndexType beginIndexY, IndexType endIndexY,
                  const Function& function)
{
    ParallelFuncs::parallel_for(beginIndexX, endIndexX,
        [&](size_t i)
        {
            for(IndexType j = beginIndexY; j < endIndexY; ++j)
            {
                function(i, j);
            }
        });
}


template<class IndexType, class Function>
void parallel_for_row_major(IndexType beginIndexX, IndexType endIndexX,
                            IndexType beginIndexY, IndexType endIndexY,
                            const Function& function)
{
    ParallelFuncs::parallel_for(beginIndexY, endIndexY,
        [&](size_t j)
        {
            for(IndexType i = beginIndexX; i < endIndexX; ++i)
            {
                function(i, j);
            }
        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 3D
template<class IndexType, class Function>
void parallel_for(IndexType beginIndexX, IndexType endIndexX,
                  IndexType beginIndexY, IndexType endIndexY,
                  IndexType beginIndexZ, IndexType endIndexZ,
                  const Function& function)
{
    ParallelFuncs::parallel_for(beginIndexX, endIndexX,
        [&](size_t i)
        {
            for(IndexType j = beginIndexY; j < endIndexY; ++j)
            {
                for(IndexType k = beginIndexZ; k < endIndexZ; ++k)
                {
                    function(i, j, k);
                }
            }
        });
}

template<class IndexType, class Function>
void parallel_for_row_major(IndexType beginIndexX, IndexType endIndexX,
                            IndexType beginIndexY, IndexType endIndexY,
                            IndexType beginIndexZ, IndexType endIndexZ,
                            const Function& function)
{
    ParallelFuncs::parallel_for(beginIndexZ, endIndexZ,
        [&](size_t k)
        {
            for(IndexType j = beginIndexY; j < endIndexY; ++j)
            {
                for(IndexType i = beginIndexX; i < endIndexX; ++i)
                {
                    function(i, j, k);
                }
            }
        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // namespace ParallelFuncs
