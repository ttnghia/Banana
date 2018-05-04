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

#pragma once

#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BlockPCGSolver
{
public:
    BlockPCGSolver() = default;

    RealType residual() const noexcept { return m_OutResidual; }
    UInt     iterations() const noexcept { return m_OutIterations; }

    ////////////////////////////////////////////////////////////////////////////////
    void setSolverParameters(RealType toleranceFactor, UInt maxIterations) { m_ToleranceFactor = toleranceFactor; m_MaxIterations = maxIterations; }
    void setZeroInitial(bool bZeroInitial) { m_bZeroInitial = bZeroInitial; }
    void enableZeroInitial() { m_bZeroInitial = true; }
    void disableZeroInitial() { m_bZeroInitial = false; }

    bool solve(const BlockSparseMatrix<N, RealType>& matrix, const Vec_VecX<N, RealType>& rhs, Vec_VecX<N, RealType>& result);
    bool solve_precond(const BlockSparseMatrix<N, RealType>& matrix, const Vec_VecX<N, RealType>& rhs, Vec_VecX<N, RealType>& result);

private:
    void formPreconditioner(const BlockSparseMatrix<N, RealType>& matrix);
    void applyPreconditioner(const Vec_VecX<N, RealType>& x, Vec_VecX<N, RealType>& result);

    ////////////////////////////////////////////////////////////////////////////////
    Vec_VecX<N, RealType>               z, s, r;
    Vector<MatXxX<N, RealType>>         m_JacobiPreconditioner;
    FixedBlockSparseMatrix<N, RealType> m_FixedSparseMatrix;

    RealType m_ToleranceFactor = RealType(1e-20);
    UInt     m_MaxIterations   = 10000u;
    bool     m_bZeroInitial    = true;

    ////////////////////////////////////////////////////////////////////////////////
    // output
    RealType m_OutResidual   = 0;
    UInt     m_OutIterations = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.Impl.hpp>