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

#include <Banana/LinearAlgebra/SparseMatrix.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// A simple compressed sparse column data structure (with separate diagonal)
// for lower triangular matrices
template<class ScalarType>
struct SparseColumnLowerFactor
{
    UInt32                  m_Size;
    std::vector<ScalarType> m_InvDiag;  // reciprocals of diagonal elements
    std::vector<ScalarType> m_ColValue; // values below the diagonal, listed column by column
    std::vector<UInt32>     m_ColIndex; // a list of all row indices, for each column in turn
    std::vector<UInt32>     m_ColStart; // where each column begins in rowindex (plus an extra entry at the end, of #nonzeros)
    std::vector<ScalarType> m_aDiag;    // just used in factorization: minimum "safe" diagonal entry allowed

    explicit SparseColumnLowerFactor(UInt32 size = 0) : m_Size(size), m_InvDiag(size), m_ColStart(size + 1), m_aDiag(size) {}

    void clear(void);
    void resize(UInt32 newSize);
    void writeMatlab(std::ostream& output, const char* variableName);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class PCGSolver
{
public:
    enum class PreconditionerTypes
    {
        Jacobi,
        MICCL0,
        Symmetric_MICCL0
    };


    PCGSolver(void) {}

    void setSolverParameters(ScalarType toleranceFactor, int maxIterations, ScalarType MICCL0Param = 0.97, ScalarType minDiagonalRatio = 0.25);
    void setPreconditioners(PreconditionerTypes precond);
    void setZeroInitial(bool bZeroInitial);
    void enableZeroInitial();
    void disableZeroInitial();
    bool solve(const SparseMatrix<ScalarType>& matrix, const std::vector<ScalarType>& rhs, std::vector<ScalarType>& result, ScalarType& residual_out, UInt32& iterations_out);
    bool solve_precond(const SparseMatrix<ScalarType>& matrix, const std::vector<ScalarType>& rhs, std::vector<ScalarType>& result, ScalarType& residual_out, UInt32& iterations_out);

private:
    void formPreconditioner(const SparseMatrix<ScalarType>& matrix);
    void applyPreconditioner(const std::vector<ScalarType>& x, std::vector<ScalarType>& result);
    void applyJacobiPreconditioner(const std::vector<ScalarType>& x, std::vector<ScalarType>& result);

    void solveLower(const SparseColumnLowerFactor<ScalarType>& factor, const std::vector<ScalarType>& rhs, std::vector<ScalarType>& result);
    void solveLower_TransposeInPlace(const SparseColumnLowerFactor<ScalarType>& factor, std::vector<ScalarType>& x);

    void formPreconditioner_Jacobi(const SparseMatrix<ScalarType>& matrix);
    void formPreconditioner_MICC0L0(const SparseMatrix<ScalarType>& matrix, SparseColumnLowerFactor<ScalarType>& factor, ScalarType MICCL0Param = 0.97, ScalarType minDiagonalRatio = 0.25);
    void formPreconditioner_Symmetric_MICC0L0(const SparseMatrix<ScalarType>& matrix, SparseColumnLowerFactor<ScalarType>& factor, ScalarType minDiagonalRatio = 0.25);

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<ScalarType>       z, s, r;
    FixedSparseMatrix<ScalarType> m_FixedSparseMatrix;

    SparseColumnLowerFactor<ScalarType> m_ICCPreconditioner;
    std::vector<ScalarType>             m_JacobiPreconditioner;

    // solver parameters
    PreconditionerTypes m_PreconditionerType = PreconditionerTypes::MICCL0;
    ScalarType          m_ToleranceFactor    = 1e-20;
    UInt32              m_MaxIterations      = 10000;
    ScalarType          m_MICCL0Param        = 0.97;
    ScalarType          m_MinDiagonalRatio   = 0.25;
    bool                m_bZeroInitial       = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/PCGSolver_Impl.hpp>