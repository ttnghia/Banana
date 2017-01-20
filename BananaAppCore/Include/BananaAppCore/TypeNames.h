//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/17/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#ifdef _MSC_VER
// Use Visual C++'s memory checking functionality
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _MSC_VER

/*
At the beginning of the main function, add this code:
#ifdef _MSC_VER
        //_crtBreakAlloc = 1828;
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _MSC_VER
*/

#include <vector>
#include <cstdint>

// this definition should depend of each project
#ifndef __Using_Yocto_Lib__
#define __Using_GLM_Lib__
#endif


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Int8 = int8_t;
using Int16 = int16_t;
using Int32 = int32_t;
using  Int64 = int64_t;

using UInt8 = uint8_t;
using UInt16 = uint16_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Using_GLM_Lib__

#define GLM_FORCE_CXX11
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>

// Vectors
template<class ScalarType>
using Vec2 = glm::tvec2<ScalarType>;

template<class ScalarType>
using Vec3 = glm::tvec3<ScalarType>;

template<class ScalarType>
using Vec4 = glm::tvec4<ScalarType>;

// matrices
template<class ScalarType>
using Mat2x2 = glm::mat<2, 2, ScalarType>;
template<class ScalarType>
const Mat2x2<ScalarType> Identity2x2 = glm::mat<2, 2, ScalarType>(1.0);

template<class ScalarType>
using Mat3x3 = glm::mat<3, 3, ScalarType>;
template<class ScalarType>
const Mat3x3<ScalarType> Identity3x3 = glm::mat<3, 3, ScalarType>(1.0);

template<class ScalarType>
using Mat3x4 = glm::mat<3, 4, ScalarType>;

template<class ScalarType>
using Mat4x3 = glm::mat<4, 3, ScalarType>;

template<class ScalarType>
using Mat4x4 = glm::mat<4, 4, ScalarType>;
template<class ScalarType>
const Mat4x4<ScalarType> Identity4x4 = glm::mat<4, 4, ScalarType>(1.0);

// vectors of vectors
template<class ScalarType>
using  Vec_Vec2 = std::vector<Vec2<ScalarType> >;

template<class ScalarType>
using Vec_Vec3 = std::vector<Vec3<ScalarType> >;

template<class ScalarType>
using Vec_Vec4 = std::vector<Vec4<ScalarType> >;

// vectors of matrices
template<class ScalarType>
using Vec_Mat2x2 = std::vector<Mat2x2<ScalarType> >;

template<class ScalarType>
using  Vec_Mat3x3 = std::vector<Mat3x3<ScalarType> >;

template<class ScalarType>
using Vec_Mat3x4 = std::vector<Mat3x4<ScalarType> >;

template<class ScalarType>
using Vec_Mat4x3 = std::vector<Mat4x3<ScalarType> >;

template<class ScalarType>
using Vec_Mat4x4 = std::vector<Mat4x4<ScalarType> >;

// vectors of vector of vectors
template<class ScalarType>
using Vec_VecVec3 = std::vector<std::vector<Vec3<ScalarType> > >;

// vectors of vector of matrices
template<class ScalarType>
using Vec_VecMat3x3 = std::vector<std::vector<Mat3x3<ScalarType> > >;

#else // not using glm lib

#include <yocto/yocto_math.h>

// Vectors
template<class ScalarType>
using Vec2 = ym::vec<ScalarType, 2>;

template<class ScalarType>
using Vec3 = ym::vec<ScalarType, 3>;

template<class ScalarType>
using Vec4 = ym::vec<ScalarType, 4>;

// matrices
template<class ScalarType>
using Mat2x2 = ym::mat<ScalarType, 2, 2>;
template<class ScalarType>
const Mat2x2<ScalarType> Identity2x2 = ym::identity_mat<ScalarType, 2>();

template<class ScalarType>
using Mat3x3 = ym::mat<ScalarType, 3, 3>;
template<class ScalarType>
const Mat3x3<ScalarType> Identity3x3 = ym::identity_mat<ScalarType, 3>();

template<class ScalarType>
using Mat3x4 = ym::mat<ScalarType, 3, 4>;

template<class ScalarType>
using Mat4x3 = ym::mat<ScalarType, 4, 3>;

template<class ScalarType>
using Mat4x4 = ym::mat<ScalarType, 4, 4>;
template<class ScalarType>
const Mat4x4<ScalarType> Identity4x4 = ym::identity_mat<ScalarType, 4>();

// vectors of vectors
template<class ScalarType>
using  Vec_Vec2 = std::vector<Vec2<ScalarType> >;

template<class ScalarType>
using Vec_Vec3 = std::vector<Vec3<ScalarType> >;

template<class ScalarType>
using Vec_Vec4 = std::vector<Vec4<ScalarType> >;

// vectors of matrices
template<class ScalarType>
using Vec_Mat2x2 = std::vector<Mat2x2<ScalarType> >;

template<class ScalarType>
using  Vec_Mat3x3 = std::vector<Mat3x3<ScalarType> >;

template<class ScalarType>
using Vec_Mat3x4 = std::vector<Mat3x4<ScalarType> >;

template<class ScalarType>
using Vec_Mat4x3 = std::vector<Mat4x3<ScalarType> >;

template<class ScalarType>
using Vec_Mat4x4 = std::vector<Mat4x4<ScalarType> >;

// vectors of vector of vectors
template<class ScalarType>
using Vec_VecVec3 = std::vector<std::vector<Vec3<ScalarType> > >;

// vectors of vector of matrices
template<class ScalarType>
using Vec_VecMat3x3 = std::vector<std::vector<Mat3x3<ScalarType> > >;

#endif // __Using_GLM_Lib__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// vectors of basic types
using Vec_Int8 = std::vector<Int8>;
using Vec_Int16 = std::vector<Int16>;
using Vec_Int = std::vector<Int32>;
using Vec_Int32 = std::vector<Int32>;
using Vec_Int64 = std::vector<Int64>;

using Vec_UInt8 = std::vector<UInt8>;
using Vec_UInt16 = std::vector<UInt16>;
using Vec_UInt = std::vector<UInt32>;
using Vec_UInt32 = std::vector<UInt32>;
using Vec_UInt64 = std::vector<UInt64>;

using Vec_Char = std::vector<char>;
using Vec_Float = std::vector<float>;
using Vec_Double = std::vector<double>;

// vectors of vectors
using Vec_VecInt8 = std::vector<std::vector<Int8> >;
using Vec_VecInt16 = std::vector<std::vector<Int16> >;
using Vec_VecInt = std::vector<std::vector<Int32> >;
using Vec_VecInt32 = std::vector<std::vector<Int32> >;
using Vec_VecInt64 = std::vector<std::vector<Int64> >;

using Vec_VecUInt8 = std::vector<std::vector<UInt8> >;
using Vec_VecUInt16 = std::vector<std::vector<UInt16> >;
using Vec_VecUInt = std::vector<std::vector<UInt32> >;
using Vec_VecUInt32 = std::vector<std::vector<UInt32> >;
using Vec_VecUInt64 = std::vector<std::vector<UInt64> >;

using Vec_VecChar = std::vector<std::vector<char> >;
using Vec_VecFloat = std::vector<std::vector<float> >;
using Vec_VecDouble = std::vector<std::vector<double> >;

// others
typedef std::pair<Int32, Int32>  IPair;
typedef std::pair<UInt32, UInt32>  UIntPair;
typedef std::pair<float, float>  FloatPair;
typedef std::pair<double, double>  DoublePair;

