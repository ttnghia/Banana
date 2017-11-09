//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/16/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifndef __Banana_TypeNames__
#define __Banana_TypeNames__

#include <vector>
#include <cstdint>

// this definition should depend of each project
//#define __Using_Eigen_Lib__
//#define __Using_GLM_Lib__
//#define __Using_Cem_Lib__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using Int8   = int8_t;
using Int16  = int16_t;
using Int    = int32_t;
using  Int64 = int64_t;

using UInt8  = uint8_t;
using UInt16 = uint16_t;
using UInt   = uint32_t;
using UInt64 = uint64_t;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Using_Eigen_Lib__

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/StdVector>
#include <Eigen/Core>

#define DEFAULT_VECTOR_STORAGE_ORDER Eigen::ColMajor
#define DEFAULT_MATRIX_STORAGE_ORDER Eigen::RowMajor

// Vectors
template<class RealType>
using Vec2 = Eigen::Matrix<RealType, 2, 1, DEFAULT_VECTOR_STORAGE_ORDER, 2, 1>;

template<class RealType>
using Vec3 = Eigen::Matrix<RealType, 3, 1, DEFAULT_VECTOR_STORAGE_ORDER, 3, 1>;

template<class RealType>
using Vec4 = Eigen::Matrix<RealType, 4, 1, DEFAULT_VECTOR_STORAGE_ORDER, 4, 1>;

// matrices
template<class RealType>
using Mat2x2 = Eigen::Matrix<RealType, 2, 2, DEFAULT_MATRIX_STORAGE_ORDER, 2, 2>;
template<class RealType>
const Mat2x2<RealType> Identity2x2 = Mat2x2<RealType>::Identity();

template<class RealType>
using Mat3x3 = Eigen::Matrix<RealType, 3, 3, DEFAULT_MATRIX_STORAGE_ORDER, 3, 3>;
template<class RealType>
const Mat3x3<RealType> Identity3x3 = Mat3x3<RealType>::Identity();

template<class RealType>
using Mat3x4 = Eigen::Matrix<RealType, 3, 4, DEFAULT_MATRIX_STORAGE_ORDER, 3, 4>;

template<class RealType>
using Mat4x3 = Eigen::Matrix<RealType, 4, 3, DEFAULT_MATRIX_STORAGE_ORDER, 4, 3>;

template<class RealType>
using Mat4x4 = Eigen::Matrix<RealType, 4, 4, DEFAULT_MATRIX_STORAGE_ORDER, 4, 4>;
template<class RealType>
const Mat4x4<RealType> Identity4x4 = Mat4x4<RealType>::Identity();


// vectors of vectors
template<class RealType>
using  Vec_Vec2 = std::vector<Vec2<RealType> >;

template<class RealType>
using Vec_Vec3 = std::vector<Vec3<RealType> >;

template<class RealType>
using Vec_Vec4 = std::vector<Vec4<RealType> >;

// vectors of matrices
template<class RealType>
using Vec_Mat2x2 = std::vector<Mat2x2<RealType> >;

template<class RealType>
using  Vec_Mat3x3 = std::vector<Mat3x3<RealType> >;

template<class RealType>
using Vec_Mat3x4 = std::vector<Mat3x4<RealType> >;

template<class RealType>
using Vec_Mat4x3 = std::vector<Mat4x3<RealType> >;

template<class RealType>
using Vec_Mat4x4 = std::vector<Mat4x4<RealType> >;

// vectors of vector of vectors
template<class RealType>
using Vec_VecVec3 = std::vector<std::vector<Vec3<RealType> > >;

// vectors of vector of matrices
template<class RealType>
using Vec_VecMat3x3 = std::vector<std::vector<Mat3x3<RealType> > >;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define SPECIALIZE_STL_VECTOR(VectorType)                       \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<float> )  \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<double> ) \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<Int8> )   \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<Int16> )  \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<Int> )    \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<Int64> )  \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<UInt8> )  \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<UInt16> ) \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<UInt> )   \
    EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(VectorType<UInt64> )


SPECIALIZE_STL_VECTOR(Vec2)
SPECIALIZE_STL_VECTOR(Vec3)
SPECIALIZE_STL_VECTOR(Vec4)
SPECIALIZE_STL_VECTOR(Mat2x2)
SPECIALIZE_STL_VECTOR(Mat3x3)
SPECIALIZE_STL_VECTOR(Mat3x4)
SPECIALIZE_STL_VECTOR(Mat4x3)
SPECIALIZE_STL_VECTOR(Mat4x4)
SPECIALIZE_STL_VECTOR(Triplet)

#else // not using eigen lib
#ifdef __Using_GLM_Lib__

#define GLM_FORCE_INLINE
#include <glm/glm.hpp>

// Vectors
template<class RealType>
using Vec2 = glm::tvec2<RealType>;

template<class RealType>
using Vec3 = glm::tvec3<RealType>;

template<class RealType>
using Vec4 = glm::tvec4<RealType>;

// matrices
template<class RealType>
using Mat2x2 = glm::mat<2, 2, RealType>;
template<class RealType>
const Mat2x2<RealType> Identity2x2 = glm::mat<2, 2, RealType>(1.0);

template<class RealType>
using Mat3x3 = glm::mat<3, 3, RealType>;
template<class RealType>
const Mat3x3<RealType> Identity3x3 = glm::mat<3, 3, RealType>(1.0);

template<class RealType>
using Mat3x4 = glm::mat<3, 4, RealType>;

template<class RealType>
using Mat4x3 = glm::mat<4, 3, RealType>;

template<class RealType>
using Mat4x4 = glm::mat<4, 4, RealType>;
template<class RealType>
const Mat4x4<RealType> Identity4x4 = glm::mat<4, 4, RealType>(1.0);

// vectors of vectors
template<class RealType>
using  Vec_Vec2 = std::vector<Vec2<RealType> >;

template<class RealType>
using Vec_Vec3 = std::vector<Vec3<RealType> >;

template<class RealType>
using Vec_Vec4 = std::vector<Vec4<RealType> >;

// vectors of matrices
template<class RealType>
using Vec_Mat2x2 = std::vector<Mat2x2<RealType> >;

template<class RealType>
using  Vec_Mat3x3 = std::vector<Mat3x3<RealType> >;

template<class RealType>
using Vec_Mat3x4 = std::vector<Mat3x4<RealType> >;

template<class RealType>
using Vec_Mat4x3 = std::vector<Mat4x3<RealType> >;

template<class RealType>
using Vec_Mat4x4 = std::vector<Mat4x4<RealType> >;

// vectors of vector of vectors
template<class RealType>
using Vec_VecVec3 = std::vector<std::vector<Vec3<RealType> > >;

// vectors of vector of matrices
template<class RealType>
using Vec_VecMat3x3 = std::vector<std::vector<Mat3x3<RealType> > >;

#else // not using eigen nor glm lib

#ifdef __Using_Cem_Lib__
#include "./cyCore.h"
#include "./cyPoint.h"
#include "./cyMatrix.h"

template<class RealType>
using Vec3 = cy::Point3<RealType>;

template<class RealType>
using Vec4 = cy::Point4<RealType>;


template<class RealType>
using Mat3x3 = cy::Matrix3<RealType>;

template<class RealType>
using Mat4x4 = cy::Matrix4<RealType>;

#else
#include <yocto/yocto_math.h>

// Vectors
template<class RealType>
using Vec2 = ym::vec<RealType, 2>;

template<class RealType>
using Vec3 = ym::vec<RealType, 3>;

template<class RealType>
using Vec4 = ym::vec<RealType, 4>;

// matrices
template<class RealType>
using Mat2x2 = ym::mat<RealType, 2, 2>;
template<class RealType>
const Mat2x2<RealType> Identity2x2 = ym::identity_mat<RealType, 2>();

template<class RealType>
using Mat3x3 = ym::mat<RealType, 3, 3>;
template<class RealType>
const Mat3x3<RealType> Identity3x3 = ym::identity_mat<RealType, 3>();

template<class RealType>
using Mat3x4 = ym::mat<RealType, 3, 4>;

template<class RealType>
using Mat4x3 = ym::mat<RealType, 4, 3>;

template<class RealType>
using Mat4x4 = ym::mat<RealType, 4, 4>;
template<class RealType>
const Mat4x4<RealType> Identity4x4 = ym::identity_mat<RealType, 4>();

// vectors of vectors
template<class RealType>
using  Vec_Vec2 = std::vector<Vec2<RealType> >;

template<class RealType>
using Vec_Vec3 = std::vector<Vec3<RealType> >;

template<class RealType>
using Vec_Vec4 = std::vector<Vec4<RealType> >;

// vectors of matrices
template<class RealType>
using Vec_Mat2x2 = std::vector<Mat2x2<RealType> >;

template<class RealType>
using  Vec_Mat3x3 = std::vector<Mat3x3<RealType> >;

template<class RealType>
using Vec_Mat3x4 = std::vector<Mat3x4<RealType> >;

template<class RealType>
using Vec_Mat4x3 = std::vector<Mat4x3<RealType> >;

template<class RealType>
using Vec_Mat4x4 = std::vector<Mat4x4<RealType> >;

// vectors of vector of vectors
template<class RealType>
using Vec_VecVec3 = std::vector<std::vector<Vec3<RealType> > >;

// vectors of vector of matrices
template<class RealType>
using Vec_VecMat3x3 = std::vector<std::vector<Mat3x3<RealType> > >;

#endif // __Using_GLM_Lib__
#endif // __Using_Eigen_Lib__
#endif
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// vectors of basic types
using Vec_Int8  = std::vector<Int8>;
using Vec_Int16 = std::vector<Int16>;
using Vec_Int   = std::vector<Int>;
using Vec_Int32 = std::vector<Int>;
using Vec_Int64 = std::vector<Int64>;

using Vec_UInt8  = std::vector<UInt8>;
using Vec_UInt16 = std::vector<UInt16>;
using Vec_UInt   = std::vector<UInt>;
using Vec_UInt32 = std::vector<UInt>;
using Vec_UInt64 = std::vector<UInt64>;

using Vec_Char   = std::vector<char>;
using Vec_Float  = std::vector<float>;
using Vec_Double = std::vector<double>;

// vectors of vectors
using Vec_VecInt8  = std::vector<std::vector<Int8> >;
using Vec_VecInt16 = std::vector<std::vector<Int16> >;
using Vec_VecInt   = std::vector<std::vector<Int> >;
using Vec_VecInt32 = std::vector<std::vector<Int> >;
using Vec_VecInt64 = std::vector<std::vector<Int64> >;

using Vec_VecUInt8  = std::vector<std::vector<UInt8> >;
using Vec_VecUInt16 = std::vector<std::vector<UInt16> >;
using Vec_VecUInt   = std::vector<std::vector<UInt> >;
using Vec_VecUInt32 = std::vector<std::vector<UInt> >;
using Vec_VecUInt64 = std::vector<std::vector<UInt64> >;

using Vec_VecChar   = std::vector<std::vector<char> >;
using Vec_VecFloat  = std::vector<std::vector<float> >;
using Vec_VecDouble = std::vector<std::vector<double> >;

// others
typedef std::pair<Int, Int>         IPair;
typedef std::pair<UInt, UInt>       UIntPair;
typedef std::pair<float, float>     FloatPair;
typedef std::pair<double, double>   DoublePair;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif // __Banana_TypeNames__