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

#include <Banana/TypeNames.h>
#include <Banana/Macros.h>
#include <Banana/STLHelpers.h>

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
    UInt32 m_Size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt32> > m_ColIndex;

    // values corresponding to indices
    std::vector<std::vector<MatrixType> > m_ColValue;

public:
    explicit BlockSparseMatrix(UInt32 size = 0) : m_Size(size), m_ColIndex(size), m_ColValue(size) {}

    unsigned int size() const noexcept;
    void         resize(UInt32 newSize);
    void         clear(void);

    std::vector<UInt32>&         getIndices(UInt32 row);
    std::vector<RealType>&       getValues(UInt32 row);
    const std::vector<UInt32>&   getIndices(UInt32 row) const;
    const std::vector<RealType>& getValues(UInt32 row) const;

    const MatrixType& operator ()(UInt32 i, UInt32 j) const;

    void setElement(UInt32 i, UInt32 j, const MatrixType& newValue);
    void addElement(UInt32 i, UInt32 j, const MatrixType& incrementValue);
    void eraseElement(UInt32 i, UInt32 j);


    void printDebug() const noexcept;
    void checkSymmetry() const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
    void writeBinaryFile(const char* fileName, int showPercentage = -1) const;
    bool loadFromBinaryFile(const char* fileName, int showPercentage = -1);

    ////////////////////////////////////////////////////////////////////////////////
    template<class VectorType>
    static void multiply(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result);

    template<class VectorType, class RealType>
    static void multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, const RealType alpha, std::vector<VectorType>& result);

    template<class VectorType, class RealType>
    static void add_multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, const RealType alpha, std::vector<VectorType>& result);

    template<class VectorType>
    static void multiply_and_subtract(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result);

    template<class RealType>
    static void add_scaled(const BlockSparseMatrix<MatrixType>& A, const BlockSparseMatrix<MatrixType>& B, const RealType alpha, BlockSparseMatrix<MatrixType>& matrix);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
template<class MatrixType>
class FixedBlockSparseMatrix
{
private:
    UInt32 m_Size;

    // nonzero values row by row
    std::vector<MatrixType> m_ColValue;

    // corresponding column indices
    std::vector<UInt32> m_ColIndex;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    std::vector<UInt32> m_RowStart;

public:
    explicit FixedBlockSparseMatrix(UInt32 size = 0) : m_Size(size), m_ColValue(0), m_ColIndex(0), m_RowStart(size + 1) {}

    unsigned int size() const noexcept;
    void         resize(UInt32 newSize);
    void         clear(void);
    void         constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& fixedMatrix);

    std::vector<UInt32>&         getIndices(UInt32 row);
    std::vector<UInt32>&         getRowStarts(UInt32 row);
    std::vector<RealType>&       getValues(UInt32 row);
    const std::vector<UInt32>&   getIndices(UInt32 row) const;
    const std::vector<UInt32>&   getRowStarts(UInt32 row) const;
    const std::vector<RealType>& getValues(UInt32 row) const;

    ////////////////////////////////////////////////////////////////////////////////
    template<class VectorType>
    static void multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result);

    template<class VectorType>
    static void multiply_and_subtract(const FixedBlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/BlockSparseMatrix.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana