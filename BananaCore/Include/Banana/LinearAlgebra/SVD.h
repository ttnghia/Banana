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

#include <cmath>
#include <cstdint>

#define _gamma  5.828427124  // FOUR_GAMMA_SQUARED = sqrt(8)+3;
#define _cstar  0.923879532  // cos(pi/8)
#define _sstar  0.3826834323 // sin(p/8)
#define EPSILON 1e-6

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SVDDecomposition
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/* This is a novel and fast routine for the reciprocal square root of an
   IEEE float (single precision).
   http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
   http://playstation2-linux.com/download/p2lsd/fastrsqrt.pdf
   http://www.beyond3d.com/content/articles/8/
 */

template<class T>
inline T rsqrt(T)
{
    fprintf(stderr, "Wrong call to unimplemented function.\n    Line: %d, file: %s\n", __LINE__, __FILE__);
    fflush(stderr);
    exit(-1);
}

/* This is rsqrt with an additional step of the Newton iteration, for
   increased accuracy. The constant 0x5f37599e makes the relative error
   range from 0 to -0.00000463.
   You can't balance the error by adjusting the constant. */
template<>
inline float rsqrt<float>(float x)
{
    const float xhalf = 0.5f * x;
    int32_t     i     = *(int32_t*)&x; // View x as an int.
    i = 0x5f37599e - (i >> 1);         // Initial guess.
    x = *(float*)&i;                   // View i as float.
    x = x * (1.5f - xhalf * x * x);    // Newton step.
    x = x * (1.5f - xhalf * x * x);    // Newton step again.
    x = x * (1.5f - xhalf * x * x);    // Newton step again.
    return x;
}

template<>
inline double rsqrt<double>(double x)
{
    const double xhalf = 0.5 * x;
    int64_t      i     = *(int64_t*)&x; // View x as an int.
    i = 0x5fe6eb50c7b537a9 - (i >> 1);  // Initial guess.
    x = *(double*)&i;                   // View i as float.
    x = x * (1.5 - xhalf * x * x);      // Newton step.
    x = x * (1.5 - xhalf * x * x);      // Newton step again.
    x = x * (1.5 - xhalf * x * x);      // Newton step again.
    return x;
}

template<class T>
inline T accurateSqrt(T x)
{
    return x * rsqrt<T>(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void condSwap(bool c, T& X, T& Y)
{
    // used in step 2
    T Z = X;
    X = c ? Y : X;
    Y = c ? Z : Y;
}

template<class T>
inline void condNegSwap(bool c, T& X, T& Y)
{
    // used in step 2 and 3
    T Z = -X;
    X = c ? Y : X;
    Y = c ? Z : Y;
}

// matrix multiplication M = A * B
template<class T>
inline void multAB(T a11, T a12, T a13, T a21, T a22, T a23, T a31, T a32, T a33,
                   T b11, T b12, T b13, T b21, T b22, T b23, T b31, T b32, T b33,
                   T& m11, T& m12, T& m13, T& m21, T& m22, T& m23, T& m31, T& m32, T& m33)
{
    m11 = a11 * b11 + a12 * b21 + a13 * b31;
    m12 = a11 * b12 + a12 * b22 + a13 * b32;
    m13 = a11 * b13 + a12 * b23 + a13 * b33;
    m21 = a21 * b11 + a22 * b21 + a23 * b31;
    m22 = a21 * b12 + a22 * b22 + a23 * b32;
    m23 = a21 * b13 + a22 * b23 + a23 * b33;
    m31 = a31 * b11 + a32 * b21 + a33 * b31;
    m32 = a31 * b12 + a32 * b22 + a33 * b32;
    m33 = a31 * b13 + a32 * b23 + a33 * b33;
}

// matrix multiplication M = Transpose[A] * B
template<class T>
inline void multAtB(T a11, T a12, T a13, T a21, T a22, T a23, T a31, T a32, T a33,
                    T b11, T b12, T b13, T b21, T b22, T b23, T b31, T b32, T b33,
                    T& m11, T& m12, T& m13, T& m21, T& m22, T& m23, T& m31, T& m32, T& m33)
{
    m11 = a11 * b11 + a21 * b21 + a31 * b31;
    m12 = a11 * b12 + a21 * b22 + a31 * b32;
    m13 = a11 * b13 + a21 * b23 + a31 * b33;
    m21 = a12 * b11 + a22 * b21 + a32 * b31;
    m22 = a12 * b12 + a22 * b22 + a32 * b32;
    m23 = a12 * b13 + a22 * b23 + a32 * b33;
    m31 = a13 * b11 + a23 * b21 + a33 * b31;
    m32 = a13 * b12 + a23 * b22 + a33 * b32;
    m33 = a13 * b13 + a23 * b23 + a33 * b33;
}

template<class T>
inline void quatToMat3(const T* qV, T& m11, T& m12, T& m13, T& m21, T& m22, T& m23, T& m31, T& m32, T& m33)
{
    T w = qV[3];
    T x = qV[0];
    T y = qV[1];
    T z = qV[2];

    T qxx = x * x;
    T qyy = y * y;
    T qzz = z * z;
    T qxz = x * z;
    T qxy = x * y;
    T qyz = y * z;
    T qwx = w * x;
    T qwy = w * y;
    T qwz = w * z;

    m11 = 1 - 2 * (qyy + qzz);
    m12 = 2 * (qxy - qwz);
    m13 = 2 * (qxz + qwy);
    m21 = 2 * (qxy + qwz);
    m22 = 1 - 2 * (qxx + qzz);
    m23 = 2 * (qyz - qwx);
    m31 = 2 * (qxz - qwy);
    m32 = 2 * (qyz + qwx);
    m33 = 1 - 2 * (qxx + qyy);
}

template<class T>
inline void approximateGivensQuaternion(T a11, T a12, T a22, T& ch, T& sh)
{
    /*
     * Given givens angle computed by approximateGivensAngles,
     * compute the corresponding rotation quaternion.
     */
    ch = 2 * (a11 - a22);
    sh = a12;
    bool b = _gamma * sh * sh < ch * ch;
    // fast rsqrt function suffices
    // rsqrt2 (https://code.google.com/p/lppython/source/browse/algorithm/HDcode/newCode/rsqrt.c?r=26)
    // is even faster but results in too much error
    T w = rsqrt<T>(ch * ch + sh * sh);
    ch = b ? w * ch : (T)_cstar;
    sh = b ? w * sh : (T)_sstar;
}

template<class T>
inline void jacobiConjugation(const int x, const int y, const int z,
                              T& s11, T& s21, T& s22, T& s31, T& s32, T& s33,
                              T* qV)
{
    T ch, sh;
    approximateGivensQuaternion(s11, s21, s22, ch, sh);

    T scale = ch * ch + sh * sh;
    T a     = (ch * ch - sh * sh) / scale;
    T b     = (2 * sh * ch) / scale;

    // make temp copy of S
    T _s11 = s11;
    T _s21 = s21;
    T _s22 = s22;
    T _s31 = s31;
    T _s32 = s32;
    T _s33 = s33;

    // perform conjugation S = Q'*S*Q
    // Q already implicitly solved from a, b
    s11 = a * (a * _s11 + b * _s21) + b * (a * _s21 + b * _s22);
    s21 = a * (-b * _s11 + a * _s21) + b * (-b * _s21 + a * _s22);
    s22 = -b * (-b * _s11 + a * _s21) + a * (-b * _s21 + a * _s22);
    s31 = a * _s31 + b * _s32;
    s32 = -b * _s31 + a * _s32;
    s33 = _s33;

    // update cumulative rotation qV
    T tmp[3];
    tmp[0] = qV[0] * sh;
    tmp[1] = qV[1] * sh;
    tmp[2] = qV[2] * sh;
    sh    *= qV[3];

    qV[0] *= ch;
    qV[1] *= ch;
    qV[2] *= ch;
    qV[3] *= ch;

    // (x,y,z) corresponds to ((0,1,2),(1,2,0),(2,0,1))
    // for (p,q) = ((0,1),(1,2),(0,2))
    qV[z] += sh;
    qV[3] -= tmp[z]; // w
    qV[x] += tmp[y];
    qV[y] -= tmp[x];

    // re-arrange matrix for next iteration
    _s11 = s22;
    _s21 = s32;
    _s22 = s33;
    _s31 = s21;
    _s32 = s31;
    _s33 = s11;
    s11  = _s11;
    s21  = _s21;
    s22  = _s22;
    s31  = _s31;
    s32  = _s32;
    s33  = _s33;
}

template<class T>
inline T dist2(T x, T y, T z)
{
    return x * x + y * y + z * z;
}

// finds transformation that diagonalizes a symmetric matrix
template<class T>
inline void jacobiEigenanlysis( // symmetric matrix
    T& s11,
    T& s21, T& s22,
    T& s31, T& s32, T& s33,
    // quaternion representation of V
    T* qV)
{
    qV[3] = 1;
    qV[0] = 0;
    qV[1] = 0;
    qV[2] = 0; // follow same indexing convention as GLM

    for(int i = 0; i < 4; i++)
    {
        // we wish to eliminate the maximum off-diagonal element
        // on every iteration, but cycling over all 3 possible rotations
        // in fixed order (p,q) = (1,2) , (2,3), (1,3) still retains
        //  asymptotic convergence
        jacobiConjugation(0, 1, 2, s11, s21, s22, s31, s32, s33, qV); // p,q = 0,1
        jacobiConjugation(1, 2, 0, s11, s21, s22, s31, s32, s33, qV); // p,q = 1,2
        jacobiConjugation(2, 0, 1, s11, s21, s22, s31, s32, s33, qV); // p,q = 0,2
    }
}

template<class T>
inline void sortSingularValues(// matrix that we want to decompose
    T& b11, T& b12, T& b13,
    T& b21, T& b22, T& b23,
    T& b31, T& b32, T& b33,
    // sort V simultaneously
    T& v11, T& v12, T& v13,
    T& v21, T& v22, T& v23,
    T& v31, T& v32, T& v33)
{
    T    rho1 = dist2(b11, b21, b31);
    T    rho2 = dist2(b12, b22, b32);
    T    rho3 = dist2(b13, b23, b33);
    bool c;
    c = rho1 < rho2;
    condNegSwap(c, b11, b12);
    condNegSwap(c, v11, v12);
    condNegSwap(c, b21, b22);
    condNegSwap(c, v21, v22);
    condNegSwap(c, b31, b32);
    condNegSwap(c, v31, v32);
    condSwap(c, rho1, rho2);
    c = rho1 < rho3;
    condNegSwap(c, b11, b13);
    condNegSwap(c, v11, v13);
    condNegSwap(c, b21, b23);
    condNegSwap(c, v21, v23);
    condNegSwap(c, b31, b33);
    condNegSwap(c, v31, v33);
    condSwap(c, rho1, rho3);
    c = rho2 < rho3;
    condNegSwap(c, b12, b13);
    condNegSwap(c, v12, v13);
    condNegSwap(c, b22, b23);
    condNegSwap(c, v22, v23);
    condNegSwap(c, b32, b33);
    condNegSwap(c, v32, v33);
}

template<class T>
void QRGivensQuaternion(T a1, T a2, T& ch, T& sh)
{
    // a1 = pivot point on diagonal
    // a2 = lower triangular entry we want to annihilate
    T epsilon = (T)EPSILON;
    T rho     = accurateSqrt<T>(a1 * a1 + a2 * a2);

    sh = rho > epsilon ? a2 : 0;
    ch = fabs(a1) + fmax(rho, epsilon);
    bool b = a1 < 0;
    condSwap(b, sh, ch);
    T w = rsqrt<T>(ch * ch + sh * sh);
    ch *= w;
    sh *= w;
}

template<class T>
inline void QRDecomposition(// matrix that we want to decompose
    T b11, T b12, T b13,
    T b21, T b22, T b23,
    T b31, T b32, T b33,
    // output Q
    T& q11, T& q12, T& q13,
    T& q21, T& q22, T& q23,
    T& q31, T& q32, T& q33,
    // output R
    T& r11, T& r12, T& r13,
    T& r21, T& r22, T& r23,
    T& r31, T& r32, T& r33)
{
    T ch1, sh1, ch2, sh2, ch3, sh3;
    T a, b;

    // first givens rotation (ch,0,0,sh)
    QRGivensQuaternion(b11, b21, ch1, sh1);
    a = 1 - 2 * sh1 * sh1;
    b = 2 * ch1 * sh1;
    // apply B = Q' * B
    r11 = a * b11 + b * b21;
    r12 = a * b12 + b * b22;
    r13 = a * b13 + b * b23;
    r21 = -b * b11 + a * b21;
    r22 = -b * b12 + a * b22;
    r23 = -b * b13 + a * b23;
    r31 = b31;
    r32 = b32;
    r33 = b33;

    // second givens rotation (ch,0,-sh,0)
    QRGivensQuaternion(r11, r31, ch2, sh2);
    a = 1 - 2 * sh2 * sh2;
    b = 2 * ch2 * sh2;
    // apply B = Q' * B;
    b11 = a * r11 + b * r31;
    b12 = a * r12 + b * r32;
    b13 = a * r13 + b * r33;
    b21 = r21;
    b22 = r22;
    b23 = r23;
    b31 = -b * r11 + a * r31;
    b32 = -b * r12 + a * r32;
    b33 = -b * r13 + a * r33;

    // third givens rotation (ch,sh,0,0)
    QRGivensQuaternion(b22, b32, ch3, sh3);
    a = 1 - 2 * sh3 * sh3;
    b = 2 * ch3 * sh3;
    // R is now set to desired value
    r11 = b11;
    r12 = b12;
    r13 = b13;
    r21 = a * b21 + b * b31;
    r22 = a * b22 + b * b32;
    r23 = a * b23 + b * b33;
    r31 = -b * b21 + a * b31;
    r32 = -b * b22 + a * b32;
    r33 = -b * b23 + a * b33;

    // construct the cumulative rotation Q=Q1 * Q2 * Q3
    // the number of Ting point operations for three quaternion multiplications
    // is more or less comparable to the explicit form of the joined matrix.
    // certainly more memory-efficient!
    T sh12 = sh1 * sh1;
    T sh22 = sh2 * sh2;
    T sh32 = sh3 * sh3;

    q11 = (-1 + 2 * sh12) * (-1 + 2 * sh22);
    q12 = 4 * ch2 * ch3 * (-1 + 2 * sh12) * sh2 * sh3 + 2 * ch1 * sh1 * (-1 + 2 * sh32);
    q13 = 4 * ch1 * ch3 * sh1 * sh3 - 2 * ch2 * (-1 + 2 * sh12) * sh2 * (-1 + 2 * sh32);

    q21 = 2 * ch1 * sh1 * (1 - 2 * sh22);
    q22 = -8 * ch1 * ch2 * ch3 * sh1 * sh2 * sh3 + (-1 + 2 * sh12) * (-1 + 2 * sh32);
    q23 = -2 * ch3 * sh3 + 4 * sh1 * (ch3 * sh1 * sh3 + ch1 * ch2 * sh2 * (-1 + 2 * sh32));

    q31 = 2 * ch2 * sh2;
    q32 = 2 * ch3 * (1 - 2 * sh22) * sh3;
    q33 = (-1 + 2 * sh22) * (-1 + 2 * sh32);
}

template<class T>
void svd(// input A
    T a11, T a12, T a13,
    T a21, T a22, T a23,
    T a31, T a32, T a33,
    // output U
    T& u11, T& u12, T& u13,
    T& u21, T& u22, T& u23,
    T& u31, T& u32, T& u33,
    // output S
    T& s11, T& s12, T& s13,
    T& s21, T& s22, T& s23,
    T& s31, T& s32, T& s33,
    // output V
    T& v11, T& v12, T& v13,
    T& v21, T& v22, T& v23,
    T& v31, T& v32, T& v33)
{
    // normal equations matrix
    T ATA11, ATA12, ATA13;
    T ATA21, ATA22, ATA23;
    T ATA31, ATA32, ATA33;

    multAtB(a11, a12, a13, a21, a22, a23, a31, a32, a33,
            a11, a12, a13, a21, a22, a23, a31, a32, a33,
            ATA11, ATA12, ATA13, ATA21, ATA22, ATA23, ATA31, ATA32, ATA33);

    // symmetric eigenalysis
    T qV[4];
    jacobiEigenanlysis(ATA11, ATA21, ATA22, ATA31, ATA32, ATA33, qV);
    quatToMat3(qV, v11, v12, v13, v21, v22, v23, v31, v32, v33);

    T b11, b12, b13;
    T b21, b22, b23;
    T b31, b32, b33;
    multAB(a11, a12, a13, a21, a22, a23, a31, a32, a33,
           v11, v12, v13, v21, v22, v23, v31, v32, v33,
           b11, b12, b13, b21, b22, b23, b31, b32, b33);

    // sort singular values and find V
    sortSingularValues(b11, b12, b13, b21, b22, b23, b31, b32, b33,
                       v11, v12, v13, v21, v22, v23, v31, v32, v33);

    // QR decomposition
    QRDecomposition(b11, b12, b13, b21, b22, b23, b31, b32, b33,
                    u11, u12, u13, u21, u22, u23, u31, u32, u33,
                    s11, s12, s13, s21, s22, s23, s31, s32, s33);
}

/// polar decomposition can be reconstructed trivially from SVD result
// A = UP
template<class T>
void polarDecomposition(T a11, T a12, T a13,
                        T a21, T a22, T a23,
                        T a31, T a32, T a33,
                        // output U
                        T& u11, T& u12, T& u13,
                        T& u21, T& u22, T& u23,
                        T& u31, T& u32, T& u33,
                        // output P
                        T& p11, T& p12, T& p13,
                        T& p21, T& p22, T& p23,
                        T& p31, T& p32, T& p33)
{
    T w11, w12, w13, w21, w22, w23, w31, w32, w33;
    T s11, s12, s13, s21, s22, s23, s31, s32, s33;
    T v11, v12, v13, v21, v22, v23, v31, v32, v33;

    svd(a11, a12, a13, a21, a22, a23, a31, a32, a33,
        w11, w12, w13, w21, w22, w23, w31, w32, w33,
        s11, s12, s13, s21, s22, s23, s31, s32, s33,
        v11, v12, v13, v21, v22, v23, v31, v32, v33);

    // P = VSV'
    T t11, t12, t13, t21, t22, t23, t31, t32, t33;
    multAB(v11, v12, v13, v21, v22, v23, v31, v32, v33,
           s11, s12, s13, s21, s22, s23, s31, s32, s33,
           t11, t12, t13, t21, t22, t23, t31, t32, t33);

    multAB(t11, t12, t13, t21, t22, t23, t31, t32, t33,
           v11, v21, v31, v12, v22, v32, v13, v23, v33,
           p11, p12, p13, p21, p22, p23, p31, p32, p33);

    // U = WV'
    multAB(w11, w12, w13, w21, w22, w23, w31, w32, w33,
           v11, v21, v31, v12, v22, v32, v13, v23, v33,
           u11, u12, u13, u21, u22, u23, u31, u32, u33);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SVDDecomposition

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana