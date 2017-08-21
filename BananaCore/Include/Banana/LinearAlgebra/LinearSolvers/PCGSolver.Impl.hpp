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


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SparseColumnLowerFactor struct
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseColumnLowerFactor<RealType>::clear(void)
{
    m_Size = 0;
    m_InvDiag.clear();
    m_ColValue.clear();
    m_ColIndex.clear();
    m_ColStart.clear();
    m_aDiag.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseColumnLowerFactor<RealType>::resize(UInt32 newSize)
{
    m_Size = newSize;
    m_InvDiag.resize(m_Size);
    m_aDiag.resize(m_Size);
    m_ColStart.resize(m_Size + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseColumnLowerFactor<RealType>::writeMatlab(std::ostream& output, const char* variableName)
{
    output << variableName << "=sparse([";

    for(UInt32 i = 0; i < m_Size; ++i)
    {
        output << " " << i + 1;

        for(UInt32 j = m_ColStart[i]; j < m_ColStart[i + 1]; ++j)
        {
            output << " " << getIndices(j) + 1;
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PCGSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::setSolverParameters(RealType toleranceFactor, int maxIterations, RealType MICCL0Param /*= 0.97*/, RealType minDiagonalRatio /*= 0.25*/)
{
    m_ToleranceFactor = fmax(toleranceFactor, RealType(1e-30));

    m_MaxIterations    = maxIterations;
    m_MICCL0Param      = MICCL0Param;
    m_MinDiagonalRatio = minDiagonalRatio;
}

template<class RealType>
void PCGSolver<RealType>::setPreconditioners(PreconditionerTypes precond)
{
    m_PreconditionerType = precond;
}

template<class RealType>
void PCGSolver<RealType>::setZeroInitial(bool bZeroInitial)
{
    m_bZeroInitial = bZeroInitial;
}

template<class RealType>
void PCGSolver<RealType>::enableZeroInitial()
{
    m_bZeroInitial = true;
}

template<class RealType>
void PCGSolver<RealType>::disableZeroInitial()
{
    m_bZeroInitial = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool PCGSolver<RealType>::solve(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& rhs, std::vector<RealType>& result, RealType& residual_out, UInt32& iterations_out)
{
    UInt32 n = matrix.size();

    if(z.size() != n)
    {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    if(m_bZeroInitial)
    {
        for(size_t i = 0; i < result.size(); ++i)
            result[i] = 0;
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    residual_out = ParallelBLAS::maxAbs<RealType>(r);

    if(residual_out < 1e-20)
    {
        iterations_out = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * residual_out;
    RealType rho = ParallelBLAS::dotProductScalar<RealType>(r, r);

    if(rho < 1e-20 || isnan(rho))
    {
        iterations_out = 0;
        return false;
    }

    z = r;

    for(UInt32 iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        multiply(m_FixedSparseMatrix, z, s);
        RealType alpha = rho / ParallelBLAS::dotProductScalar<RealType>(s, z);
        tbb::parallel_invoke(
            [&]
        {
            ParallelBLAS::addScaled<RealType, RealType>(alpha, z, result);
        },
            [&]
        {
            ParallelBLAS::addScaled<RealType, RealType>(-alpha, s, r);
        });

        residual_out = ParallelBLAS::maxAbs<RealType>(r);

        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        RealType rho_new = ParallelBLAS::dotProductScalar<RealType>(r, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::scaledAdd<RealType, RealType>(beta, r, z);
        rho = rho_new;
    }

    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool PCGSolver<RealType>::solve_precond(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& rhs, std::vector<RealType>& result, RealType& residual_out, UInt32& iterations_out)
{
    UInt32 n = matrix.size();

    if(z.size() != n)
    {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    if(m_bZeroInitial)
    {
        for(size_t i = 0; i < result.size(); ++i)
            result[i] = 0;
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);

    FixedSparseMatrix<RealType>::multiply(m_FixedSparseMatrix, result, s);
    r = rhs;
    ParallelBLAS::addScaled<RealType, RealType>(-1.0, s, r);


    residual_out = ParallelSTL::maxAbs<RealType>(r);

    if(residual_out < 1e-20)
    {
        iterations_out = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * residual_out;

    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    RealType rho = ParallelBLAS::dotProductScalar<RealType>(z, r);

    if(rho < 1e-20 || isnan(rho))
    {
        iterations_out = 0;
        return false;
    }

    s = z;

    for(UInt32 iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        FixedSparseMatrix<RealType>::multiply(m_FixedSparseMatrix, s, z);
        RealType alpha = rho / ParallelBLAS::dotProductScalar<RealType>(s, z);
        tbb::parallel_invoke([&]
        {
            ParallelBLAS::addScaled<RealType, RealType>(alpha, s, result);
        },
                             [&]
        {
            ParallelBLAS::addScaled<RealType, RealType>(-alpha, z, r);
        });

        residual_out = ParallelSTL::maxAbs<RealType>(r);

        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        RealType rho_new = ParallelBLAS::dotProductScalar<RealType>(z, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::addScaled<RealType, RealType>(beta, s, z);
        s.swap(z); // s=beta*s+z
        rho = rho_new;
    }


    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner(const SparseMatrix<RealType>& matrix)
{
    switch(m_PreconditionerType)
    {
        case PreconditionerTypes::JACOBI:
            formPreconditioner_Jacobi(matrix);
            break;

        case PreconditionerTypes::MICCL0:
            formPreconditioner_MICC0L0(matrix, m_ICCPreconditioner);
            break;

        case PreconditionerTypes::MICCL0_SYMMETRIC:
            formPreconditioner_Symmetric_MICC0L0(matrix, m_ICCPreconditioner);
            break;
    }
}

template<class RealType>
void PCGSolver<RealType>::applyPreconditioner(const std::vector<RealType>& x, std::vector<RealType>& result)
{
    if(m_PreconditionerType != PreconditionerTypes::JACOBI)
    {
        solveLower(m_ICCPreconditioner, x, result);
        solveLower_TransposeInPlace(m_ICCPreconditioner, result);
    }
    else
    {
        applyJacobiPreconditioner(x, result);
    }
}

template<class RealType>
void PCGSolver<RealType>::applyJacobiPreconditioner(const std::vector<RealType>& x, std::vector<RealType>& result)
{
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              result[i] = m_JacobiPreconditioner[i] * x[i];
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Solution routines with lower triangular matrix.
// solve L*result=rhs
template<class RealType>
void PCGSolver<RealType>::solveLower(const SparseColumnLowerFactor<RealType>& factor, const std::vector<RealType>& rhs, std::vector<RealType>& result)
{
    assert(factor.m_Size == static_cast<UInt32>(rhs.size()));
    assert(factor.m_Size == static_cast<UInt32>(result.size()));
    result = rhs;

    for(UInt32 i = 0, is = factor.m_Size; i < is; ++i)
    {
        result[i] *= factor.m_InvDiag[i];

        for(UInt32 j = factor.m_ColStart[i], jEnd = factor.m_ColStart[i + 1]; j < jEnd; ++j)
        {
            result[factor.m_ColIndex[j]] -= factor.m_ColValue[j] * result[i];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solve L^T*result=rhs
template<class RealType>
void PCGSolver<RealType>::solveLower_TransposeInPlace(const SparseColumnLowerFactor<RealType>& factor, std::vector<RealType>& x)
{
    assert(factor.m_Size == static_cast<UInt32>(x.size()));
    UInt32 i = factor.m_Size;

    do
    {
        --i;

        for(UInt32 j = factor.m_ColStart[i], jEnd = factor.m_ColStart[i + 1]; j < jEnd; ++j)
        {
            x[i] -= factor.m_ColValue[j] * x[factor.m_ColIndex[j]];
        }

        x[i] *= factor.m_InvDiag[i];
    } while(i != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_Jacobi(const SparseMatrix<RealType>& matrix)
{
    m_JacobiPreconditioner.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              auto& v = matrix.getIndices(i);
                              auto it = std::lower_bound(v.begin(), v.end(), i);
                              m_JacobiPreconditioner[i] = (it != v.end()) ? RealType(1.0) / matrix.getValues(i)[std::distance(v.begin(), it)] : 0;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Incomplete Cholesky factorization, level zero, with option for modified version.
// Set MICCL0Param between zero (regular incomplete Cholesky) and
// one (fully modified version), with values close to one usually giving the best
// results. The minDiagonalRatio parameter is used to detect and correct
// problems in factorization: if a pivot is this much less than the diagonal
// entry from the original matrix, the original matrix entry is used instead.
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_MICC0L0(const SparseMatrix<RealType>& matrix, SparseColumnLowerFactor<RealType>& factor, RealType MICCL0Param /*= 0.97*/, RealType minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into factor (Note: assuming A is symmetric of course!)
    factor.resize(matrix.size());
    for(size_t i = 0, iEnd = factor.m_InvDiag.size(); i < iEnd; ++i)
    {
        factor.m_InvDiag[i] = 0;
        factor.m_aDiag[i]   = 0;
    }

    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);


    for(UInt32 i = 0, iEnd = matrix.size(); i < iEnd; ++i)
    {
        factor.m_ColStart[i] = static_cast<UInt32>(factor.m_ColIndex.size());

        for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.getIndices(i).size()); j < jEnd; ++j)
        {
            if(matrix.getIndices(i)[j] > i)
            {
                factor.m_ColIndex.push_back(matrix.getIndices(i)[j]);
                factor.m_ColValue.push_back(matrix.getValues(i)[j]);
            }
            else if(matrix.getIndices(i)[j] == i)
            {
                factor.m_InvDiag[i] = factor.m_aDiag[i] = matrix.getValues(i)[j];
            }
        }
    }


    factor.m_ColStart[matrix.size()] = static_cast<UInt32>(factor.m_ColIndex.size());
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


    for(UInt32 k = 0, kEnd = matrix.size(); k < kEnd; ++k)
    {
        if(factor.m_aDiag[k] == 0)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(factor.m_InvDiag[k] < minDiagonalRatio * factor.m_aDiag[k])
        {
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_aDiag[k]); // drop to Gauss-Seidel here if the pivot looks dangerously small
        }
        else
        {
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_InvDiag[k]);
        }

        // finalize the k'th column L(:,k)
        for(UInt32 p = factor.m_ColStart[k], pEnd = factor.m_ColStart[k + 1]; p < pEnd; ++p)
        {
            factor.m_ColValue[p] *= factor.m_InvDiag[k];
        }

        // incompletely eliminate L(:,k) from future columns, modifying diagonals
        for(UInt32 p = factor.m_ColStart[k], pEnd = factor.m_ColStart[k + 1]; p < pEnd; ++p)
        {
            UInt32   j          = factor.m_ColIndex[p];   // work on column j
            RealType multiplier = factor.m_ColValue[p];
            RealType missing    = 0;
            UInt32   a          = factor.m_ColStart[k];
            // first look for contributions to missing from dropped entries above the diagonal in column j
            UInt32 b = 0;

            while(a < factor.m_ColStart[k + 1] && factor.m_ColIndex[a] < j)
            {
                // look for factor.rowindex[a] in matrix.index[j] starting at b
                while(b < matrix.getIndices(j).size())
                {
                    if(matrix.getIndices(j)[b] < factor.m_ColIndex[a])
                    {
                        ++b;
                    }
                    else if(matrix.getIndices(j)[b] == factor.m_ColIndex[a])
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
            factor.m_InvDiag[j] -= MICCL0Param * multiplier * missing;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void PCGSolver<RealType>::formPreconditioner_Symmetric_MICC0L0(const SparseMatrix<RealType>& matrix, SparseColumnLowerFactor<RealType>& factor, RealType minDiagonalRatio /*= 0.25*/)
{
    // first copy lower triangle of matrix into factor (Note: assuming A is symmetric of course!)
    factor.resize(matrix.size());
    for(size_t i = 0, iEnd = factor.m_InvDiag.size(); i < iEnd; ++i)
    {
        factor.m_InvDiag[i] = 0;
        factor.m_aDiag[i]   = 0;
    }

    factor.m_ColValue.resize(0);
    factor.m_ColIndex.resize(0);

    for(UInt32 i = 0, iEnd = matrix.size(); i < iEnd; ++i)
    {
        factor.m_ColStart[i] = static_cast<UInt32>(factor.m_ColIndex.size());

        for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.getIndices(i).size()); j < jEnd; ++j)
        {
            if(matrix.getIndices(i)[j] > i)
            {
                factor.m_ColIndex.push_back(matrix.getIndices(i)[j]);
                factor.m_ColValue.push_back(matrix.getValues(i)[j]);
            }
            else if(matrix.getIndices(i)[j] == i)
            {
                factor.m_InvDiag[i] = factor.m_aDiag[i] = matrix.getValues(i)[j];
            }
        }
    }

    factor.m_ColStart[matrix.size()] = static_cast<UInt32>(factor.m_ColIndex.size());
    // now do the incomplete factorization (figure out numerical values)

    for(UInt32 k = 0, kEnd = matrix.size(); k < kEnd; ++k)
    {
        if(factor.m_aDiag[k] == 0)
        {
            continue;    // null row/column
        }

        // figure out the final L(k,k) entry
        if(factor.m_InvDiag[k] < minDiagonalRatio * factor.m_aDiag[k])
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_aDiag[k]);    // drop to Gauss-Seidel here if the pivot looks dangerously small
        else
            factor.m_InvDiag[k] = 1 / sqrt(factor.m_InvDiag[k]);

        // finalize the k'th column L(:,k)
        for(UInt32 p = factor.m_ColStart[k], pEnd = factor.m_ColStart[k + 1]; p < pEnd; ++p)
            factor.m_ColValue[p] *= factor.m_InvDiag[k];


        for(UInt32 p = factor.m_ColStart[k], pEnd = factor.m_ColStart[k + 1]; p < pEnd; ++p)
        {
            UInt32   j          = factor.m_ColIndex[p];   // work on column j
            RealType multiplier = factor.m_ColValue[p];

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
