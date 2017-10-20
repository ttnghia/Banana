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
    Vector<RealType> m_X, m_Y;           // x,y coordinates of poInts
    Vector<RealType> m_a, m_b, m_c;      // CubicSpline coefficients
    RealType         m_b0, m_c0;         // for left extrapolation
    // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i

    BDType   m_Left                 = BDType::SecondOrder;
    BDType   m_Right                = BDType::SecondOrder;
    RealType m_LeftValue            = RealType(0);
    RealType m_RightValue           = RealType(0);
    bool     m_bLinearExtrapolation = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Animation/CubicSpline.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
