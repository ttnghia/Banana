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
#include <map>
#include <set>
#include <cstdint>
#include <string>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/component_wise.hpp>

#include <json.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// define the floating point precision for all projects

#if defined(DOUBLE_PRECISION) || defined(HIGH_PRECISION)
#  define BANANA_DOUBLE_PRECISION
using Real = double;
#else
#  define BANANA_SINGLE_PRECISION
using Real = float;
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Basic types
using Int8  = int8_t;
using Int16 = int16_t;
using Int   = int32_t;
using Int32 = int32_t;
using Int64 = int64_t;

////////////////////////////////////////////////////////////////////////////////
using UChar  = unsigned char;
using UInt8  = uint8_t;
using UInt16 = uint16_t;
using UInt   = uint32_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;

////////////////////////////////////////////////////////////////////////////////
using String  = std::string;
using JParams = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
using PairInt8  = std::pair<Int8, Int8>;
using PairInt16 = std::pair<Int16, Int16>;
using PairInt32 = std::pair<Int, Int>;
using PairInt   = std::pair<Int, Int>;
using PairInt64 = std::pair<Int64, Int64>;

////////////////////////////////////////////////////////////////////////////////
using PairUInt8  = std::pair<UInt8, UInt8>;
using PairUInt16 = std::pair<UInt16, UInt16>;
using PairUInt32 = std::pair<UInt, UInt>;
using PairUInt   = std::pair<UInt, UInt>;
using PairUInt64 = std::pair<UInt64, UInt64>;

////////////////////////////////////////////////////////////////////////////////
using Pairf    = std::pair<float, float>;
using Paird    = std::pair<double, double>;
using PairReal = std::pair<double, double>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// generic types
template<class Type> using Vector = std::vector<Type>;
template<class Type> using Set    = std::set<Type>;
template<class Type> using Quat   = glm::tquat<Type>;
template<class Type> using Vec2   = glm::vec<2, Type>;
template<class Type> using Vec3   = glm::vec<3, Type>;
template<class Type> using Vec4   = glm::vec<4, Type>;
template<class Type> using Mat2x2 = glm::mat<2, 2, Type>;
template<class Type> using Mat3x3 = glm::mat<3, 3, Type>;
template<class Type> using Mat4x4 = glm::mat<4, 4, Type>;

template<int N, class Type> using VecX   = glm::vec<N, Type>;
template<int N, class Type> using MatXxX = glm::mat<N, N, Type>;

template<int N, class Type> using Vec_VecX   = Vector<VecX<N, Type> >;
template<int N, class Type> using Vec_MatXxX = Vector<MatXxX<N, Type> >;

template<int N, class Type> using Vec_VecVecX   = Vector<Vector<VecX<N, Type> > >;
template<int N, class Type> using Vec_VecMatXxX = Vector<Vector<MatXxX<N, Type> > >;

////////////////////////////////////////////////////////////////////////////////
template<class Type> using Vec_Vec    = Vector<Vector<Type> >;
template<class Type> using Vec_Vec2   = Vector<Vec2<Type> >;
template<class Type> using Vec_Vec3   = Vector<Vec3<Type> >;
template<class Type> using Vec_Vec4   = Vector<Vec4<Type> >;
template<class Type> using Vec_Mat2x2 = Vector<Mat2x2<Type> >;
template<class Type> using Vec_Mat3x3 = Vector<Mat3x3<Type> >;
template<class Type> using Vec_Mat4x4 = Vector<Mat4x4<Type> >;

template<class T, class S>
using Map = std::map<T, S>;

template<class Type> using SharedPtr = std::shared_ptr<Type>;
template<class Type> using UniquePtr = std::unique_ptr<Type>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Quatf = Quat<float>;
using Quatd = Quat<double>;
using Quatr = Quat<Real>;

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

using Vec2r = Vec2<Real>;
using Vec3r = Vec3<Real>;
using Vec4r = Vec4<Real>;

using Mat2x2f = Mat2x2<float>;
using Mat3x3f = Mat3x3<float>;
using Mat4x4f = Mat4x4<float>;

using Mat2x2d = Mat2x2<double>;
using Mat3x3d = Mat3x3<double>;
using Mat4x4d = Mat4x4<double>;

using Mat2x2r = Mat2x2<Real>;
using Mat3x3r = Mat3x3<Real>;
using Mat4x4r = Mat4x4<Real>;

////////////////////////////////////////////////////////////////////////////////
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
using Vec_UChar  = Vector<unsigned char>;
using Vec_Float  = Vector<float>;
using Vec_Double = Vector<double>;
using Vec_Real   = Vector<Real>;
using Vec_String = Vector<String>;

using Vec_Vec2i = Vector<Vec2i>;
using Vec_Vec3i = Vector<Vec3i>;
using Vec_Vec4i = Vector<Vec4i>;

using Vec_Vec2ui = Vector<Vec2ui>;
using Vec_Vec3ui = Vector<Vec3ui>;
using Vec_Vec4ui = Vector<Vec4ui>;

using Vec_Vec2f = Vector<Vec2f>;
using Vec_Vec3f = Vector<Vec3f>;
using Vec_Vec4f = Vector<Vec4f>;

using Vec_Vec2d = Vector<Vec2d>;
using Vec_Vec3d = Vector<Vec3d>;
using Vec_Vec4d = Vector<Vec4d>;

using Vec_Vec2r = Vector<Vec2r>;
using Vec_Vec3r = Vector<Vec3r>;
using Vec_Vec4r = Vector<Vec4r>;

using Vec_Mat2x2f = Vector<Mat2x2f>;
using Vec_Mat3x3f = Vector<Mat3x3f>;
using Vec_Mat4x4f = Vector<Mat4x4f>;

using Vec_Mat2x2d = Vector<Mat2x2d>;
using Vec_Mat3x3d = Vector<Mat3x3d>;
using Vec_Mat4x4d = Vector<Mat4x4d>;

using Vec_Mat2x2r = Vector<Mat2x2r>;
using Vec_Mat3x3r = Vector<Mat3x3r>;
using Vec_Mat4x4r = Vector<Mat4x4r>;

////////////////////////////////////////////////////////////////////////////////
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
using Vec_VecUChar  = Vector<Vector<unsigned char> >;
using Vec_VecFloat  = Vector<Vector<float> >;
using Vec_VecDouble = Vector<Vector<double> >;
using Vec_VecReal   = Vector<Vector<Real> >;

using Vec_VecVec2i = Vector<Vector<Vec2i> >;
using Vec_VecVec3i = Vector<Vector<Vec3i> >;
using Vec_VecVec4i = Vector<Vector<Vec4i> >;

using Vec_VecVec2ui = Vector<Vector<Vec2ui> >;
using Vec_VecVec3ui = Vector<Vector<Vec3ui> >;
using Vec_VecVec4ui = Vector<Vector<Vec4ui> >;

using Vec_VecVec2f = Vector<Vector<Vec2f> >;
using Vec_VecVec3f = Vector<Vector<Vec3f> >;
using Vec_VecVec4f = Vector<Vector<Vec4f> >;

using Vec_VecVec2d = Vector<Vector<Vec2d> >;
using Vec_VecVec3d = Vector<Vector<Vec3d> >;
using Vec_VecVec4d = Vector<Vector<Vec4d> >;

using Vec_VecVec2r = Vector<Vector<Vec2r> >;
using Vec_VecVec3r = Vector<Vector<Vec3r> >;
using Vec_VecVec4r = Vector<Vector<Vec4r> >;

using Vec_VecMat2x2f = Vector<Vector<Mat2x2f> >;
using Vec_VecMat3x3f = Vector<Vector<Mat3x3f> >;
using Vec_VecMat4x4f = Vector<Vector<Mat4x4f> >;

using Vec_VecMat2x2d = Vector<Vector<Mat2x2d> >;
using Vec_VecMat3x3d = Vector<Vector<Mat3x3d> >;
using Vec_VecMat4x4d = Vector<Vector<Mat4x4d> >;

using Vec_VecMat2x2r = Vector<Vector<Mat2x2r> >;
using Vec_VecMat3x3r = Vector<Vector<Mat3x3r> >;
using Vec_VecMat4x4r = Vector<Vector<Mat4x4r> >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// include macros after typename definitions
#include <Banana/Macros.h>
