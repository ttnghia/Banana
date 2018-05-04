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
template<Int N, class RealType>
class BlockSparseMatrix
{
private:
    UInt m_Size;

    // for each row, a list of all column indices (sorted)
    Vec_VecUInt m_ColIndex;

    // values corresponding to indices
    Vec_VecMatXxX<N, RealType> m_ColValue;

public:
    explicit BlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColIndex(size), m_ColValue(size) {}

    UInt size() const noexcept { return m_Size; }
    void resize(UInt newSize);
    void clear(void);

    template<class IndexType> Vec_UInt& getIndices(IndexType row) { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> const Vec_UInt& getIndices(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> Vec_MatXxX<N, RealType>& getValues(IndexType row) { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }
    template<class IndexType> const Vec_MatXxX<N, RealType>& getValues(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }

    template<class IndexType> const MatXxX<N, RealType>& operator()(IndexType i, IndexType j) const;

    template<class IndexType> void setElement(IndexType i, IndexType j, const MatXxX<N, RealType>& newValue);
    template<class IndexType> void addElement(IndexType i, IndexType j, const MatXxX<N, RealType>& incrementValue);
    template<class IndexType> void eraseElement(IndexType i, IndexType j);

    void printDebug() const noexcept;
    void checkSymmetry(RealType threshold = RealType(1e-8)) const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
template<Int N, class RealType>
class FixedBlockSparseMatrix
{
private:
    UInt m_Size;

    // nonzero values row by row
    Vec_MatXxX<N, RealType> m_ColValue;

    // corresponding column indices
    Vec_UInt m_ColIndex;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    Vec_UInt m_RowStart;

public:
    explicit FixedBlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColValue(0), m_ColIndex(0), m_RowStart(size + 1) {}

    UInt size() const noexcept { return m_Size; }
    void resize(UInt newSize) { m_Size = newSize; m_RowStart.resize(m_Size + 1); }
    void clear(void) { m_ColValue.resize(0); m_ColIndex.resize(0); m_RowStart.resize(0); }
    void constructFromSparseMatrix(const BlockSparseMatrix<N, RealType>& fixedMatrix);

    template<class IndexType> Vec_UInt& getIndices(IndexType row) { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> const Vec_UInt& getIndices(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColIndex[row]; }
    template<class IndexType> Vec_UInt& getRowStarts(IndexType row) { assert(static_cast<UInt>(row) < m_Size); return m_RowStart[row]; }
    template<class IndexType> const Vec_UInt& getRowStarts(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_RowStart[row]; }
    template<class IndexType> Vec_Real& getValues(IndexType row) { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }
    template<class IndexType> const Vec_Real& getValues(IndexType row) const { assert(static_cast<UInt>(row) < m_Size); return m_ColValue[row]; }

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const FixedBlockSparseMatrix<N, RealType>& matrix, const Vec_VecX<N, RealType>& x, Vec_VecX<N, RealType>& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.Impl.hpp>