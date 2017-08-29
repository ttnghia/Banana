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
    __BNN_SETUP_DATA_TYPE(Real)
private:
    UInt m_Size;

    // for each row, a list of all column indices (sorted)
    Vec_VecUInt m_ColIndex;

    // values corresponding to indices
    Vec_Vec<MatrixType> m_ColValue;

public:
    explicit BlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColIndex(size), m_ColValue(size) {}

    UInt size() const noexcept;
    void   resize(UInt newSize);
    void   clear(void);

    Vec_UInt&       getIndices(UInt row);
    Vec_Real&       getValues(UInt row);
    const Vec_UInt& getIndices(UInt row) const;
    const Vec_Real& getValues(UInt row) const;

    const MatrixType& operator ()(UInt i, UInt j) const;

    void setElement(UInt i, UInt j, const MatrixType& newValue);
    void addElement(UInt i, UInt j, const MatrixType& incrementValue);
    void eraseElement(UInt i, UInt j);


    void printDebug() const noexcept;
    void checkSymmetry() const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
    void writeBinaryFile(const char* fileName, int showPercentage = -1) const;
    bool loadFromBinaryFile(const char* fileName, int showPercentage = -1);

    ////////////////////////////////////////////////////////////////////////////////
    template<class VectorType>
    static void multiply(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result);

    template<class VectorType, class Real>
    static void multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, const Real alpha, Vector<VectorType>& result);

    template<class VectorType, class Real>
    static void add_multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, const Real alpha, Vector<VectorType>& result);

    template<class VectorType>
    static void multiply_and_subtract(const BlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result);

    template<class Real>
    static void add_scaled(const BlockSparseMatrix<MatrixType>& A, const BlockSparseMatrix<MatrixType>& B, const Real alpha, BlockSparseMatrix<MatrixType>& matrix);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
template<class MatrixType>
class FixedBlockSparseMatrix
{
private:
    UInt m_Size;

    // nonzero values row by row
    Vector<MatrixType> m_ColValue;

    // corresponding column indices
    Vec_UInt m_ColIndex;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    Vec_UInt m_RowStart;

public:
    explicit FixedBlockSparseMatrix(UInt size = 0) : m_Size(size), m_ColValue(0), m_ColIndex(0), m_RowStart(size + 1) {}

    UInt size() const noexcept;
    void   resize(UInt newSize);
    void   clear(void);
    void   constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& fixedMatrix);

    Vec_UInt&       getIndices(UInt row);
    Vec_UInt&       getRowStarts(UInt row);
    Vec_Real&       getValues(UInt row);
    const Vec_UInt& getIndices(UInt row) const;
    const Vec_UInt& getRowStarts(UInt row) const;
    const Vec_Real& getValues(UInt row) const;

    ////////////////////////////////////////////////////////////////////////////////
    template<class VectorType>
    static void multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result);

    template<class VectorType>
    static void multiply_and_subtract(const FixedBlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/BlockSparseMatrix.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana