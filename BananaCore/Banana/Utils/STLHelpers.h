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

#include <vector>
#include <map>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace STLHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
struct PairHash
{
    void hash_combine(std::size_t& seed, T v) const
    {
        static std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    std::size_t operator()(const std::pair<T, T>& p) const
    {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
struct VecHash
{
    void hash_combine(std::size_t& seed, T v) const
    {
        static std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    std::size_t operator()(const VecX<N, T>& x) const
    {
        size_t seed = 0;
        for(Int i = 0; i < N; ++i) {
            hash_combine(seed, x[i]);
        }
        return seed;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Sorted
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool contain(const Vector<T>& vec, T item)
{
    return std::binary_search(vec.begin(), vec.end(), item);
}

template<class T, class IndexType>
inline bool contain(const Vector<T>& vec, T item, IndexType& itemIndex)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);
    itemIndex = static_cast<IndexType>(std::distance(vec.begin(), it));

    return (it != vec.end() && item == *it);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline void insertSorted(Vector<T>& vec, const T& item, typename Vector<T>::iterator& it)
{
    it = vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

template<typename T>
inline auto insertSorted(Vector<T>& vec, const T& item)
{
    auto it = vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
    return std::distance(vec.begin(), it);
}

template<typename T1, typename T2>
inline void insertPairSorted(Vector<T1>& vec1, const T1& item1, Vector<T2>& vec2, const T2& item2)
{
    auto k = STLHelpers::Sorted::insertSorted(vec1, item1);
    vec2.insert(vec2.begin() + k, item2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline bool eraseIfExist(Vector<T>& vec, const T& item)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);

    if(it != vec.end() && item == *it) {   // has item
        vec.erase(it);
        return true;
    }

    return false;
}

template<typename T>
inline bool eraseIfExist(Vector<T>& vec, const T& item, size_t& itemIndex)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);
    itemIndex = static_cast<size_t>(std::distance(vec.begin(), it));

    if(it != vec.end() && item == *it) {   // has item
        vec.erase(it);
        return true;
    }

    return false;
}

template<class T>
inline bool eraseUnorderedIfExist(Vector<T>& vec, const T& item)
{
    size_t k = 0;

    if(STLHelpers::Sorted::contain(vec, item, k)) {
        STLHelpers::eraseUnordered(vec, k);
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool replaceOnce(Vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k = 0;

    if(STLHelpers::Sorted::contain(vec, oldElement, k)) {
        vec[k] = newElement;
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}       // end namespace Sorted
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<class T>
T maxAbs(const Vector<T>& vec)
{
    T maxVal = 0;

    for(T x : vec) {
        const T tmp = std::fabs(x);
        if(tmp > maxVal) {
            maxVal = tmp;
        }
    }

    return maxVal;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MapType, class KeyType>
inline bool hasKey(const MapType& map, KeyType key)
{
    return (map.find(key) != map.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool contain(const Vector<T>& vec, T item)
{
    for(T x : vec) {
        if(x == item) {
            return true;
        }
    }
    return false;
}

template<class T>
inline bool contain(const Vector<T>& vec, T item, size_t& itemIndex)
{
    for(size_t i = 0; i < vec.size(); ++i) {
        if(vec[i] == item) {
            itemIndex = i;
            return true;
        }
    }

    itemIndex = vec.size();
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline bool eraseIfExist(Vector<T>& vec, const T& item)
{
    for(auto it = vec.begin(); it != vec.end(); ++it) {
        if(*it == item) { // has item
            vec.erase(it);
            return true;
        }
    }

    return false;
}

template<typename T>
inline bool eraseIfExist(Vector<T>& vec, const T& item, size_t& itemIndex)
{
    for(auto it = vec.begin(); it != vec.end(); ++it) {
        if(*it == item) { // has item
            itemIndex = static_cast<size_t>(std::distance(vec.begin(), it));
            vec.erase(it);
            return true;
        }
    }

    itemIndex = vec.size();
    return false;
}

template<class T>
inline void eraseUnordered(Vector<T>& vec, size_t index)
{
    vec[index] = vec.back();
    vec.pop_back();
}

template<class T>
inline bool eraseUnorderedIfExist(Vector<T>& vec, const T& item)
{
    size_t k = 0;

    if(STLHelpers::contain(vec, item, k)) {
        STLHelpers::eraseUnordered(vec, k);
        return true;
    }

    return false;
}

template<class T, class S>
inline size_t eraseByMarker(Vector<T>& vec, const Vector<S>& marker, S eraseValue = S(1))
{
    assert(marker.size() == vec.size());
    size_t last = 0;
    size_t i    = 0;
    for(; i < vec.size(); ++i, ++last) {
        while(marker[i] == eraseValue) { ++i; }
        if(i >= vec.size()) { break; }
        vec[last] = vec[i];
    }
    vec.resize(last);
    return marker.size() - last;
}

template<class T, class S>
inline size_t eraseByMarker(Vector<T>& vec, UInt dim, const Vector<S>& marker, S eraseValue = S(1))
{
    assert(marker.size() == vec.size());
    size_t last = 0;
    size_t i    = 0;
    for(; i < vec.size(); ++i, ++last) {
        while(marker[i] == eraseValue) { ++i; }
        if(i >= vec.size()) { break; }
        for(UInt j = 0; j < dim; ++j) {
            vec[last * dim + j] = vec[i * dim + j];
        }
    }
    vec.resize(last);
    return marker.size() - last;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool replaceOnce(Vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k = 0;

    if(STLHelpers::contain(vec, oldElement, k)) {
        vec[k] = newElement;
        return true;
    }

    return false;
}

template<class T>
inline size_t replaceAll(Vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k           = 0;
    size_t numReplaced = 0;

    for(size_t i = 0; i < vec.size(); ++i) {
        if(vec[i] == oldElement) {
            vec[i] = newElement;
            ++numReplaced;
        }
    }

    return numReplaced;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void writeMatlab(std::ostream& output, const Vector<T>& vec, const char* variableName, bool bColumnVector = true, int precision = 18)
{
    std::streamsize oldPrecision = output.precision();
    output.precision(precision);

    output << variableName << "=[";
    for(T item: vec) {
        output << item << " ";
    }

    output << "]";

    if(bColumnVector) {
        output << "'";
    }

    output << ";" << std::endl;
    output.precision(oldPrecision);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ltrim(String& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch)
                                    {
                                        return ch != ' ';
                                    }));
}

// trim from end (in place)
inline void rtrim(String& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch)
                         {
                             return ch != ' ';
                         }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(String& s)
{
    ltrim(s);
    rtrim(s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace STLHelpers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana