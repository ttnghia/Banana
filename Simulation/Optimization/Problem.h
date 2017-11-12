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
#define EXPECT_NEAR(x, y, z)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Optimization
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Problem
{
protected:

    bool hasLowerBound_ = false;
    bool hasUpperBound_ = false;

    Vector<RealType> lowerBound_;
    Vector<RealType> upperBound_;

public:

    Problem() {}

    void setBoxConstraint(Vector<RealType> lb, Vector<RealType> ub)
    {
        setLowerBound(lb);
        setUpperBound(ub);
    }

    void setLowerBound(Vector<RealType> lb)
    {
        lowerBound_    = lb;
        hasLowerBound_ = true;
    }

    void setUpperBound(Vector<RealType> ub)
    {
        upperBound_    = ub;
        hasUpperBound_ = true;
    }

    bool hasLowerBound()
    {
        return hasLowerBound_;
    }

    bool hasUpperBound()
    {
        return hasUpperBound_;
    }

    Vector<RealType> lowerBound()
    {
        return lowerBound_;
    }

    Vector<RealType> upperBound()
    {
        return upperBound_;
    }

    /**
     * @brief returns objective value in x
     * @details [long description]
     *
     * @param x [description]
     * @return [description]
     */
    virtual RealType value(const Vector<RealType>& x) = 0;

    /**
     * @brief overload value for nice syntax
     * @details [long description]
     *
     * @param x [description]
     * @return [description]
     */
    RealType operator ()(const Vector<RealType>& x)
    {
        return value(x);
    }

    /**
     * @brief returns gradient in x as reference parameter
     * @details should be overwritten by symbolic gradient
     *
     * @param grad [description]
     */
    virtual void gradient(const Vector<RealType>& x, Vector<RealType>& grad)
    {
        finiteGradient(x, grad);
    }

    // Compute the value AND the gradient
    virtual RealType valueGradient(const Vector<RealType>& x, Vector<RealType>& grad)
    {
        gradient(x, grad);
        return value(x);
    }

    /**
     * @brief This computes the hessian
     * @details should be overwritten by symbolic hessian, if solver relies on hessian
     */
    //virtual void hessian(const Vector<RealType>& x, MatrixX<RealType>& hessian)
    //{
    //    finiteHessian(x, hessian);
    //}

    virtual bool checkGradient(const Vector<RealType>& x, int accuracy = 3)
    {
        // TODO: check if derived class exists:
        // int(typeid(&Rosenbrock<float>::gradient) == typeid(&Problem<float>::gradient)) == 1 --> overwritten
        const int        D = x.size();
        Vector<RealType> actual_grad(D);
        Vector<RealType> expected_grad(D);
        gradient(x, actual_grad);
        finiteGradient(x, expected_grad, accuracy);

        bool correct = true;

        for(int d = 0; d < D; ++d) {
            RealType scale = std::max((std::max(fabs(actual_grad[d]), fabs(expected_grad[d]))), RealType(1.));
            EXPECT_NEAR(actual_grad[d], expected_grad[d], 1e-2 * scale);
            if(fabs(actual_grad[d] - expected_grad[d]) > 1e-2 * scale) {
                correct = false;
            }
        }
        return correct;
    }

    //virtual bool checkHessian(const Vector<RealType>& x, int accuracy = 3)
    //{
    //    // TODO: check if derived class exists:
    //    // int(typeid(&Rosenbrock<float>::gradient) == typeid(&Problem<float>::gradient)) == 1 --> overwritten
    //    const int D       = x.rows();
    //    bool      correct = true;

    //    MatrixX<RealType> actual_hessian   = MatrixX<RealType>::Zero(D, D);
    //    MatrixX<RealType> expected_hessian = MatrixX<RealType>::Zero(D, D);
    //    hessian(x, actual_hessian);
    //    finiteHessian(x, expected_hessian, accuracy);
    //    for(int d = 0; d < D; ++d) {
    //        for(int e = 0; e < D; ++e) {
    //            RealType scale = std::max(static_cast<RealType>(std::max(fabs(actual_hessian(d, e)), fabs(expected_hessian(d, e)))), (RealType)1.);
    //            EXPECT_NEAR(actual_hessian(d, e), expected_hessian(d, e), 1e-1 * scale);
    //            if(fabs(actual_hessian(d, e) - expected_hessian(d, e)) > 1e-1 * scale) {
    //                correct = false;
    //            }
    //        }
    //    }
    //    return correct;
    //}

    virtual void finiteGradient(const Vector<RealType>& x, Vector<RealType>& grad, int accuracy = 0) final
    {
        // accuracy can be 0, 1, 2, 3
        const RealType                            eps   = 2.2204e-6;
        const size_t                              D     = x.size();
        const std::vector<std::vector<RealType> > coeff =
        { { 1, -1 }, { 1, -8, 8, -1 }, { -1, 9, -45, 45, -9, 1 }, { 3, -32, 168, -672, 672, -168, 32, -3 } };
        const std::vector<std::vector<RealType> > coeff2 =
        { { 1, -1 }, { -2, -1, 1, 2 }, { -3, -2, -1, 1, 2, 3 }, { -4, -3, -2, -1, 1, 2, 3, 4 } };
        const std::vector<RealType> dd = { 2, 12, 60, 840 };

        Vector<RealType> finiteDiff(D);
        for(size_t d = 0; d < D; d++) {
            finiteDiff[d] = 0;
            for(int s = 0; s < 2 * (accuracy + 1); ++s) {
                Vector<RealType> xx = x;
                xx[d]         += coeff2[accuracy][s] * eps;
                finiteDiff[d] += coeff[accuracy][s] * value(xx);
            }
            finiteDiff[d] /= (dd[accuracy] * eps);
        }
        grad = finiteDiff;
    }

    /*
            virtual void finiteHessian(const Vector<RealType>& x, MatrixX<RealType>& hessian, int accuracy = 0) final
            {
                    const RealType eps = std::numeric_limits<RealType>::epsilon() * 10e7;
                    const size_t   DIM = x.size();

                    if(accuracy == 0) {
                            for(size_t i = 0; i < DIM; i++) {
                                    for(size_t j = 0; j < DIM; j++) {
                                            Vector<RealType> xx = x;
                                            RealType         f4 = value(xx);
                                            xx[i] += eps;
                                            xx[j] += eps;
                                            RealType f1 = value(xx);
                                            xx[j] -= eps;
                                            RealType f2 = value(xx);
                                            xx[j] += eps;
                                            xx[i] -= eps;
                                            RealType f3 = value(xx);
                                            hessian(i, j) = (f1 - f2 - f3 + f4) / (eps * eps);
                                    }
                            }
                    } else {
                            Vector<RealType> xx;
                            for(size_t i = 0; i < DIM; i++) {
                                    for(size_t j = 0; j < DIM; j++) {
                                            RealType term_1 = 0;
                                            xx = x.eval(); xx[i] += 1 * eps;  xx[j] += -2 * eps;  term_1 += value(xx);
                                            xx = x.eval(); xx[i] += 2 * eps;  xx[j] += -1 * eps;  term_1 += value(xx);
                                            xx = x.eval(); xx[i] += -2 * eps; xx[j] += 1 * eps;   term_1 += value(xx);
                                            xx = x.eval(); xx[i] += -1 * eps; xx[j] += 2 * eps;   term_1 += value(xx);

                                            RealType term_2 = 0;
                                            xx = x.eval(); xx[i] += -1 * eps; xx[j] += -2 * eps;  term_2 += value(xx);
                                            xx = x.eval(); xx[i] += -2 * eps; xx[j] += -1 * eps;  term_2 += value(xx);
                                            xx = x.eval(); xx[i] += 1 * eps;  xx[j] += 2 * eps;   term_2 += value(xx);
                                            xx = x.eval(); xx[i] += 2 * eps;  xx[j] += 1 * eps;   term_2 += value(xx);

                                            RealType term_3 = 0;
                                            xx = x.eval(); xx[i] += 2 * eps;  xx[j] += -2 * eps;  term_3 += value(xx);
                                            xx = x.eval(); xx[i] += -2 * eps; xx[j] += 2 * eps;   term_3 += value(xx);
                                            xx = x.eval(); xx[i] += -2 * eps; xx[j] += -2 * eps;  term_3 -= value(xx);
                                            xx = x.eval(); xx[i] += 2 * eps;  xx[j] += 2 * eps;   term_3 -= value(xx);

                                            RealType term_4 = 0;
                                            xx = x.eval(); xx[i] += -1 * eps; xx[j] += -1 * eps;  term_4 += value(xx);
                                            xx = x.eval(); xx[i] += 1 * eps;  xx[j] += 1 * eps;   term_4 += value(xx);
                                            xx = x.eval(); xx[i] += 1 * eps;  xx[j] += -1 * eps;  term_4 -= value(xx);
                                            xx = x.eval(); xx[i] += -1 * eps; xx[j] += 1 * eps;   term_4 -= value(xx);

                                            hessian(i, j) = (-63 * term_1 + 63 * term_2 + 44 * term_3 + 74 * term_4) / (600.0 * eps * eps);
                                    }
                            }
                    }
            }*/
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::Optimization
