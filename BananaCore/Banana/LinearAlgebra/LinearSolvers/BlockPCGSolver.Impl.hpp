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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
bool BlockPCGSolver<MatrixType>::solve(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& rhs, Vector<VectorType>& result)
{
    const UInt n = matrix.size();
    if(z.size() != n) {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    if(m_bZeroInitial) {
        for(size_t i = 0; i < result.size(); ++i) {
            result[i] = VectorType(0);
        }
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    m_OutResidual = ParallelSTL::maxAbs<VectorType::length(), RealType>(r);

    if(m_OutResidual < m_ToleranceFactor) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;
    RealType rho = ParallelBLAS::dotProduct<VectorType::length(), RealType>(r, r);

    if(fabs(rho) < m_ToleranceFactor || isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    z = r;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<MatrixType>::multiply(m_FixedSparseMatrix, z, s);
        RealType tmp = ParallelBLAS::dotProduct<VectorType::length(), RealType>(s, z);

        if(fabs(tmp) < m_ToleranceFactor || isnan(tmp)) {
            m_OutIterations = iteration;
            return true;
        }

        RealType alpha = rho / tmp;

        tbb::parallel_invoke(
            [&]
            {
                ParallelBLAS::addScaled(alpha, z, result);
            },
            [&]
            {
                ParallelBLAS::addScaled(-alpha, s, r);
            });

        m_OutResidual = ParallelSTL::maxAbs<VectorType::length(), RealType>(r);

        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        RealType rho_new = ParallelBLAS::dotProduct<VectorType::length(), RealType>(r, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::scaledAdd(beta, r, z);
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
bool BlockPCGSolver<MatrixType>::solve_precond(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& rhs, Vector<VectorType>& result)
{
    UInt n = matrix.size();

    if(z.size() != n) {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    // zero out the result
    if(m_bZeroInitial) {
        for(size_t i = 0; i < result.size(); ++i) {
            result[i] = VectorType(0);
        }
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    FixedBlockSparseMatrix<MatrixType>::multiply(m_FixedSparseMatrix, result, s);
    ParallelBLAS::addScaled(RealType(-1.0), s, r);

    m_OutResidual = ParallelSTL::maxAbs<VectorType::length(), RealType>(r);

    if(m_OutResidual < m_ToleranceFactor) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;

    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    RealType rho = ParallelBLAS::dotProduct<VectorType::length(), RealType>(z, r);

    if(fabs(rho) < m_ToleranceFactor || isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    s = z;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<MatrixType>::multiply(m_FixedSparseMatrix, s, z);
        RealType alpha = rho / ParallelBLAS::dotProduct<VectorType::length(), RealType>(s, z);
        tbb::parallel_invoke(
            [&]
            {
                ParallelBLAS::addScaled(alpha, s, result);
            },
            [&]
            {
                ParallelBLAS::addScaled(-alpha, z, r);
            });

        m_OutResidual = ParallelSTL::maxAbs<VectorType::length(), RealType>(r);

        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        RealType rho_new = ParallelBLAS::dotProduct<VectorType::length(), RealType>(z, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::addScaled(beta, s, z);
        s.swap(z);             // s=beta*s+z
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockPCGSolver<MatrixType>::formPreconditioner(const BlockSparseMatrix<MatrixType>& matrix)
{
    m_JacobiPreconditioner.resize(matrix.size());
    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i)
                            {
                                const auto& v             = matrix.getIndices(i);
                                auto it                   = std::lower_bound(v.begin(), v.end(), i);
                                m_JacobiPreconditioner[i] = (it != v.end()) ? glm::inverse(matrix.getValues(i)[std::distance(v.begin(), it)]) : MatrixType(0);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockPCGSolver<MatrixType>::applyPreconditioner(const Vector<VectorType>& x, Vector<VectorType>& result)
{
    Scheduler::parallel_for(x.size(), [&](size_t i) { result[i] = m_JacobiPreconditioner[i] * x[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana