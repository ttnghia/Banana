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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelFuncs
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for
template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdx, IndexType endIdx, const Function& function)
{
    tbb::parallel_for(tbb::blocked_range<IndexType>(beginIdx, endIdx), [&](tbb::blocked_range<IndexType> r)
                      {
                          for(IndexType i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              function(i);
                          }
                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 2D
template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdxX, IndexType endIdxX,
                         IndexType beginIdxY, IndexType endIdxY,
                         const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxX, endIdxX, [&](IndexType i)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j)
                                    {
                                        function(i, j);
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for_row_major(IndexType beginIdxX, IndexType endIdxX,
                                   IndexType beginIdxY, IndexType endIdxY,
                                   const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxY, endIdxY, [&](IndexType j)
                                {
                                    for(IndexType i = beginIdxX; i < endIdxX; ++i)
                                    {
                                        function(i, j);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 3D
template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdxX, IndexType endIdxX,
                         IndexType beginIdxY, IndexType endIdxY,
                         IndexType beginIdxZ, IndexType endIdxZ,
                         const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxX, endIdxX, [&](IndexType i)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j)
                                    {
                                        for(IndexType k = beginIdxZ; k < endIdxZ; ++k)
                                        {
                                            function(i, j, k);
                                        }
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for_row_major(IndexType beginIdxX, IndexType endIdxX,
                                   IndexType beginIdxY, IndexType endIdxY,
                                   IndexType beginIdxZ, IndexType endIdxZ,
                                   const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxZ, endIdxZ, [&](IndexType k)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j)
                                    {
                                        for(IndexType i = beginIdxX; i < endIdxX; ++i)
                                        {
                                            function(i, j, k);
                                        }
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // namespace ParallelFuncs

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana