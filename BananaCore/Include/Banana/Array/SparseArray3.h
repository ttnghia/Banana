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
#include <unordered_map>
#include <istream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class SparseArray3
{
private:
    using Key = std::pair<unsigned int, std::pair<unsigned int, unsigned int> >;

    template<class IndexType>
    Key makeKey(IndexType i, IndexType j, IndexType k)
    {
        return std::pair(static_cast<unsigned int>(i),
                         std::pair(static_cast<unsigned int>(j), static_cast<unsigned int>(k)));
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::unordered_map<Key, T> m_Data;

public:
    using iterator       = typename std::unordered_map<Key, T>::iterator;
    using const_iterator = typename std::unordered_map<Key, T>::const_iterator;
    using size_type      = typename std::unordered_map<Key, T>::size_type;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors & destructor
    SparseArray3(void) {}

    ~SparseArray3(void)
    {
        m_Data.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    const T& operator()(IndexType i, IndexType j, IndexType k) const
    {
        return m_Data[makeKey<IndexType>(i, j, k)];
    }

    template<class IndexType>
    T& operator()(IndexType i, IndexType j, IndexType k)
    {
        return m_Data[makeKey<IndexType>(i, j, k)];
    }

    template<class IndexType>
    const T& operator()(const glm::tvec3<IndexType>& index) const
    {
        return m_Data[makeKey<IndexType>(index[0], index[1], index[2])];
    }

    template<class IndexType>
    T& operator()(const glm::tvec3<IndexType>& index)
    {
        return m_Data[makeKey<IndexType>(index[0], index[1], index[2])];
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool operator==(const SparseArray3<T>& other) const
    {
        return m_Data == other.m_Data;
    }

    bool operator!=(const SparseArray3<T>& other) const
    {
        return m_Data != other.m_Data;
    }

    bool equalSize(const SparseArray3<T>& other) const
    {
        return m_SizeX == other.m_SizeX && m_SizeY == other.m_SizeY && m_SizeZ == other.m_SizeZ;
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
        m_Data.clear();
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

    std::unordered_map<Key, T>& map_data()
    {
        return m_Data;
    }

    const std::unordered_map<Key, T>& map_data() const
    {
        return m_Data;
    }

    ////////////////////////////////////////////////////////////////////////////////
    size_type size(void) const
    {
        return m_Data.size();
    }

    size_type maxSize(void) const
    {
        return m_Data.max_size();
    }

    void reserve(size_type count)
    {
        m_Data.reserve(count);
    }

    void swap(SparseArray3<T>& other)
    {
        m_Data.swap(other.m_Data);
    }
};  // end class SparseArray3

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using SparseArray3c   = SparseArray3<char>;
using SparseArray3uc  = SparseArray3<unsigned char>;
using SparseArray3s   = SparseArray3<short>;
using SparseArray3us  = SparseArray3<unsigned short>;
using SparseArray3ll  = SparseArray3<long long>;
using SparseArray3ull = SparseArray3<unsigned long long>;
using SparseArray3i   = SparseArray3<int>;
using SparseArray3ui  = SparseArray3<unsigned int>;
using SparseArray3f   = SparseArray3<float>;
using SparseArray3d   = SparseArray3<double>;

using SparseArray3_VecChar   = SparseArray3<std::vector<char> >;
using SparseArray3_VecInt    = SparseArray3<std::vector<int> >;
using SparseArray3_VecUInt   = SparseArray3<std::vector<unsigned int> >;
using SparseArray3_VecFloat  = SparseArray3<std::vector<float> >;
using SparseArray3_VecDouble = SparseArray3<std::vector<double> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana