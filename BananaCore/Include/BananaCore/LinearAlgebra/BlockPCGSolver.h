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

#include <Noodle/Core/Parallel/ParallelBLAS.h>
#include <Noodle/Core/LinearAlgebra/SparseMatrix/BlockSparseMatrix.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class ScalarType>
class BlockPCGSolver
{
public:
    BlockPCGSolver()
    {
        set_solver_parameters(1e-20, 10000);
        m_bZeroInitial = true;
    }

    void set_solver_parameters(ScalarType tolerance_factor, int max_iterations)
    {
        m_ToleranceFactor = tolerance_factor;

        if(m_ToleranceFactor < 1e-30)
        {
            m_ToleranceFactor = 1e-30;
        }

        m_MaxIterations = max_iterations;
    }

    void setZeroInitial(bool zero_initial)
    {
        m_bZeroInitial = zero_initial;
    }

    void enableZeroInitial()
    {
        m_bZeroInitial = true;
    }

    void disableZeroInitial()
    {
        m_bZeroInitial = false;
    }

    bool solve(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result, ScalarType& residual_out, int& iterations_out)
    {
        const UInt32 n = matrix.size;

        if(z.size() != n)
        {
            s.resize(n);
            z.resize(n);
            r.resize(n);
        }

        // zero out the result
        if(m_bZeroInitial)
        {
            for(size_t i = 0; i < result.size(); ++i)
            {
                result[i] = VectorType(0);
            }
        }

        m_FixedMatrix.construct_from_matrix(matrix);
        r = rhs;

        residual_out = ParallelBLAS::maxAbs<ScalarType, VectorType>(r);

        if(fabs(residual_out) < 1e-20)
        {
            iterations_out = 0;
            return true;
        }

        ScalarType tol = m_ToleranceFactor * residual_out;
        ScalarType rho = ParallelBLAS::dotProduct<ScalarType, VectorType>(r, r);

        if(fabs(rho) < 1e-20 || isnan(rho))
        {
            iterations_out = 0;
            return true;
        }

        z = r;

        for(int iteration = 0; iteration < m_MaxIterations; ++iteration)
        {
            multiply<MatrixType, VectorType>(m_FixedMatrix, z, s);
            ScalarType tmp = ParallelBLAS::dotProduct<ScalarType, VectorType>(s, z);

            if(fabs(tmp) < 1e-20 || isnan(tmp))
            {
                iterations_out = iteration;
                return true;
            }

            ScalarType alpha = rho / tmp;

            tbb::parallel_invoke(
                [&]
            {
                ParallelBLAS::addScaled<ScalarType, VectorType>(alpha, z, result);
            },
                [&]
            {
                ParallelBLAS::addScaled<ScalarType, VectorType>(-alpha, s, r);
            });

            residual_out = ParallelBLAS::maxAbs<ScalarType, VectorType>(r);

            if(residual_out < tol)
            {
                iterations_out = iteration + 1;
                return true;
            }

            ScalarType rho_new = ParallelBLAS::dotProduct<ScalarType, VectorType>(r, r);
            ScalarType beta = rho_new / rho;
            ParallelBLAS::scaledAdd<ScalarType, VectorType>(beta, r, z);
            rho = rho_new;
        }

        iterations_out = m_MaxIterations;
        return false;
    }

    bool solve_precond(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result, ScalarType& residual_out, int& iterations_out)
    {
        UInt32 n = matrix.size;

        if(z.size() != n)
        {
            s.resize(n);
            z.resize(n);
            r.resize(n);
        }

        // zero out the result
        if(m_bZeroInitial)
        {
            for(size_t i = 0; i < result.size(); ++i)
            {
                result[i] = VectorType(0);
            }
        }

        m_FixedMatrix.construct_from_matrix(matrix);
        r = rhs;

        multiply(m_FixedMatrix, result, s);
        ParallelBLAS::addScaled<ScalarType, VectorType>(-1.0, s, r);

        residual_out = ParallelBLAS::maxAbs<ScalarType, VectorType>(r);

        if(fabs(residual_out) < 1e-20)
        {
            iterations_out = 0;
            return true;
        }

        ScalarType tol = m_ToleranceFactor * residual_out;

        form_preconditioner(matrix, m_Preconditioner);
        apply_preconditioner(r, z);

        ScalarType rho = ParallelBLAS::dotProduct<ScalarType, VectorType>(z, r);

        if(fabs(rho) < 1e-20 || isnan(rho))
        {
            iterations_out = 0;
            return true;
        }

        s = z;

        for(int iteration = 0; iteration < m_MaxIterations; ++iteration)
        {
            multiply(m_FixedMatrix, s, z);
            ScalarType alpha = rho / ParallelBLAS::dotProduct<ScalarType, VectorType>(s, z);
            tbb::parallel_invoke(
                [&]
            {
                ParallelBLAS::addScaled<ScalarType, VectorType>(alpha, s, result);
            },
                [&]
            {
                ParallelBLAS::addScaled<ScalarType, VectorType>(-alpha, z, r);
            });

            residual_out = ParallelBLAS::maxAbs<ScalarType, VectorType>(r);

            if(residual_out <= tol)
            {
                iterations_out = iteration + 1;
                return true;
            }

            apply_preconditioner(r, z);

            ScalarType rho_new = ParallelBLAS::dotProduct<ScalarType, VectorType>(z, r);
            ScalarType beta = rho_new / rho;
            ParallelBLAS::addScaled<ScalarType, VectorType>(beta, s, z);
            s.swap(z); // s=beta*s+z
            rho = rho_new;
        }

        iterations_out = m_MaxIterations;
        return false;
    }

private:
    void form_preconditioner(const BlockSparseMatrix<MatrixType>& matrix, std::vector<MatrixType>& diag)
    {
        diag.resize(matrix.size);

        static tbb::affinity_partitioner ap;
        tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size), [&](tbb::blocked_range<UInt32> r)
        {
            for(UInt32 i = r.begin(); i != r.end(); ++i)
            {
                const UInt32 rowSize = static_cast<UInt32>(matrix.m_ColIndex[i].size());

                for(UInt32 j = 0; j < rowSize; ++j)
                {
                    if(matrix.m_ColIndex[i][j] == i)
                    {
                        diag[i] = glm::inverse(matrix.m_ColValue[i][j]);
                        //                        diag[i] = 1. / matrix.m_ColValue[i][j];
                        break;
                    }
                }
            }
        }, ap); // end parallel_for
    }

    void apply_preconditioner(const std::vector<VectorType>& x, std::vector<VectorType>& result)
    {
        for(size_t i = 0; i < x.size(); ++i)
        {
            result[i] = m_Preconditioner[i] * x[i];
        }
    }

    std::vector<VectorType>            z, s, r;          // temporary vectors for CG
    std::vector<MatrixType>            m_Preconditioner;
    BlockFixedSparseMatrix<MatrixType> m_FixedMatrix;    // used within loop

                                                         // parameters
    ScalarType m_ToleranceFactor;
    int        m_MaxIterations;
    bool       m_bZeroInitial;
};
