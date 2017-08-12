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
template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::setSolverParameters(RealType toleranceFactor, UInt32 maxIterations)
{
    m_ToleranceFactor = fmax(toleranceFactor, 1e-30);
    m_MaxIterations   = maxIterations;
}

template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::setZeroInitial(bool bZeroInitial)
{
    m_bZeroInitial = bZeroInitial;
}

template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::enableZeroInitial()
{
    m_bZeroInitial = true;
}

template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::disableZeroInitial()
{
    m_bZeroInitial = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class RealType>
bool BlockPCGSolver<MatrixType, VectorType, RealType>::solve(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result,
        RealType& residual_out, UInt32& iterations_out)
{
    const UInt32 n = matrix.size();

    if(z.size() != n)
    {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

// zero out the result
    if(m_bZeroInitial)
    {
        for(auto vec& : result)
        {
            vec = VectorType(0);
        }
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    residual_out = ParallelBLAS::maxAbs<RealType, VectorType>(r);

    if(residual_out < 1e-20)
    {
        iterations_out = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * residual_out;
    RealType rho = ParallelBLAS::dotProduct<RealType, VectorType>(r, r);

    if(fabs(rho) < 1e-20 || isnan(rho))
    {
        iterations_out = 0;
        return true;
    }

    z = r;

    for(UInt32 iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        multiply<MatrixType, VectorType>(m_FixedSparseMatrix, z, s);
        RealType tmp = ParallelBLAS::dotProduct<RealType, VectorType>(s, z);

        if(fabs(tmp) < 1e-20 || isnan(tmp))
        {
            iterations_out = iteration;
            return true;
        }

        RealType alpha = rho / tmp;

        tbb::parallel_invoke(
            [&]
        {
            ParallelBLAS::addScaled<RealType, VectorType>(alpha, z, result);
        },
            [&]
        {
            ParallelBLAS::addScaled<RealType, VectorType>(-alpha, s, r);
        });

        residual_out = ParallelBLAS::maxAbs<RealType, VectorType>(r);

        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        RealType rho_new = ParallelBLAS::dotProduct<RealType, VectorType>(r, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::scaledAdd<RealType, VectorType>(beta, r, z);
        rho = rho_new;
    }

    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class RealType>
bool BlockPCGSolver<MatrixType, VectorType, RealType>::solve_precond(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result, RealType& residual_out, UInt32& iterations_out)
{
    UInt32 n = matrix.size();

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

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    multiply(m_FixedSparseMatrix, result, s);
    ParallelBLAS::addScaled<RealType, VectorType>(-1.0, s, r);

    residual_out = ParallelBLAS::maxAbs<RealType, VectorType>(r);

    if(fabs(residual_out) < 1e-20)
    {
        iterations_out = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * residual_out;

    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    RealType rho = ParallelBLAS::dotProduct<RealType, VectorType>(z, r);

    if(fabs(rho) < 1e-20 || isnan(rho))
    {
        iterations_out = 0;
        return true;
    }

    s = z;

    for(UInt32 iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        multiply(m_FixedSparseMatrix, s, z);
        RealType alpha = rho / ParallelBLAS::dotProduct<RealType, VectorType>(s, z);
        tbb::parallel_invoke(
            [&]
        {
            ParallelBLAS::addScaled<RealType, VectorType>(alpha, s, result);
        },
            [&]
        {
            ParallelBLAS::addScaled<RealType, VectorType>(-alpha, z, r);
        });

        residual_out = ParallelBLAS::maxAbs<RealType, VectorType>(r);

        if(residual_out < tol)
        {
            iterations_out = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        RealType rho_new = ParallelBLAS::dotProduct<RealType, VectorType>(z, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::addScaled<RealType, VectorType>(beta, s, z);
        s.swap(z);     // s=beta*s+z
        rho = rho_new;
    }

    iterations_out = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::formPreconditioner(const BlockSparseMatrix<MatrixType>& matrix)
{
    m_JacobiPreconditioner.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            auto& v = matrix.getIndices(i);
            auto it = std::lower_bound(v.begin(), v.end(), i);
            m_JacobiPreconditioner[i] = (it != v.end()) ? glm::inverse(matrix.getValues(i)[std::distance(v.begin(), it)]) : MatrixType(0);
        }
    }, ap);     // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class RealType>
void BlockPCGSolver<MatrixType, VectorType, RealType>::applyPreconditioner(const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()), [&](tbb::blocked_range<UInt32> r)
    {
        for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            result[i] = m_JacobiPreconditioner[i] * x[i];
        }
    }, ap);     // end parallel_for
}
