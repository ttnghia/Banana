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
#include <Banana/Utils/MathHelpers.h>

#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
template<class RealType>
class BandMatrix
{
private:
    Vector<Vector<RealType> > m_Upper;      // upper band
    Vector<Vector<RealType> > m_Lower;      // lower band
public:
    BandMatrix() = default;
    BandMatrix(Int dim, Int n_u, Int n_l) { resize(dim, n_u, n_l); }
    void resize(Int dim, Int n_u, Int n_l); // init with dim,n_u,n_l
    Int  dim() const { return (m_Upper.size() > 0) ? static_cast<Int>(m_Upper[0].size()) : 0; }
    Int  nUpper() const { return static_cast<Int>(m_Upper.size() - 1); }
    Int  nLower() const { return static_cast<Int>(m_Lower.size() - 1); }

    RealType& operator ()(Int i, Int j);
    RealType operator  ()(Int i, Int j) const;

    RealType&        saved_diag(Int i);
    RealType         saved_diag(Int i) const;
    void             lu_decompose();
    Vector<RealType> r_solve(const Vector<RealType>& b) const;
    Vector<RealType> l_solve(const Vector<RealType>& b) const;
    Vector<RealType> lu_solve(const Vector<RealType>& b, bool is_lu_decomposed = false);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class CubicSpline
{
public:
    enum class BDType
    {
        FirstOrder  = 1,
        SecondOrder = 2
    };

    CubicSpline() = default;

    // optional, but if called it has to come be before setPoints()
    void              setBoundary(BDType left, RealType leftValue, BDType right, RealType rightValue, bool bLinearExtrapolation = false);
    void              setPoints(const Vector<RealType>& X, const Vector<RealType>& Y, bool bCubicSpline = true);
    RealType operator ()(RealType x) const;
    RealType          deriv(Int order, RealType x) const;

private:
    Vector<RealType> m_X, m_Y;               // x,y coordinates of poInts
    Vector<RealType> m_a, m_b, m_c;          // CubicSpline coefficients
    RealType         m_b0, m_c0;             // for left extrapolation
    // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i

    BDType   m_Left                 = BDType::SecondOrder;
    BDType   m_Right                = BDType::SecondOrder;
    RealType m_LeftValue            = RealType(0);
    RealType m_RightValue           = RealType(0);
    bool     m_bLinearExtrapolation = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void BandMatrix<RealType >::resize(Int dim, Int n_u, Int n_l)
{
    assert(dim > 0);
    assert(n_u >= 0);
    assert(n_l >= 0);

    m_Upper.resize(n_u + 1);
    m_Lower.resize(n_l + 1);

    for(size_t i = 0; i < m_Upper.size(); ++i) {
        m_Upper[i].resize(dim);
    }
    for(size_t i = 0; i < m_Lower.size(); ++i) {
        m_Lower[i].resize(dim);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType& BandMatrix<RealType>::operator ()(Int i, Int j)
{
    Int k = j - i;               // what band is the entry
    assert((i >= 0) && (i < dim()) && (j >= 0) && (j < dim()));
    assert((-nLower() <= k) && (k <= nUpper()));
    // k=0 -> diogonal, k<0 lower left part, k>0 upper right part
    if(k >= 0) {
        return m_Upper[k][i];
    } else {
        return m_Lower[-k][i];
    }
}

template<class RealType>
RealType BandMatrix<RealType>::operator ()(Int i, Int j) const
{
    Int k = j - i;               // what band is the entry
    assert((i >= 0) && (i < dim()) && (j >= 0) && (j < dim()));
    assert((-nLower() <= k) && (k <= nUpper()));
    // k=0 -> diogonal, k<0 lower left part, k>0 upper right part
    if(k >= 0) {
        return m_Upper[k][i];
    } else {
        return m_Lower[-k][i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// second diag (used in LU decomposition), saved in m_lower
template<class RealType>
RealType BandMatrix<RealType >::saved_diag(Int i) const
{
    assert((i >= 0) && (i < dim()));
    return m_Lower[0][i];
}

template<class RealType>
RealType& BandMatrix<RealType >::saved_diag(Int i)
{
    assert((i >= 0) && (i < dim()));
    return m_Lower[0][i];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// LR-Decomposition of a band matrix
template<class RealType>
void BandMatrix<RealType >::lu_decompose()
{
    Int      i_max, j_max;
    Int      j_min;
    RealType x;

    // preconditioning
    // normalize column i so that a_ii=1
    for(Int i = 0; i < dim(); ++i) {
        assert(this->operator()(i, i) != RealType(0));
        saved_diag(i) = RealType(1.0) / this->operator()(i, i);
        j_min         = MathHelpers::max(0, i - nLower());
        j_max         = MathHelpers::min(dim() - 1, i + nUpper());
        for(Int j = j_min; j <= j_max; j++) {
            this->operator()(i, j) *= saved_diag(i);
        }
        this->operator()(i, i) = RealType(1.0);                          // prevents rounding errors
    }

    // Gauss LR-Decomposition
    for(Int k = 0; k < dim(); k++) {
        i_max = MathHelpers::min(dim() - 1, k + nLower());                  // nLower not a mistake!
        for(Int i = k + 1; i <= i_max; ++i) {
            assert(this->operator()(k, k) != RealType(0));
            x                      = -this->operator()(i, k) / this->operator()(k, k);
            this->operator()(i, k) = -x;                                                 // assembly part of L
            j_max                  = MathHelpers::min(dim() - 1, k + nUpper());
            for(Int j = k + 1; j <= j_max; j++) {
// assembly part of R
                this->operator()(i, j) = this->operator()(i, j) + x * this->operator()(k, j);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solves Ly=b
template<class RealType>
Vector<RealType> BandMatrix<RealType >::l_solve(const Vector<RealType>& b) const
{
    assert(dim() == (Int)b.size());
    Vector<RealType> x(dim());
    Int              j_start;
    RealType         sum;
    for(Int i = 0; i < dim(); ++i) {
        sum     = 0;
        j_start = MathHelpers::max(0, i - nLower());
        for(Int j = j_start; j < i; j++) {
            sum += this->operator()(i, j) * x[j];
        }
        x[i] = (b[i] * saved_diag(i)) - sum;
    }
    return x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solves Rx=y
template<class RealType>
Vector<RealType> BandMatrix<RealType >::r_solve(const Vector<RealType>& b) const
{
    assert(dim() == (Int)b.size());
    Vector<RealType> x(dim());
    Int              j_stop;
    RealType         sum;
    for(Int i = dim() - 1; i >= 0; i--) {
        sum    = 0;
        j_stop = MathHelpers::min(dim() - 1, i + nUpper());
        for(Int j = i + 1; j <= j_stop; j++) {
            sum += this->operator()(i, j) * x[j];
        }
        x[i] = (b[i] - sum) / this->operator()(i, i);
    }
    return x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vector<RealType> BandMatrix<RealType >::lu_solve(const Vector<RealType>& b, bool is_lu_decomposed)
{
    assert(dim() == (Int)b.size());
    Vector<RealType> x, y;
    if(is_lu_decomposed == false) {
        this->lu_decompose();
    }
    y = l_solve(b);
    x = r_solve(y);
    return x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void CubicSpline<RealType >::setBoundary(BDType left, RealType leftValue, BDType right, RealType rightValue, bool bLinearExtrapolation /*= false*/)
{
    assert(m_X.size() == 0);                  // setPoints() must not have happened yet
    m_Left                 = left;
    m_Right                = right;
    m_LeftValue            = leftValue;
    m_RightValue           = rightValue;
    m_bLinearExtrapolation = bLinearExtrapolation;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void CubicSpline<RealType >::setPoints(const Vector<RealType>& X, const Vector<RealType>& Y, bool bCubicSpline /*= true*/)
{
    assert(X.size() == Y.size());
    assert(X.size() > 2);
    m_X = X;
    m_Y = Y;
    Int n = Int(X.size());
    // TODO: maybe sort x and y, rather than returning an error
    for(Int i = 0; i < n - 1; ++i) {
        assert(m_X[i] < m_X[i + 1]);
    }

    if(bCubicSpline == true) { // cubic CubicSpline Interpolation
// setting up the matrix and right hand side of the equation system
// for the parameters b[]
        BandMatrix<RealType> A(n, 1, 1);
        Vector<RealType>     rhs(n);
        for(Int i = 1; i < n - 1; ++i) {
            A(i, i - 1) = RealType(1.0 / 3.0) * (X[i] - X[i - 1]);
            A(i, i)     = RealType(2.0 / 3.0) * (X[i + 1] - X[i - 1]);
            A(i, i + 1) = RealType(1.0 / 3.0) * (X[i + 1] - X[i]);
            rhs[i]      = (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]) - (Y[i] - Y[i - 1]) / (X[i] - X[i - 1]);
        }
        // boundary conditions
        if(m_Left == CubicSpline<RealType>::BDType::SecondOrder) {
// 2*b[0] = f''
            A(0, 0) = RealType(2.0);
            A(0, 1) = RealType(0);
            rhs[0]  = m_LeftValue;
        } else if(m_Left == CubicSpline<RealType>::BDType::FirstOrder) {
// c[0] = f', needs to be re-expressed in terms of b:
// (2b[0]+b[1])(x[1]-x[0]) = 3 ((y[1]-y[0])/(x[1]-x[0]) - f')
            A(0, 0) = RealType(2.0) * (X[1] - X[0]);
            A(0, 1) = RealType(1.0) * (X[1] - X[0]);
            rhs[0]  = RealType(3.0) * ((Y[1] - Y[0]) / (X[1] - X[0]) - m_LeftValue);
        } else {
            assert(false);
        }
        if(m_Right == CubicSpline<RealType>::BDType::SecondOrder) {
// 2*b[n-1] = f''
            A(n - 1, n - 1) = RealType(2.0);
            A(n - 1, n - 2) = RealType(0);
            rhs[n - 1]      = m_RightValue;
        } else if(m_Right == CubicSpline<RealType>::BDType::FirstOrder) {
// c[n-1] = f', needs to be re-expressed in terms of b:
// (b[n-2]+2b[n-1])(x[n-1]-x[n-2])
// = 3 (f' - (y[n-1]-y[n-2])/(x[n-1]-x[n-2]))
            A(n - 1, n - 1) = RealType(2.0) * (X[n - 1] - X[n - 2]);
            A(n - 1, n - 2) = RealType(1.0) * (X[n - 1] - X[n - 2]);
            rhs[n - 1]      = RealType(3.0) * (m_RightValue - (Y[n - 1] - Y[n - 2]) / (X[n - 1] - X[n - 2]));
        } else {
            assert(false);
        }

        // solve the equation system to obtain the parameters b[]
        m_b = A.lu_solve(rhs);

        // calculate parameters a[] and c[] based on b[]
        m_a.resize(n);
        m_c.resize(n);
        for(Int i = 0; i < n - 1; ++i) {
            m_a[i] = RealType(1.0 / 3.0) * (m_b[i + 1] - m_b[i]) / (X[i + 1] - X[i]);
            m_c[i] = (Y[i + 1] - Y[i]) / (X[i + 1] - X[i])
                     - RealType(1.0 / 3.0) * (RealType(2.0) * m_b[i] + m_b[i + 1]) * (X[i + 1] - X[i]);
        }
    } else { // linear Interpolation
        m_a.resize(n);
        m_b.resize(n);
        m_c.resize(n);
        for(Int i = 0; i < n - 1; ++i) {
            m_a[i] = RealType(0);
            m_b[i] = RealType(0);
            m_c[i] = (m_Y[i + 1] - m_Y[i]) / (m_X[i + 1] - m_X[i]);
        }
    }

    // for left extrapolation coefficients
    m_b0 = (m_bLinearExtrapolation == false) ? m_b[0] : RealType(0);
    m_c0 = m_c[0];

    // for the right extrapolation coefficients
    // f_{n-1}(x) = b*(x-x_{n-1})^2 + c*(x-x_{n-1}) + y_{n-1}
    RealType h = X[n - 1] - X[n - 2];
    // m_b[n-1] is determined by the boundary condition
    m_a[n - 1] = RealType(0);
    m_c[n - 1] = RealType(3.0) * m_a[n - 2] * h * h + RealType(2.0) * m_b[n - 2] * h + m_c[n - 2];           // = f'_{n-2}(x_{n-1})
    if(m_bLinearExtrapolation == true) {
        m_b[n - 1] = RealType(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType CubicSpline<RealType>::operator ()(RealType x) const
{
    size_t n = m_X.size();
    // find the closest poInt m_X[idx] < x, idx=0 even if x<m_X[0]
    Vector<RealType>::const_iterator it;
    it = std::lower_bound(m_X.begin(), m_X.end(), x);
    Int idx = MathHelpers::max(Int(it - m_X.begin()) - 1, 0);

    RealType h = x - m_X[idx];
    RealType Interpol;
    if(x < m_X[0]) {
// extrapolation to the left
        Interpol = (m_b0 * h + m_c0) * h + m_Y[0];
    } else if(x > m_X[n - 1]) {
// extrapolation to the right
        Interpol = (m_b[n - 1] * h + m_c[n - 1]) * h + m_Y[n - 1];
    } else {
        // Interpolation
        Interpol = ((m_a[idx] * h + m_b[idx]) * h + m_c[idx]) * h + m_Y[idx];
    }
    return Interpol;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType CubicSpline<RealType >::deriv(Int order, RealType x) const
{
    assert(order > 0);

    size_t n = m_X.size();
    // find the closest poInt m_X[idx] < x, idx=0 even if x<m_X[0]
    Vector<RealType>::const_iterator it;
    it = std::lower_bound(m_X.begin(), m_X.end(), x);
    Int idx = MathHelpers::max(Int(it - m_X.begin()) - 1, 0);

    RealType h = x - m_X[idx];
    RealType Interpol;
    if(x < m_X[0]) {
// extrapolation to the left
        switch(order) {
            case 1:
                Interpol = RealType(2.0) * m_b0 * h + m_c0;
                break;
            case 2:
                Interpol = RealType(2.0) * m_b0 * h;
                break;
            default:
                Interpol = RealType(0);
                break;
        }
    } else if(x > m_X[n - 1]) {
// extrapolation to the right
        switch(order) {
            case 1:
                Interpol = RealType(2.0) * m_b[n - 1] * h + m_c[n - 1];
                break;
            case 2:
                Interpol = RealType(2.0) * m_b[n - 1];
                break;
            default:
                Interpol = RealType(0);
                break;
        }
    } else {
        // Interpolation
        switch(order) {
            case 1:
                Interpol = (RealType(3.0) * m_a[idx] * h + RealType(2.0) * m_b[idx]) * h + m_c[idx];
                break;
            case 2:
                Interpol = RealType(6.0) * m_a[idx] * h + RealType(2.0) * m_b[idx];
                break;
            case 3:
                Interpol = RealType(6.0) * m_a[idx];
                break;
            default:
                Interpol = RealType(0);
                break;
        }
    }
    return Interpol;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
