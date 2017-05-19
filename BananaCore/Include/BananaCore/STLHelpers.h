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
#include <map>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace STLHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Sorted
{
template<class T>
inline bool contain(const std::vector<T>& vec, T item)
{
    return std::binary_search(vec.begin(), vec.end(), item);
}

template<class T>
inline bool contain(const std::vector<T>& vec, T item, size_t& itemIndex)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);
    itemIndex = static_cast<size_t>(std::distance(vec.begin(), it));

    return (it != vec.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline void insertSorted(std::vector<T>& vec, const T& item, std::vector<T> ::iterator& it)
{
    it = vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

template<typename T>
inline auto insertSorted(std::vector<T>& vec, const T& item)
{
    auto it = vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
    return std::distance(vec.begin(), it);
}

template<typename T1, typename T2>
inline void insertPairSorted(std::vector<T1>& vec1, const T1& item1, std::vector<T2>& vec2, const T2& item2)
{
    auto k = STLHelpers::Sorted::insertSorted(vec1, item1);
    vec2.insert(vec2.begin() + k, item2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline bool eraseIfExist(std::vector<T>& vec, const T& item)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);

    if(it != vec.end())     // has item
    {
        vec.erase(it);
        return true;
    }

    return false;
}

template<typename T>
inline bool eraseIfExist(std::vector<T>& vec, const T& item, size_t& itemIndex)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), item);
    itemIndex = static_cast<size_t>(std::distance(vec.begin(), it));

    if(it != vec.end())     // has item
    {
        vec.erase(it);
        return true;
    }

    return false;
}


template<class T>
inline bool eraseUnorderedIfExist(std::vector<T>& vec, const T& item)
{
    size_t k = 0;

    if(STLHelpers::Sorted::contain(vec, item, k))
    {
        STLHelpers::eraseUnordered(vec, k);
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool replaceOnce(std::vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k = 0;

    if(STLHelpers::Sorted::contain(vec, oldElement, k))
    {
        vec[k] = newElement;
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Sorted
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T maxAbs(const std::vector<T>& vec)
{
    T maxVal = 0;

    for(T x : vec)
    {
        const T tmp = std::fabs(x);
        if(tmp > maxVal)
        {
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
inline bool contain(const std::vector<T>& vec, T item)
{
    for(T x : vec)
    {
        if(x == item)
        {
            return true;
        }
    }
    return false;
}

template<class T>
inline bool contain(const std::vector<T>& vec, T item, size_t& itemIndex)
{
    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(vec[i] == item)
        {
            itemIndex = i;
            return true;
        }
    }

    itemIndex = vec.size();
    return false;
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline bool eraseIfExist(std::vector<T>& vec, const T& item)
{
    for(auto it = vec.begin(); it != vec.end(); ++it)
    {
        if(*it == item) // has item
        {
            vec.erase(it);
            return true;
        }
    }

    return false;
}

template<typename T>
inline bool eraseIfExist(std::vector<T>& vec, const T& item, size_t& itemIndex)
{
    for(auto it = vec.begin(); it != vec.end(); ++it)
    {
        if(*it == item) // has item
        {
            itemIndex = static_cast<size_t>(std::distance(vec.begin(), it));
            vec.erase(it);
            return true;
        }
    }

    itemIndex = vec.size();
    return false;
}


template<class T>
inline void eraseUnordered(std::vector<T>& vec, size_t index)
{
    vec[index] = vec.back();
    vec.pop_back();
}


template<class T>
inline bool eraseUnorderedIfExist(std::vector<T>& vec, const T& item)
{
    size_t k = 0;

    if(STLHelpers::contain(vec, item, k))
    {
        STLHelpers::eraseUnordered(vec, k);
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline bool replaceOnce(std::vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k = 0;

    if(STLHelpers::contain(vec, oldElement, k))
    {
        vec[k] = newElement;
        return true;
    }

    return false;
}

template<class T>
inline size_t replaceAll(std::vector<T>& vec, const T& oldElement, const T& newElement)
{
    size_t k           = 0;
    size_t numReplaced = 0;

    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(vec[i] == oldElement)
        {
            vec[i] = newElement;
            ++numReplaced;
        }
    }

    return numReplaced;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void writeMatlab(std::ostream& output, const std::vector<T>& vec, const char* variableName, bool bColumnVector = true, int precision = 18)
{
    std::streamsize oldPrecision = output.precision();
    output.precision(precision);

    output << variableName << "=[";
    for(T item: vec)
    {
        output << item << " ";
    }

    output << "]";

    if(bColumnVector)
    {
        output << "'";
    }

    output << ";" << std::endl;
    output.precision(oldPrecision);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace STLHelpers