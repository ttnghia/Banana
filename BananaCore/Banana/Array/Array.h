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
#include <vector>
#include <istream>
#include <sstream>
#include <cstdlib>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
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
    Array() = default;
    Array(const Array<N, T>& other) : m_Size(other.m_Size), m_Data(other.m_Data) {}

    template<class IndexType>
    Array(const VecX<N, IndexType>& size)
    {
        for(Int i = 0; i < N; ++i) {
            m_Size[i] = static_cast<size_type>(size[i]);
        }
        m_Data.resize(glm::compMul(m_Size));
    }

    ~Array(void) { m_Data.clear(); }

    ////////////////////////////////////////////////////////////////////////////////
    // assignment operator
    Array<N, T>& operator=(const Array<N, T>& other)
    {
        // check for self-assignment
        if(&other == this) {
            return *this;
        }
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
    auto        empty() const { return m_Data.empty(); }
    auto        capacity(void) const { return m_Data.capacity(); }
    auto        dataSize(void) const { return m_Data.size(); }
    const auto& size() const { return m_Size; }

    ////////////////////////////////////////////////////////////////////////////////
    // index processing
    template<class IndexType>
    bool isValidIndex(const VecX<N, IndexType>& index) const
    {
        for(Int i = 0; i < N; ++i) {
            if(index[i] < 0 || index[i] >= m_Size[i]) {
                return false;
            }
        }
        return true;
    }

    template<class IndexType>
    void checkIndex(const VecX<N, IndexType>& index) const
    {
        bool bIndexValid = isValidIndex<IndexType>(index);
        if(!bIndexValid) {
            std::stringstream ss;
            ss << "Invalid array index: ";

            for(Int i = 0; i < N - 1; ++i) {
                ss << index[i] << "/" << m_Size[i] << ", ";
            }
            ss << index[N - 1] << "/" << m_Size[N - 1];

            printf("%s\n", ss.str().c_str());
        }
        assert(bIndexValid);
    }

    bool equalSize(const Array<N, T>& other) const
    {
        for(Int i = 0; i < N; ++i) {
            if(m_Size[i] != other.m_Size[i]) {
                return false;
            }
        }
        return true;
    }

    template<class IndexType>
    bool equalSize(const VecX<N, IndexType>& otherSize) const
    {
        for(Int i = 0; i < N; ++i) {
            if(m_Size[i] != otherSize[i]) {
                return false;
            }
        }
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Array2D =>
    template<class IndexType>
    bool isValidIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return (i >= 0 &&
                j >= 0 &&
                static_cast<size_type>(i) < m_Size[0] &&
                static_cast<size_type>(j) < m_Size[1]);
    }

    template<class IndexType>
    size_type getCellLinearizedIndex(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
#ifndef NDEBUG
        checkIndex<IndexType>(Vec2<IndexType>(i, j));
#endif
        return static_cast<size_type>(j) * m_Size[0] + static_cast<size_type>(i);
    }

    template<class IndexType>
    size_type getCellLinearizedIndex(const Vec2<IndexType>& index) const
    {
        static_assert(N == 2, "Array dimension != 2");
        getCellLinearizedIndex<IndexType>(index[0], index[1]);
    }

    template<class IndexType>
    const T& operator()(IndexType i, IndexType j) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    T& operator()(IndexType i, IndexType j)
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j)];
    }

    template<class IndexType>
    const T& operator()(const Vec2<IndexType>& index) const
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1])];
    }

    template<class IndexType>
    T& operator()(const Vec2<IndexType>& index)
    {
        static_assert(N == 2, "Array dimension != 2");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1])];
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Array3D =>
    template<class IndexType>
    bool isValidIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return (i >= 0 &&
                j >= 0 &&
                k >= 0 &&
                static_cast<size_type>(i) < m_Size[0] &&
                static_cast<size_type>(j) < m_Size[1] &&
                static_cast<size_type>(k) < m_Size[2]);
    }

    template<class IndexType>
    size_type getCellLinearizedIndex(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
#ifndef NDEBUG
        checkIndex<IndexType>(Vec3<IndexType>(i, j, k));
#endif
        return (static_cast<size_type>(k) * m_Size[1] + static_cast<size_type>(j)) * m_Size[0] + static_cast<size_type>(i);
    }

    template<class IndexType>
    size_type getCellLinearizedIndex(const Vec3<IndexType>& index) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return getCellLinearizedIndex<IndexType>(index[0], index[1], index[2]);
    }

    template<class IndexType>
    const T& operator()(IndexType i, IndexType j, IndexType k) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    T& operator()(IndexType i, IndexType j, IndexType k)
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(i, j, k)];
    }

    template<class IndexType>
    const T& operator()(const Vec3<IndexType>& index) const
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1], index[2])];
    }

    template<class IndexType>
    T& operator()(const Vec3<IndexType>& index)
    {
        static_assert(N == 3, "Array dimension != 3");
        return m_Data[getCellLinearizedIndex<IndexType>(index[0], index[1], index[2])];
    }

    // => Array3D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // iterators
    auto&       data() { return m_Data; }
    const auto& data() const { return m_Data; }

    const auto& front(void) const { assert(m_Data.size() > 0); return m_Data.front(); }
    auto&       front(void) { assert(m_Data.size() > 0); return m_Data.front(); }

    const auto& back(void) const { assert(m_Data.size() > 0); return m_Data.back(); }
    auto&       back(void) { assert(m_Data.size() > 0); return m_Data.back(); }

    auto begin(void) { return m_Data.begin(); }
    auto end(void) { return m_Data.end(); }

    auto cbegin(void) const { return m_Data.cbegin(); }
    auto cend(void) const { return m_Data.cend(); }

    auto rbegin(void) { return reverse_iterator(end()); }
    auto rend(void) { return reverse_iterator(begin()); }

    auto crbegin(void) const { return const_reverse_iterator(cend()); }
    auto crend(void) const { return const_reverse_iterator(cbegin()); }

    ////////////////////////////////////////////////////////////////////////////////
    // data manipulation
    template<class IndexType>
    void assign(const VecX<N, IndexType>& size, const T& value)
    {
        m_Size = size;
        m_Data.assign(glm::compMul(m_Size), value);
    }

    template<class IndexType>
    void assign(const VecX<N, IndexType>& size, const T* copydata)
    {
        m_Size = size;
        m_Data.assign(glm::compMul(m_Size), copydata);
    }

    void assign(const T& value) { m_Data.assign(m_Data.size(), value); }
    void copyDataFrom(const Array<N, T>& other) { __BNN_REQUIRE(equalSize(other)); m_Data = other.m_Data; }
    void setZero() { m_Data.assign(m_Data.size(), 0); }
    void clear() { m_Data.resize(0); m_Size = VecX<N, size_type>(0); }
    void swap(Array<N, T>& other) { std::swap(m_Size, other.m_Size); m_Data.swap(other.m_Data); }

    template<class IndexType>
    void reserve(const VecX<N, IndexType>& size) { m_Data.reserve(glm::compMul(size)); }

    template<class IndexType>
    void resize(const VecX<N, IndexType>& newSize) { m_Size = newSize;  m_Data.resize(glm::compMul(m_Size)); }

    template<class IndexType>
    void resize(const VecX<N, IndexType>& newSize, const T& value) { m_Size = newSize;  m_Data.resize(glm::compMul(m_Size), value); }

    ////////////////////////////////////////////////////////////////////////////////
    // Array2D =>
    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, const T& value)
    {
        static_assert(N == 2, "Array dimension != 2");
        assign(Vec2<IndexType>(sizeX, sizeY), value);
    }

    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, const T* copydata)
    {
        static_assert(N == 2, "Array dimension != 2");
        assign(Vec2<IndexType>(sizeX, sizeY), copydata);
    }

    template<class IndexType>
    void reserve(IndexType sizeX, IndexType sizeY)
    {
        static_assert(N == 2, "Array dimension != 2");
        m_Data.reserve(sizeX * sizeY);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY)
    {
        static_assert(N == 2, "Array dimension != 2");
        resize(Vec2<IndexType>(sizeX, sizeY));
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, const T& value)
    {
        static_assert(N == 2, "Array dimension != 2");
        resize(Vec2<IndexType>(sizeX, sizeY), value);
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // Array3D =>
    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        static_assert(N == 3, "Array dimension != 3");
        assign(Vec3<IndexType>(sizeX, sizeY, sizeZ), value);
    }

    template<class IndexType>
    void assign(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T* copydata)
    {
        static_assert(N == 3, "Array dimension != 3");
        assign(Vec3<IndexType>(sizeX, sizeY, sizeZ), copydata);
    }

    template<class IndexType>
    void reserve(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        static_assert(N == 3, "Array dimension != 3");
        m_Data.reserve(sizeX * sizeY * sizeZ);
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ)
    {
        static_assert(N == 3, "Array dimension != 3");
        resize(Vec3<IndexType>(sizeX, sizeY, sizeZ));
    }

    template<class IndexType>
    void resize(IndexType sizeX, IndexType sizeY, IndexType sizeZ, const T& value)
    {
        static_assert(N == 3, "Array dimension != 3");
        resize(Vec3<IndexType>(sizeX, sizeY, sizeZ), value);
    }

    // => Array2D
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // file IO
    bool saveToFile(const String& fileName)
    {
        DataBuffer buffer;
        for(Int i = 0; i < N; ++i) {
            buffer.append<UInt>(static_cast<UInt>(m_Size[i]));
        }
        buffer.append(m_Data, false);
        return FileHelpers::writeFile(buffer.data(), buffer.size(), fileName);
    }

    bool loadFromFile(const String& fileName)
    {
        DataBuffer buffer;
        if(!FileHelpers::readFile(buffer.buffer(), fileName)) {
            return false;
        }

        for(Int i = 0; i < N; ++i) {
            UInt tmp;
            buffer.getData<UInt>(tmp, sizeof(UInt) * i);
            m_Size[i] = static_cast<size_type>(tmp);
        }
        __BNN_REQUIRE(buffer.buffer().size() == N * sizeof(UInt) + sizeof(T) * glm::compMul(m_Size));
        buffer.getData(m_Data, sizeof(UInt) * N, static_cast<UInt>(glm::compMul(m_Size)));
        return true;
    }

private:
    VecX<N, size_type> m_Size = VecX<N, size_type>(0);
    Vector<T>          m_Data;
};  // end class Array

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T> using Array2 = Array<2, T>;
template<class T> using Array3 = Array<3, T>;

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
using Array2r   = Array2<Real>;

using Array2_VecChar   = Array2<Vector<char>>;
using Array2_VecInt    = Array2<Vector<int>>;
using Array2_VecUInt   = Array2<Vector<unsigned int>>;
using Array2_VecFloat  = Array2<Vector<float>>;
using Array2_VecDouble = Array2<Vector<double>>;
using Array2_VecReal   = Array2<Vector<Real>>;

////////////////////////////////////////////////////////////////////////////////
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
using Array3r   = Array3<Real>;

using Array3_VecChar   = Array3<Vector<char>>;
using Array3_VecInt    = Array3<Vector<int>>;
using Array3_VecUInt   = Array3<Vector<unsigned int>>;
using Array3_VecFloat  = Array3<Vector<float>>;
using Array3_VecDouble = Array3<Vector<double>>;
using Array3_VecReal   = Array3<Vector<Real>>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana