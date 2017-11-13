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

#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>

#include <Optimization/Optimization.h>
#include <Optimization/MoreThuente.h>
#include <Optimization/Problem.h>

#include <iostream>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Optimization
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T, int Ord>
class ISolver
{
public:
    ISolver() = default;

    /**
     * @brief minimize an objective function given a gradient
     * @details this is just the abstract interface
     *
     * @param x0 starting point
     * @param funObjective objective function
     */
    virtual void minimize(Problem<T>& objFunc, Vector<T>& x0) = 0;

    ////////////////////////////////////////////////////////////////////////////////
    Options<T>         settings_;
    int                n_iters;
    std::vector<T>     obj_vals;
    std::vector<float> runtimes;

protected:
    const int order_ = Ord;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+




/**
 * @brief  LBFGS implementation based on Nocedal & Wright Numerical Optimization book (Section 7.2)
 * @tparam T scalar type
 * @tparam P problem type
 * @tparam Ord order of solver
 */

template<class RealType>
class LBFGSSolver : public ISolver<RealType, 1>
{
public:
    void minimize(Problem<RealType>& objFunc, Vector<RealType>& x0)
    {
        int      _m      = std::min(int(this->settings_.maxIter), 10);
        int      _noVars = x0.size();
        RealType _eps_g  = this->settings_.gradTol;
        RealType _eps_x  = 1e-8;


        MatrixX<RealType> s = MatrixX<RealType>(_noVars, _m);
        MatrixX<RealType> y = MatrixX<RealType>(_noVars, _m);

        Vector<RealType> alpha = Vector<RealType>(_m, 0);
        Vector<RealType> rho   = Vector<RealType>(_m, 0);
        Vector<RealType> grad(_noVars), q(_noVars), grad_old(_noVars), x_old(_noVars);

        //	RealType f = objFunc.value(x0);
        RealType f              = objFunc.valueGradient(x0, grad);
        RealType gamma_k        = this->settings_.init_hess;
        RealType gradNorm       = 0;
        RealType alpha_init     = std::min(1.0, 1.0 / ParallelSTL::maxAbs(grad));
        int      globIter       = 0;
        int      maxiter        = this->settings_.maxIter;
        RealType new_hess_guess = 1.0;         // only changed if we converged to a solution

        for(int k = 0; k < maxiter; k++) {
            x_old    = x0;
            grad_old = grad;
            q        = grad;
            globIter++;

            //L - BFGS first - loop recursion
            int iter = std::min(_m, k);
            for(int i = iter - 1; i >= 0; --i) {
                rho[i]   = 1.0 / ParallelBLAS::dotProduct(s.col(i), y.col(i));
                alpha[i] = rho[i] * ParallelBLAS::dotProduct(s.col(i), q);
                ParallelBLAS::addScaled(-alpha[i], y.col(i), q);
                //q = q - alpha[i] * y.col(i);
            }

            //L - BFGS second - loop recursion
            //q = gamma_k * q;
            ParallelBLAS::scale(gamma_k, q);
            for(int i = 0; i < iter; ++i) {
                RealType beta = rho[i] * ParallelBLAS::dotProduct(q, y.col(i));
                ParallelBLAS::addScaled((alpha[i] - beta), s.col(i), q);
                //q = q + (alpha[i] - beta) * s.col(i);
            }

            // is there a descent
            RealType dir = ParallelBLAS::dotProduct(q, grad);
            if(dir < 1e-4) {
                q          = grad;
                maxiter   -= k;
                k          = 0;
                alpha_init = std::min(1.0, 1.0 / ParallelSTL::maxAbs(grad));
            }

            const RealType rate = MoreThuente<RealType, decltype(objFunc), 1>::linesearch(x0, ParallelBLAS::multiply(RealType(-1.0), q), objFunc, alpha_init);
            //		const RealType rate = linesearch(objFunc, x0, -q, f, grad, 1.0);


            ParallelBLAS::addScaled(-rate, q, x0);
            //x0 = x0 - rate * q;

            if(ParallelBLAS::norm2(ParallelBLAS::minus(x_old, x0)) < _eps_x) {
                //			std::cout << "x diff norm: " << (x_old - x0).squaredNorm() << std::endl;
                break;
            }                 // usually this is a problem so exit

            //		f = objFunc.value(x0);
            f = objFunc.valueGradient(x0, grad);

            gradNorm = ParallelSTL::maxAbs(grad);
            if(gradNorm < _eps_g) {
                // Only change hessian guess if we break out the loop via convergence.
                //			std::cout << "grad norm: " << gradNorm << std::endl;
                new_hess_guess = gamma_k;
                break;
            }

            Vector<RealType> s_temp = ParallelBLAS::minus(x0, x_old);
            Vector<RealType> y_temp = ParallelBLAS::minus(grad, grad_old);
            //Vector<RealType> s_temp = x0 - x_old;
            //Vector<RealType> y_temp = grad - grad_old;

            // update the history
            if(k < _m) {
                s.col(k) = s_temp;
                y.col(k) = y_temp;
            } else {
                std::rotate(s.data().begin(), s.data().begin() + 1, s.data().end());
                s.col(s.nCols() - 1) = s_temp;

                std::rotate(y.data().begin(), y.data().begin() + 1, y.data().end());
                y.col(y.nCols() - 1) = y_temp;

                /*s.leftCols(_m - 1) = s.rightCols(_m - 1).eval();
                   s.rightCols(1)     = s_temp;
                   y.leftCols(_m - 1) = y.rightCols(_m - 1).eval();
                   y.rightCols(1)     = y_temp;*/
            }


            gamma_k    = ParallelBLAS::dotProduct(s_temp, y_temp) / ParallelBLAS::dotProduct(y_temp, y_temp);
            alpha_init = 1.0;
        }

        this->n_iters             = globIter;
        this->settings_.init_hess = new_hess_guess;
    }       // end minimize
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::Optimization
