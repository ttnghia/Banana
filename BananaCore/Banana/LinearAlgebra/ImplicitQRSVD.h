//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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
#include <limits>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::QRSVD
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/**
   \brief 2x2 SVD (singular value decomposition) A=USV'
   \param[in] A Input matrix.
   \param[out] U Robustly a rotation matrix.
   \param[out] Sigma Vector of singular values sorted with decreasing magnitude. The second one can be negative.
   \param[out] V Robustly a rotation matrix.
 */
template<class T>
void svd(const Mat2x2<T>& A, const Mat2x2<T>& U, const Vec2<T>& Sigma, const Mat2x2<T>& V, T tol = T(64.0)* std::numeric_limits<T>::epsilon());

/**
   \brief 3X3 SVD (singular value decomposition) A=USV'
   \param[in] A Input matrix.
   \param[out] U is a rotation matrix.
   \param[out] sigma Diagonal matrix, sorted with decreasing magnitude. The third one can be negative.
   \param[out] V is a rotation matrix.
 */
template<class T> Int svd(const Mat3x3<T>& A, Mat3x3<T>& U, Vec3<T>& sigma, Mat3x3<T>& V, T tol = T(128.0)* std::numeric_limits<T>::epsilon());

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/**
   \brief 2x2 polar decomposition.
   \param[in] A matrix.
   \param[out] R Robustly a rotation matrix.
   \param[out] S_Sym Symmetric. Whole matrix is stored

   Whole matrix S is stored since its faster to calculate due to simd vectorization
   Polar guarantees negative sign is on the small magnitude singular value.
   S is guaranteed to be the closest one to identity.
   R is guaranteed to be the closest rotation to A.
 */
template<class T> void polar(const Mat2x2<T>& A, const Mat2x2<T>& R, const Mat2x2<T>& S_Sym);
template<class T> void polar(const Mat3x3<T>& A, Mat3x3<T>& R, Mat3x3<T>& S_Sym);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SVDDecomposition
