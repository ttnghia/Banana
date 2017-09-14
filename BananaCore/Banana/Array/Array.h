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

#include <algorithm>
#include <cassert>
#include <vector>
#include <istream>
#include <sstream>
#include <cstdlib>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Base array class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
class Array
{
public:
    using iterator               = typename Vector<T>::iterator;
    using const_iterator         = typename Vector<T>::const_iterator;
    using reverse_iterator       = typename Vector<T>::reverse_iterator;
    using const_reverse_iterator = typename Vector<T>::const_reverse_iterator;
    using size_type              = typename Vector<T>::size_type;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors & destructor
    Array() : m_Size(VecX<N, size_type>(0)) {}
    Array(const Array<N, T>& other) : m_Size(other.m_Size), m_Data(other.m_Data) {}

    template<class IndexType>
    Array(const VecX<N, IndexType>& size) : m_Size(size) { m_Data.resize(product(m_Size)); }

    ~Array(void) { m_Data.clear(); }

    ////////////////////////////////////////////////////////////////////////////////
    // copy operator
    Array<N, T>& operator =(const Array<N, T>& other)
    {
        // check for self-assignment
        if(&other == this)
            return *this;

        m_Size = other.m_Size;
        m_Data = other.m_Data;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Array2D constructor =>
    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY) : m_Size(sizeX, sizeY), m_Data(sizeX * sizeY)
    {
        static_assert(N == 2, "Array dimension != 2");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, const Vector<T>& data) : m_Size(sizeX, sizeY), m_Data(data)
    {
        static_assert(N == 2, "Array dimension != 2");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, const T& value) : m_Size(sizeX, sizeY), m_Data(sizeX * sizeY, value)
    {
        static_assert(N == 2, "Array dimension != 2");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, T* copyData) : m_Size(sizeX, sizeY), m_Data(sizeX * sizeY, copyData)
    {
        static_assert(N == 2, "Array dimension != 2");
    }

    // => end Array2D constructor
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Array3D constructor =>
    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, IndexType sizeZ) : m_Size(sizeX, sizeY, sizeZ), m_Data(sizeX * sizeY * sizeZ)
    {
        static_assert(N == 3, "Array dimension != 3");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, IndexType sizeZ, Vector<T>& data) : m_Size(sizeX, sizeY, sizeZ), m_Data(data)
    {
        static_assert(N == 3, "Array dimension != 3");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value) : m_Size(sizeX, sizeY, sizeZ), m_Data(sizeX * sizeY * sizeZ, value)
    {
        static_assert(N == 3, "Array dimension != 3");
    }

    template<class IndexType>
    Array(IndexType sizeX, IndexType sizeY, IndexType sizeZ, T* copyData) : m_Size(sizeX, sizeY, sizeZ), m_Data(sizeX * sizeY * sizeZ, copyData)
    {
        static_assert(N == 3, "Array dimension != 3");
    }

    // => end Array3D constructor
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // size access
    __BNN_FORCE_INLINE bool                      empty() const { return m_Data.empty(); }
    __BNN_FORCE_INLINE size_type                 capacity(void) const { return m_Data.capacity(); }
    __BNN_FORCE_INLINE size_type                 dataSize(void) const { return m_Data.size(); }
    __BNN_FORCE_INLINE const VecX<N, size_type>& size() const { return m_Size; }

    ////////////////////////////////////////////////////////////////////////////////
    // index processing
    template<class IndexType>
    __BNN_FORCE_INLINE bool isValidIndex(const VecX<N, IndexType>& index) const
    {
        for(Int i = 0; i < N; ++i)
        {
            if(index[i] < 0 || index[i] >= m_Size[i])
                return false;
        }

        return true;
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void checkIndex(const VecX<N, IndexType>& index) const
    {
        bool bIndexValid = isValidIndex<IndexType>(index);

        if(!bIndexValid)
        {
            std::stringstream ss;
            ss << "Invalid Array index: ";

            for(Int i = 0; i < N - 1; ++i)
                ss << index[i] << "/" << m_Size[i] << ", ";
            ss << index[N - 1] << "/" << m_Size[N - 1];

            printf("%s\n", ss.str().c_str());
        }

        assert(bIndexValid);
    }

    bool equalSize(const Array<N, T>& other) const
    {
        for(Int i = 0; i < N; ++i)
        {
            if(m_Size[i] != other.m_Size[i])
                return false;
        }
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Array2D =>
    template<class IndexType>
    __BNN_FORCE_INLINE bool isValidIndex(IndexType i, IndexType j) const
    {
        return (i >= 0 &&
                j >= 0 &&
                static_cast<size_type>(i) < m_Size[0] &&
                static_cast<size_type>(j) < m_Size[1]);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE size_type getCellLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
#ifndef NDEBUG
        checkIndex<IndexType>(Vec2<IndexType>(i, j));
#endif
        return static_cast<size_type>(j) * m_Size[0] + static_cast<size_type>(i);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE size_type getCellLinearizedIndex(const Vec2<IndexType>& index) const
    {
        static_assert(N == 2, "Array dimension != 2");
        getCellLinearizedIndex<IndexType>(index[0], index[1]);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE const T& operator ()(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE T& operator ()(IndexType i, IndexType j)
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE const T& operator ()(const Vec2<IndexType>& index) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1])];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE T& operator ()(const Vec2<IndexType>& index)
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1])];
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Array3D =>
    template<class IndexType>
    __BNN_FORCE_INLINE bool isValidIndex(IndexType i, IndexType j, IndexType k) const
    {
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<size_type>(i) < m_Size[0] &&
                static_cast<size_type>(j) < m_Size[1] &&
                static_cast<size_type>(k) < m_Size[2]);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE size_type getCellLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
#ifndef NDEBUG
        checkIndex<IndexType>(Vec3<IndexType>(i, j, k));
#endif
        return (static_cast<size_type>(k) * m_Size[1] + static_cast<size_type>(j)) * m_Size[0] + static_cast<size_type>(i);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE size_type getCellLinearizedIndex(const Vec3<IndexType>& index) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return getCellLinearizedIndex<IndexType>(index[0], index[1], index[2]);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE const T& operator ()(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE T& operator ()(IndexType i, IndexType j, IndexType k)
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE const T& operator ()(const Vec3<IndexType>& index) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1], index[2])];
    }

    template<class IndexType>
    __BNN_FORCE_INLINE T& operator ()(const Vec3<IndexType>& index)
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1], index[2])];
    }

    // => Array3D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // iterators
    __BNN_FORCE_INLINE Vector<T>&       data() { return m_Data; }
    __BNN_FORCE_INLINE const Vector<T>& data() const { return m_Data; }

    __BNN_FORCE_INLINE const T& front(void) const { assert(m_Data.size() > 0); return m_Data.front(); }
    __BNN_FORCE_INLINE T&       front(void) { assert(m_Data.size() > 0); return m_Data.front(); }

    __BNN_FORCE_INLINE const T& back(void) const { assert(m_Data.size() > 0); return m_Data.back(); }
    __BNN_FORCE_INLINE T&       back(void) { assert(m_Data.size() > 0); return m_Data.back(); }

    __BNN_FORCE_INLINE iterator begin(void) { return m_Data.begin(); }
    __BNN_FORCE_INLINE iterator end(void) { return m_Data.end(); }

    __BNN_FORCE_INLINE const_iterator cbegin(void) const { return m_Data.cbegin(); }
    __BNN_FORCE_INLINE const_iterator cend(void) const { return m_Data.cend(); }

    __BNN_FORCE_INLINE reverse_iterator rbegin(void) { return reverse_iterator(end()); }
    __BNN_FORCE_INLINE reverse_iterator rend(void) { return reverse_iterator(begin()); }

    __BNN_FORCE_INLINE const_reverse_iterator crbegin(void) const { return const_reverse_iterator(cend()); }
    __BNN_FORCE_INLINE const_reverse_iterator crend(void) const { return const_reverse_iterator(cbegin()); }

    ////////////////////////////////////////////////////////////////////////////////
    // data manipulation
    template<class IndexType>
    __BNN_FORCE_INLINE void assign(const VecX<N, IndexType>& size, const T& value)
    {
        m_Size = size;
        m_Data.assign(product(m_Size), value);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void assign(const VecX<N, IndexType>& size, const T* copydata)
    {
        m_Size = size;
        m_Data.assign(product(m_Size), copydata);
    }

    __BNN_FORCE_INLINE void assign(const T& value) { m_Data.assign(m_Data.size(), value); }
    __BNN_FORCE_INLINE void copyDataFrom(const Array<N, T>& other) { __BNN_ASSERT(equalSize(other)); m_Data = other.m_Data; }
    __BNN_FORCE_INLINE void setZero() { m_Data.assign(m_Data.size(), 0); }
    __BNN_FORCE_INLINE void clear() { m_Data.resize(0); m_Size = VecX<N, size_type>(0); }
    __BNN_FORCE_INLINE void swap(Array<N, T>& other) { std::swap(m_Size, other.m_Size); m_Data.swap(other.m_Data); }

    template<class IndexType>
    __BNN_FORCE_INLINE void reserve(const VecX<N, IndexType>& size) { m_Data.reserve(product(size)); }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(const VecX<N, IndexType>& newSize) { m_Size = newSize;  m_Data.resize(product(m_Size)); }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(const VecX<N, IndexType>& newSize, const T& value) { m_Size = newSize;  m_Data.resize(product(m_Size), value); }

    ////////////////////////////////////////////////////////////////////////////////
    // Array2D =>
    template<class IndexType>
    __BNN_FORCE_INLINE void assign(IndexType sizeX, IndexType sizeY, const T& value)
    {
        static_assert(N == 2, "Array dimension != 2");
        assign(Vec2<IndexType>(sizeX, sizeY), value);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void assign(IndexType sizeX, IndexType sizeY, const T* copydata)
    {
        static_assert(N == 2, "Array dimension != 2");
        assign(Vec2<IndexType>(sizeX, sizeY), copydata);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void reserve(IndexType sizeX, IndexType sizeY)
    {
        static_assert(N == 2, "Array dimension != 2");
        m_Data.reserve(sizeX * sizeY);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(IndexType sizeX, IndexType sizeY)
    {
        static_assert(N == 2, "Array dimension != 2");
        resize(Vec2<IndexType>(sizeX, sizeY));
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(IndexType sizeX, IndexType sizeY, const T& value)
    {
        static_assert(N == 2, "Array dimension != 2");
        resize(Vec2<IndexType>(sizeX, sizeY), value);
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Array3D =>
    template<class IndexType>
    __BNN_FORCE_INLINE void assign(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        static_assert(N == 3, "Array dimension != 3");
        assign(Vec3<IndexType>(sizeX, sizeY, sizeZ), value);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void assign(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T* copydata)
    {
        static_assert(N == 3, "Array dimension != 3");
        assign(Vec3<IndexType>(sizeX, sizeY, sizeZ), copydata);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void reserve(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        static_assert(N == 3, "Array dimension != 3");
        m_Data.reserve(sizeX * sizeY * sizeZ);
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        static_assert(N == 3, "Array dimension != 3");
        resize(Vec3<IndexType>(sizeX, sizeY, sizeZ));
    }

    template<class IndexType>
    __BNN_FORCE_INLINE void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        static_assert(N == 3, "Array dimension != 3");
        resize(Vec3<IndexType>(sizeX, sizeY, sizeZ), value);
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

private:
    VecX<N, size_type> m_Size;
    Vector<T>          m_Data;

    __BNN_FORCE_INLINE size_type product(const VecX<N, size_type>& vec)
    {
        size_type count = vec[0];
        for(Int i = 1; i < N; ++i)
            count *= vec[i];

        return count;
    }
};  // end class Array

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParallelObjects { class SpinLock; }

using Array2c        = Array<2, char>;
using Array2uc       = Array<2, unsigned char>;
using Array2s        = Array<2, short>;
using Array2us       = Array<2, unsigned short>;
using Array2ll       = Array<2, long long>;
using Array2ull      = Array<2, unsigned long long>;
using Array2i        = Array<2, int>;
using Array2ui       = Array<2, unsigned int>;
using Array2f        = Array<2, float>;
using Array2d        = Array<2, double>;
using Array2r        = Array<2, Real>;
using Array2SpinLock = Array<2, ParallelObjects::SpinLock>;

using Array2_VecChar   = Array<2, Vector<char> >;
using Array2_VecInt    = Array<2, Vector<int> >;
using Array2_VecUInt   = Array<2, Vector<unsigned int> >;
using Array2_VecFloat  = Array<2, Vector<float> >;
using Array2_VecDouble = Array<2, Vector<double> >;

////////////////////////////////////////////////////////////////////////////////
using Array3c        = Array<3, char>;
using Array3uc       = Array<3, unsigned char>;
using Array3s        = Array<3, short>;
using Array3us       = Array<3, unsigned short>;
using Array3ll       = Array<3, long long>;
using Array3ull      = Array<3, unsigned long long>;
using Array3i        = Array<3, int>;
using Array3ui       = Array<3, unsigned int>;
using Array3f        = Array<3, float>;
using Array3d        = Array<3, double>;
using Array3r        = Array<3, Real>;
using Array3SpinLock = Array<3, ParallelObjects::SpinLock>;

using Array3_VecChar   = Array<3, Vector<char> >;
using Array3_VecInt    = Array<3, Vector<int> >;
using Array3_VecUInt   = Array<3, Vector<unsigned int> >;
using Array3_VecFloat  = Array<3, Vector<float> >;
using Array3_VecDouble = Array<3, Vector<double> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana