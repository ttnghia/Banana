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

#include <json.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::JSONHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool readValue(const JParams& j, T& v, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return false;
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return false;
    }

    v = jval.get<T>();
    return true;
}

template<class T>
auto readValue(const JParams& j, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return std::make_pair(T(), false);
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return std::make_pair(T(), false);
    }

    T v = jval.get<T>();
    return std::make_pair(v, true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readBool(const JParams& j, bool& v, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return false;
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return false;
    }

    if(jval.is_number_integer()) {
        int val = jval.get<int>();
        v = (val != 0);
    } else {
        v = jval.get<bool>();
    }

    return true;
}

inline auto readBool(const JParams& j, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return std::make_pair(false, false);
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return std::make_pair(false, false);
    }

    bool v = false;
    if(jval.is_number_integer()) {
        int val = jval.get<int>();
        v = (val != 0);
    } else {
        v = jval.get<bool>();
    }

    return std::make_pair(v, false);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool readVector(const JParams& j, VecX<N, T>& vec, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return false;
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return false;
    }
    Vector<T> values = jval.get<Vector<T>>();

    Int minSize = static_cast<Int>(values.size());
    Int maxSize = N;
    if(minSize > maxSize) {
        std::swap(minSize, maxSize);
    }

    for(Int i = 0; i < minSize; ++i) {
        vec[i] = values[i];
    }
    if(minSize < N) {
        for(Int i = minSize; i < N; ++i) {
            vec[i] = T(0);
        }
    }
    return true;
}

template<Int N, class T>
auto readVector(const JParams& j, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return std::make_pair(VecX<N, T>(), false);
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return std::make_pair(VecX<N, T>(), false);
    }

    Vector<T> values  = jval.get<Vector<T>>();
    Int       minSize = static_cast<Int>(values.size());
    Int       maxSize = N;
    if(minSize > maxSize) {
        std::swap(minSize, maxSize);
    }

    VecX<N, T> vec(0);
    for(Int i = 0; i < minSize; ++i) {
        vec[i] = values[i];
    }
    return std::make_pair(vec, true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool readVector(const JParams& j, Vector<T>& vec, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return false;
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return false;
    }

    vec = jval.get<Vector<T>>();
    return true;
}

template<class T>
auto readVector(const JParams& j, const String& valueName)
{
    if(j.find(valueName) == j.end()) {
        return std::make_pair(Vector<T>(), false);
    }
    const JParams jval = j[valueName];

    if(jval.is_null()) {
        return std::make_pair(Vector<T>(), false);
    }

    Vector<T> vec = jval.get<Vector<T>>();
    return std::make_pair(vec, true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::JSONHelpers
