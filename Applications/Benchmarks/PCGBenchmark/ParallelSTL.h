//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/11/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifndef __Parallel_STL__
#define __Parallel_STL__

#include <cassert>
#include <vector>   
#include <cmath>


#include <tbb/tbb.h>
#include "./ParallelClass.h"
#include "./Scheduler.h"


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelSTL
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// inf-norm (maximum absolute value)
//
template<class RealType>
inline RealType abs_max(const std::vector<RealType>& x)
{
    ParallelClass::vector_max_abs<RealType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class RealType, class VectorType>
inline RealType vec_abs_max(const std::vector<VectorType>& x)
{
    ParallelClass::vecvec_max_abs<RealType, VectorType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class RealType>
inline RealType min_element(const std::vector<RealType>& x)
{
    ParallelClass::vector_min<RealType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class RealType>
inline RealType max_element(const std::vector<RealType>& x)
{
    ParallelClass::vector_max<RealType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class RealType>
inline void min_max_element(const std::vector<RealType>& x,
                            RealType& min_element, RealType& max_element)
{
    ParallelClass::vector_min_max<RealType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    min_element = m.result_min;
    max_element = m.result_max;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class RealType>
inline void sort_asd(std::vector<RealType>& v)
{
    tbb::parallel_sort(v);
}

template<class RealType>
inline void sort_dsd(std::vector<RealType>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<RealType>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// fill
template <class RandomIterator, class T>
void parallel_fill(const RandomIterator& begin,
                   const RandomIterator& end,
                   const T& value)
{
    auto diff = end - begin;
    if(diff <= 0)
    {
        return;
    }

    size_t size = static_cast<size_t>(diff);
    parallel_for(0, size, [begin, value](size_t i)
    {
        begin[i] = value;
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelSTL

#endif // __Parallel_STL__

