//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>
#include <Banana/Data/DataIO.h>
#include <Banana/Utils/FileHelpers.h>

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <istream>
#include <sstream>
#include <cstdlib>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class SparseArray3
{
private:
    using Key = std::pair<UInt, std::pair<UInt, UInt> >;

    template<class IndexType>
    Key makeKey(IndexType i, IndexType j, IndexType k)
    {
        return std::pair(static_cast<UInt>(i), std::pair(static_cast<UInt>(j), static_cast<UInt>(k)));
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::unordered_map<Key, T> m_Data;

public:
    using iterator       = typename std::unordered_map<Key, T>::iterator;
    using const_iterator = typename std::unordered_map<Key, T>::const_iterator;
    using size_type      = typename std::unordered_map<Key, T>::size_type;

    ////////////////////////////////////////////////////////////////////////////////
    // constructors & destructor
    SparseArray3(void) = default;

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType>
    const T& operator ()(IndexType i, IndexType j, IndexType k) const
    {
        return m_Data[makeKey<IndexType>(i, j, k)];
    }

    template<class IndexType>
    T& operator ()(IndexType i, IndexType j, IndexType k)
    {
        return m_Data[makeKey<IndexType>(i, j, k)];
    }

    template<class IndexType>
    const T& operator ()(const Vec3<IndexType>& index) const
    {
        return m_Data[makeKey<IndexType>(index[0], index[1], index[2])];
    }

    template<class IndexType>
    T& operator ()(const Vec3<IndexType>& index)
    {
        return m_Data[makeKey<IndexType>(index[0], index[1], index[2])];
    }

    template<class IndexType>
    bool hasKey(IndexType i, IndexType j, IndexType k) const
    {
        return (m_Data.find(makeKey<IndexType>(i, j, k)) != m_Data.end());
    }

    template<class IndexType>
    bool hasKey(const Vec3<IndexType>& index) const
    {
        return (m_Data.find(makeKey<IndexType>(index[0], index[1], index[2])) != m_Data.end());
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool operator ==(const SparseArray3<T>& other) const
    {
        return m_Data == other.m_Data;
    }

    bool operator !=(const SparseArray3<T>& other) const
    {
        return m_Data != other.m_Data;
    }

    bool equalSize(const SparseArray3<T>& other) const
    {
        return size() == other.size();
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

    void clear()
    {
        m_Data.resize(0);
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

    std::unordered_map<Key, T>& data()
    {
        return m_Data;
    }

    const std::unordered_map<Key, T>& data() const
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
using SparseArray3ui  = SparseArray3<UInt>;
using SparseArray3f   = SparseArray3<float>;
using SparseArray3d   = SparseArray3<double>;
using SparseArray3r   = SparseArray3<Real>;

using SparseArray3_VecChar   = SparseArray3<std::vector<char> >;
using SparseArray3_VecInt    = SparseArray3<std::vector<int> >;
using SparseArray3_VecUInt   = SparseArray3<std::vector<UInt> >;
using SparseArray3_VecFloat  = SparseArray3<std::vector<float> >;
using SparseArray3_VecDouble = SparseArray3<std::vector<double> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana