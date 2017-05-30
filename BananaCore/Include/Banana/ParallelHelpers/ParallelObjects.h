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
    VectorDotProduct(const std::vector<ScalarType>& vec1, const std::vector<ScalarType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), m_Result(0) {}
    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec2), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            m_Result += m_Vec1[i] * m_Vec2[i];
        }
    }

    void join(VectorDotProduct& vdp)
    {
        m_Result += vdp.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<ScalarType>& m_Vec1;
    const std::vector<ScalarType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorDotProduct
{
    VectorDotProduct(const std::vector<VectorType>& vec1, const std::vector<VectorType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), m_Result(0) {}
    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec1), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
        {
#ifdef __Using_Eigen_Lib__
            m_Result += (v1[i]).dot(v2[i]);
#else
#ifdef __Using_GLM_Lib__
            m_Result += glm::dot(v1[i], v2[i]);
#else           // use default yocto
            m_Result += ym::dot(v1[i], v2[i]);
#endif // __Using_GLM_Lib__
#endif          // __Using_Eigen_Lib__
        }
    }

    void join(VectorDotProduct& vdp)
    {
        m_Result += vdp.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<VectorType>& m_Vec1;
    const std::vector<VectorType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMinElement
{
public:
    VectorMinElement(const std::vector<ScalarType>& vec) : m_Vector(vec), m_Result(1e100) {}
    VectorMinElement(VectorMinElement& vme, tbb::split) : m_Vector(vme.m_Vector), m_Result(1e100) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
        {
            m_Result = m_Result < m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void join(VectorMinElement& vme)
    {
        m_Result = m_Result < vme.m_Result ? m_Result : vme.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMaxElement
{
public:
    VectorMaxElement(const std::vector<ScalarType>& vec) : m_Vector(vec), m_Result(-1e100) {}
    VectorMaxElement(VectorMaxElement& vme, tbb::split) : m_Vector(vme.m_Vector), m_Result(-1e100) {}

    // overload () so it does finding max
    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
        {
            m_Result = m_Result > m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void join(VectorMaxElement& vme)
    {
        m_Result = m_Result > vme.m_Result ? m_Result : vme.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMaxAbs
{
public:
    VectorMaxAbs(const std::vector<ScalarType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
        {
            ScalarType tmp = fabs(m_Vector[i]);
            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void join(VectorMaxAbs& vma)
    {
        m_Result = m_Result > vma.m_Result ? m_Result : vma.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorMaxAbs
{
public:
    VectorMaxAbs(const std::vector<VectorType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
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

            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void join(VectorMaxAbs& v)
    {
        m_Result = m_Result > v.m_Result ? m_Result : v.m_Result;
    }

    ScalarType getResult() const noexcept { return m_Result; }

private:
    ScalarType m_Result;

    const std::vector<VectorType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorMinMaxElements
{
public:
    VectorMinMaxElements(const std::vector<ScalarType>& vec) : m_Vector(vec), m_ResultMin(1e100), m_ResultMax(-1e100) {}
    VectorMinMaxElements(VectorMinMaxElements& vmme, tbb::split) : m_Vector(vmme.m_Vector), m_ResultMin(1e100), m_ResultMax(-1e100) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            m_ResultMin = m_ResultMin < m_Vector[i] ? m_ResultMin : m_Vector[i];
            m_ResultMax = m_ResultMax > m_Vector[i] ? m_ResultMax : m_Vector[i];
        }
    }

    void join(VectorMinMaxElements& vmme)
    {
        m_ResultMin = m_ResultMin < vmme.m_ResultMin ? m_ResultMin : vmme.m_ResultMin;
        m_ResultMax = m_ResultMax > vmme.m_ResultMax ? m_ResultMax : vmme.m_ResultMax;
    }

    ScalarType getMin() const noexcept { return m_ResultMin; }
    ScalarType getMax() const noexcept { return m_ResultMax; }

private:
    ScalarType m_ResultMin;
    ScalarType m_ResultMax;

    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType, class VectorType>
class VectorMinMaxNorm2
{
public:
    VectorMinMaxNorm2(const std::vector<ScalarType>& vec) : m_Vector(vec), m_ResultMin(1e100), m_ResultMax(-1e100) {}
    VectorMinMaxNorm2(VectorMinMaxNorm2& vmmn, tbb::split) : m_Vector(vmmn.m_Vector), m_ResultMin(1e100), m_ResultMax(-1e100) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
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

            m_ResultMin = m_ResultMin < mag2 ? m_ResultMin : mag2;
            m_ResultMax = m_ResultMax > mag2 ? m_ResultMax : mag2;
        }
    }

    void join(VectorMinMaxNorm2& vmmn)
    {
        m_ResultMin = m_ResultMin < vmmn.m_ResultMin ? m_ResultMin : vmmn.m_ResultMin;
        m_ResultMax = m_ResultMax > vmmn.m_ResultMax ? m_ResultMax : vmmn.m_ResultMax;
    }

    ScalarType getMin() const noexcept { return m_ResultMin; }
    ScalarType getMax() const noexcept { return m_ResultMax; }

private:
    ScalarType m_ResultMin;
    ScalarType m_ResultMax;

    const std::vector<ScalarType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class VectorSum
{
public:
    VectorSum(const std::vector<ScalarType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorSum(VectorSum& vsum, tbb::split) : m_Vector(vsum.v), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd;++i)
        {
            m_Result += m_Vector[i];
        }
    }

    void join(VectorSum& vsum)
    {
        m_Result += vsum.m_Result;
    }

    ScalarType getSum() const noexcept { return m_Result; }
private:
    const std::vector<ScalarType>& m_Vector;
    ScalarType                     m_Result;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ParallelObjects
