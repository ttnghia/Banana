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

#include <limits>
#include <vector>
#include <tbb/tbb.h>

#undef min
#undef max

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ... T>
class VectorDotProduct;

template<class RealType>
class VectorDotProduct<RealType>
{
public:
    VectorDotProduct(const std::vector<RealType>& vec1, const std::vector<RealType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), m_Result(0) {}
    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec2), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            m_Result += m_Vec1[i] * m_Vec2[i];
    }

    void join(VectorDotProduct& vdp)
    {
        m_Result += vdp.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType m_Result;

    const std::vector<RealType>& m_Vec1;
    const std::vector<RealType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
class VectorDotProduct<RealType, VectorType>
{
public:
    VectorDotProduct(const std::vector<VectorType>& vec1, const std::vector<VectorType>& vec2) : m_Vec1(vec1), m_Vec2(vec2), m_Result(0) {}
    VectorDotProduct(VectorDotProduct& vdp, tbb::split) : m_Vec1(vdp.m_Vec1), m_Vec2(vdp.m_Vec1), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
            m_Result += glm::dot(m_Vec1[i], m_Vec2[i]);
    }

    void join(VectorDotProduct& vdp)
    {
        m_Result += vdp.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType m_Result;

    const std::vector<VectorType>& m_Vec1;
    const std::vector<VectorType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class VectorMinElement
{
public:
    VectorMinElement(const std::vector<RealType>& vec) : m_Vector(vec), m_Result(std::numeric_limits<RealType>::max()) {}
    VectorMinElement(VectorMinElement& vme, tbb::split) : m_Vector(vme.m_Vector), m_Result(std::numeric_limits<RealType>::max()) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            m_Result = m_Result < m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void join(VectorMinElement& vme)
    {
        m_Result = m_Result < vme.m_Result ? m_Result : vme.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType m_Result;

    const std::vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class VectorMaxElement
{
public:
    VectorMaxElement(const std::vector<RealType>& vec) : m_Vector(vec), m_Result(std::numeric_limits<RealType>::min()) {}
    VectorMaxElement(VectorMaxElement& vme, tbb::split) : m_Vector(vme.m_Vector), m_Result(std::numeric_limits<RealType>::min()) {}

    // overload () so it does finding max
    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            m_Result = m_Result > m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void join(VectorMaxElement& vme)
    {
        m_Result = m_Result > vme.m_Result ? m_Result : vme.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType m_Result;

    const std::vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ... T>
class VectorMaxAbs;

template<class RealType>
class VectorMaxAbs<RealType>
{
public:
    VectorMaxAbs(const std::vector<RealType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmp = fabs(m_Vector[i]);
            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void join(VectorMaxAbs& vma)
    {
        m_Result = m_Result > vma.m_Result ? m_Result : vma.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType m_Result;

    const std::vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
class VectorMaxAbs<RealType, VectorType>
{
public:
    VectorMaxAbs(const std::vector<VectorType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorMaxAbs(VectorMaxAbs& vma, tbb::split) : m_Vector(vma.m_Vector), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmp = std::abs(m_Vector[i][0]);
            for(auto k = 1; k < m_Vector[i].length(); ++k)
            {
                tmp = tmp < std::abs(m_Vector[i][k]) ? std::abs(m_Vector[i][k]) : tmp;
            }

            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void join(VectorMaxAbs& vma)
    {
        m_Result = m_Result > vma.m_Result ? m_Result : vma.m_Result;
    }

    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                       m_Result;
    const std::vector<VectorType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
class VectorMaxNorm2
{
public:
    VectorMaxNorm2(const std::vector<VectorType>& vec) : m_Vector(vec), m_ResultMax(std::numeric_limits<RealType>::min()) {}
    VectorMaxNorm2(VectorMaxNorm2& vmmn, tbb::split) : m_Vector(vmmn.m_Vector), m_ResultMax(std::numeric_limits<RealType>::min()) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType mag2 = glm::length2(m_Vector[i]);
            m_ResultMax = m_ResultMax > mag2 ? m_ResultMax : mag2;
        }
    }

    void join(VectorMaxNorm2& vmmn)
    {
        m_ResultMax = m_ResultMax > vmmn.m_ResultMax ? m_ResultMax : vmmn.m_ResultMax;
    }

    RealType getResult() const noexcept { return m_ResultMax; }

private:
    RealType                       m_ResultMax;
    const std::vector<VectorType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class VectorMinMaxElements
{
public:
    VectorMinMaxElements(const std::vector<RealType>& vec) : m_Vector(vec), m_ResultMin(std::numeric_limits<RealType>::max()), m_ResultMax(std::numeric_limits<RealType>::min()) {}
    VectorMinMaxElements(VectorMinMaxElements& vmme, tbb::split) : m_Vector(vmme.m_Vector), m_ResultMin(std::numeric_limits<RealType>::max()), m_ResultMax(std::numeric_limits<RealType>::min()) {}

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

    RealType getMin() const noexcept { return m_ResultMin; }
    RealType getMax() const noexcept { return m_ResultMax; }

private:
    RealType m_ResultMin;
    RealType m_ResultMax;

    const std::vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
class VectorMinMaxNorm2
{
public:
    VectorMinMaxNorm2(const std::vector<VectorType>& vec) : m_Vector(vec), m_ResultMin(std::numeric_limits<RealType>::max()), m_ResultMax(std::numeric_limits<RealType>::min()) {}
    VectorMinMaxNorm2(VectorMinMaxNorm2& vmmn, tbb::split) : m_Vector(vmmn.m_Vector), m_ResultMin(std::numeric_limits<RealType>::max()), m_ResultMax(std::numeric_limits<RealType>::min()) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType mag2 = glm::length2(m_Vector[i]);
            m_ResultMin = m_ResultMin < mag2 ? m_ResultMin : mag2;
            m_ResultMax = m_ResultMax > mag2 ? m_ResultMax : mag2;
        }
    }

    void join(VectorMinMaxNorm2& vmmn)
    {
        m_ResultMin = m_ResultMin < vmmn.m_ResultMin ? m_ResultMin : vmmn.m_ResultMin;
        m_ResultMax = m_ResultMax > vmmn.m_ResultMax ? m_ResultMax : vmmn.m_ResultMax;
    }

    RealType getMin() const noexcept { return m_ResultMin; }
    RealType getMax() const noexcept { return m_ResultMax; }

private:
    RealType m_ResultMin;
    RealType m_ResultMax;

    const std::vector<VectorType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class VectorSum
{
public:
    VectorSum(const std::vector<RealType>& vec) : m_Vector(vec), m_Result(0) {}
    VectorSum(VectorSum& vsum, tbb::split) : m_Vector(vsum.v), m_Result(0) {}

    void operator()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            m_Result += m_Vector[i];
        }
    }

    void join(VectorSum& vsum)
    {
        m_Result += vsum.m_Result;
    }

    RealType getSum() const noexcept { return m_Result; }
private:
    const std::vector<RealType>& m_Vector;
    RealType                     m_Result;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParallelObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana