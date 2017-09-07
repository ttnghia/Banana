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
template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::setSolverParameters(Real toleranceFactor, UInt maxIterations)
{
    m_ToleranceFactor = fmax(toleranceFactor, 1e-30);
    m_MaxIterations   = maxIterations;
}

template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::setZeroInitial(bool bZeroInitial)
{
    m_bZeroInitial = bZeroInitial;
}

template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::enableZeroInitial()
{
    m_bZeroInitial = true;
}

template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::disableZeroInitial()
{
    m_bZeroInitial = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class Real>
bool BlockPCGSolver<MatrixType, VectorType, Real>::solve(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result)
{
    const UInt n = matrix.size();

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

    m_OutResidual = ParallelBLAS::maxAbs<Real, VectorType>(r);

    if(m_OutResidual < 1e-20)
    {
        m_OutIterations = 0;
        return true;
    }

    Real tol = m_ToleranceFactor * m_OutResidual;
    Real rho = ParallelBLAS::dotProduct<Real, VectorType>(r, r);

    if(fabs(rho) < 1e-20 || isnan(rho))
    {
        m_OutIterations = 0;
        return true;
    }

    z = r;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        multiply<MatrixType, VectorType>(m_FixedSparseMatrix, z, s);
        Real tmp = ParallelBLAS::dotProduct<Real, VectorType>(s, z);

        if(fabs(tmp) < 1e-20 || isnan(tmp))
        {
            m_OutIterations = iteration;
            return true;
        }

        Real alpha = rho / tmp;

        tbb::parallel_invoke(
            [&]
        {
            ParallelBLAS::addScaled<Real, VectorType>(alpha, z, result);
        },
            [&]
        {
            ParallelBLAS::addScaled<Real, VectorType>(-alpha, s, r);
        });

        m_OutResidual = ParallelBLAS::maxAbs<Real, VectorType>(r);

        if(m_OutResidual < tol)
        {
            m_OutIterations = iteration + 1;
            return true;
        }

        Real rho_new = ParallelBLAS::dotProduct<Real, VectorType>(r, r);
        Real beta    = rho_new / rho;
        ParallelBLAS::scaledAdd<Real, VectorType>(beta, r, z);
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class Real>
bool BlockPCGSolver<MatrixType, VectorType, Real>::solve_precond(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& rhs, std::vector<VectorType>& result)
{
    UInt n = matrix.size();

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
    ParallelBLAS::addScaled<Real, VectorType>(-1.0, s, r);

    m_OutResidual = ParallelBLAS::maxAbs<Real, VectorType>(r);

    if(fabs(m_OutResidual) < 1e-20)
    {
        m_OutIterations = 0;
        return true;
    }

    Real tol = m_ToleranceFactor * m_OutResidual;

    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    Real rho = ParallelBLAS::dotProduct<Real, VectorType>(z, r);

    if(fabs(rho) < 1e-20 || isnan(rho))
    {
        m_OutIterations = 0;
        return true;
    }

    s = z;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration)
    {
        multiply(m_FixedSparseMatrix, s, z);
        Real alpha = rho / ParallelBLAS::dotProduct<Real, VectorType>(s, z);
        tbb::parallel_invoke(
            [&]
        {
            ParallelBLAS::addScaled<Real, VectorType>(alpha, s, result);
        },
            [&]
        {
            ParallelBLAS::addScaled<Real, VectorType>(-alpha, z, r);
        });

        m_OutResidual = ParallelBLAS::maxAbs<Real, VectorType>(r);

        if(m_OutResidual < tol)
        {
            m_OutIterations = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        Real rho_new = ParallelBLAS::dotProduct<Real, VectorType>(z, r);
        Real beta    = rho_new / rho;
        ParallelBLAS::addScaled<Real, VectorType>(beta, s, z);
        s.swap(z);     // s=beta*s+z
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::formPreconditioner(const BlockSparseMatrix<MatrixType>& matrix)
{
    m_JacobiPreconditioner.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              auto& v = matrix.getIndices(i);
                              auto it = std::lower_bound(v.begin(), v.end(), i);
                              m_JacobiPreconditioner[i] = (it != v.end()) ? glm::inverse(matrix.getValues(i)[std::distance(v.begin(), it)]) : MatrixType(0);
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType, class VectorType, class Real>
void BlockPCGSolver<MatrixType, VectorType, Real>::applyPreconditioner(const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, x.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(size_t i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              result[i] = m_JacobiPreconditioner[i] * x[i];
                          }
                      }, ap); // end parallel_for
}
