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

#include <Banana/Setup.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>

#include <iostream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix
//
template<class MatrixType>
class BlockSparseMatrix
{
private:
    using RealType   = typename MatrixType::value_type;
    using VectorType = typename MatrixType::col_type;

    UInt m_Size;

    // for each row, a list of all column indices (sorted)
    Vec_VecUInt m_ColIndex;

    // values corresponding to indices
    Vec_Vec<MatrixType> m_ColValue;

public:
    explicit BlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColIndex(size), m_ColValue(size) {}

    UInt size() const noexcept { return m_Size; }
    void reserve(UInt size);
    void resize(UInt newSize);
    void clear();

    template<class IndexType> const auto& getIndices(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> const auto& getValues(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }

    template<class IndexType> const auto& operator()(IndexType i, IndexType j) const;

    template<class IndexType> void setElement(IndexType i, IndexType j, const MatrixType& newValue);
    template<class IndexType> void addElement(IndexType i, IndexType j, const MatrixType& incrementValue);
    template<class IndexType> void eraseElement(IndexType i, IndexType j);

    void printDebug() const noexcept;
    void checkSymmetry(RealType threshold = RealType(1e-8)) const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
template<class MatrixType>
class FixedBlockSparseMatrix
{
private:
    using RealType   = typename MatrixType::value_type;
    using VectorType = typename MatrixType::col_type;

    UInt m_Size;

    // nonzero values row by row
    Vector<MatrixType> m_ColValue;

    // corresponding column indices
    Vec_UInt m_ColIndex;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    Vec_UInt m_RowStart;

public:
    explicit FixedBlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColValue(0), m_ColIndex(0), m_RowStart(size + 1) {}

    UInt size() const noexcept { return m_Size; }
    void resize(UInt newSize) { m_Size = newSize; m_RowStart.resize(m_Size + 1); }
    void clear() { m_ColValue.resize(0); m_ColIndex.resize(0); m_RowStart.resize(0); }
    void constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& fixedMatrix);

    template<class IndexType> const auto& getIndices(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> const auto& getRowStarts(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_RowStart[row]; }
    template<class IndexType> const auto& getValues(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.Impl.hpp>