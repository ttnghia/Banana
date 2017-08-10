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

#include <Banana/TypeNames.h>

#include <algorithm>
#include <cassert>
#include <vector>
#include <istream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class Array2
{
private:
    typename std::vector<T> ::size_type m_SizeX, m_SizeY;
    std::vector<T>                      m_Data;

public:
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
    using size_type              = typename std::vector<T>::size_type;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors & destructor
    Array2(void) : m_SizeX(0), m_SizeY(0) {}

    template<class IndexType>
    Array2(IndexType sizeX, IndexType sizeY) : m_SizeX(sizeX), m_SizeY(sizeY), m_Data(sizeX * sizeY) {}

    template<class IndexType>
    Array2(IndexType sizeX, IndexType sizeY, const std::vector<T>& data) : m_SizeX(sizeX), m_SizeY(sizeY), m_Data(data) {}

    template<class IndexType>
    Array2(IndexType sizeX, IndexType sizeY, const T& value) : m_SizeX(sizeX), m_SizeY(sizeY), m_Data(sizeX * sizeY, value) {}

    template<class IndexType>
    Array2(IndexType sizeX, IndexType sizeY, T* datdata) : m_SizeX(sizeX), m_SizeY(sizeY), m_Data(sizeX * sizeY, datdata) {}

    Array2(const Array2<T>& other) : m_SizeX(other.m_SizeX), m_SizeY(other.m_SizeY), m_Data(other.m_Data) {}

    ~Array2(void)
    {
        m_Data.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    bool isValidIndex(IndexType i, IndexType j)  const
    {
        return (i >= 0 &&
                j >= 0 &&
                static_cast<size_type>(i) < m_SizeX &&
                static_cast<size_type>(j) < m_SizeY);
    }

    template<class IndexType>
    bool isValidIndex(const glm::tvec2<IndexType>& index) const
    {
        return isValidIndex<IndexType>(index[0], index[1]);
    }

    template<class IndexType>
    void checkIndex(IndexType i, IndexType j) const
    {
        bool bIndexValid = isValidIndex<IndexType>(i, j);

        if(!bIndexValid)
        {
            std::cout << "Invalid Array2 index: i = " << i << " / sizeX = " << m_SizeX << ", j = " << j << " / sizeY = " << m_SizeY << std::endl;
        }

        assert(bIndexValid);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    size_type getLinearizedIndex(IndexType i, IndexType j)
    {
        checkIndex<IndexType>(i, j);
        return static_cast<size_type>(j) * m_SizeX + static_cast<size_type>(i);
    }

    template<class IndexType>
    size_type getLinearizedIndex(const glm::tvec2<IndexType>& index)
    {
        getLinearizedIndex<IndexType>(index[0], index[1]);
    }

    template<class IndexType>
    const T& operator()(IndexType i, IndexType j) const
    {
        return m_Data[getLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    T& operator()(IndexType i, IndexType j)
    {
        return m_Data[getLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    const T& operator()(const glm::tvec2<IndexType>& index) const
    {
        return m_Data[getLinearizedIndex<IndexType>(index[0], index[1])];
    }

    template<class IndexType>
    T& operator()(const glm::tvec2<IndexType>& index)
    {
        return m_Data[getLinearizedIndex<IndexType>(index[0], index[1])];
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool operator ==(const Array2<T>& other) const
    {
        return m_SizeX == other.m_SizeX && m_SizeY == other.m_SizeY && m_Data == other.m_Data;
    }

    bool operator !=(const Array2<T>& other) const
    {
        return m_SizeX != other.m_SizeX || m_SizeY != other.m_SizeY || m_Data != other.m_Data;
    }

    bool equalSize(const Array2<T>& other) const
    {
        return m_SizeX == other.m_SizeX && m_SizeY == other.m_SizeY;
    }

    void assign(const T& value)
    {
        m_Data.assign(m_Data.size(), value);
    }

    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, const T& value)
    {
        m_Data.assign(sizeX * sizeY, value);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
    }

    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, const T* copydata)
    {
        m_Data.assign(sizeX * sizeY, copydata);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
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
        m_SizeX = m_SizeY = 0;
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

    size_type size(void) const
    {
        return m_Data.size();
    }

    size_type maxSize(void) const
    {
        return m_Data.max_size();
    }

    template<class IndexType>
    void reserve(IndexType sizeX, IndexType sizeY)
    {
        m_Data.reserve(sizeX * sizeY);
    }

    template<class IndexType>
    void reserve(const glm::tvec2<IndexType>& newSize)
    {
        m_Data.reserve(newSize[0] * newSize[1]);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY)
    {
        m_Data.resize(sizeX * sizeY);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
    }

    template<class IndexType>
    void resize(const glm::tvec2<IndexType>& newSize)
    {
        resize<IndexType>(newSize[0], newSize[1]);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, const T& value)
    {
        m_Data.resize(sizeX * sizeY, value);
        m_SizeX = static_cast<size_type>(sizeX);
        m_SizeY = static_cast<size_type>(sizeY);
    }

    template<class IndexType>
    void resize(const glm::tvec2<IndexType>& newSize, const T& value)
    {
        resize<IndexType>(newSize[0], newSize[1], value);
    }

    void setZero(void)
    {
        m_Data.assign(m_Data.size(), 0);
    }

    void swap(Array2<T>& other)
    {
        std::swap(m_SizeX, other.m_SizeX);
        std::swap(m_SizeY, other.m_SizeY);
        m_Data.swap(other.m_Data);
    }
};  // end class Array2

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Array2c   = Array2<char>;
using Array2uc  = Array2<unsigned char>;
using Array2s   = Array2<short>;
using Array2us  = Array2<unsigned short>;
using Array2ll  = Array2<long long>;
using Array2ull = Array2<unsigned long long>;
using Array2i   = Array2<int>;
using Array2ui  = Array2<unsigned int>;
using Array2f   = Array2<float>;
using Array2d   = Array2<double>;

using Array2_VecChar   = Array2<std::vector<char> >;
using Array2_VecInt    = Array2<std::vector<int> >;
using Array2_VecUInt   = Array2<std::vector<unsigned int> >;
using Array2_VecFloat  = Array2<std::vector<float> >;
using Array2_VecDouble = Array2<std::vector<double> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana