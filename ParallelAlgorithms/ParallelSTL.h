//------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------

#ifndef __Parallel_STL__
#define __Parallel_STL__

#include <cassert>
#include <vector>

#include <tbb/tbb.h>
#include "./ParallelClass.h"


//------------------------------------------------------------------------------------------
namespace ParallelSTL
{
//------------------------------------------------------------------------------------------
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
inline ScalarType abs_max(const std::vector<VectorType>& x)
{
    ParallelClass::vecvec_max_abs<ScalarType, VectorType> m(x);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), m);

    return m.result;
}

//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
} // end namespace ParallelSTL

#endif // __Parallel_STL__

