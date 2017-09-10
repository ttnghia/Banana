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

#include <Banana/Setup.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace TensorHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class MatrixType>
inline RealType trace(const MatrixType& mat)
{
    RealType prod = RealType(0);
    for(int i = 0; i < mat.length(); ++i)
        prod += mat[i][i];
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class MatrixType>
inline MatrixType dev(const MatrixType& mat)
{
    return mat - MatrixType(TensorHelpers::trace<RealType>(mat) / RealType(mat.length()));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class MatrixType>
inline void sumToDiag(MatrixType& mat, RealType c)
{
    for(int i = 0; i < mat.length(); ++i)
        mat[i][i] += c;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class VectorType>
inline RealType product(const VectorType& vec)
{
    RealType prod = vec[0];

    for(int i = 1; i < vec.length(); ++i)
        prod *= vec[i];

    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class MatrixType>
inline void diagProduct(MatrixType& mat, const VectorType& vec)
{
    for(int i = 0; i < vec.length(); ++i)
    {
        for(int j = 0; j < vec.length(); ++j)
            mat[i][j] *= vec[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Matrix * Matrix^-1
template<class VectorType, class MatrixType>
inline void diagProductInv(MatrixType& mat, const VectorType& vec)
{
    for(int i = 0; i < vec.length(); ++i)
    {
        for(int j = 0; j < vec.length(); ++j)
            mat[i][j] /= vec[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class MatrixType>
inline RealType frobeniusInnerProduct(const MatrixType& m1, const MatrixType& m2)
{
    RealType prod = RealType(0);

    for(int i = 0; i < m1.length(); ++i)
    {
        for(int j = 0; j < m1.length(); ++j)
        {
            prod += m1[i][j] * m2[i][j];
        }
    }
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class MatrixType>
inline VectorType innerProduct(const VectorType& vec, const MatrixType& mat)
{
    VectorType prod(0);

    for(int i = 0; i < vec.length(); ++i)
        for(int j = 0; j < vec.length(); ++j)
            prod[i] += vec[j] * mat[j][i];

    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
inline MatrixType innerProduct(const MatrixType& m1, const MatrixType& m2)
{
    MatrixType prod(0);

    for(int i = 0; i < vec.length(); ++i)
        for(int j = 0; j < vec.length(); ++j)
            for(int k = 0; k < vec.length(); ++k)
                prod[i][j] += m1[i][k] * m2[k][j];

    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TODO: check row-col major
template<class RealType>
inline Mat2x2<RealType> cofactor(const Mat2x2<RealType>& mat)
{
    return Mat2x2<RealType>(mat[1][1], -mat[0][1],
                            -mat[1][0], mat[0][0]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TODO: test value row-col major
template<class RealType>
inline RealType elementCofactor(const Mat3x3<RealType>& mat, int x, int y)
{
    RealType         cofactor_v;
    RealType         minor;
    Mat2x2<RealType> minor_mat;

    minor_mat[0][0] = mat[(x + 1) % 3][(y + 1) % 3];
    minor_mat[1][1] = mat[(x + 2) % 3][(y + 2) % 3];
    minor_mat[0][1] = mat[(x + 1) % 3][(y + 2) % 3];
    minor_mat[1][0] = mat[(x + 2) % 3][(y + 1) % 3];
    minor           = glm::determinant(minor_mat);

    cofactor_v = ((x + y) & 1) ? -minor : minor;
    if(y == 1)
    {
        cofactor_v = -cofactor_v;
    }
    return cofactor_v;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline Mat3x3<RealType> cofactor(const Mat3x3<RealType>& mat)
{
    Mat2x2<RealType> result;

    for(int i = 0; i < mat.length(); ++i)
        for(int j = 0; j < mat.length(); ++j)
            result[i][j] = elementCofactor(mat, i, j);
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, class MatrixType>
inline RealType vonMisesPlaneStress(const MatrixType& mat)
{
    const RealType vm = mat[0][0] * mat[0][0] + mat[1][1] * mat[1][1] + mat[2][2] * mat[2][2] -
                        mat[0][0] * mat[1][1] - mat[1][1] * mat[2][2] - mat[2][2] * mat[0][0] +
                        (mat[0][1] * mat[1][0] + mat[1][2] * mat[2][1] + mat[2][0] * mat[0][2]) * RealType(3.0);

    return vm > 0 ? std::sqrt(vm) : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace TensorHelpers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana