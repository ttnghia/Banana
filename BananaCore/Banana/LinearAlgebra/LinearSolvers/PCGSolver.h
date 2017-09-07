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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// A simple compressed sparse column data structure (with separate diagonal)
// for lower triangular matrices
struct SparseColumnLowerFactor
{
    UInt     nRows;
    Vec_Real invDiag;  // reciprocals of diagonal elements
    Vec_UInt colIndex; // a list of all row indices, for each column in turn
    Vec_Real colValue; // values below the diagonal, listed column by column
    Vec_UInt colStart; // where each column begins in row index (plus an extra entry at the end, of #nonzeros)
    Vec_Real aDiag;    // just used in factorization: minimum "safe" diagonal entry allowed

    explicit SparseColumnLowerFactor(UInt size = 0) : nRows(size), invDiag(size), colStart(size + 1), aDiag(size) {}

    void clear(void);
    void resize(UInt newSize);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PCGSolver
{
public:
    enum Preconditioner
    {
        JACOBI,
        MICCL0,
        MICCL0_SYMMETRIC
    };

    PCGSolver() = default;
    Real residual() const noexcept { return m_OutResidual; }
    UInt iterations() const noexcept { return m_OutIterations; }

    ////////////////////////////////////////////////////////////////////////////////
    void setPreconditioners(Preconditioner precond) { m_PreconditionerType = precond; }
    void setZeroInitial(bool bZeroInitial) { m_bZeroInitial = bZeroInitial; }
    void enableZeroInitial() { m_bZeroInitial = true; }
    void disableZeroInitial() { m_bZeroInitial = false; }
    void setSolverParameters(Real toleranceFactor, int maxIterations, Real MICCL0Param = Real(0.97), Real minDiagonalRatio = Real(0.25));
    bool solve(const SparseMatrix& matrix, const Vec_Real& rhs, Vec_Real& result);
    bool solve_precond(const SparseMatrix& matrix, const Vec_Real& rhs, Vec_Real& result);

private:
    void resize(UInt size);
    void formPreconditioner(const SparseMatrix& matrix);
    void applyPreconditioner(const Vec_Real& x, Vec_Real& result);
    void applyJacobiPreconditioner(const Vec_Real& x, Vec_Real& result);

    void solveLower(const Vec_Real& rhs, Vec_Real& result);
    void solveLower_TransposeInPlace(Vec_Real& x);

    void formPreconditioner_Jacobi(const SparseMatrix& matrix);
    void formPreconditioner_MICC0L0(const SparseMatrix& matrix, Real MICCL0Param = Real(0.97), Real minDiagonalRatio = Real(0.25));
    void formPreconditioner_Symmetric_MICC0L0(const SparseMatrix& matrix, Real minDiagonalRatio = Real(0.25));

    ////////////////////////////////////////////////////////////////////////////////
    // solver variables
    Vec_Real          z, s, r;
    FixedSparseMatrix m_FixedSparseMatrix;

    SparseColumnLowerFactor m_ICCPrecond;
    Vec_Real                m_JacobiPrecond;

    ////////////////////////////////////////////////////////////////////////////////
    // solver parameters
    Preconditioner m_PreconditionerType = Preconditioner::MICCL0;
    Real           m_ToleranceFactor    = Real(1e-20);
    UInt           m_MaxIterations      = 10000;
    Real           m_MICCL0Param        = Real(0.97);
    Real           m_MinDiagonalRatio   = Real(0.25);
    bool           m_bZeroInitial       = true;

    ////////////////////////////////////////////////////////////////////////////////
    // output
    Real m_OutResidual   = 0;
    UInt m_OutIterations = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
