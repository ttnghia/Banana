//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/12/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <tbb/tbb.h>

#include <algorithm>
#include <functional>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelFuncs
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for
template <class IndexType, class Function>
void parallel_for(
    IndexType begin,
    IndexType end,
    const Function& function)
{
    auto diff = end - begin;
    if(diff <= 0)
    {
        return;
    }

    size_t size = static_cast<size_t>(diff);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            function(i);
        }
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 2D
template <class IndexType, class Function>
void parallel_for(
    IndexType beginIndexX,
    IndexType endIndexX,
    IndexType beginIndexY,
    IndexType endIndexY,
    const Function& function)
{
    parallel_for(
        beginIndexX, endIndexX,
        [&](size_t i)
    {
        for(IndexType j = beginIndexY; j < endIndexY; ++j)
        {
            function(i, j);
        }
    });
}

template <class IndexType, class Function>
void parallel_for_row_major(
    IndexType beginIndexX,
    IndexType endIndexX,
    IndexType beginIndexY,
    IndexType endIndexY,
    const Function& function)
{
    parallel_for(
        beginIndexY, endIndexY,
        [&](size_t j)
    {
        for(IndexType i = beginIndexX; i < endIndexX; ++i)
        {
            function(i, j);
        }
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// parallel for 3D
template <class IndexType, class Function>
void parallel_for(
    IndexType beginIndexX,
    IndexType endIndexX,
    IndexType beginIndexY,
    IndexType endIndexY,
    IndexType beginIndexZ,
    IndexType endIndexZ,
    const Function& function)
{
    parallel_for(
        beginIndexX, endIndexX,
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

template <class IndexType, class Function>
void parallel_for_row_major(
    IndexType beginIndexX,
    IndexType endIndexX,
    IndexType beginIndexY,
    IndexType endIndexY,
    IndexType beginIndexZ,
    IndexType endIndexZ,
    const Function& function)
{
    parallel_for(
        beginIndexZ, endIndexZ,
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // namespace ParallelFuncs
