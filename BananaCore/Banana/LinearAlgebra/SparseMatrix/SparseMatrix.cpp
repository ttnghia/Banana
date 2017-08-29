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

#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>

#include <Banana/Utils/STLHelpers.h>
#include <tbb/tbb.h>
#include <iostream>
#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
UInt SparseMatrix::size() const noexcept
{
    return m_Size;
}

void SparseMatrix::resize(UInt newSize)
{
    m_Size = newSize;
    m_ColIndex.resize(m_Size);
    m_ColValue.resize(m_Size);
}

void SparseMatrix::clear(void)
{
    for(UInt i = 0; i < m_Size; ++i)
    {
        m_ColIndex[i].resize(0);
        m_ColValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec_UInt& SparseMatrix::getIndices(UInt row)
{
    assert(row < m_Size);
    return m_ColIndex[row];
}

const Vec_UInt& SparseMatrix::getIndices(UInt row) const
{
    assert(row < m_Size);
    return m_ColIndex[row];
}

Vec_Real& SparseMatrix::getValues(UInt row)
{
    assert(row < m_Size);
    return m_ColValue[row];
}

const Vec_Real& SparseMatrix::getValues(UInt row) const
{
    assert(row < m_Size);
    return m_ColValue[row];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SparseMatrix::operator()(UInt i, UInt j) const
{
    UInt k = 0;

    if(STLHelpers::Sorted::contain(m_ColIndex[i], j, k))
    {
        return m_ColValue[i][k];
    }
    else
    {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::setElement(UInt i, UInt j, Real newValue)
{
    assert(i < m_Size && j < m_Size);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(m_ColIndex[i], j, k))
    {
        m_ColValue[i][k] = newValue;
    }
    else
    {
        STLHelpers::Sorted::insertPairSorted(m_ColIndex[i], j, m_ColValue[i], newValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::addElement(UInt i, UInt j, Real incrementValue)
{
    assert(i < m_Size && j < m_Size);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(m_ColIndex[i], j, k))
    {
        m_ColValue[i][k] += incrementValue;
    }
    else
    {
        STLHelpers::Sorted::insertPairSorted(m_ColIndex[i], j, m_ColValue[i], incrementValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::eraseElement(UInt i, UInt j)
{
    assert(i < m_Size && j < m_Size);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(m_ColIndex[i], j, k))
    {
        m_ColIndex[i].erase(m_ColIndex[i].begin() + k);
        m_ColValue[i].erase(m_ColValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::printDebug() const noexcept
{
    for(UInt i = 0; i < m_Size; ++i)
    {
        if(m_ColIndex[i].size() == 0)
        {
            continue;
        }

        std::cout << "Line " << i << ": " << std::endl;

        for(UInt j = 0; j < m_ColIndex[i].size(); ++j)
        {
            std::cout << m_ColIndex[i][j] << "(" << m_ColValue[i][j] << "), " << std::endl;
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::checkSymmetry() const noexcept
{
    volatile bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << m_Size << std::endl;

    tbb::parallel_for(tbb::blocked_range<UInt>(0, m_Size), [&](tbb::blocked_range<UInt> r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              for(UInt j = i + 1; j < m_Size; ++j)
                              {
                                  if(STLHelpers::Sorted::contain(m_ColIndex[i], j))
                                  {
                                      auto err = std::abs((*this)(i, j) - (*this)(j, i));

                                      if(err > 1e-5)
                                      {
                                          check = false;
                                          std::cout << "Invalid matrix element at index " << i << ", " << j
                                                    << ", err = " << err << ": "
                                                    << "matrix(" << i << ", " << j << ") = " << (*this)(i, j) << " != "
                                                    << "matrix(" << j << ", " << i << ") = " << (*this)(j, i) << std::endl;
                                      }
                                  }
                              }
                          }
                      });

    if(check)
    {
        std::cout << "All matrix elements are valid!" << std::endl;
    }
    else
    {
        std::cout << "There are some invalid matrix elements!" << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const
{
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open())
    {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }

    UInt onePercent   = static_cast<UInt>(numElements / 100.0);
    UInt numProcessed = 0;
    UInt numElements  = 0;

    if(showPercentage > 0)
    {
        for(UInt i = 0; i < m_Size; ++i)
        {
            for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j)
            {
                UInt colIndex = m_ColIndex[i][j];

                if(colIndex < i)
                {
                    continue;
                }

                ++numElements;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt i = 0; i < m_Size; ++i)
    {
        for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j)
        {
            file << i + 1 << "    " << m_ColIndex[i][j] + 1 << "    " << m_ColValue[i][j] << std::endl;

            ++numProcessed;
            if((showPercentage > 0) && (numProcessed % (showPercentage * onePercent) == 0))
            {
                printf("%u/%u...\n", numProcessed, numElements);
            }
        }
    }
    file.close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::writeBinaryFile(const char* fileName, int showPercentage /*= -1*/) const
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    if(!file.is_open())
    {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }


    UInt numElements = 0;

    for(UInt i = 0; i < m_Size; ++i)
    {
        for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j)
        {
            UInt colIndex = m_ColIndex[i][j];

            if(colIndex < i)
            {
                continue;
            }

            ++numElements;
        }
    }


    UInt     onePercent   = static_cast<UInt>(numElements / 100.0);
    UInt     numProcessed = 0;
    Vec_UInt rowIndex(numElements);
    Vec_UInt colIndex(numElements);
    Vec_Real data(numElements);

    for(UInt i = 0; i < m_Size; ++i)
    {
        for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j)
        {
            UInt colIndex = m_ColIndex[i][j];

            if(colIndex < i)
            {
                continue;
            }

            rowIndex[numProcessed] = i;
            colIndex[numProcessed] = colIndex;
            data[numProcessed]     = static_cast<Real>(m_ColValue[i][j]);

            ++numProcessed;

            if((showPercentage > 0) && (numProcessed % (showPercentage * onePercentage) == 0))
            {
                printf("%u/%u...\n", numProcessed, numElements);
            }
        }
    }

    file.write(&m_Size,         sizeof(UInt));
    file.write(&numElements,    sizeof(UInt));
    file.write(rowIndex.data(), numElements * sizeof(UInt));
    file.write(colIndex.data(), numElements * sizeof(UInt));
    file.write(data.data(),     numElements * sizeof(Real));

    file.close();
    printf("File written, num. elements: %u, filename: %s\n", numElements, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool SparseMatrix::loadFromBinaryFile(const char* fileName, int showPercentage /*= -1*/)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        printf("Cannot open file for reading!\n");
        return false;
    }

    UInt              fileSize = (UInt)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), fileSize);
    file.close();


    ////////////////////////////////////////////////////////////////////////////////
    UInt newSize     = 0;
    UInt numElements = 0;

    memcpy(&newSize,     buffer.data(),                sizeof(UInt));
    memcpy(&numElements, &buffer.data()[sizeof(UInt)], sizeof(UInt));
    UInt* row_ptr    = reinterpret_cast<UInt*>(&buffer.data()[sizeof(UInt) * 2]);
    UInt* column_ptr = reinterpret_cast<UInt*>(&buffer.data()[(numElements + 2) * sizeof(UInt)]);
    Real* data_ptr   = reinterpret_cast<Real*>(&buffer.data()[(numElements * 2 + 2) * sizeof(UInt)]);

    resize(newSize);
    UInt onePercent   = static_cast<UInt>(numElements / 100.0);
    UInt numProcessed = 0;

    for(UInt k = 0; k < numElements; ++k)
    {
        setElement(row_ptr[numProcessed],    column_ptr[numProcessed], data_ptr[numProcessed]);
        setElement(column_ptr[numProcessed], row_ptr[numProcessed],    data_ptr[numProcessed]);

        ++numProcessed;

        if((showPercentage > 0) && (numProcessed % (showPercentage * onePercent) == 0))
        {
            printf("%u/%u...\n", numProcessed, numElements);
        }
    }

    printf("File read, num. elements: %u, filename: %s\n", numElements, fileName);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
void SparseMatrix::multiply(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              Real tmpResult = 0;

                              for(UInt j = 0, jEnd = static_cast<UInt>(matrix.m_ColIndex[i].size()); j < jEnd; ++j)
                              {
                                  tmpResult += matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
void SparseMatrix::multiply_and_subtract(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              Real tmpResult = result[i];

                              for(UInt j = 0, jEnd = static_cast<UInt>(matrix.m_ColIndex[i].size()); j < jEnd; ++j)
                              {
                                  tmpResult -= matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
UInt FixedSparseMatrix::size() const noexcept
{
    return m_Size;
}

void FixedSparseMatrix::resize(UInt newSize)
{
    m_Size = newSize;
    m_RowStart.resize(m_Size + 1);
}

void FixedSparseMatrix::clear(void)
{
    m_ColValue.resize(0);
    m_ColIndex.resize(0);
    m_RowStart.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FixedSparseMatrix::constructFromSparseMatrix(const SparseMatrix& matrix)
{
    resize(matrix.size());
    m_RowStart[0] = 0;

    for(UInt i = 0; i < m_Size; ++i)
    {
        m_RowStart[i + 1] = m_RowStart[i] + static_cast<UInt>(matrix.getIndices(i).size());
    }

    // in cases the matrix has empty row, accessing start row index may be out of range
    // so, add one extra element for padding
    m_ColIndex.resize(m_RowStart[m_Size] + 1);
    m_ColValue.resize(m_RowStart[m_Size] + 1);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              memcpy(&m_ColIndex[m_RowStart[i]], matrix.getIndices(i).data(), matrix.getIndices(i).size() * sizeof(UInt));
                          }

                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              memcpy(&m_ColValue[m_RowStart[i]], matrix.getValues(i).data(), matrix.getValues(i).size() * sizeof(Real));
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec_UInt& FixedSparseMatrix::getIndices(UInt row)
{
    assert(row < m_Size);
    return m_ColIndex[row];
}

const Vec_UInt& FixedSparseMatrix::getIndices(UInt row) const
{
    assert(row < m_Size);
    return m_ColIndex[row];
}

Vec_UInt& FixedSparseMatrix::getRowStarts(UInt row)
{
    assert(row < m_Size);
    return m_RowStart[row];
}

const Vec_UInt& FixedSparseMatrix::getRowStarts(UInt row) const
{
    assert(row < m_Size);
    return m_RowStart[row];
}

Vec_Real& FixedSparseMatrix::getValues(UInt row)
{
    assert(row < m_Size);
    return m_ColValue[row];
}

const Vec_Real& FixedSparseMatrix::getValues(UInt row) const
{
    assert(row < m_Size);
    return m_ColValue[row];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
void FixedSparseMatrix::multiply(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt>& r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              Real tmpResult = 0;

                              for(UInt j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j)
                              {
                                  tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
void FixedSparseMatrix::multiply_and_subtract(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());


    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt>(0, matrix.size()), [&](tbb::blocked_range<UInt> r)
                      {
                          for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              Real tmpResult = result[i];

                              for(UInt j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j)
                              {
                                  tmpResult -= matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespa