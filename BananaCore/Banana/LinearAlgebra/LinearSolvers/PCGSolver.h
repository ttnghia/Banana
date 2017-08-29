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

#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum class PreconditionerTypes
{
    JACOBI,
    MICCL0,
    MICCL0_SYMMETRIC
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// A simple compressed sparse column data structure (with separate diagonal)
// for lower triangular matrices
template<class Real>
struct SparseColumnLowerFactor
{
    __BNN_SETUP_DATA_TYPE(Real)

    UInt m_Size;
    Vec_Real m_InvDiag;  // reciprocals of diagonal elements
    Vec_Real m_ColValue; // values below the diagonal, listed column by column
    Vec_UInt m_ColIndex; // a list of all row indices, for each column in turn
    Vec_UInt m_ColStart; // where each column begins in row index (plus an extra entry at the end, of #nonzeros)
    Vec_Real m_aDiag;    // just used in factorization: minimum "safe" diagonal entry allowed

    explicit SparseColumnLowerFactor(UInt size = 0) : m_Size(size), m_InvDiag(size), m_ColStart(size + 1), m_aDiag(size) {}

    void clear(void);
    void resize(UInt newSize);
    void writeMatlab(std::ostream& output, const char* variableName);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
class PCGSolver
{
    __BNN_SETUP_DATA_TYPE(Real)
public:
    PCGSolver() = default;

    void setSolverParameters(Real toleranceFactor, int maxIterations, Real MICCL0Param = 0.97, Real minDiagonalRatio = 0.25);
    void setPreconditioners(PreconditionerTypes precond);
    void setZeroInitial(bool bZeroInitial);
    void enableZeroInitial();
    void disableZeroInitial();
    bool solve(const SparseMatrix<Real>& matrix, const Vec_Real& rhs, Vec_Real& result, Real& residual_out, UInt& iterations_out);
    bool solve_precond(const SparseMatrix<Real>& matrix, const Vec_Real& rhs, Vec_Real& result, Real& residual_out, UInt& iterations_out);

private:
    void formPreconditioner(const SparseMatrix<Real>& matrix);
    void applyPreconditioner(const Vec_Real& x, Vec_Real& result);
    void applyJacobiPreconditioner(const Vec_Real& x, Vec_Real& result);

    void solveLower(const SparseColumnLowerFactor<Real>& factor, const Vec_Real& rhs, Vec_Real& result);
    void solveLower_TransposeInPlace(const SparseColumnLowerFactor<Real>& factor, Vec_Real& x);

    void formPreconditioner_Jacobi(const SparseMatrix<Real>& matrix);
    void formPreconditioner_MICC0L0(const SparseMatrix<Real>& matrix, SparseColumnLowerFactor<Real>& factor, Real MICCL0Param = 0.97, Real minDiagonalRatio = 0.25);
    void formPreconditioner_Symmetric_MICC0L0(const SparseMatrix<Real>& matrix, SparseColumnLowerFactor<Real>& factor, Real minDiagonalRatio = 0.25);

    ////////////////////////////////////////////////////////////////////////////////
    Vec_Real                z, s, r;
    FixedSparseMatrix<Real> m_FixedSparseMatrix;

    SparseColumnLowerFactor<Real> m_ICCPreconditioner;
    Vec_Real                      m_JacobiPreconditioner;

    // solver parameters
    PreconditionerTypes m_PreconditionerType = PreconditionerTypes::MICCL0;
    Real                m_ToleranceFactor    = Real(1e-20);
    UInt              m_MaxIterations      = 10000;
    Real                m_MICCL0Param        = Real(0.97);
    Real                m_MinDiagonalRatio   = Real(0.25);
    bool                m_bZeroInitial       = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
