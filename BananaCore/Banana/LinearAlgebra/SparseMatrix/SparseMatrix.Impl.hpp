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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class RealType>
void SparseMatrix<RealType >::resize(UInt newSize)
{
    nRows = newSize;
    colIndex.resize(nRows);
    colValue.resize(nRows);
}

template<class RealType>
void SparseMatrix<RealType >::clear(void)
{
    for(UInt i = 0; i < nRows; ++i) {
        colIndex[i].resize(0);
        colValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
RealType SparseMatrix<RealType>::operator ()(IndexType i, IndexType j) const
{
    assert(static_cast<UInt>(i) < nRows && static_cast<UInt>(j) < nRows);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(colIndex[i], static_cast<UInt>(j), k)) {
        return colValue[i][k];
    } else {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
void SparseMatrix<RealType >::setElement(IndexType i, IndexType j, RealType newValue)
{
    assert(static_cast<UInt>(i) < nRows && static_cast<UInt>(j) < nRows);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(colIndex[i], static_cast<UInt>(j), k)) {
        colValue[i][k] = newValue;
    } else {
        STLHelpers::Sorted::insertPairSorted(colIndex[i], static_cast<UInt>(j), colValue[i], newValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
void SparseMatrix<RealType >::addElement(IndexType i, IndexType j, RealType incrementValue)
{
    assert(static_cast<UInt>(i) < nRows && static_cast<UInt>(j) < nRows);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(colIndex[i], static_cast<UInt>(j), k)) {
        colValue[i][k] += incrementValue;
    } else {
        STLHelpers::Sorted::insertPairSorted(colIndex[i], static_cast<UInt>(j), colValue[i], incrementValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
template<class IndexType>
void SparseMatrix<RealType >::eraseElement(IndexType i, IndexType j)
{
    assert(static_cast<UInt>(i) < nRows && static_cast<UInt>(j) < nRows);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(colIndex[i], static_cast<UInt>(j), k)) {
        colIndex[i].erase(colIndex[i].begin() + k);
        colValue[i].erase(colValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseMatrix<RealType >::printDebug(UInt maxRows /*= 0*/) const noexcept
{
    if(maxRows == 0) { maxRows = nRows; }
    for(UInt i = 0; i < maxRows; ++i) {
        if(colIndex[i].size() == 0) {
            continue;
        }

        std::cout << "Line " << i << ": ";

        for(UInt j = 0; j < colIndex[i].size(); ++j) {
            std::cout << colIndex[i][j] << "(" << NumberHelpers::formatToScientific(colValue[i][j]) << "), ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseMatrix<RealType >::checkSymmetry() const noexcept
{
    volatile bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << nRows << std::endl;

    ParallelFuncs::parallel_for<UInt>(0, nRows,
                                      [&](UInt i)
                                      {
                                          for(UInt j = i + 1; j < nRows; ++j) {
                                              if(STLHelpers::Sorted::contain(colIndex[i], j)) {
                                                  auto err = std::abs((*this)(i, j) - (*this)(j, i));

                                                  if(err > 1e-8) {
                                                      check = false;
                                                      std::cout << "Invalid matrix element at index " << i << ", " << j
                                                                << ", err = " << err << ": "
                                                                << "matrix(" << i << ", " << j << ") = " << (*this)(i, j) << " != "
                                                                << "matrix(" << j << ", " << i << ") = " << (*this)(j, i) << std::endl;
                                                  }
                                              }
                                          }
                                      });

    if(check) {
        std::cout << "All matrix elements are valid!" << std::endl;
    } else {
        std::cout << "There are some invalid matrix elements!" << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseMatrix<RealType >::printTextFile(const char* fileName)
{
    Vector<String> matContent;

    for(UInt i = 0; i < nRows; ++i) {
        if(colIndex[i].size() == 0) { continue; }

        for(size_t j = 0; j < colIndex[i].size(); ++j) {
            String str = std::to_string(i + 1);
            str += "    ";
            str += std::to_string(colIndex[i][j] + 1);
            str += "    ";
            str += std::to_string(colValue[i][j]);
            matContent.push_back(str);
        }
    }

    FileHelpers::writeFile(matContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
template<class RealType>
void FixedSparseMatrix<RealType >::constructFromSparseMatrix(const SparseMatrix<RealType>& matrix)
{
    resize(matrix.nRows);
    rowStart[0] = 0;

    for(UInt i = 0; i < nRows; ++i) {
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
                                          memcpy(&colValue[rowStart[i]], matrix.colValue[i].data(), matrix.colValue[i].size() * sizeof(RealType));
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class RealType>
void FixedSparseMatrix<RealType >::multiply(const FixedSparseMatrix<RealType>& matrix, const Vector<RealType>& x, Vector<RealType>& result)
{
    assert(matrix.nRows == static_cast<UInt>(x.size()));
    result.resize(matrix.nRows);

    ParallelFuncs::parallel_for<UInt>(matrix.nRows,
                                      [&](UInt i)
                                      {
                                          RealType tmpResult = 0;
                                          for(UInt j = matrix.rowStart[i], jEnd = matrix.rowStart[i + 1]; j < jEnd; ++j) {
                                              tmpResult += matrix.colValue[j] * x[matrix.colIndex[j]];
                                          }

                                          result[i] = tmpResult;
                                      });
}
