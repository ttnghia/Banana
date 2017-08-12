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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// A simple compressed sparse column data structure (with separate diagonal)
// for lower triangular matrices
template<class RealType>
struct SparseColumnLowerFactor
{
    UInt32                m_Size;
    std::vector<RealType> m_InvDiag;  // reciprocals of diagonal elements
    std::vector<RealType> m_ColValue; // values below the diagonal, listed column by column
    std::vector<UInt32>   m_ColIndex; // a list of all row indices, for each column in turn
    std::vector<UInt32>   m_ColStart; // where each column begins in row index (plus an extra entry at the end, of #nonzeros)
    std::vector<RealType> m_aDiag;    // just used in factorization: minimum "safe" diagonal entry allowed

    explicit SparseColumnLowerFactor(UInt32 size = 0) : m_Size(size), m_InvDiag(size), m_ColStart(size + 1), m_aDiag(size) {}

    void clear(void);
    void resize(UInt32 newSize);
    void writeMatlab(std::ostream& output, const char* variableName);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class PCGSolver
{
public:
    enum class PreconditionerTypes
    {
        Jacobi,
        MICCL0,
        Symmetric_MICCL0
    };


    PCGSolver() = default;

    void setSolverParameters(RealType toleranceFactor, int maxIterations, RealType MICCL0Param = 0.97, RealType minDiagonalRatio = 0.25);
    void setPreconditioners(PreconditionerTypes precond);
    void setZeroInitial(bool bZeroInitial);
    void enableZeroInitial();
    void disableZeroInitial();
    bool solve(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& rhs, std::vector<RealType>& result, RealType& residual_out, UInt32& iterations_out);
    bool solve_precond(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& rhs, std::vector<RealType>& result, RealType& residual_out, UInt32& iterations_out);

private:
    void formPreconditioner(const SparseMatrix<RealType>& matrix);
    void applyPreconditioner(const std::vector<RealType>& x, std::vector<RealType>& result);
    void applyJacobiPreconditioner(const std::vector<RealType>& x, std::vector<RealType>& result);

    void solveLower(const SparseColumnLowerFactor<RealType>& factor, const std::vector<RealType>& rhs, std::vector<RealType>& result);
    void solveLower_TransposeInPlace(const SparseColumnLowerFactor<RealType>& factor, std::vector<RealType>& x);

    void formPreconditioner_Jacobi(const SparseMatrix<RealType>& matrix);
    void formPreconditioner_MICC0L0(const SparseMatrix<RealType>& matrix, SparseColumnLowerFactor<RealType>& factor, RealType MICCL0Param = 0.97, RealType minDiagonalRatio = 0.25);
    void formPreconditioner_Symmetric_MICC0L0(const SparseMatrix<RealType>& matrix, SparseColumnLowerFactor<RealType>& factor, RealType minDiagonalRatio = 0.25);

    ////////////////////////////////////////////////////////////////////////////////
    std::vector<RealType>       z, s, r;
    FixedSparseMatrix<RealType> m_FixedSparseMatrix;

    SparseColumnLowerFactor<RealType> m_ICCPreconditioner;
    std::vector<RealType>             m_JacobiPreconditioner;

    // solver parameters
    PreconditionerTypes m_PreconditionerType = PreconditionerTypes::MICCL0;
    RealType            m_ToleranceFactor    = 1e-20;
    UInt32              m_MaxIterations      = 10000;
    RealType            m_MICCL0Param        = 0.97;
    RealType            m_MinDiagonalRatio   = 0.25;
    bool                m_bZeroInitial       = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/PCGSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
