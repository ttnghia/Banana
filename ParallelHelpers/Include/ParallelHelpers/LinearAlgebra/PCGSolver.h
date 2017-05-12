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

#include <ParallelHelpers/ParallelBLAS.h>
#include <ParallelHelpers/LinearAlgebra/SparseMatrix.h>
#include <BananaCore/MathHelpers.h>

enum Preconditioners
{
    IDENTITY,
    JACOBI,
    MICCL0,
    MICCL0_SYMMETRIC
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// A simple compressed sparse column data structure (with separate diagonal)
// for lower triangular matrices
template<class ScalarType>
struct SparseColumnLowerFactor
{
    UInt32                  m_Size;
    std::vector<ScalarType> m_InvDiag;  // reciprocals of diagonal elements
    std::vector<ScalarType> m_ColValue; // values below the diagonal, listed column by column
    std::vector<UInt32>     m_ColIndex; // a list of all row indices, for each column in turn
    std::vector<UInt32>     m_ColStart; // where each column begins in rowindex (plus an extra entry at the end, of #nonzeros)
    std::vector<ScalarType> m_aDiag;    // just used in factorization: minimum "safe" diagonal entry allowed

    explicit SparseColumnLowerFactor(UInt32 n_ = 0)
        : m_Size(n_), m_InvDiag(n_), m_ColStart(n_ + 1), m_aDiag(n_)
    {}

    void clear(void)
    {
        m_Size = 0;
        m_InvDiag.clear();
        m_ColValue.clear();
        m_ColIndex.clear();
        m_ColStart.clear();
        m_aDiag.clear();
    }

    void resize(UInt32 n_)
    {
        m_Size = n_;
        m_InvDiag.resize(m_Size);
        m_ColStart.resize(m_Size + 1);
        m_aDiag.resize(m_Size);
    }

    void write_matlab(std::ostream& output, const char* variable_name)
    {
        output << variable_name << "=sparse([";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            output << " " << i + 1;

            for(UInt32 j = m_ColStart[i]; j < m_ColStart[i + 1]; ++j)
            {
                output << " " << m_ColIndex[j] + 1;
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            output << " " << i + 1;

            for(UInt32 j = m_ColStart[i]; j < m_ColStart[i + 1]; ++j)
            {
                output << " " << i + 1;
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            output << " " << (m_InvDiag[i] != 0 ? 1 / m_InvDiag[i] : 0);

            for(UInt32 j = m_ColStart[i]; j < m_ColStart[i + 1]; ++j)
            {
                output << " " << m_ColValue[j];
            }
        }

        output << "], " << m_Size << ", " << m_Size << ");" << std::endl;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Incomplete Cholesky factorization, level zero, with option for modified version.
// Set modification_parameter between zero (regular incomplete Cholesky) and
// one (fully modified version), with values close to one usually giving the best
// results. The min_diagonal_ratio parameter is used to detect and correct
// problems in factorization: if a pivot is this much less than the diagonal
// entry from the original matrix, the original matrix entry is used instead.

template<class T>
void factor_modified_incomplete_cholesky0(const SparseMatrix<T>& matrix,
        SparseColumnLowerFactor<T>& factor,
        T modification_parameter = 0.97, T min_diagonal_ratio = 0.25)
{
    // first copy lower triangle of matrix into factor (Note: assuming A is symmetric of course!)
    factor.resize(matrix.m_Size);
    MathHelpers::zero(factor.m_InvDiag); // important: eliminate old values from previous solves!
    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);
    MathHelpers::zero(factor.m_aDiag);

    for(UInt32 i = 0; i < matrix.m_Size; ++i)
    {
        factor.m_ColStart[i] = (UInt32)factor.m_ColIndex.size();

        for(UInt32 j = 0; j < matrix.m_ColIndex[i].size(); ++j)
        {
            if(matrix.m_ColIndex[i][j] > i)
            {
                factor.m_ColIndex.push_back(matrix.m_ColIndex[i][j]);
                factor.m_ColValue.push_back(matrix.m_ColValue[i][j]);
            }
            else if(matrix.m_ColIndex[i][j] == i)
            {
                factor.m_InvDiag[i] = factor.m_aDiag[i] = matrix.m_ColValue[i][j];
            }
        }
    }


    factor.m_ColStart[matrix.m_Size] = (UInt32)factor.m_ColIndex.size();
    // now do the incomplete factorization (figure out numerical values)

    // MATLAB code:
    // L=tril(A);
    // for k=1:size(L,2)
    //   L(k,k)=sqrt(L(k,k));
    //   L(k+1:end,k)=L(k+1:end,k)/L(k,k);
    //   for j=find(L(:,k))'
    //     if j>k
    //       fullupdate=L(:,k)*L(j,k);
    //       incompleteupdate=fullupdate.*(A(:,j)~=0);
    //       missing=sum(fullupdate-incompleteupdate);
    //       L(j:end,j)=L(j:end,j)-incompleteupdate(j:end);
    //       L(j,j)=L(j,j)-omega*missing;
    //     end
    //   end
    // end


    for(UInt32 k = 0; k < matrix.m_Size; ++k)
    {
        if(factor.m_aDiag[k] == 0)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(factor.m_InvDiag[k] < min_diagonal_ratio * factor.m_aDiag[k])
        {
            factor.m_InvDiag[k] = 1 / sqrt(
                    factor.m_aDiag[k]); // drop to Gauss-Seidel here if the pivot looks dangerously small
        }
        else
        {
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_InvDiag[k]);
        }

        // finalize the k'th column L(:,k)
        for(UInt32 p = factor.m_ColStart[k]; p < factor.m_ColStart[k + 1]; ++p)
        {
            factor.m_ColValue[p] *= factor.m_InvDiag[k];
        }

        // incompletely eliminate L(:,k) from future columns, modifying diagonals
        for(UInt32 p = factor.m_ColStart[k]; p < factor.m_ColStart[k + 1]; ++p)
        {
            UInt32 j          = factor.m_ColIndex[p]; // work on column j
            T      multiplier = factor.m_ColValue[p];
            T      missing    = 0;
            UInt32 a          = factor.m_ColStart[k];
            // first look for contributions to missing from dropped entries above the diagonal in column j
            UInt32 b = 0;

            while(a < factor.m_ColStart[k + 1] && factor.m_ColIndex[a] < j)
            {
                // look for factor.rowindex[a] in matrix.index[j] starting at b
                while(b < matrix.m_ColIndex[j].size())
                {
                    if(matrix.m_ColIndex[j][b] < factor.m_ColIndex[a])
                    {
                        ++b;
                    }
                    else if(matrix.m_ColIndex[j][b] == factor.m_ColIndex[a])
                    {
                        break;
                    }
                    else
                    {
                        missing += factor.m_ColValue[a];
                        break;
                    }
                }

                ++a;
            }

            // adjust the diagonal j,j entry
            if(a < factor.m_ColStart[k + 1] && factor.m_ColIndex[a] == j)
            {
                factor.m_InvDiag[j] -= multiplier * factor.m_ColValue[a];
            }

            ++a;
            // and now eliminate from the nonzero entries below the diagonal in column j (or add to missing if we can't)
            b = factor.m_ColStart[j];

            while(a < factor.m_ColStart[k + 1] && b < factor.m_ColStart[j + 1])
            {
                if(factor.m_ColIndex[b] < factor.m_ColIndex[a])
                {
                    ++b;
                }
                else if(factor.m_ColIndex[b] == factor.m_ColIndex[a])
                {
                    factor.m_ColValue[b] -= multiplier * factor.m_ColValue[a];
                    ++a;
                    ++b;
                }
                else
                {
                    missing += factor.m_ColValue[a];
                    ++a;
                }
            }

            // and if there's anything left to do, add it to missing
            while(a < factor.m_ColStart[k + 1])
            {
                missing += factor.m_ColValue[a];
                ++a;
            }

            // and do the final diagonal adjustment from the missing entries
            factor.m_InvDiag[j] -= modification_parameter * multiplier * missing;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void factor_modified_incomplete_cholesky0_symmetry(const SparseMatrix<T>& matrix,
        SparseColumnLowerFactor<T>&                                       factor,
        T                                                                 min_diagonal_ratio = 0.25)
{
    // first copy lower triangle of matrix into factor (Note: assuming A is symmetric of course!)
    factor.resize(matrix.m_Size);
    MathHelpers::zero(factor.m_InvDiag); // important: eliminate old values from previous solves!
    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);
    MathHelpers::zero(factor.m_aDiag);

    for(UInt32 i = 0, is = matrix.m_Size; i < is; ++i)
    {
        factor.m_ColStart[i] = static_cast<UInt32>(factor.m_ColIndex.size());

        for(UInt32 j = 0, jend = static_cast<UInt32>(matrix.m_ColIndex[i].size()); j < jend; ++j)
        {
            if(matrix.m_ColIndex[i][j] > i)
            {
                factor.m_ColIndex.push_back(matrix.m_ColIndex[i][j]);
                factor.m_ColValue.push_back(matrix.m_ColValue[i][j]);
            }
            else if(matrix.m_ColIndex[i][j] == i)
            {
                factor.m_InvDiag[i] = factor.m_aDiag[i] = matrix.m_ColValue[i][j];
            }
        }
    }

    factor.m_ColStart[matrix.m_Size] = static_cast<UInt32>(factor.m_ColIndex.size());
    // now do the incomplete factorization (figure out numerical values)

    for(UInt32 k = 0, ks = matrix.m_Size; k < ks; ++k)
    {
        if(factor.m_aDiag[k] == 0)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(factor.m_InvDiag[k] < min_diagonal_ratio * factor.m_aDiag[k])
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_aDiag[k]);    // drop to Gauss-Seidel here if the pivot looks dangerously small
        else
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_InvDiag[k]);

        // finalize the k'th column L(:,k)
        for(UInt32 p = factor.m_ColStart[k], ps = factor.m_ColStart[k + 1]; p < ps; ++p)
            factor.m_ColValue[p] *= factor.m_InvDiag[k];


        for(UInt32 p = factor.m_ColStart[k], ps = factor.m_ColStart[k + 1]; p < ps; ++p)
        {
            UInt32 j          = factor.m_ColIndex[p]; // work on column j
            T      multiplier = factor.m_ColValue[p];

            factor.m_InvDiag[j] -= multiplier * factor.m_ColValue[p];

            UInt32 a  = p + 1;
            UInt32 b  = factor.m_ColStart[j];
            UInt32 as = factor.m_ColStart[k + 1];
            UInt32 bs = factor.m_ColStart[j + 1];

            while(a < as && b < bs)
            {
                if(factor.m_ColIndex[b] == factor.m_ColIndex[a])
                {
                    factor.m_ColValue[b] -= multiplier * factor.m_ColValue[a];
                    ++a;
                    ++b;
                }
                else if(factor.m_ColIndex[b] < factor.m_ColIndex[a])
                {
                    ++b;
                }
                else
                {
                    ++a;
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void factor_Identity(const SparseMatrix<T>& matrix, SparseColumnLowerFactor<T>& factor)
{
    factor.resize(matrix.m_Size);
    MathHelpers::zero(factor.m_InvDiag); // important: eliminate old values from previous solves!
    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);
    MathHelpers::zero(factor.m_aDiag);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            factor.m_ColStart[i] = 0;
            factor.m_InvDiag[i] = factor.m_aDiag[i] = 1.0;
        }
    }, ap); // end parallel_for

    factor.m_ColStart[matrix.m_Size] = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void factor_Jacobi(const SparseMatrix<T>& matrix, SparseColumnLowerFactor<T>& factor)
{
    // first copy lower triangle of matrix into factor (Note: assuming A is symmetric of course!)
    factor.resize(matrix.m_Size);
    MathHelpers::zero(factor.m_InvDiag); // important: eliminate old values from previous solves!
    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);
    MathHelpers::zero(factor.m_aDiag);

    //    for(UInt32 i = 0; i < matrix.size; ++i)
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            factor.m_ColStart[i] = 0;

            for(UInt32 j = 0; j < matrix.m_ColIndex[i].size(); ++j)
            {
                if(matrix.m_ColIndex[i][j] == i)
                {
                    factor.m_InvDiag[i] = factor.m_aDiag[i] = 1.0 / matrix.m_ColValue[i][j];
                }
            }
        }
    }, ap); // end parallel_for

    factor.m_ColStart[matrix.m_Size] = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Solution routines with lower triangular matrix.
// solve L*result=rhs
template<class T>
void solve_lower(const SparseColumnLowerFactor<T>& factor, const std::vector<T>& rhs, std::vector<T>& result)
{
    __NOODLE_ASSERT(factor.m_Size == rhs.size());
    __NOODLE_ASSERT(factor.m_Size == result.size());
    result = rhs;

    for(UInt32 i = 0, is = factor.m_Size; i < is; ++i)
    {
        result[i] *= factor.m_InvDiag[i];

        for(UInt32 j = factor.m_ColStart[i], jend = factor.m_ColStart[i + 1]; j < jend; ++j)
        {
            result[factor.m_ColIndex[j]] -= factor.m_ColValue[j] * result[i];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solve L^T*result=rhs
template<class T>
void solve_lower_transpose_in_place(const SparseColumnLowerFactor<T>& factor, std::vector<T>& x)
{
    __NOODLE_ASSERT(factor.m_Size == x.size());
    __NOODLE_ASSERT(factor.m_Size > 0);
    UInt32 i = factor.m_Size;

    do
    {
        --i;

        for(UInt32 j = factor.m_ColStart[i], jend = factor.m_ColStart[i + 1]; j < jend; ++j)
        {
            x[i] -= factor.m_ColValue[j] * x[factor.m_ColIndex[j]];
        }

        x[i] *= factor.m_InvDiag[i];
    } while(i != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Encapsulates the Conjugate Gradient algorithm with incomplete Cholesky
// factorization preconditioner.
template<class ScalarType>
class PCGSolver
{
public:
    PCGSolver(void)
    {
        setSolverParameters(1e-12, 100, 0.97, 0.25);
        setPreconditioners(IDENTITY);
    }

    void setSolverParameters(ScalarType tolerance_factor_, int max_iterations_,
            ScalarType modified_incomplete_cholesky_parameter_ = 0.97, ScalarType min_diagonal_ratio_ = 0.25)
    {
        m_ToleranceFactor = tolerance_factor_;

        if(m_ToleranceFactor < 1e-30)
        {
            m_ToleranceFactor = 1e-30;
        }

        m_MaxIterations                        = max_iterations_;
        modified_incomplete_cholesky_parameter = modified_incomplete_cholesky_parameter_;
        min_diagonal_ratio                     = min_diagonal_ratio_;
    }

    void setPreconditioners(Preconditioners precond)
    {
        m_Preconditioner = precond;
    }

    bool solve(const SparseMatrix<ScalarType>& matrix, const std::vector<ScalarType>& rhs, std::vector<ScalarType>& result, ScalarType& residual_out, int& iterations_out)
    {
        UInt32 n = matrix.m_Size;

        if(z.size() != n)
        {
            s.resize(n);
            z.resize(n);
            r.resize(n);
        }

        MathHelpers::zero(result);
        fixed_matrix.construct_from_matrix(matrix);
        r = rhs;

        residual_out = ParallelBLAS::maxAbs<ScalarType, ScalarType>(r);

        if(residual_out == 0)
        {
            iterations_out = 0;
            return true;
        }

        double tol = m_ToleranceFactor * residual_out;
        double rho = ParallelBLAS::dotProduct<ScalarType, ScalarType>(r, r);

        if(rho == 0 || rho != rho)
        {
            iterations_out = 0;
            return false;
        }

        z = r;

        for(int iteration = 0; iteration < m_MaxIterations; ++iteration)
        {
            multiply(fixed_matrix, z, s);
            double alpha = rho / ParallelBLAS::dotProduct<ScalarType, ScalarType>(s, z);
            tbb::parallel_invoke(
                    [&]
            {
                ParallelBLAS::addScaled<ScalarType, ScalarType>(alpha, z, result);
            },
                    [&]
            {
                ParallelBLAS::addScaled<ScalarType, ScalarType>(-alpha, s, r);
            });

            residual_out = ParallelBLAS::maxAbs(r);

            if(residual_out <= tol)
            {
                iterations_out = iteration + 1;
                return true;
            }

            double rho_new = ParallelBLAS::dotProduct<ScalarType, ScalarType>(r, r);
            double beta    = rho_new / rho;
            ParallelBLAS::scaledAdd<ScalarType, ScalarType>(beta, r, z);
            rho = rho_new;
        }


        iterations_out = m_MaxIterations;
        return false;
    }


    bool solve_precond(const SparseMatrix<ScalarType>& matrix, const std::vector<ScalarType>& rhs, std::vector<ScalarType>& result, ScalarType& residual_out, int& iterations_out)
    {
        UInt32 n = matrix.m_Size;

        if(z.size() != n)
        {
            s.resize(n);
            z.resize(n);
            r.resize(n);
        }


        MathHelpers::zero(result);
        fixed_matrix.construct_from_matrix(matrix);

        multiply(fixed_matrix, result, s);
        r = rhs;
        ParallelBLAS::addScaled<ScalarType, ScalarType>(-1.0, s, r);



        residual_out = ParallelBLAS::maxAbs<ScalarType, ScalarType>(r);

        if(residual_out == 0)
        {
            iterations_out = 0;
            return true;
        }

        double tol = m_ToleranceFactor * residual_out;

        form_preconditioner(matrix);
        apply_preconditioner(r, z);


        double rho = ParallelBLAS::dotProduct<ScalarType, ScalarType>(z, r);

        if(rho == 0 || rho != rho)
        {
            iterations_out = 0;
            return false;
        }

        s = z;

        for(int iteration = 0; iteration < m_MaxIterations; ++iteration)
        {
            multiply(fixed_matrix, s, z);
            double alpha = rho / ParallelBLAS::dotProduct<ScalarType, ScalarType>(s, z);
            tbb::parallel_invoke([&]
            {
                ParallelBLAS::addScaled<ScalarType, ScalarType>(alpha, s, result);
            },
                    [&]
            {
                ParallelBLAS::addScaled<ScalarType, ScalarType>(-alpha, z, r);
            });

            residual_out = ParallelBLAS::maxAbs<ScalarType, ScalarType>(r);

            if(residual_out <= tol)
            {
                iterations_out = iteration + 1;
                return true;
            }

            apply_preconditioner(r, z);

            double rho_new = ParallelBLAS::dotProduct<ScalarType, ScalarType>(z, r);
            double beta    = rho_new / rho;
            ParallelBLAS::addScaled<ScalarType, ScalarType>(beta, s, z);
            s.swap(z); // s=beta*s+z
            rho = rho_new;
        }


        iterations_out = m_MaxIterations;
        return false;
    }

protected:

    // internal structures
    SparseColumnLowerFactor<ScalarType> ic_factor;    // modified incomplete cholesky factor
    std::vector<ScalarType>             z, s, r;      // temporary vectors for PCG
    FixedSparseMatrix<ScalarType>       fixed_matrix; // used within loop

    // parameters
    int        m_MaxIterations;
    ScalarType m_ToleranceFactor;
    ScalarType modified_incomplete_cholesky_parameter;
    ScalarType min_diagonal_ratio;

    Preconditioners m_Preconditioner;

    void form_preconditioner(const SparseMatrix<ScalarType>& matrix)
    {
        switch(m_Preconditioner)
        {
            case IDENTITY:
                factor_Identity(matrix, ic_factor);
                break;

            case JACOBI:
                factor_Jacobi(matrix, ic_factor);
                break;

            case MICCL0:
                factor_modified_incomplete_cholesky0(matrix, ic_factor);
                break;

            case MICCL0_SYMMETRIC:
                factor_modified_incomplete_cholesky0_symmetry(matrix, ic_factor);
                break;
        }
    }

    void apply_preconditioner(const std::vector<ScalarType>& x, std::vector<ScalarType>& result)
    {
        solve_lower(ic_factor, x, result);
        solve_lower_transpose_in_place(ic_factor, result);
    }
};
