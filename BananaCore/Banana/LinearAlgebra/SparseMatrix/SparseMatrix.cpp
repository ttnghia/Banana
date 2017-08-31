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
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>

#include <iostream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
void SparseMatrix::resize(UInt newSize)
{
    nRows = newSize;
    colIndex.resize(nRows);
    colValue.resize(nRows);
}

void SparseMatrix::clear(void)
{
    for(UInt i = 0; i < nRows; ++i)
    {
        colIndex[i].resize(0);
        colValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SparseMatrix::operator()(UInt i, UInt j) const
{
    UInt k = 0;

    if(STLHelpers::Sorted::contain(colIndex[i], j, k))
    {
        return colValue[i][k];
    }
    else
    {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::setElement(UInt i, UInt j, Real newValue)
{
    assert(i < nRows && j < nRows);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(colIndex[i], j, k))
    {
        colValue[i][k] = newValue;
    }
    else
    {
        STLHelpers::Sorted::insertPairSorted(colIndex[i], j, colValue[i], newValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::addElement(UInt i, UInt j, Real incrementValue)
{
    assert(i < nRows && j < nRows);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(colIndex[i], j, k))
    {
        colValue[i][k] += incrementValue;
    }
    else
    {
        STLHelpers::Sorted::insertPairSorted(colIndex[i], j, colValue[i], incrementValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::eraseElement(UInt i, UInt j)
{
    assert(i < nRows && j < nRows);

    UInt k = 0;

    if(STLHelpers::Sorted::contain(colIndex[i], j, k))
    {
        colIndex[i].erase(colIndex[i].begin() + k);
        colValue[i].erase(colValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SparseMatrix::printDebug(UInt maxRows /*= 0*/) const noexcept
{
    if(maxRows == 0) maxRows = nRows;
    for(UInt i = 0; i < maxRows; ++i)
    {
        if(colIndex[i].size() == 0)
            continue;

        std::cout << "Line " << i << ": ";

        for(UInt j = 0; j < colIndex[i].size(); ++j)
        {
            std::cout << colIndex[i][j] << "(" << colValue[i][j] << "), ";
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
    std::cout << "Matrix size: " << nRows << std::endl;

    ParallelFuncs::parallel_for<UInt>(0, nRows,
                                      [&](UInt i)
                                      {
                                          for(UInt j = i + 1; j < nRows; ++j)
                                          {
                                              if(STLHelpers::Sorted::contain(colIndex[i], j))
                                              {
                                                  auto err = std::abs((*this)(i, j) - (*this)(j, i));

                                                  if(err > 1e-8)
                                                  {
                                                      check = false;
                                                      std::cout << "Invalid matrix element at index " << i << ", " << j
                                                                << ", err = " << err << ": "
                                                                << "matrix(" << i << ", " << j << ") = " << (*this)(i, j) << " != "
                                                                << "matrix(" << j << ", " << i << ") = " << (*this)(j, i) << std::endl;
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
void SparseMatrix::printTextFile(const char* fileName)
{
    Vector<String> matContent;

    for(UInt i = 0; i < nRows; ++i)
    {
        if(colIndex[i].size() == 0) continue;

        for(size_t j = 0; j < colIndex[i].size(); ++j)
        {
            String str = std::to_string(i + 1);
            str += "    ";
            str += std::to_string(j + 1);
            str += "    ";
            str += std::to_string(colValue[i][j]);
            matContent.push_back(str);
        }
    }

    FileHelpers::writeFile(matContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
void SparseMatrix::multiply(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.nRows == static_cast<UInt>(x.size()));
    result.resize(matrix.nRows);

    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          Real tmpResult = 0;
                                          for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j)
                                              tmpResult += matrix.colValue[i][j] * x[matrix.colIndex[i][j]];

                                          result[i] = tmpResult;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
void SparseMatrix::multiply_and_subtract(const SparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.nRows == static_cast<UInt>(x.size()));
    result.resize(matrix.nRows);

    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          Real tmpResult = result[i];
                                          for(UInt j = 0, jEnd = static_cast<UInt>(matrix.colIndex[i].size()); j < jEnd; ++j)
                                              tmpResult -= matrix.colValue[i][j] * x[matrix.colIndex[i][j]];

                                          result[i] = tmpResult;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
void FixedSparseMatrix::resize(UInt newSize)
{
    nRows = newSize;
    rowStart.resize(nRows + 1);
}

void FixedSparseMatrix::clear(void)
{
    colValue.resize(0);
    colIndex.resize(0);
    rowStart.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FixedSparseMatrix::constructFromSparseMatrix(const SparseMatrix& matrix)
{
    resize(matrix.nRows);
    rowStart[0] = 0;

    for(UInt i = 0; i < nRows; ++i)
    {
        rowStart[i + 1] = rowStart[i] + static_cast<UInt>(matrix.colIndex[i].size());
    }

    // in cases the matrix has empty row, accessing start row index may be out of range
    // so, add one extra element for padding
    colIndex.resize(rowStart[nRows] + 1);
    colValue.resize(rowStart[nRows] + 1);

    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          memcpy(&colIndex[rowStart[i]], matrix.colIndex[i].data(), matrix.colIndex[i].size() * sizeof(UInt));
                                          memcpy(&colValue[rowStart[i]], matrix.colValue[i].data(), matrix.colValue[i].size() * sizeof(Real));
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
void FixedSparseMatrix::multiply(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.nRows == static_cast<UInt>(x.size()));
    result.resize(matrix.nRows);

    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          Real tmpResult = 0;
                                          for(UInt j = matrix.rowStart[i], jEnd = matrix.rowStart[i + 1]; j < jEnd; ++j)
                                              tmpResult += matrix.colValue[j] * x[matrix.colIndex[j]];

                                          result[i] = tmpResult;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
void FixedSparseMatrix::multiply_and_subtract(const FixedSparseMatrix& matrix, const Vec_Real& x, Vec_Real& result)
{
    assert(matrix.nRows == static_cast<UInt>(x.size()));
    result.resize(matrix.nRows);

    ParallelFuncs::parallel_for<UInt>(0, matrix.nRows,
                                      [&](UInt i)
                                      {
                                          Real tmpResult = result[i];
                                          for(UInt j = matrix.rowStart[i], jEnd = matrix.rowStart[i + 1]; j < jEnd; ++j)
                                              tmpResult -= matrix.colValue[j] * x[matrix.colIndex[j]];

                                          result[i] = tmpResult;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespa