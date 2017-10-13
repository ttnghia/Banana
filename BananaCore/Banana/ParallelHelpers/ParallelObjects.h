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

#include <Banana/Setup.h>

#include <atomic>
#include <limits>
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
class SpinLock
{
public:
    SpinLock() = default;
    SpinLock(const SpinLock&) {}
    SpinLock& operator =(const SpinLock&) { return *this; }

    void lock()
    {
        while(m_Lock.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock()
    {
        m_Lock.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_Lock = ATOMIC_FLAG_INIT;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class DotProduct
{
public:
    DotProduct(const Vector<VecX<N, RealType> >& vec1, const Vector<VecX<N, RealType> >& vec2) : m_Vec1(vec1), m_Vec2(vec2) {}
    DotProduct(DotProduct<N, RealType>& pobj, tbb::split) : m_Vec1(pobj.m_Vec1), m_Vec2(pobj.m_Vec1) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
            m_Result += glm::dot(m_Vec1[i], m_Vec2[i]);
    }

    void     join(DotProduct<N, RealType>& pobj) { m_Result += pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                          m_Result = 0;
    const Vector<VecX<N, RealType> >& m_Vec1;
    const Vector<VecX<N, RealType> >& m_Vec2;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class DotProduct<1, RealType>
{
public:
    DotProduct(const Vector<RealType>& vec1, const Vector<RealType>& vec2) : m_Vec1(vec1), m_Vec2(vec2) {}
    DotProduct(DotProduct<1, RealType>& pobj, tbb::split) : m_Vec1(pobj.m_Vec1), m_Vec2(pobj.m_Vec2) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
            m_Result += m_Vec1[i] * m_Vec2[i];
    }

    void     join(DotProduct<1, RealType>& pobj) { m_Result += pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                m_Result = 0;
    const Vector<RealType>& m_Vec1;
    const Vector<RealType>& m_Vec2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MinElement
{
public:
    MinElement(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MinElement(MinElement<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType vmin = glm::compMin(m_Vector[i]);
            m_Result = m_Result < vmin ? m_Result : vmin;
        }
    }

    void     join(MinElement<N, RealType>& pobj) { m_Result = m_Result < pobj.m_Result ? m_Result : pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                          m_Result = std::numeric_limits<RealType>::max();
    const Vector<VecX<N, RealType> >& m_Vector;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class MinElement<1, RealType>
{
public:
    MinElement(const Vector<RealType>& vec) : m_Vector(vec) {}
    MinElement(MinElement<1, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            m_Result = m_Result < m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void     join(MinElement<1, RealType>& pobj) { m_Result = m_Result < pobj.m_Result ? m_Result : pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                m_Result = std::numeric_limits<RealType>::max();
    const Vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MaxElement
{
public:
    MaxElement(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MaxElement(MaxElement<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType vmax = glm::compMax(m_Vector[i]);
            m_Result = m_Result > vmax ? m_Result : vmax;
        }
    }

    void     join(MaxElement<N, RealType>& pobj) { m_Result = m_Result > pobj.m_Result ? m_Result : pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                          m_Result = -std::numeric_limits<RealType>::max();
    const Vector<VecX<N, RealType> >& m_Vector;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class MaxElement<1, RealType>
{
public:
    MaxElement(const Vector<RealType>& vec) : m_Vector(vec) {}
    MaxElement(MaxElement<1, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            m_Result = m_Result > m_Vector[i] ? m_Result : m_Vector[i];
        }
    }

    void     join(MaxElement<1, RealType>& pobj) { m_Result = m_Result > pobj.m_Result ? m_Result : pobj.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                m_Result = -std::numeric_limits<RealType>::max();
    const Vector<RealType>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MaxAbs
{
public:
    MaxAbs(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MaxAbs(MaxAbs<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType tmp = glm::compMax(glm::abs(m_Vector[i]));
            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void     join(MaxAbs<N, RealType>& vma) { m_Result = m_Result > vma.m_Result ? m_Result : vma.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                          m_Result = 0;
    const Vector<VecX<N, RealType> >& m_Vector;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class MaxAbs<1, RealType>
{
public:
    MaxAbs(const Vector<RealType>& vec) : m_Vector(vec) {}
    MaxAbs(MaxAbs<1, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType tmp = fabs(m_Vector[i]);
            m_Result = m_Result > tmp ? m_Result : tmp;
        }
    }

    void     join(MaxAbs<1, RealType>& vma) { m_Result = m_Result > vma.m_Result ? m_Result : vma.m_Result; }
    RealType getResult() const noexcept { return m_Result; }

private:
    RealType                m_Result = 0;
    const Vector<RealType>& m_Vector;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MaxNorm2
{
public:
    MaxNorm2(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MaxNorm2(MaxNorm2<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType mag2 = glm::length2(m_Vector[i]);
            m_Result = m_Result > mag2 ? m_Result : mag2;
        }
    }

    void     join(MaxNorm2<N, RealType>& pobj) { m_Result = m_Result > pobj.m_Result ? m_Result : pobj.m_Result; }
    RealType getResult() const noexcept { return sqrt(m_Result); }

private:
    RealType                          m_Result = 0;
    const Vector<VecX<N, RealType> >& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MinMaxElements
{
public:
    MinMaxElements(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MinMaxElements(MinMaxElements<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            const auto& vec = m_Vector[i];
            for(int j = 0; j < N; ++j) {
                m_ResultMin[j] = (m_ResultMin[j] < vec[j]) ? m_ResultMin[j] : vec[j];
                m_ResultMax[j] = (m_ResultMax[j] > vec[j]) ? m_ResultMax[j] : vec[j];
            }
        }
    }

    void join(MinMaxElements<N, RealType>& pobj)
    {
        for(int j = 0; j < N; ++j) {
            m_ResultMin[j] = (m_ResultMin[j] < pobj.m_ResultMin[j]) ? m_ResultMin[j] : pobj.m_ResultMin[j];
            m_ResultMax[j] = (m_ResultMax[j] > pobj.m_ResultMax[j]) ? m_ResultMax[j] : pobj.m_ResultMax[j];
        }
    }

    const VecX<N, RealType>& getMin() const noexcept { return m_ResultMin; }
    const VecX<N, RealType>& getMax() const noexcept { return m_ResultMax; }

private:
    VecX<N, RealType>                 m_ResultMin = VecX<N, RealType>(std::numeric_limits<RealType>::max());
    VecX<N, RealType>                 m_ResultMax = VecX<N, RealType>(-std::numeric_limits<RealType>::max());
    const Vector<VecX<N, RealType> >& m_Vector;
};

////////////////////////////////////////////////////////////////////////////////
// N = 0 : plain C array
template<class RealType>
class MinMaxElements<0, RealType>
{
public:
    MinMaxElements(const RealType* vec) : m_Vector(vec) {}
    MinMaxElements(MinMaxElements<0, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            m_ResultMin = m_ResultMin < m_Vector[i] ? m_ResultMin : m_Vector[i];
            m_ResultMax = m_ResultMax > m_Vector[i] ? m_ResultMax : m_Vector[i];
        }
    }

    void join(MinMaxElements<0, RealType>& pobj)
    {
        m_ResultMin = m_ResultMin < pobj.m_ResultMin ? m_ResultMin : pobj.m_ResultMin;
        m_ResultMax = m_ResultMax > pobj.m_ResultMax ? m_ResultMax : pobj.m_ResultMax;
    }

    RealType getMin() const noexcept { return m_ResultMin; }
    RealType getMax() const noexcept { return m_ResultMax; }

private:
    RealType m_ResultMin = std::numeric_limits<RealType>::max();
    RealType m_ResultMax = -std::numeric_limits<RealType>::max();

    const RealType* m_Vector;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class MinMaxElements<1, RealType>
{
public:
    MinMaxElements(const Vector<RealType>& vec) : m_Vector(vec) {}
    MinMaxElements(MinMaxElements<1, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            m_ResultMin = m_ResultMin < m_Vector[i] ? m_ResultMin : m_Vector[i];
            m_ResultMax = m_ResultMax > m_Vector[i] ? m_ResultMax : m_Vector[i];
        }
    }

    void join(MinMaxElements<1, RealType>& pobj)
    {
        m_ResultMin = m_ResultMin < pobj.m_ResultMin ? m_ResultMin : pobj.m_ResultMin;
        m_ResultMax = m_ResultMax > pobj.m_ResultMax ? m_ResultMax : pobj.m_ResultMax;
    }

    RealType getMin() const noexcept { return m_ResultMin; }
    RealType getMax() const noexcept { return m_ResultMax; }

private:
    RealType m_ResultMin = std::numeric_limits<RealType>::max();
    RealType m_ResultMax = -std::numeric_limits<RealType>::max();

    const Vector<RealType>& m_Vector;
};



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MinMaxNorm2
{
public:
    MinMaxNorm2(const Vector<VecX<N, RealType> >& vec) : m_Vector(vec) {}
    MinMaxNorm2(MinMaxNorm2<N, RealType>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i) {
            RealType mag2 = glm::length2(m_Vector[i]);
            m_ResultMin = m_ResultMin < mag2 ? m_ResultMin : mag2;
            m_ResultMax = m_ResultMax > mag2 ? m_ResultMax : mag2;
        }
    }

    void join(MinMaxNorm2<N, RealType>& pobj)
    {
        m_ResultMin = m_ResultMin < pobj.m_ResultMin ? m_ResultMin : pobj.m_ResultMin;
        m_ResultMax = m_ResultMax > pobj.m_ResultMax ? m_ResultMax : pobj.m_ResultMax;
    }

    RealType getMin() const noexcept { return sqrt(m_ResultMin); }
    RealType getMax() const noexcept { return sqrt(m_ResultMax); }

private:
    RealType m_ResultMin = std::numeric_limits<RealType>::max();
    RealType m_ResultMax = 0;

    const Vector<VecX<N, RealType> >& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class VectorSum
{
public:
    VectorSum(const Vector<T>& vec) : m_Vector(vec) {}
    VectorSum(VectorSum<T>& pobj, tbb::split) : m_Vector(pobj.m_Vector) {}

    void operator ()(const tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
            m_Result += m_Vector[i];
    }

    void join(VectorSum<T>& pobj) { m_Result += pobj.m_Result; }
    T    getSum() const noexcept { return m_Result; }
private:
    T                m_Result = T(0);
    const Vector<T>& m_Vector;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParallelObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana