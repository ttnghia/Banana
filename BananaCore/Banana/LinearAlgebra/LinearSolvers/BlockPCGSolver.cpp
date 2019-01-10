//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
bool BlockPCGSolver<MatrixType>::solve(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& rhs, Vector<VectorType>& result) {
    const UInt n = matrix.size();
    if(z.size() != n) {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    if(m_bZeroInitial) {
        result.assign(result.size(), VectorType(0));
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

    if(std::abs(rho) < m_ToleranceFactor || std::isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    z = r;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<MatrixType>::multiply(m_FixedSparseMatrix, z, s);
        RealType tmp = ParallelBLAS::dotProduct<VectorType::length(), RealType>(s, z);

        if(std::abs(tmp) < m_ToleranceFactor || std::isnan(tmp)) {
            m_OutIterations = iteration;
            return true;
        }

        RealType alpha = rho / tmp;

        tbb::parallel_invoke(
            [&] { ParallelBLAS::addScaled(alpha, z, result); },
            [&] { ParallelBLAS::addScaled(-alpha, s, r); }
            );

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
bool BlockPCGSolver<MatrixType>::solve_precond(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& rhs, Vector<VectorType>& result) {
    UInt n = matrix.size();

    if(z.size() != n) {
        s.resize(n);
        z.resize(n);
        r.resize(n);
    }

    // zero out the result
    if(m_bZeroInitial) {
        result.assign(result.size(), VectorType(0));
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
    if(std::abs(rho) < m_ToleranceFactor || std::isnan(rho)) {
        m_OutIterations = 0;
        return true;
    }

    s = z;

    for(UInt iteration = 0; iteration < m_MaxIterations; ++iteration) {
        FixedBlockSparseMatrix<MatrixType>::multiply(m_FixedSparseMatrix, s, z);
        RealType tmp = ParallelBLAS::dotProduct<VectorType::length(), RealType>(s, z);

        if(std::abs(tmp) < m_ToleranceFactor || std::isnan(tmp)) {
            m_OutIterations = iteration;
            return true;
        }

        RealType alpha = rho / tmp;

        tbb::parallel_invoke(
            [&] { ParallelBLAS::addScaled(alpha, s, result); },
            [&] { ParallelBLAS::addScaled(-alpha, z, r); }
            );

        m_OutResidual = ParallelSTL::maxAbs<VectorType::length(), RealType>(r);
        if(m_OutResidual < tol) {
            m_OutIterations = iteration + 1;
            return true;
        }

        applyPreconditioner(r, z);

        RealType rho_new = ParallelBLAS::dotProduct<VectorType::length(), RealType>(z, r);
        RealType beta    = rho_new / rho;

        ParallelBLAS::addScaled(beta, s, z);
        s.swap(z); // s=beta*s+z
        rho = rho_new;
    }

    m_OutIterations = m_MaxIterations;
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockPCGSolver<MatrixType>::formPreconditioner(const BlockSparseMatrix<MatrixType>& matrix) {
    m_JacobiPreconditioner.resize(matrix.size());
    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i) {
                                const auto& v = matrix.getIndices(i);
                                const auto it = std::lower_bound(v.begin(), v.end(), i);
                                MatrixType tmp_inv(0);
                                if(it != v.end()) {
                                    MatrixType tmp = matrix.getValues(i)[std::distance(v.begin(), it)];
                                    for(Int j = 0; j < MatrixType::length(); ++j) {
                                        tmp_inv[j][j] = tmp[j][j] != 0 ? typename MatrixType::value_type(1.0) / tmp[j][j] : 0;
                                    }
                                }
                                m_JacobiPreconditioner[i] = tmp_inv;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockPCGSolver<MatrixType>::applyPreconditioner(const Vector<VectorType>& x, Vector<VectorType>& result) {
    Scheduler::parallel_for(x.size(), [&](size_t i) { result[i] = m_JacobiPreconditioner[i] * x[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class BlockPCGSolver<Mat2x2f>;
template class BlockPCGSolver<Mat3x3f>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
