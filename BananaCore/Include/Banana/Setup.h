//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Copyright (c) 2017 by
//    __   _   _     _____
//  /\ \ \__ _| |__ (_) __ _ /__  \_ __ _  _ ___ _ __  __ _
//  / \/ / _` | '_ \| |/ _` |  / /\/ '__| | | |/ _ \| '_ \ / _` |
// / /\ / (_| | | | | | (_| | / / | | | |_| | (_) | | | | (_| |
// \_\ \/ \__, |_| |_|_|\__,_| \/  |_|  \__,_|\___/|_| |_|\__, |
//     |___/                       |___/
//
// <nghiatruong.vn@gmail.com>
// All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <limits>
#include <vector>
#include <cstdint>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// define the floating point precision for all projects

#if defined(SINGLE_PRECISION) || defined(LOW_PRECISION)
#  define BANANA_SINGLE_PRECISION
using Real = float;
#else
#  define BANANA_DOUBLE_PRECISION
using Real = double;
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Int8  = int8_t;
using Int16 = int16_t;
using Int   = int32_t;
using Int32 = int32_t;
using Int64 = int64_t;

using UInt8  = uint8_t;
using UInt16 = uint16_t;
using UInt   = uint32_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;

using String = std::string;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// generic types
template<class Type>
using Vector = std::vector<Type>;

template<class Type>
using Vec_Vec = std::vector<std::vector<Type> >;

template<class Type>
using Vec2 = glm::tvec2<Type>;

template<class Type>
using Vec3 = glm::tvec3<Type>;

template<class Type>
using Vec4 = glm::tvec4<Type>;

template<class Type>
using Mat2x2 = glm::mat<2, 2, Type>;

template<class Type>
using Mat3x3 = glm::mat<3, 3, Type>;

template<class Type>
using Mat3x4 = glm::mat<3, 4, Type>;

template<class Type>
using Mat4x3 = glm::mat<4, 3, Type>;

template<class Type>
using Mat4x4 = glm::mat<4, 4, Type>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// floating point type

// Vectors
using Vec2r = Vec2<Real>;
using Vec3r = Vec3<Real>;
using Vec4r = Vec4<Real>;

// matrices
using Mat2x2r = Mat2x2<Real>;
using Mat3x3r = Mat3x3<Real>;
using Mat3x4r = Mat3x4<Real>;
using Mat4x3r = Mat4x3<Real>;
using Mat4x4r = Mat4x4<Real>;

// vectors
using Vec_Vec2r = Vector<Vec2r>;
using Vec_Vec3r = Vector<Vec3r>;
using Vec_Vec4r = Vector<Vec4r>;

// vectors of matrices
using Vec_Mat2x2r = Vector<Mat2x2r>;
using Vec_Mat3x3r = Vector<Mat3x3r>;
using Vec_Mat3x4r = Vector<Mat3x4r>;
using Vec_Mat4x3r = Vector<Mat4x3r>;
using Vec_Mat4x4r = Vector<Mat4x4r>;

// vectors of vector of ...
using Vec_VecVec3r   = std::vector<std::vector<Vec3r> >;
using Vec_VecMat3x3r = std::vector<std::vector<Mat3x3r> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Vec2i = Vec2<Int>;
using Vec3i = Vec3<Int>;
using Vec4i = Vec4<Int>;

using Vec2ui = Vec2<UInt>;
using Vec3ui = Vec3<UInt>;
using Vec4ui = Vec4<UInt>;

using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;

using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;

using Vec_Int8  = Vector<Int8>;
using Vec_Int16 = Vector<Int16>;
using Vec_Int   = Vector<Int>;
using Vec_Int32 = Vector<Int>;
using Vec_Int64 = Vector<Int64>;

using Vec_UInt8  = Vector<UInt8>;
using Vec_UInt16 = Vector<UInt16>;
using Vec_UInt   = Vector<UInt>;
using Vec_UInt32 = Vector<UInt>;
using Vec_UInt64 = Vector<UInt64>;

using Vec_Char   = Vector<char>;
using Vec_Float  = Vector<float>;
using Vec_Double = Vector<double>;

// vectors of vectors
using Vec_VecInt8  = Vector<Vector<Int8> >;
using Vec_VecInt16 = Vector<Vector<Int16> >;
using Vec_VecInt   = Vector<Vector<Int> >;
using Vec_VecInt32 = Vector<Vector<Int> >;
using Vec_VecInt64 = Vector<Vector<Int64> >;

using Vec_VecUInt8  = Vector<Vector<UInt8> >;
using Vec_VecUInt16 = Vector<Vector<UInt16> >;
using Vec_VecUInt   = Vector<Vector<UInt> >;
using Vec_VecUInt32 = Vector<Vector<UInt> >;
using Vec_VecUInt64 = Vector<Vector<UInt64> >;

using Vec_VecChar   = Vector<Vector<char> >;
using Vec_VecFloat  = Vector<Vector<float> >;
using Vec_VecDouble = Vector<Vector<double> >;

// others
using IPair8  = std::pair<Int8, Int8>;
using IPair16 = std::pair<Int16, Int16>;
using IPair32 = std::pair<Int, Int>;
using IPair   = std::pair<Int, Int>;
using IPair64 = std::pair<Int64, Int64>;

using UIPair8  = std::pair<UInt8, UInt8>;
using UIPair16 = std::pair<UInt16, UInt16>;
using UIPair32 = std::pair<UInt, UInt>;
using UIPair   = std::pair<UInt, UInt>;
using UIPair64 = std::pair<UInt64, UInt64>;

using FloatPair  = std::pair<float, float>;
using DoublePair = std::pair<double, double>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>