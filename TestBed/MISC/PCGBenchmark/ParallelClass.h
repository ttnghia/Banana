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

#ifndef __Parallel_Class__
#define __Parallel_Class__

#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelClass
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class vector_dot_product
{
    const std::vector<ScalarType>& v1;
    const std::vector<ScalarType>& v2;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vector_dot_product(const std::vector<ScalarType>& vec1,
                       const std::vector<ScalarType>& vec2)
        : v1(vec1), v2(vec2), result(0)
    {}

    // splitting constructor
    vector_dot_product(vector_dot_product& vdp, tbb::split)
        : v1(vdp.v1), v2(vdp.v2), result(0)
    {}

    // overload () so it does a dot product
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
            result += v1[i] * v2[i];
        }
    }
    // join method adds partial results
    void join(vector_dot_product& v)
    {
        result += v.result;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class vecvec_dot_product
{
    const std::vector<VectorType>& v1;
    const std::vector<VectorType>& v2;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vecvec_dot_product(const std::vector<VectorType>& vec1, const std::vector<VectorType>& vec2)
        : v1(vec1), v2(vec2), result(0)
    {}

    // splitting constructor
    vecvec_dot_product(vecvec_dot_product& vdp, tbb::split)
        : v1(vdp.v1), v2(vdp.v2), result(0)
    {}

    // overload () so it does a dot product
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {

#ifdef __Using_Eigen_Lib__
            result += (v1[i]).dot(v2[i]);
#else
#ifdef __Using_GLM_Lib__
            result += glm::dot(v1[i], v2[i]);
#else
#ifdef __Using_Cem_Lib__
            result += v1[i].Dot(v2[i]);
#else // use default yocto
            result += ym::dot(v1[i], v2[i]);
#endif // __Using_Cem_Lib__
#endif // __Using_GLM_Lib__
#endif // __Using_Eigen_Lib__

        }
    }
    // join method adds partial results
    void join(vecvec_dot_product& v)
    {
        result += v.result;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class vector_min
{
    const std::vector<ScalarType>& v;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vector_min(const std::vector<ScalarType>& vec)
        : v(vec), result(1e100)
    {}

    // splitting constructor
    vector_min(vector_min& vdp, tbb::split)
        : v(vdp.v), result(1e100)
    {}

    // overload () so it does finding max
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
            result = result < v[i] ? result : v[i];
        }
    }

    void join(vector_min& v)
    {
        result = result < v.result ? result : v.result;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class vector_max
{
    const std::vector<ScalarType>& v;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vector_max(const std::vector<ScalarType>& vec)
        : v(vec), result(-1e100)
    {}

    // splitting constructor
    vector_max(vector_max& vdp, tbb::split)
        : v(vdp.v), result(-1e100)
    {}

    // overload () so it does finding max
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
            result = result > v[i] ? result : v[i];
        }
    }

    void join(vector_max& v)
    {
        result = result > v.result ? result : v.result;
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class vector_min_max
{
    const std::vector<ScalarType>& v;
public:
    ScalarType result_min;  // put the result here
    ScalarType result_max;  // put the result here

    // constructor copies the arguments into local storage
    vector_min_max(const std::vector<ScalarType>& vec)
        : v(vec), result_min(1e100), result_max(-1e100)
    {}

    // splitting constructor
    vector_min_max(vector_min_max& vdp, tbb::split)
        : v(vdp.v), result_min(1e100), result_max(-1e100)
    {}

    // overload () so it does finding max
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
            result_min = result_min < v[i] ? result_min : v[i];
            result_max = result_max > v[i] ? result_max : v[i];
        }
    }

    void join(vector_min_max& v)
    {
        result_min = result_min < v.result_min ? result_min : v.result_min;
        result_max = result_max > v.result_max ? result_max : v.result_max;
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class vector_max_abs
{
    const std::vector<ScalarType>& v;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vector_max_abs(const std::vector<ScalarType>& vec)
        : v(vec), result(0)
    {}

    // splitting constructor
    vector_max_abs(vector_max_abs& vdp, tbb::split)
        : v(vdp.v), result(0)
    {}

    // overload () so it does finding max
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
            ScalarType tmp = fabs(v[i]);
            result = result > tmp ? result : tmp;
        }
    }

    void join(vector_max_abs& v)
    {
        result = result > v.result ? result : v.result;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class vecvec_max_abs
{
    const std::vector<VectorType>& v;
public:
    ScalarType result;  // put the result here
    // constructor copies the arguments into local storage
    vecvec_max_abs(const std::vector<VectorType>& vec)
        : v(vec), result(0)
    {}

    // splitting constructor
    vecvec_max_abs(vecvec_max_abs& vdp, tbb::split)
        : v(vdp.v), result(0)
    {}

    // overload () so it does finding max
    void operator() (const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); i++)
        {
#ifdef __Using_Eigen_Lib__
            ScalarType tmp = (ScalarType)v[i].cwiseAbs().maxCoeff();
#else
#ifdef __Using_GLM_Lib__
            ScalarType tmp = std::abs(v[i][0]);
            for(auto k = 1; k < v[i].length(); ++k)
            {
                tmp = tmp < std::abs(v[i][k]) ? std::abs(v[i][k]) : tmp;
            }
#else
#ifdef __Using_Cem_Lib__
            ScalarType tmp = std::abs(v[i][0]);
            for(auto k = 1; k < 3; ++k)
            {
                tmp = tmp < std::abs(v[i][k]) ? std::abs(v[i][k]) : tmp;
            }
#else
            ScalarType tmp = std::max(std::abs(v[i][ym::min_element(v[i])]),
                                      std::abs(v[i][ym::max_element(v[i])]));
#endif // __Using_Cem_Lib__
#endif // __Using_GLM_Lib__
#endif // __Using_Eigen_Lib__

            result = result > tmp ? result : tmp;
        }
    }

    void join(vecvec_max_abs& v)
    {
        result = result > v.result ? result : v.result;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelClass

#endif // __Parallel_Class__
