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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Optimization
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct Options
{
    RealType gradTol;
    RealType rate;
    RealType init_hess;
    size_t   maxIter;
    size_t   m;
    bool     store_obj;
    bool     store_runtime;
    bool     use_max_iters;

    Options()
    {
        rate          = RealType(0.00005);
        maxIter       = RealType(100000);
        gradTol       = RealType(1e-4);
        m             = 10;
        store_obj     = false;
        store_runtime = false;
        use_max_iters = false;
        init_hess     = RealType(1.0);     // only used by lbfgs
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


template<class RealType>
class MatrixX
{
public:
    template<class IndexType>
    MatrixX(IndexType nRows, IndexType nCols, RealType defaultVal = RealType(0)) : m_nRows(nRows), m_nCols(nCols)
    {
        m_Data.resize(nCols);
        for(auto& col : m_Data) {
            col.resize(m_nRows, defaultVal);
        }
    }

    auto nRows() const { return m_nRows; }
    auto nCols() const { return m_nCols; }

    template<class IndexType> const auto& col(IndexType col) const { return m_Data[col]; }
    template<class IndexType> auto&       col(IndexType col) { return m_Data[col]; }

    const auto& data() const { return m_Data; }
    auto&       data() { return m_Data; }

private:
    UInt                      m_nRows = 0;
    UInt                      m_nCols = 0;
    Vector<Vector<RealType> > m_Data;
};

//template<typename T>
//bool checkConvergence(T val_new, T val_old, Vector<T> grad, Vector<T> x_new, Vector<T> x_old)
//{
//    T ftol = 1e-10;
//    T gtol = 1e-8;
//    T xtol = 1e-32;
//
//    // value crit.
//    if((x_new - x_old).cwiseAbs().maxCoeff() < xtol) {
//        return true;
//    }
//
//    // // absol. crit
//    if(abs(val_new - val_old) / (abs(val_new) + ftol) < ftol) {
//        std::cout << abs(val_new - val_old) / (abs(val_new) + ftol) << std::endl;
//        std::cout << val_new << std::endl;
//        std::cout << val_old << std::endl;
//        std::cout << abs(val_new - val_old) / (abs(val_new) + ftol) << std::endl;
//        return true;
//    }
//
//    // gradient crit
//    T g = grad.template lpNorm<Eigen::Infinity>();
//    if(g < gtol) {
//        return true;
//    }
//    return false;
//}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::Optimization
