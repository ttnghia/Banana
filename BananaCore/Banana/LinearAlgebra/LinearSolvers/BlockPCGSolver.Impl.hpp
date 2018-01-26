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

template<Int N, class RealType>
bool BlockPCGSolver<N, RealType >::solve(const BlockSparseMatrix<N, RealType>& matrix, const Vec_VecX<N, RealType>& rhs, Vec_VecX<N, RealType>& result)
{
    const UInt n = matrix.size();
    if(z.size() != n) {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    if(m_bZeroInitial) {
        for(size_t i = 0; i < result.size(); ++i) {
            result[i] = VecX<N, RealType>(0);
        }
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    m_OutResidual = ParallelSTL::maxAbs<N, RealType>(r);

    if(m_OutResidual < m_ToleranceFactor) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;
    RealType rho = ParallelBLAS::dotProduct<N, RealType>(r, r);

    if(fabs(rho) < m_ToleranceFactor || isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    z = r;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<N, RealType>::multiply(m_FixedSparseMatrix, z, s);
        RealType tmp = ParallelBLAS::dotProduct<N, RealType>(s, z);

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

        m_OutResidual = ParallelSTL::maxAbs<N, RealType>(r);

        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        RealType rho_new = ParallelBLAS::dotProduct<N, RealType>(r, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::scaledAdd(beta, r, z);
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool BlockPCGSolver<N, RealType >::solve_precond(const BlockSparseMatrix<N, RealType>& matrix, const Vec_VecX<N, RealType>& rhs, Vec_VecX<N, RealType>& result)
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
            result[i] = VecX<N, RealType>(0);
        }
    }

    m_FixedSparseMatrix.constructFromSparseMatrix(matrix);
    r = rhs;

    FixedBlockSparseMatrix<3, RealType>::multiply(m_FixedSparseMatrix, result, s);
    ParallelBLAS::addScaled(RealType(-1.0), s, r);

    m_OutResidual = ParallelSTL::maxAbs<N, RealType>(r);

    if(m_OutResidual < m_ToleranceFactor) {
        m_OutIterations = 0;
        return true;
    }

    RealType tol = m_ToleranceFactor * m_OutResidual;

    formPreconditioner(matrix);
    applyPreconditioner(r, z);

    RealType rho = ParallelBLAS::dotProduct<N, RealType>(z, r);

    if(fabs(rho) < m_ToleranceFactor || isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    s = z;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<N, RealType>::multiply(m_FixedSparseMatrix, s, z);
        RealType alpha = rho / ParallelBLAS::dotProduct<N, RealType>(s, z);
        tbb::parallel_invoke(
            [&]
            {
                ParallelBLAS::addScaled(alpha, s, result);
            },
            [&]
            {
                ParallelBLAS::addScaled(-alpha, z, r);
            });

        m_OutResidual = ParallelSTL::maxAbs<N, RealType>(r);

        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        RealType rho_new = ParallelBLAS::dotProduct<N, RealType>(z, r);
        RealType beta    = rho_new / rho;
        ParallelBLAS::addScaled(beta, s, z);
        s.swap(z);             // s=beta*s+z
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlockPCGSolver<N, RealType >::formPreconditioner(const BlockSparseMatrix<N, RealType>& matrix)
{
    m_JacobiPreconditioner.resize(matrix.size());
    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i)
                            {
                                auto& v = matrix.getIndices(i);
                                auto it = std::lower_bound(v.begin(), v.end(), i);
                                m_JacobiPreconditioner[i] = (it != v.end()) ? glm::inverse(matrix.getValues(i)[std::distance(v.begin(), it)]) : MatXxX<N, RealType>(0);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlockPCGSolver<N, RealType >::applyPreconditioner(const Vec_VecX<N, RealType>& x, Vec_VecX<N, RealType>& result)
{
    Scheduler::parallel_for(x.size(), [&](size_t i) { result[i] = m_JacobiPreconditioner[i] * x[i]; });
}
