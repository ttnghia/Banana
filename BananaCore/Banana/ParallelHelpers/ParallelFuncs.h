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

#include <Banana/Setup.h>

#include <tbb/tbb.h>

#include <algorithm>
#include <functional>
#include <vector>

//#define __Banana_No_Parallel
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelFuncs
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdx, IndexType endIdx, const Function& function)
{
#if defined(__Banana_No_Parallel) || defined(__Banana_Disable_Parallel) || defined(__BNN_NO_PARALLEL) || defined(__BNN_DISABLE_PARALLEL)
    for(IndexType i = beginIdx; i < endIdx; ++i) {
        function(i);
    }
#else
    tbb::parallel_for(tbb::blocked_range<IndexType>(beginIdx, endIdx),
                      [&](tbb::blocked_range<IndexType> r)
                      {
                          for(IndexType i = r.begin(), iEnd = r.end(); i != iEnd; ++i) {
                              function(i);
                          }
                      });
#endif
}

template<class IndexType, class Function>
inline void parallel_for(IndexType endIdx, const Function& function)
{
    ParallelFuncs::parallel_for(IndexType(0), endIdx, function);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 2D
template<class IndexType, class Function>
inline void parallel_for_row_major(IndexType beginIdxX, IndexType endIdxX,
                                   IndexType beginIdxY, IndexType endIdxY,
                                   const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxX, endIdxX,
                                [&](IndexType i)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j) {
                                        function(i, j);
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdxX, IndexType endIdxX,
                         IndexType beginIdxY, IndexType endIdxY,
                         const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxY, endIdxY,
                                [&](IndexType j)
                                {
                                    for(IndexType i = beginIdxX; i < endIdxX; ++i) {
                                        function(i, j);
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for_row_major(const Vec2<IndexType>& endIdx, const Function& function)
{
    ParallelFuncs::parallel_for_row_major(IndexType(0), endIdx[0], IndexType(0), endIdx[1], function);
}

template<class IndexType, class Function>
inline void parallel_for(const Vec2<IndexType>& endIdx, const Function& function)
{
    ParallelFuncs::parallel_for(IndexType(0), endIdx[0], IndexType(0), endIdx[1], function);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 3D
template<class IndexType, class Function>
inline void parallel_for_row_major(IndexType beginIdxX, IndexType endIdxX,
                                   IndexType beginIdxY, IndexType endIdxY,
                                   IndexType beginIdxZ, IndexType endIdxZ,
                                   const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxX, endIdxX,
                                [&](IndexType i)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j) {
                                        for(IndexType k = beginIdxZ; k < endIdxZ; ++k) {
                                            function(i, j, k);
                                        }
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for(IndexType beginIdxX, IndexType endIdxX,
                         IndexType beginIdxY, IndexType endIdxY,
                         IndexType beginIdxZ, IndexType endIdxZ,
                         const Function& function)
{
    ParallelFuncs::parallel_for(beginIdxZ, endIdxZ,
                                [&](IndexType k)
                                {
                                    for(IndexType j = beginIdxY; j < endIdxY; ++j) {
                                        for(IndexType i = beginIdxX; i < endIdxX; ++i) {
                                            function(i, j, k);
                                        }
                                    }
                                });
}

template<class IndexType, class Function>
inline void parallel_for_row_major(const Vec3<IndexType>& endIdx, const Function& function)
{
    ParallelFuncs::parallel_for_row_major(IndexType(0), endIdx[0], IndexType(0), endIdx[1], IndexType(0), endIdx[2], function);
}

template<class IndexType, class Function>
inline void parallel_for(const Vec3<IndexType>& endIdx, const Function& function)
{
    ParallelFuncs::parallel_for(IndexType(0), endIdx[0], IndexType(0), endIdx[1], IndexType(0), endIdx[2], function);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // namespace ParallelFuncs

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana