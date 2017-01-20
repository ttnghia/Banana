//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifndef __Parallel_STL__
#define __Parallel_STL__

#include <cassert>
#include <vector>   
#include <cmath>


#include <tbb/tbb.h>
#include "./ParallelClass.h"
#include "./ParallelFuncs.h"


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelSTL
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// inf-norm (maximum absolute value)
//
template<class ScalarType>
inline ScalarType abs_max(const std::vector<ScalarType>& x)
{
    ParallelClass::vector_max_abs<ScalarType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class ScalarType, class VectorType>
inline ScalarType vec_abs_max(const std::vector<VectorType>& x)
{
    ParallelClass::vecvec_max_abs<ScalarType, VectorType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// min/max element
template<class ScalarType>
inline ScalarType min_element(const std::vector<ScalarType>& x)
{
    ParallelClass::vector_min<ScalarType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class ScalarType>
inline ScalarType max_element(const std::vector<ScalarType>& x)
{
    ParallelClass::vector_max<ScalarType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

template<class ScalarType>
inline void min_max_element(const std::vector<ScalarType>& x,
                            ScalarType& min_element, ScalarType& max_element)
{
    ParallelClass::vector_min_max<ScalarType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    min_element = m.result_min;
    max_element = m.result_max;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sorting
template<class ScalarType>
inline void sort_asd(std::vector<ScalarType>& v)
{
    tbb::parallel_sort(v);
}

template<class ScalarType>
inline void sort_dsd(std::vector<ScalarType>& v)
{
    tbb::parallel_sort(std::begin(v), std::end(v), std::greater<ScalarType>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelSTL

#endif // __Parallel_STL__

