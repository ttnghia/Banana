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

#ifndef __Parallel_BLAS__
#define __Parallel_BLAS__

#include <cassert>
#include <vector>

#include <tbb/tbb.h>
#include "./ParallelClass.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelBLAS
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// dot products
//
template<class RealType>
inline RealType dot_product(const std::vector<RealType>& x, const std::vector<RealType>& y)
{
    assert(x.size() == y.size());

    ParallelClass::vector_dot_product<RealType> v(x, y);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), v);

    return v.result;
}

template<class RealType, class VectorType>
inline RealType vec_dot_product(const std::vector<VectorType>& x, const std::vector<VectorType>& y)
{
    assert(x.size() == y.size());

    ParallelClass::vecvec_dot_product<RealType, VectorType> v(x, y);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, x.size()), v);

    return v.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// saxpy (y=alpha*x+y)
//
template<class RealType, class VectorType>
inline void add_scaled(RealType alpha, const std::vector<VectorType>& x,
                       std::vector<VectorType>& y)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
                      [&, alpha](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            y[i] += alpha * x[i];
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// y=x+beta*y
//
template<class RealType, class VectorType>
inline void scaled_add(RealType beta, const std::vector<VectorType>& x,
                       std::vector<VectorType>& y)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
                      [&, beta](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            y[i] = beta * y[i] + x[i];
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// x *= alpha
//
template<class RealType, class VectorType>
inline void scale(RealType alpha, const std::vector<VectorType>& x)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
                      [&, alpha](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            x[i] *= alpha;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelBLAS


#endif // __Parallel_BLAS__
