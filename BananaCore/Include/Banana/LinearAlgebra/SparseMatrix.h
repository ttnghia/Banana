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
#include <Banana/STLHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix
//
template<class RealType>
class SparseMatrix
{
private:
    UInt32 m_Size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt32> > m_Index;

    // values corresponding to indices
    std::vector<std::vector<RealType> > m_Value;

public:
    explicit SparseMatrix(UInt32 size = 0) : m_Size(size), m_Index(size), m_Value(size) {}

    unsigned int size() const noexcept;
    void         resize(UInt32 newSize);
    void         clear(void);

    const std::vector<UInt32>& getIndices(UInt32 row) const;
    const std::vector<UInt32>& getValues(UInt32 row) const;

    RealType operator ()(UInt32 i, UInt32 j) const;

    void setElement(UInt32 i, UInt32 j, RealType newValue);
    void addElement(UInt32 i, UInt32 j, RealType incrementValue);
    void eraseElement(UInt32 i, UInt32 j);

    void printDebug() const noexcept;
    void checkSymmetry() const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
    void writeBinaryFile(const char* fileName, int showPercentage = -1) const;
    bool loadFromBinaryFile(const char* fileName, int showPercentage = -1);

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result);
    static void multiply_and_subtract(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
template<class RealType>
class FixedSparseMatrix
{
private:
    UInt32 m_Size;

    // nonzero values row by row
    std::vector<RealType> m_Value;

    // corresponding column indices
    std::vector<UInt32> m_Index;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    std::vector<UInt32> m_RowStart;

public:
    explicit FixedSparseMatrix(UInt32 size = 0) : m_Size(size), m_Value(0), m_Index(0), m_RowStart(size + 1) {}

    void clear(void);
    void resize(UInt32 newSize);
    void constructFromSparseMatrix(const SparseMatrix<RealType>& fixedMatrix);

    template<class RealType>
    static void multiply(const FixedSparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result);

    template<class RealType>
    static void multiply_and_subtract(const FixedSparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/LinearAlgebra/SparseMatrix_Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana