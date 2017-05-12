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

#include <vector>
#include <tbb/tbb.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorDotProduct
{
public:
    ScalarType result;

    VectorDotProduct(const std::vector<ScalarType>& vec1, const std::vector<ScalarType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), result(0)
    {}

    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec2), result(0)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            result += m_Vec1[i] * m_Vec2[i];
        }
    }

    void join(VectorDotProduct& vdp)
    {
        result += vdp.result;
    }

private:
    const std::vector<ScalarType>& m_Vec1;
    const std::vector<ScalarType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorDotProduct
{
    ScalarType result;

    VectorDotProduct(const std::vector<VectorType>& vec1, const std::vector<VectorType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), result(0)
    {}

    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec1), result(0)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
        {
#ifdef __Using_Eigen_Lib__
            m_Result += (v1[i]).dot(v2[i]);
#else
#ifdef __Using_GLM_Lib__
            m_Result += glm::dot(v1[i], v2[i]);
#else           // use default yocto
            result += ym::dot(v1[i], v2[i]);
#endif // __Using_GLM_Lib__
#endif          // __Using_Eigen_Lib__
        }
    }

    void join(VectorDotProduct& vdp)
    {
        result += vdp.result;
    }

private:
    const std::vector<VectorType>& m_Vec1;
    const std::vector<VectorType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMinElement
{
public:
    ScalarType result;

    VectorMinElement(const std::vector<ScalarType>& vec) : m_Vector(vec), result(1e100)
    {}

    VectorMinElement(VectorMinElement& vme, tbb::split) : m_Vector(vme.m_Vector), result(1e100)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
        {
            result = result < m_Vector[i] ? result : m_Vector[i];
        }
    }

    void join(VectorMinElement& vme)
    {
        result = result < vme.result ? result : vme.result;
    }

private:
    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMaxElement
{
public:
    ScalarType result;

    VectorMaxElement(const std::vector<ScalarType>& vec) : m_Vector(vec), result(-1e100)
    {}

    VectorMaxElement(VectorMaxElement& vme, tbb::split) : m_Vector(vme.m_Vector), result(-1e100)
    {}

    // overload () so it does finding max
    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
        {
            result = result > m_Vector[i] ? result : m_Vector[i];
        }
    }

    void join(VectorMaxElement& vme)
    {
        result = result > vme.result ? result : vme.result;
    }

private:
    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMaxAbs
{
public:
    ScalarType result;

    VectorMaxAbs(const std::vector<ScalarType>& vec) : m_Vector(vec), result(0)
    {}

    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), result(0)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
        {
            ScalarType tmp = fabs(m_Vector[i]);
            result = result > tmp ? result : tmp;
        }
    }

    void join(VectorMaxAbs& vma)
    {
        result = result > vma.result ? result : vma.result;
    }

private:
    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorMaxAbs
{
public:
    ScalarType result;

    VectorMaxAbs(const std::vector<VectorType>& vec) : m_Vector(vec), result(0)
    {}

    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), result(0)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
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
            ScalarType tmp = std::max(std::abs(m_Vector[i][ym::min_element(m_Vector[i])]), std::abs(m_Vector[i][ym::max_element(m_Vector[i])]));
#endif // __Using_GLM_Lib__
#endif // __Using_Eigen_Lib__

            result = result > tmp ? result : tmp;
        }
    }

    void join(VectorMaxAbs& v)
    {
        result = result > v.result ? result : v.result;
    }

private:
    const std::vector<VectorType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMinMaxElements
{
public:
    ScalarType resultMin;
    ScalarType resultMax;

    VectorMinMaxElements(const std::vector<ScalarType>& vec) : m_Vector(vec), resultMin(1e100), resultMax(-1e100)
    {}

    VectorMinMaxElements(VectorMinMaxElements& vmme, tbb::split) : m_Vector(vmme.m_Vector), resultMin(1e100), resultMax(-1e100)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            resultMin = resultMin < m_Vector[i] ? resultMin : m_Vector[i];
            resultMax = resultMax > m_Vector[i] ? resultMax : m_Vector[i];
        }
    }

    void join(VectorMinMaxElements& vmme)
    {
        resultMin = resultMin < vmme.resultMin ? resultMin : vmme.resultMin;
        resultMax = resultMax > vmme.resultMax ? resultMax : vmme.resultMax;
    }

private:
    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorMinMaxNorm2
{
public:
    ScalarType resultMin;
    ScalarType resultMax;

    VectorMinMaxNorm2(const std::vector<ScalarType>& vec) : m_Vector(vec), resultMin(1e100), resultMax(-1e100)
    {}

    VectorMinMaxNorm2(VectorMinMaxNorm2& vmmn, tbb::split) : m_Vector(vmmn.m_Vector), resultMin(1e100), resultMax(-1e100)
    {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; i++)
        {
#ifdef __Using_Eigen_Lib__
            ScalarType mag2 = v[i].squaredNorm();
#else
#ifdef __Using_GLM_Lib__
            ScalarType mag2 = glm::length2(v[i]);
#else             // use default yocto
            magx; // should throw error
#endif // __Using_GLM_Lib__
#endif            // __Using_Eigen_Lib__

            resultMin = resultMin < mag2 ? resultMin : mag2;
            resultMax = resultMax > mag2 ? resultMax : mag2;
        }
    }

    void join(VectorMinMaxNorm2& vmmn)
    {
        resultMin = resultMin < vmmn.resultMin ? resultMin : vmmn.resultMin;
        resultMax = resultMax > vmmn.resultMax ? resultMax : vmmn.resultMax;
    }

private:
    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelObjects
