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
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/LinearAlgebra/ImplicitQRSVD.h>

#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::LinaHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool hasValidElements(const VecX<N, RealType>& vec)
{
    for(Int i = 0; i < N; ++i) {
        if(!NumberHelpers::isValidNumber(vec[i])) {
            return false;
        }
    }
    return true;
}

template<Int N, class RealType>
bool hasValidElements(const MatXxX<N, RealType>& mat)
{
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            if(!NumberHelpers::isValidNumber(mat[i][j])) {
                return false;
            }
        }
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType maxAbs(const MatXxX<N, RealType>& mat)
{
    RealType result = RealType(0);
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            result = MathHelpers::max(result, fabs(mat[i][j]));
        }
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType norm2(const MatXxX<N, RealType>& mat)
{
    RealType prod = RealType(0);
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            prod += mat[i][j] * mat[i][j];
        }
    }
    return sqrt(prod);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T, class S>
void fill(MatXxX<N, T>& mat, S x)
{
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            mat[i][j] = T(x);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType trace(const MatXxX<N, RealType>& mat)
{
    RealType prod = RealType(0);
    for(Int i = 0; i < N; ++i) {
        prod += mat[i][i];
    }
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
MatXxX<N, RealType> dev(const MatXxX<N, RealType>& mat)
{
    return mat - MatXxX<N, RealType>(LinaHelpers::trace<RealType>(mat) / RealType(N));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void sumToDiag(MatXxX<N, RealType>& mat, RealType c)
{
    for(Int i = 0; i < N; ++i) {
        mat[i][i] += c;
    }
}

template<Int N, class RealType>
MatXxX<N, RealType> getDiagSum(const MatXxX<N, RealType>& mat, RealType c)
{
    auto result = mat;
    for(Int i = 0; i < N; ++i) {
        result[i][i] += c;
    }
    return result;
}

template<Int N, class RealType>
MatXxX<N, RealType> getDiagSum(const MatXxX<N, RealType>& mat, const VecX<N, RealType>& c)
{
    auto result = mat;
    for(Int i = 0; i < N; ++i) {
        result[i][i] += c[i];
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> extractDiag(const MatXxX<N, RealType>& mat)
{
    VecX<N, RealType> diag;
    for(Int i = 0; i < N; ++i) {
        diag[i] = mat[i][i];
    }
    return diag;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
MatXxX<N, RealType> diagMatrix(const VecX<N, RealType>& diag)
{
    MatXxX<N, RealType> mat(0);
    for(Int i = 0; i < N; ++i) {
        mat[i][i] = diag[i];
    }
    return mat;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void diagProduct(MatXxX<N, RealType>& mat, const VecX<N, RealType>& vec)
{
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            mat[i][j] *= vec[i];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Matrix * Matrix^-1
template<Int N, class RealType>
void diagProductInv(MatXxX<N, RealType>& mat, const VecX<N, RealType>& vec)
{
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            mat[i][j] /= vec[i];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType frobeniusInnerProduct(const MatXxX<N, RealType>& m1, const MatXxX<N, RealType>& m2)
{
    RealType prod = RealType(0);
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            prod += m1[i][j] * m2[i][j];
        }
    }
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> innerProduct(const VecX<N, RealType>& vec, const MatXxX<N, RealType>& mat)
{
    VecX<N, RealType> prod(0);
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            prod[i] += vec[j] * mat[j][i];
        }
    }
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
MatXxX<N, RealType> innerProduct(const MatXxX<N, RealType>& m1, const MatXxX<N, RealType>& m2)
{
    MatXxX<N, RealType> prod(0);
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            for(Int k = 0; k < N; ++k) {
                prod[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return prod;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TODO: check row-col major
template<class RealType>
Mat2x2<RealType> cofactor(const Mat2x2<RealType>& mat)
{
    return Mat2x2<RealType>(mat[1][1], -mat[0][1],
                            -mat[1][0], mat[0][0]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TODO: test value row-col major
template<class RealType>
RealType elementCofactor(const Mat3x3<RealType>& mat, Int x, Int y)
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
    if(y == 1) {
        cofactor_v = -cofactor_v;
    }
    return cofactor_v;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Mat3x3<RealType> cofactor(const Mat3x3<RealType>& mat)
{
    Mat2x2<RealType> result;
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            result[i][j] = elementCofactor(mat, i, j);
        }
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType vonMisesPlaneStress(const Mat3x3<RealType>& mat)
{
    const RealType vm = mat[0][0] * mat[0][0] + mat[1][1] * mat[1][1] + mat[2][2] * mat[2][2] -
                        mat[0][0] * mat[1][1] - mat[1][1] * mat[2][2] - mat[2][2] * mat[0][0] +
                        (mat[0][1] * mat[1][0] + mat[1][2] * mat[2][1] + mat[2][0] * mat[0][2]) * RealType(3.0);

    return vm > 0 ? std::sqrt(vm) : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
MatXxX<N, RealType> randMatrix(RealType minVal = RealType(0), RealType maxVal = RealType(1.0))
{
    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis(minVal, maxVal);

    MatXxX<N, RealType> result;
    for(Int i = 0; i < N; ++i) {
        for(Int j = 0; j < N; ++j) {
            result[i][j] = dis(gen);
        }
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class SizeType, class RealType>
Vector<MatXxX<N, RealType>> randVecMatrices(SizeType size, RealType minVal = RealType(0), RealType maxVal = RealType(1.0))
{
    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis(minVal, maxVal);

    Vector<MatXxX<N, RealType>> results;
    results.resize(size);
    for(SizeType idx = 0; idx < size; ++idx) {
        for(Int i = 0; i < N; ++i) {
            for(Int j = 0; j < N; ++j) {
                results[idx][i][j] = dis(gen);
            }
        }
    }
    return results;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto orientedSVD(const MatXxX<N, RealType>& M)
{
    MatXxX<N, RealType> U, Vt;
    VecX<N, RealType>   S;

    QRSVD::svd(M, U, S, Vt);
    Vt = glm::transpose(Vt);

    MatXxX<N, RealType> J = MatXxX<N, RealType>(1.0);
    J[N - 1][N - 1] = RealType(-1.0);

    // Check for inversion
    if(glm::determinant(U) < RealType(0)) {
        U         = U * J;
        S[N - 1] *= RealType(-1.0);
    }
    if(glm::determinant(Vt) < RealType(0)) {
        Vt        = J * Vt;
        S[N - 1] *= RealType(-1.0);
    }

    return std::make_tuple(U, S, Vt);
}       // end oriented svd

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto QRDecomposition(const MatXxX<N, RealType>& M)
{
    MatXxX<N, RealType> Q, R;

    for(Int i = 0; i < N; ++i) {
        //Copy in Q the input's i-th column.
        Q[i] = M[i];

        //j = [0, i]
        // Make that column orthogonal to all the previous ones by substracting to it the non-orthogonal projection of all the previous columns.
        // Also: Fill the zero elements of R
        for(Int j = 0; j < i; ++j) {
            Q[i]   -= glm::dot(Q[i], Q[j]) * Q[j];
            R[j][i] = 0;
        }

        //Now, Q i-th column is orthogonal to all the previous columns. Normalize it.
        Q[i] = glm::normalize(Q[i]);

        //j = [i, C]
        //Finally, compute the corresponding coefficients of R by computing the projection of the resulting column on the other columns of the input.
        for(Int j = i; j < N; ++j) {
            R[j][i] = glm::dot(M[j], Q[i]);
        }
    }

    return std::make_tuple(Q, R);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto symmetryDecomposition(const MatXxX<N, RealType>& M)
{
    MatXxX<N, RealType> symComp, skewSymComp;
    auto                Mt = glm::transpose(M);

    symComp     = RealType(0.5) * (M + Mt);
    skewSymComp = RealType(0.5) * (M - Mt);
    return std::make_tuple(symComp, skewSymComp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::LinaHelpers
