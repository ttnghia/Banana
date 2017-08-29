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

#include <Banana/Setup.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix
//
class SparseMatrix
{
private:
    UInt m_Size;

    // for each row, a list of all column indices (sorted)
    Vec_VecUInt m_ColIndex;

    // values corresponding to indices
    Vec_VecReal m_ColValue;

public:
    explicit SparseMatrix(UInt size = 0) : m_Size(size), m_ColIndex(size), m_ColValue(size) {}

    UInt size() const noexcept;
    void resize(UInt newSize);
    void clear(void);

    Vec_UInt&       getIndices(UInt row);
    Vec_Real&       getValues(UInt row);
    const Vec_UInt& getIndices(UInt row) const;
    const Vec_Real& getValues(UInt row) const;

    Real operator ()(UInt i, UInt j) const;

    void setElement(UInt i, UInt j, Real newValue);
    void addElement(UInt i, UInt j, Real incrementValue);
    void eraseElement(UInt i, UInt j);

    void printDebug() const noexcept;
    void checkSymmetry() const noexcept;

    void writeMatlabFile(const char* fileName, int showPercentage = -1) const;
    void writeBinaryFile(const char* fileName, int showPercentage = -1) const;
    bool loadFromBinaryFile(const char* fileName, int showPercentage = -1);

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result);
    static void multiply_and_subtract(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix. This can be significantly faster for matrix-vector
// multiplies due to better data locality.
class FixedSparseMatrix
{
private:
    UInt m_Size;

    // nonzero values row by row
    Vec_Real m_ColValue;

    // corresponding column indices
    Vec_UInt m_ColIndex;

    // where each row starts in value and col index (and last entry is one past the end, the number of non zeros)
    Vec_UInt m_RowStart;

public:
    explicit FixedSparseMatrix(UInt size = 0) : m_Size(size), m_ColValue(0), m_ColIndex(0), m_RowStart(size + 1) {}

    UInt size() const noexcept;
    void resize(UInt newSize);
    void clear(void);
    void constructFromSparseMatrix(const SparseMatrix& fixedMatrix);

    Vec_UInt&       getIndices(UInt row);
    Vec_UInt&       getRowStarts(UInt row);
    Vec_Real&       getValues(UInt row);
    const Vec_UInt& getIndices(UInt row) const;
    const Vec_UInt& getRowStarts(UInt row) const;
    const Vec_Real& getValues(UInt row) const;

    ////////////////////////////////////////////////////////////////////////////////
    static void multiply(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result);
    static void multiply_and_subtract(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana