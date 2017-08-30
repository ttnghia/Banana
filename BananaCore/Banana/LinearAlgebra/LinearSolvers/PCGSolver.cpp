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

#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/Utils/STLHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseColumnLowerFactor::clear(void)
{
    nRows = 0;
    invDiag.clear();
    colValue.clear();
    colIndex.clear();
    colStart.clear();
    aDiag.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseColumnLowerFactor::resize(UInt newSize)
{
    nRows = newSize;
    invDiag.resize(nRows);
    aDiag.resize(nRows);
    colStart.resize(nRows + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PCGSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::setSolverParameters(Real tolerancem_ICCPrecond, int maxIterations, Real MICCL0Param /*= 0.97*/, Real minDiagonalRatio /*= 0.25*/)
{
    m_ToleranceFactor  = fmax(tolerancem_ICCPrecond, Real(1e-30));
    m_MaxIterations    = maxIterations;
    m_MICCL0Param      = MICCL0Param;
    m_MinDiagonalRatio = minDiagonalRatio;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::resize(UInt size)
{
    s.resize(size);
    z.resize(size);
    r.resize(size);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PCGSolver::solve(const SparseMatrix& matrix, const Vec_Real& rhs, Vec_Real& result, Real& residual_out, UInt& iterations_out)
{
    resize(matrix.nRows);

    result.resize(matrix.nRows);
    if(m_bZeroInitial)
        result.assign(result.size(), 0);

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    residual_out = ParallelSTL::maxAbs<Real>(r);

    if(residual_out < Tiny)
    {
        iterations_out = 0;
        return true;
    }

    Real tol = m_ToleranceFactor * residual_out;
    Real rho = ParallelBLAS::dotProductScalar<Real>(r, r);

    if(rho < Tiny || isnan(rho))
    {
        iterations_out = 0;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    z = r;
    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        FixedSparseMatrix::multiply(m_FixedSparseMatrix, z, s);
        Real alpha = rho / ParallelBLAS::dotProductScalar<Real>(s, z);
        ParallelBLAS::addScaled<Real, Real>(alpha,  z, result);
        ParallelBLAS::addScaled<Real, Real>(-alpha, s, r);

        residual_out = ParallelSTL::maxAbs<Real>(r);
        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        Real rho_new = ParallelBLAS::dotProductScalar<Real>(r, r);
        Real beta    = rho_new / rho;
        ParallelBLAS::scaledAdd<Real, Real>(beta, r, z);
        rho = rho_new;
    }

    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PCGSolver::solve_precond(const SparseMatrix& matrix, const Vec_Real& rhs, Vec_Real& result, Real& residual_out, UInt& iterations_out)
{
    resize(matrix.nRows);

    result.resize(matrix.nRows);
    if(m_bZeroInitial)
        result.assign(result.size(), 0);

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    residual_out = ParallelSTL::maxAbs<Real>(r);
    if(residual_out < Tiny)
    {
        iterations_out = 0;
        return true;
    }

    Real tol = m_ToleranceFactor * residual_out;
    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    Real rho = ParallelBLAS::dotProductScalar<Real>(z, r);
    if(rho < Tiny || isnan(rho))
    {
        iterations_out = 0;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    s = z;
    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        FixedSparseMatrix::multiply(m_FixedSparseMatrix, s, z);
        Real alpha = rho / ParallelBLAS::dotProductScalar<Real>(s, z);
        ParallelBLAS::addScaled<Real, Real>(alpha,  s, result);
        ParallelBLAS::addScaled<Real, Real>(-alpha, z, r);

        residual_out = ParallelSTL::maxAbs<Real>(r);
        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);
        Real rho_new = ParallelBLAS::dotProductScalar<Real>(z, r);
        Real beta    = rho_new / rho;
        ParallelBLAS::addScaled<Real, Real>(beta, s, z);
        s.swap(z); // s=beta*s+z
        rho = rho_new;
    }

    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::formPreconditioner(const SparseMatrix& matrix)
{
    switch(m_PreconditionerType)
    {
        case Preconditioner::JACOBI:
            formPreconditioner_Jacobi(matrix);
            break;

        case Preconditioner::MICCL0:
            formPreconditioner_MICC0L0(matrix);
            break;

        case Preconditioner::MICCL0_SYMMETRIC:
            formPreconditioner_Symmetric_MICC0L0(matrix);
            break;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::applyPreconditioner(const Vec_Real& x, Vec_Real& result)
{
    if(m_PreconditionerType != Preconditioner::JACOBI)
    {
        solveLower(x, result);
        solveLower_TransposeInPlace(result);
    }
    else
    {
        applyJacobiPreconditioner(x, result);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::applyJacobiPreconditioner(const Vec_Real& x, Vec_Real& result)
{
    ParallelFuncs::parallel_for<size_t>(0, x.size(),
                                        [&](size_t i)
                                        {
                                            result[i] = m_JacobiPrecond[i] * x[i];
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Solution routines with lower triangular matrix.
// solve L*result=rhs
void PCGSolver::solveLower(const Vec_Real& rhs, Vec_Real& result)
{
    result = rhs;

    for(UInt i = 0, iEnd = m_ICCPrecond.nRows; i < iEnd; ++i)
    {
        result[i] *= m_ICCPrecond.invDiag[i];

        for(UInt j = m_ICCPrecond.colStart[i], jEnd = m_ICCPrecond.colStart[i + 1]; j < jEnd; ++j)
            result[m_ICCPrecond.colIndex[j]] -= m_ICCPrecond.colValue[j] * result[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solve L^T*result=rhs
void PCGSolver::solveLower_TransposeInPlace(Vec_Real& x)
{
    UInt i = m_ICCPrecond.nRows;

    do
    {
        --i;
        for(UInt j = m_ICCPrecond.colStart[i], jEnd = m_ICCPrecond.colStart[i + 1]; j < jEnd; ++j)
            x[i] -= m_ICCPrecond.colValue[j] * x[m_ICCPrecond.colIndex[j]];

        x[i] *= m_ICCPrecond.invDiag[i];
    } while(i > 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::formPreconditioner_Jacobi(const SparseMatrix& matrix)
{
    m_JacobiPrecond.resize(matrix.nRows);
    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          UInt k = 0;
                                          if(STLHelpers::Sorted::contain(matrix.colIndex[i], i, k))
                                          {
                                              m_JacobiPrecond[i] = Real(1.0) / matrix.colValue[i][k];
                                          }
                                          else
                                          {
                                              m_JacobiPrecond[i] = 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Incomplete Cholesky m_ICCPrecondization, level zero, with option for modified version.
// Set MICCL0Param between zero (regular incomplete Cholesky) and
// one (fully modified version), with values close to one usually giving the best
// results. The minDiagonalRatio parameter is used to detect and correct
// problems in m_ICCPrecondization: if a pivot is this much less than the diagonal
// entry from the original matrix, the original matrix entry is used instead.
void PCGSolver::formPreconditioner_MICC0L0(const SparseMatrix& matrix, Real MICCL0Param /*= 0.97*/, Real minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into m_ICCPrecond (Note: assuming A is symmetric of course!)
    m_ICCPrecond.resize(matrix.nRows);
    for(size_t i = 0, iEnd = m_ICCPrecond.invDiag.size(); i < iEnd; ++i)
    {
        m_ICCPrecond.invDiag[i] = 0;
        m_ICCPrecond.aDiag[i]   = 0;
    }

    m_ICCPrecond.colValue.resize(0);
    m_ICCPrecond.colIndex.resize(0);

    for(UInt i = 0, iEnd = matrix.nRows; i < iEnd; ++i)
    {
        m_ICCPrecond.colStart[i] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

        for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j)
        {
            if(matrix.colIndex[i][j] > i)
            {
                m_ICCPrecond.colIndex.push_back(matrix.colIndex[i][j]);
                m_ICCPrecond.colValue.push_back(matrix.colValue[i][j]);
            }
            else if(matrix.colIndex[i][j] == i)
            {
                m_ICCPrecond.invDiag[i] = m_ICCPrecond.aDiag[i] = matrix.colValue[i][j];
            }
        }
    }

    m_ICCPrecond.colStart[matrix.nRows] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

    for(UInt k = 0, kEnd = matrix.nRows; k < kEnd; ++k)
    {
        if(m_ICCPrecond.aDiag[k] < Tiny)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(m_ICCPrecond.invDiag[k] < minDiagonalRatio * m_ICCPrecond.aDiag[k])
        {
            m_ICCPrecond.invDiag[k] = Real(1.0) / sqrt(m_ICCPrecond.aDiag[k]); // drop to Gauss-Seidel here if the pivot looks dangerously small
        }
        else
        {
            m_ICCPrecond.invDiag[k] = Real(1.0) / sqrt(m_ICCPrecond.invDiag[k]);
        }

        // finalize the k'th column L(:,k)
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p)
        {
            m_ICCPrecond.colValue[p] *= m_ICCPrecond.invDiag[k];
        }

        // incompletely eliminate L(:,k) from future columns, modifying diagonals
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p)
        {
            UInt j          = m_ICCPrecond.colIndex[p]; // work on column j
            Real multiplier = m_ICCPrecond.colValue[p];
            Real missing    = 0;
            UInt a          = m_ICCPrecond.colStart[k];
            // first look for contributions to missing from dropped entries above the diagonal in column j
            UInt b = 0;

            while(a < m_ICCPrecond.colStart[k + 1] && m_ICCPrecond.colIndex[a] < j)
            {
                // look for m_ICCPrecond.rowindex[a] in matrix.index[j] starting at b
                while(b < matrix.colIndex[j].size())
                {
                    if(matrix.colIndex[j][b] < m_ICCPrecond.colIndex[a])
                    {
                        ++b;
                    }
                    else if(matrix.colIndex[j][b] == m_ICCPrecond.colIndex[a])
                    {
                        break;
                    }
                    else
                    {
                        missing += m_ICCPrecond.colValue[a];
                        break;
                    }
                }

                ++a;
            }

            // adjust the diagonal j,j entry
            if(a < m_ICCPrecond.colStart[k + 1] && m_ICCPrecond.colIndex[a] == j)
            {
                m_ICCPrecond.invDiag[j] -= multiplier * m_ICCPrecond.colValue[a];
            }

            ++a;
            // and now eliminate from the nonzero entries below the diagonal in column j (or add to missing if we can't)
            b = m_ICCPrecond.colStart[j];

            while(a < m_ICCPrecond.colStart[k + 1] && b < m_ICCPrecond.colStart[j + 1])
            {
                if(m_ICCPrecond.colIndex[b] < m_ICCPrecond.colIndex[a])
                {
                    ++b;
                }
                else if(m_ICCPrecond.colIndex[b] == m_ICCPrecond.colIndex[a])
                {
                    m_ICCPrecond.colValue[b] -= multiplier * m_ICCPrecond.colValue[a];
                    ++a;
                    ++b;
                }
                else
                {
                    missing += m_ICCPrecond.colValue[a];
                    ++a;
                }
            }

            // and if there's anything left to do, add it to missing
            while(a < m_ICCPrecond.colStart[k + 1])
            {
                missing += m_ICCPrecond.colValue[a];
                ++a;
            }

            // and do the final diagonal adjustment from the missing entries
            m_ICCPrecond.invDiag[j] -= MICCL0Param * multiplier * missing;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCGSolver::formPreconditioner_Symmetric_MICC0L0(const SparseMatrix& matrix, Real minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into m_ICCPrecond (Note: assuming A is symmetric of course!)
    m_ICCPrecond.resize(matrix.nRows);
    for(size_t i = 0, iEnd = m_ICCPrecond.invDiag.size(); i < iEnd; ++i)
    {
        m_ICCPrecond.invDiag[i] = 0;
        m_ICCPrecond.aDiag[i]   = 0;
    }

    m_ICCPrecond.colValue.resize(0);
    m_ICCPrecond.colIndex.resize(0);

    for(UInt i = 0, iEnd = matrix.nRows; i < iEnd; ++i)
    {
        m_ICCPrecond.colStart[i] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

        for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j)
        {
            if(matrix.colIndex[i][j] > i)
            {
                m_ICCPrecond.colIndex.push_back(matrix.colIndex[i][j]);
                m_ICCPrecond.colValue.push_back(matrix.colValue[i][j]);
            }
            else if(matrix.colIndex[i][j] == i)
            {
                m_ICCPrecond.invDiag[i] = m_ICCPrecond.aDiag[i] = matrix.colValue[i][j];
            }
        }
    }

    m_ICCPrecond.colStart[matrix.nRows] = static_cast<UInt>(m_ICCPrecond.colIndex.size());
    // now do the incomplete m_ICCPrecondization (figure out numerical values)

    for(UInt k = 0, kEnd = matrix.nRows; k < kEnd; ++k)
    {
        if(m_ICCPrecond.aDiag[k] < Tiny)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(m_ICCPrecond.invDiag[k] < minDiagonalRatio * m_ICCPrecond.aDiag[k])
            m_ICCPrecond.invDiag[k] = Real(1.0) / sqrt(m_ICCPrecond.aDiag[k]);    // drop to Gauss-Seidel here if the pivot looks dangerously small
        else
            m_ICCPrecond.invDiag[k] = Real(1.0) / sqrt(m_ICCPrecond.invDiag[k]);

        // finalize the k'th column L(:,k)
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p)
            m_ICCPrecond.colValue[p] *= m_ICCPrecond.invDiag[k];


        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p)
        {
            UInt j          = m_ICCPrecond.colIndex[p]; // work on column j
            Real multiplier = m_ICCPrecond.colValue[p];

            m_ICCPrecond.invDiag[j] -= multiplier * m_ICCPrecond.colValue[p];

            UInt a  = p + 1;
            UInt b  = m_ICCPrecond.colStart[j];
            UInt as = m_ICCPrecond.colStart[k + 1];
            UInt bs = m_ICCPrecond.colStart[j + 1];

            while(a < as && b < bs)
            {
                if(m_ICCPrecond.colIndex[b] == m_ICCPrecond.colIndex[a])
                {
                    m_ICCPrecond.colValue[b] -= multiplier * m_ICCPrecond.colValue[a];
                    ++a;
                    ++b;
                }
                else if(m_ICCPrecond.colIndex[b] < m_ICCPrecond.colIndex[a])
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
} // end namespace Banana
