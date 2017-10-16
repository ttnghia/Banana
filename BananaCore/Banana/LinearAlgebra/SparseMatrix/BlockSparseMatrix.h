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

#include <tbb/tbb.h>

#include <iostream>

#include <Banana/Setup.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>

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

    Vec_UInt&                      getIndices(UInt row) { assert(row < m_Size); return m_ColIndex[row]; }
    const Vec_UInt&                getIndices(UInt row) const { assert(row < m_Size); return m_ColIndex[row]; }
    Vec_MatXxX<N, RealType>&       getValues(UInt row) { assert(row < m_Size); return m_ColValue[row]; }
    const Vec_MatXxX<N, RealType>& getValues(UInt row) const { assert(row < m_Size); return m_ColValue[row]; }

    const MatXxX<N, RealType>& operator ()(UInt i, UInt j) const;

    void setElement(UInt i, UInt j, const MatXxX<N, RealType>& newValue);
    void addElement(UInt i, UInt j, const MatXxX<N, RealType>& incrementValue);
    void eraseElement(UInt i, UInt j);

    void printDebug() const noexcept;
    void checkSymmetry() const noexcept;

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

    Vec_UInt&       getIndices(UInt row) { assert(row < m_Size); return m_ColIndex[row]; }
    const Vec_UInt& getIndices(UInt row) const { assert(row < m_Size); return m_ColIndex[row]; }
    Vec_UInt&       getRowStarts(UInt row) { assert(row < m_Size); return m_RowStart[row]; }
    const Vec_UInt& getRowStarts(UInt row) const { assert(row < m_Size); return m_RowStart[row]; }
    Vec_Real&       getValues(UInt row) { assert(row < m_Size); return m_ColValue[row]; }
    const Vec_Real& getValues(UInt row) const { assert(row < m_Size); return m_ColValue[row]; }

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const FixedBlockSparseMatrix<N, RealType>& matrix, const Vector<VecX<N, RealType> >& x, Vector<VecX<N, RealType> >& result);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana