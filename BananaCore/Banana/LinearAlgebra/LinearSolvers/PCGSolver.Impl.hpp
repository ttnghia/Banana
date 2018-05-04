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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseColumnLowerFactor<RealType>::clear(void)
{
    nRows = 0;
    invDiag.clear();
    colValue.clear();
    colIndex.clear();
    colStart.clear();
    aDiag.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseColumnLowerFactor<RealType>::reserve(UInt size)
{
    invDiag.reserve(size);
    aDiag.reserve(size);
    colStart.reserve(size + 1);
}

template<class RealType>
void SparseColumnLowerFactor<RealType>::resize(UInt newSize)
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
template<class RealType>
void PCGSolver<RealType>::setSolverParameters(RealType tolerancem_ICCPrecond, int maxIterations, RealType MICCL0Param /*= 0.97*/, RealType minDiagonalRatio /*= 0.25*/)
{
    m_ToleranceFactor  = fmax(tolerancem_ICCPrecond, RealType(1e-30));
    m_MaxIterations    = maxIterations;
    m_MICCL0Param      = MICCL0Param;
    m_MinDiagonalRatio = minDiagonalRatio;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::reserve(UInt size)
{
    s.reserve(size);
    z.reserve(size);
    r.reserve(size);

    m_FixedSparseMatrix.reserve(size);
    m_ICCPrecond.reserve(size);
}

template<class RealType>
void PCGSolver<RealType>::resize(UInt size)
{
    s.resize(size);
    z.resize(size);
    r.resize(size);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool PCGSolver<RealType>::solve(const SparseMatrix<RealType>& matrix, const Vector<RealType>& rhs, Vector<RealType>& result)
{
    resize(matrix.nRows);

    result.resize(matrix.nRows);
    if(m_bZeroInitial) {
        result.assign(result.size(), 0);
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    m_OutResidual = ParallelSTL::maxAbs<RealType>(r);

    if(m_OutResidual < std::numeric_limits<RealType>::min()) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;
    RealType rho = ParallelBLAS::dotProduct<RealType>(r, r);

    if(rho < std::numeric_limits<RealType>::min() || isnan(rho)) {
        m_OutIterations = 0;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    z = r;
    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedSparseMatrix<RealType>::multiply(m_FixedSparseMatrix, z, s);
        RealType tmp = ParallelBLAS::dotProduct<RealType>(s, z);
        if(tmp < std::numeric_limits<RealType>::min()) {
            m_OutIterations = iteration + 1;
            return true;
        }
        RealType alpha = rho / tmp;
        ParallelBLAS::addScaled<RealType>(alpha,  z, result);
        ParallelBLAS::addScaled<RealType>(-alpha, s, r);

        m_OutResidual = ParallelSTL::maxAbs<RealType>(r);
        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        RealType rho_new = ParallelBLAS::dotProduct<RealType>(r, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::scaledAdd<RealType, RealType>(beta, r, z);
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool PCGSolver<RealType>::solve_precond(const SparseMatrix<RealType>& matrix, const Vector<RealType>& rhs, Vector<RealType>& result)
{
    resize(matrix.nRows);

    result.resize(matrix.nRows);
    if(m_bZeroInitial) {
        result.assign(result.size(), 0);
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    m_OutResidual = ParallelSTL::maxAbs<RealType>(r);
    if(m_OutResidual < std::numeric_limits<RealType>::min()) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;
    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    RealType rho = ParallelBLAS::dotProduct<RealType>(z, r);
    if(rho < std::numeric_limits<RealType>::min() || isnan(rho)) {
        m_OutIterations = 0;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    s = z;
    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedSparseMatrix<RealType>::multiply(m_FixedSparseMatrix, s, z);
        RealType tmp = ParallelBLAS::dotProduct<RealType>(s, z);
        if(tmp < std::numeric_limits<RealType>::min()) {
            m_OutIterations = iteration + 1;
            return true;
        }
        RealType alpha = rho / tmp;
        ParallelBLAS::addScaled<RealType>(alpha,  s, result);
        ParallelBLAS::addScaled<RealType>(-alpha, z, r);

        m_OutResidual = ParallelSTL::maxAbs<RealType>(r);
        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);
        RealType rho_new = ParallelBLAS::dotProduct<RealType>(z, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::addScaled<RealType, RealType>(beta, s, z);
        s.swap(z);                 // s=beta*s+z
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner(const SparseMatrix<RealType>& matrix)
{
    switch(m_PreconditionerType) {
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
template<class RealType>
void PCGSolver<RealType>::applyPreconditioner(const Vector<RealType>& x, Vector<RealType>& result)
{
    if(m_PreconditionerType != Preconditioner::JACOBI) {
        solveLower(x, result);
        solveLower_TransposeInPlace(result);
    } else {
        applyJacobiPreconditioner(x, result);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::applyJacobiPreconditioner(const Vector<RealType>& x, Vector<RealType>& result)
{
    Scheduler::parallel_for<size_t>(0, x.size(),
                                    [&](size_t i)
                                    {
                                        result[i] = m_JacobiPrecond[i] * x[i];
                                    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Solution routines with lower triangular matrix.
// solve L*result=rhs
template<class RealType>
void PCGSolver<RealType>::solveLower(const Vector<RealType>& rhs, Vector<RealType>& result)
{
    result = rhs;

    for(UInt i = 0, iEnd = m_ICCPrecond.nRows; i < iEnd; ++i) {
        result[i] *= m_ICCPrecond.invDiag[i];

        for(UInt j = m_ICCPrecond.colStart[i], jEnd = m_ICCPrecond.colStart[i + 1]; j < jEnd; ++j) {
            result[m_ICCPrecond.colIndex[j]] -= m_ICCPrecond.colValue[j] * result[i];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solve L^T*result=rhs
template<class RealType>
void PCGSolver<RealType>::solveLower_TransposeInPlace(Vector<RealType>& x)
{
    UInt i = m_ICCPrecond.nRows;

    do {
        --i;
        for(UInt j = m_ICCPrecond.colStart[i], jEnd = m_ICCPrecond.colStart[i + 1]; j < jEnd; ++j) {
            x[i] -= m_ICCPrecond.colValue[j] * x[m_ICCPrecond.colIndex[j]];
        }

        x[i] *= m_ICCPrecond.invDiag[i];
    } while(i > 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_Jacobi(const SparseMatrix<RealType>& matrix)
{
    m_JacobiPrecond.resize(matrix.nRows);
    Scheduler::parallel_for<UInt>(0, matrix.nRows,
                                  [&](UInt i)
                                  {
                                      UInt k = 0;
                                      if(STLHelpers::Sorted::contain(matrix.colIndex[i], i, k)) {
                                          m_JacobiPrecond[i] = RealType(1.0) / matrix.colValue[i][k];
                                      } else {
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
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_MICC0L0(const SparseMatrix<RealType>& matrix, RealType MICCL0Param /*= 0.97*/, RealType minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into m_ICCPrecond (Note: assuming A is symmetric of course!)
    m_ICCPrecond.resize(matrix.nRows);
    for(size_t i = 0, iEnd = m_ICCPrecond.invDiag.size(); i < iEnd; ++i) {
        m_ICCPrecond.invDiag[i] = 0;
        m_ICCPrecond.aDiag[i]   = 0;
    }

    m_ICCPrecond.colValue.resize(0);
    m_ICCPrecond.colIndex.resize(0);

    for(UInt i = 0, iEnd = matrix.nRows; i < iEnd; ++i) {
        m_ICCPrecond.colStart[i] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

        for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j) {
            if(matrix.colIndex[i][j] > i) {
                m_ICCPrecond.colIndex.push_back(matrix.colIndex[i][j]);
                m_ICCPrecond.colValue.push_back(matrix.colValue[i][j]);
            } else if(matrix.colIndex[i][j] == i) {
                m_ICCPrecond.invDiag[i] = m_ICCPrecond.aDiag[i] = matrix.colValue[i][j];
            }
        }
    }

    m_ICCPrecond.colStart[matrix.nRows] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

    for(UInt k = 0, kEnd = matrix.nRows; k < kEnd; ++k) {
        if(m_ICCPrecond.aDiag[k] < std::numeric_limits<RealType>::min()) {
            continue;                            // null row/column
        }

        // figure out the final L(k,k) entry
        if(m_ICCPrecond.invDiag[k] < minDiagonalRatio * m_ICCPrecond.aDiag[k]) {
            m_ICCPrecond.invDiag[k] = RealType(1.0) / sqrt(m_ICCPrecond.aDiag[k]);                         // drop to Gauss-Seidel here if the pivot looks dangerously small
        } else {
            m_ICCPrecond.invDiag[k] = RealType(1.0) / sqrt(m_ICCPrecond.invDiag[k]);
        }

        // finalize the k'th column L(:,k)
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p) {
            m_ICCPrecond.colValue[p] *= m_ICCPrecond.invDiag[k];
        }

        // incompletely eliminate L(:,k) from future columns, modifying diagonals
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p) {
            UInt     j          = m_ICCPrecond.colIndex[p];    // work on column j
            RealType multiplier = m_ICCPrecond.colValue[p];
            RealType missing    = 0;
            UInt     a          = m_ICCPrecond.colStart[k];
            // first look for contributions to missing from dropped entries above the diagonal in column j
            UInt b = 0;

            while(a < m_ICCPrecond.colStart[k + 1] && m_ICCPrecond.colIndex[a] < j) {
                // look for m_ICCPrecond.rowindex[a] in matrix.index[j] starting at b
                while(b < matrix.colIndex[j].size()) {
                    if(matrix.colIndex[j][b] < m_ICCPrecond.colIndex[a]) {
                        ++b;
                    } else if(matrix.colIndex[j][b] == m_ICCPrecond.colIndex[a]) {
                        break;
                    } else {
                        missing += m_ICCPrecond.colValue[a];
                        break;
                    }
                }

                ++a;
            }

            // adjust the diagonal j,j entry
            if(a < m_ICCPrecond.colStart[k + 1] && m_ICCPrecond.colIndex[a] == j) {
                m_ICCPrecond.invDiag[j] -= multiplier * m_ICCPrecond.colValue[a];
            }

            ++a;
            // and now eliminate from the nonzero entries below the diagonal in column j (or add to missing if we can't)
            b = m_ICCPrecond.colStart[j];

            while(a < m_ICCPrecond.colStart[k + 1] && b < m_ICCPrecond.colStart[j + 1]) {
                if(m_ICCPrecond.colIndex[b] < m_ICCPrecond.colIndex[a]) {
                    ++b;
                } else if(m_ICCPrecond.colIndex[b] == m_ICCPrecond.colIndex[a]) {
                    m_ICCPrecond.colValue[b] -= multiplier * m_ICCPrecond.colValue[a];
                    ++a;
                    ++b;
                } else {
                    missing += m_ICCPrecond.colValue[a];
                    ++a;
                }
            }

            // and if there's anything left to do, add it to missing
            while(a < m_ICCPrecond.colStart[k + 1]) {
                missing += m_ICCPrecond.colValue[a];
                ++a;
            }

            // and do the final diagonal adjustment from the missing entries
            m_ICCPrecond.invDiag[j] -= MICCL0Param * multiplier * missing;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_Symmetric_MICC0L0(const SparseMatrix<RealType>& matrix, RealType minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into m_ICCPrecond (Note: assuming A is symmetric of course!)
    m_ICCPrecond.resize(matrix.nRows);
    for(size_t i = 0, iEnd = m_ICCPrecond.invDiag.size(); i < iEnd; ++i) {
        m_ICCPrecond.invDiag[i] = 0;
        m_ICCPrecond.aDiag[i]   = 0;
    }

    m_ICCPrecond.colValue.resize(0);
    m_ICCPrecond.colIndex.resize(0);

    for(UInt i = 0, iEnd = matrix.nRows; i < iEnd; ++i) {
        m_ICCPrecond.colStart[i] = static_cast<UInt>(m_ICCPrecond.colIndex.size());

        for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j) {
            if(matrix.colIndex[i][j] > i) {
                m_ICCPrecond.colIndex.push_back(matrix.colIndex[i][j]);
                m_ICCPrecond.colValue.push_back(matrix.colValue[i][j]);
            } else if(matrix.colIndex[i][j] == i) {
                m_ICCPrecond.invDiag[i] = m_ICCPrecond.aDiag[i] = matrix.colValue[i][j];
            }
        }
    }

    m_ICCPrecond.colStart[matrix.nRows] = static_cast<UInt>(m_ICCPrecond.colIndex.size());
    // now do the incomplete m_ICCPrecondization (figure out numerical values)

    for(UInt k = 0, kEnd = matrix.nRows; k < kEnd; ++k) {
        if(m_ICCPrecond.aDiag[k] < std::numeric_limits<RealType>::min()) {
            continue;                            // null row/column
        }

        // figure out the final L(k,k) entry
        if(m_ICCPrecond.invDiag[k] < minDiagonalRatio * m_ICCPrecond.aDiag[k]) {
            m_ICCPrecond.invDiag[k] = RealType(1.0) / sqrt(m_ICCPrecond.aDiag[k]);                            // drop to Gauss-Seidel here if the pivot looks dangerously small
        } else {
            m_ICCPrecond.invDiag[k] = RealType(1.0) / sqrt(m_ICCPrecond.invDiag[k]);
        }

        // finalize the k'th column L(:,k)
        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p) {
            m_ICCPrecond.colValue[p] *= m_ICCPrecond.invDiag[k];
        }

        for(UInt p = m_ICCPrecond.colStart[k], pEnd = m_ICCPrecond.colStart[k + 1]; p < pEnd; ++p) {
            UInt     j          = m_ICCPrecond.colIndex[p];    // work on column j
            RealType multiplier = m_ICCPrecond.colValue[p];

            m_ICCPrecond.invDiag[j] -= multiplier * m_ICCPrecond.colValue[p];

            UInt a  = p + 1;
            UInt b  = m_ICCPrecond.colStart[j];
            UInt as = m_ICCPrecond.colStart[k + 1];
            UInt bs = m_ICCPrecond.colStart[j + 1];

            while(a < as && b < bs) {
                if(m_ICCPrecond.colIndex[b] == m_ICCPrecond.colIndex[a]) {
                    m_ICCPrecond.colValue[b] -= multiplier * m_ICCPrecond.colValue[a];
                    ++a;
                    ++b;
                } else if(m_ICCPrecond.colIndex[b] < m_ICCPrecond.colIndex[a]) {
                    ++b;
                } else {
                    ++a;
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana