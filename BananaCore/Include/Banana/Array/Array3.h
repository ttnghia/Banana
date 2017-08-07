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

#include <algorithm>
#include <cassert>
#include <vector>
#include <istream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class Array3
{
private:
    typename std::vector<T>::size_type m_SizeX, m_SizeY, m_SizeZ;
    std::vector<T>                     m_Data;

public:
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
    using size_type              = typename std::vector<T>::size_type;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors & destructor
    Array3(void) : m_SizeX(0), m_SizeY(0), m_SizeZ(0) {}

    template<class IndexType>
    Array3(IndexType sizeX, IndexType sizeY, IndexType sizeZ) : m_SizeX(sizeX), m_SizeY(sizeY), m_SizeZ(sizeZ), m_Data(sizeX * sizeY * sizeZ) {}

    template<class IndexType>
    Array3(IndexType sizeX, IndexType sizeY, IndexType sizeZ, std::vector<T>& data) : m_SizeX(sizeX), m_SizeY(sizeY), m_SizeZ(sizeZ), m_Data(data) {}

    template<class IndexType>
    Array3(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value) : m_SizeX(sizeX), m_SizeY(sizeY), m_SizeZ(sizeZ), m_Data(sizeX * sizeY * sizeZ, value) {}

    template<class IndexType>
    Array3(IndexType sizeX, IndexType sizeY, IndexType sizeZ, T* data) : m_SizeX(sizeX), m_SizeY(sizeY), m_SizeZ(sizeZ), m_Data(sizeX * sizeY * sizeZ, data) {}

    Array3(const Array3<T>& other) : m_SizeX(other.m_SizeX), m_SizeY(other.m_SizeY), m_SizeZ(other.m_SizeZ), m_Data(other.m_Data) {}

    ~Array3(void)
    {
        m_Data.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidIndex(IndexType i, IndexType j, IndexType k)  const
    {
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<size_type>(i) < m_SizeX &&
                static_cast<size_type>(j) < m_SizeY &&
                static_cast<size_type>(k) < m_SizeZ);
    }

    template<class IndexType>
    bool isValidIndex(const glm::tvec3<IndexType>& index) const
    {
        return isValidIndex<IndexType>(index[0], index[1], index[2]);
    }

    template<class IndexType>
    void checkIndex(IndexType i, IndexType j, IndexType k) const
    {
        bool bIndexValid = isValidIndex<IndexType>(i, j, k);

        if(!bIndexValid)
        {
            std::cout << "Invalid Array3 index: i = " << i << " / sizeX = " << m_SizeX << ", j = " << j << " / sizeY = " << m_SizeY << ", k = " << k << " / sizeZ = " << m_SizeZ << std::endl;
        }

        assert(bIndexValid);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    size_type getLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        checkIndex<IndexType>(i, j, k);
        return (static_cast<size_type>(k) * m_SizeY + static_cast<size_type>(j)) * m_SizeX + static_cast<size_type>(i);
    }

    template<class IndexType>
    size_type getLinearizedIndex(const glm::tvec3<IndexType>& index) const
    {
        return getLinearizedIndex<IndexType>(index[0], index[1], index[2]);
    }

    template<class IndexType>
    const T& operator()(IndexType i, IndexType j, IndexType k) const
    {
        return m_Data[getLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    T& operator()(IndexType i, IndexType j, IndexType k)
    {
        return m_Data[getLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    const T& operator()(const glm::tvec3<IndexType>& index) const
    {
        return m_Data[getLinearizedIndex<IndexType>(index)];
    }

    template<class IndexType>
    T& operator()(const glm::tvec3<IndexType>& index)
    {
        return m_Data[getLinearizedIndex<IndexType>(index)];
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool operator==(const Array3<T>& other) const
    {
        return m_SizeX == other.m_SizeX && m_SizeY == other.m_SizeY && m_SizeZ == other.m_SizeZ && m_Data == other.m_Data;
    }

    bool operator!=(const Array3<T>& x) const
    {
        return m_SizeX != x.m_SizeX || m_SizeY != x.m_SizeY || m_SizeZ != x.m_SizeZ || m_Data != x.m_Data;
    }

    bool equalSize(const Array3<T>& x) const
    {
        return m_SizeX == x.m_SizeX && m_SizeY == x.m_SizeY && m_SizeZ == x.m_SizeZ;
    }

    void assign(const T& value)
    {
        m_Data.assign(m_Data.size(), value);
    }

    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        m_Data.assign(sizeX * sizeY * sizeZ, value);

        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
        m_SizeZ = static_cast<size_type>(sizeZ);
    }

    const T& back(void) const
    {
        assert(m_Data.size() > 0);
        return m_Data.back();
    }

    T& back(void)
    {
        assert(m_Data.size() > 0);
        return m_Data.back();
    }

    const_iterator cbegin(void) const
    {
        return m_Data.cbegin();
    }

    iterator begin(void)
    {
        return m_Data.begin();
    }

    size_type capacity(void) const
    {
        return m_Data.capacity();
    }

    void clear(void)
    {
        m_Data.resize(0);
        m_SizeX = m_SizeY = m_SizeZ = 0;
    }

    bool empty(void) const
    {
        return m_Data.empty();
    }

    const_iterator cend(void) const
    {
        return m_Data.cend();
    }

    iterator end(void)
    {
        return m_Data.end();
    }

    const T& front(void) const
    {
        assert(m_Data.size() > 0);
        return m_Data.front();
    }

    T& front(void)
    {
        assert(m_Data.size() > 0);
        return m_Data.front();
    }

    reverse_iterator rbegin(void)
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator crbegin(void) const
    {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend(void)
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crend(void) const
    {
        return const_reverse_iterator(cbegin());
    }

    std::vector<T>& vec_data()
    {
        return m_Data;
    }

    const std::vector<T>& vec_data() const
    {
        return m_Data;
    }

    ////////////////////////////////////////////////////////////////////////////////
    size_type sizeX() const
    {
        return m_SizeX;
    }

    size_type sizeY() const
    {
        return m_SizeY;
    }

    size_type sizeZ() const
    {
        return m_SizeZ;
    }

    size_type dataSize(void) const
    {
        return m_Data.size();
    }

    size_type maxSize(void) const
    {
        return m_Data.max_size();
    }

    template<class IndexType>
    void reserve(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        m_Data.reserve(sizeX * sizeY * sizeZ);
    }

    template<class IndexType>
    void reserve(const glm::tvec3<IndexType>& newSize)
    {
        m_Data.reserve(newSize[0] * newSize[1] * newSize[2]);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        m_Data.resize(sizeX * sizeY * sizeZ);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
        m_SizeZ = static_cast<size_type>(sizeZ);
    }

    template<class IndexType>
    void resize(const glm::tvec3<IndexType>& newSize)
    {
        resize<IndexType>(newSize[0], newSize[1], newSize[2]);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        m_Data.resize(sizeX * sizeY * sizeZ, value);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
        m_SizeZ = static_cast<size_type>(sizeZ);
    }

    template<class IndexType>
    void resize(const glm::tvec3<IndexType>& newSize, const T& value)
    {
        resize<IndexType>(newSize[0], newSize[1], newSize[2], value);
    }

    void setZero(void)
    {
        m_Data.assign(m_Data.size(), 0);
    }

    void swap(Array3<T>& other)
    {
        std::swap(m_SizeX, other.m_SizeX);
        std::swap(m_SizeY, other.m_SizeY);
        std::swap(m_SizeZ, other.m_SizeZ);
        m_Data.swap(other.m_Data);
    }
};  // end class Array3

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Array3c   = Array3<char>;
using Array3uc  = Array3<unsigned char>;
using Array3s   = Array3<short>;
using Array3us  = Array3<unsigned short>;
using Array3ll  = Array3<long long>;
using Array3ull = Array3<unsigned long long>;
using Array3i   = Array3<int>;
using Array3ui  = Array3<unsigned int>;
using Array3f   = Array3<float>;
using Array3d   = Array3<double>;

using Array3_VecChar   = Array3<std::vector<char> >;
using Array3_VecInt    = Array3<std::vector<int> >;
using Array3_VecUInt   = Array3<std::vector<unsigned int> >;
using Array3_VecFloat  = Array3<std::vector<float> >;
using Array3_VecDouble = Array3<std::vector<double> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana