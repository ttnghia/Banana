#ifndef CG_SOLVER_H
#define CG_SOLVER_H

#include <cmath>

#include <ParallelBLAS.h>
#include <ParallelSTL.h>
#include "./BlockSparseMatrix.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template <class MatrixType, class VectorType, class RealType>
class ConjugateGradientSolver
{
public:
    ConjugateGradientSolver()
    {
        set_solver_parameters(1e-20, 10000);
        zero_initial = true;
    }

    void set_solver_parameters(RealType tolerance_factor_, int max_iterations_)
    {
        tolerance_factor = tolerance_factor_;

        if(tolerance_factor < 1e-30)
        {
            tolerance_factor = 1e-30;
        }

        max_iterations = max_iterations_;
    }

    void setZeroInitial(bool _zero_initial)
    {
        zero_initial = _zero_initial;
    }

    void enableZeroInitial()
    {
        zero_initial = true;
    }

    void disableZeroInitial()
    {
        zero_initial = false;
    }

    bool solve(const BlockSparseMatrix<MatrixType>& matrix,
               const std::vector<VectorType>& rhs,
               std::vector<VectorType>& result,
               RealType& residual_out, int& iterations_out)
    {
        UInt n = matrix.size;

        if(z.size() != n)
        {
            s.resize(n);
            z.resize(n);
            r.resize(n);
        }

        // zero out the result
        if(zero_initial)
        {
            for(size_t i = 0; i < result.size(); ++i)
            {
#ifdef __Using_Eigen_Lib__
                result[i] = VectorType::Zero();
#else
                result[i][0] = 0;;
                result[i][1] = 0;;
                result[i][2] = 0;;
#endif
            }
        }

        fixed_matrix.construct_from_matrix(matrix);
        r = rhs;

        residual_out = ParallelSTL::vec_abs_max<RealType, VectorType>(r);

        if(fabs(residual_out) < tolerance_factor)
        {
            iterations_out = 0;
            return true;
        }

        RealType tol = tolerance_factor * residual_out;
        RealType rho = ParallelBLAS::vec_dot_product<RealType, VectorType>(r, r);

        if(fabs(rho) < tolerance_factor || isnan(rho))
        {
            iterations_out = 0;
            return true;
        }

        z = r;

        int iteration;

        for(iteration = 0; iteration < max_iterations; ++iteration)
        {
            multiply<MatrixType, VectorType>(fixed_matrix, z, s);
            RealType tmp = ParallelBLAS::vec_dot_product<RealType, VectorType>(s, z);


            if(fabs(tmp) < tolerance_factor || isnan(tmp))
            {
                iterations_out = iteration;
                return true;
            }

            RealType alpha = rho / tmp;

            tbb::parallel_invoke([&]
            {
                ParallelBLAS::add_scaled<RealType, VectorType>(alpha, z, result);
            },
                                 [&]
            {
                ParallelBLAS::add_scaled<RealType, VectorType>(-alpha, s, r);
            });

            residual_out = ParallelSTL::vec_abs_max<RealType, VectorType>(r);

            if(residual_out <= tol)
            {
                iterations_out = iteration + 1;
                return true;
            }

            RealType rho_new = ParallelBLAS::vec_dot_product<RealType, VectorType>(r, r);
            RealType beta = rho_new / rho;
            ParallelBLAS::scaled_add<RealType, VectorType>(beta, r, z);
            rho = rho_new;
        }


        iterations_out = iteration;
        return false;
    }


private:

    std::vector<VectorType> z, s, r; // temporary vectors for CG
    BlockFixedSparseMatrix<MatrixType> fixed_matrix; // used within loop

    // parameters
    RealType tolerance_factor;
    int max_iterations;
    bool zero_initial;
};

#endif
