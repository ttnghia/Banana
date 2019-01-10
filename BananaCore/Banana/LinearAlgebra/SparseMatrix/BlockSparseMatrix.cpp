//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.h>
#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::reserve(UInt size) {
    m_ColIndex.reserve(m_Size);
    m_ColValue.reserve(m_Size);
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::resize(UInt newSize) {
    m_Size = newSize;
    m_ColIndex.resize(m_Size);
    m_ColValue.resize(m_Size);
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::clear() {
    for(UInt i = 0; i < m_Size; ++i) {
        m_ColIndex[i].resize(0);
        m_ColValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
const auto& BlockSparseMatrix<MatrixType>::operator()(IndexType i, IndexType j) const {
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        return m_ColValue[i][k];
    } else {
        return m_Zero;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
void BlockSparseMatrix<MatrixType>::setElement(IndexType i, IndexType j, const MatrixType& newValue) {
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        m_ColValue[i][k] = newValue;
    } else {
        STLHelpers::Sorted::insertPairSorted(m_ColIndex[i], static_cast<UInt>(j), m_ColValue[i], newValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
void BlockSparseMatrix<MatrixType>::addElement(IndexType i, IndexType j, const MatrixType& incrementValue) {
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        m_ColValue[i][k] += incrementValue;
    } else {
        STLHelpers::Sorted::insertPairSorted(m_ColIndex[i], static_cast<UInt>(j), m_ColValue[i], incrementValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
void BlockSparseMatrix<MatrixType>::eraseElement(IndexType i, IndexType j) {
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        m_ColIndex[i].erase(m_ColIndex[i].begin() + k);
        m_ColValue[i].erase(m_ColValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::printDebug() const noexcept{
    for(UInt i = 0; i < m_Size; ++i) {
        if(m_ColIndex[i].size() == 0) {
            continue;
        }

        std::cout << "Line " << i << ": " << std::endl;
        for(UInt j = 0; j < m_ColIndex[i].size(); ++j) {
            //            std::cout << m_ColIndex[i][j] << "(" << glm::to_string(m_ColValue[i][j]) << "), " << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::checkSymmetry(RealType threshold /* = RealType(1e-8) */) const noexcept{
    bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << m_Size << std::endl;

    Scheduler::parallel_for(m_Size, [&](UInt i) {
                                for(UInt j = i + 1; j < m_Size; ++j) {
                                    if(STLHelpers::Sorted::contain(m_ColIndex[i], j)) {
                                        auto errM    = (*this)(i, j) - (*this)(j, i);
                                        RealType err = 0;
                                        for(Int l = 0; l < MatrixType::length(); ++l) {
                                            err += glm::length2(errM[l]);
                                        }
                                        err = std::sqrt(err);

                                        //                                        if(err > threshold) {
                                        //                                            check = false;
                                        //                                            std::cout << "Invalid matrix element at index " << i << ", " << j
                                        //                                                      << ", err = " << err << ": "
                                        //                                                      << "matrix(" << i << ", " << j << ") = " << glm::to_string((*this)(i, j)) << " != "
                                        //                                                      << "matrix(" << j << ", " << i << ") = " << glm::to_string((*this)(j, i)) << std::endl;
                                        //                                        }
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
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const {
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open()) {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }

    UInt onePercent   = static_cast<UInt>(m_Size / 100.0);
    UInt numProcessed = 0;
    UInt numElements  = 0;

    if(showPercentage > 0) {
        for(UInt i = 0; i < m_Size; ++i) {
            for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j) {
                UInt colIndex = m_ColIndex[i][j];

                if(colIndex < i) {
                    continue;
                }

                ++numElements;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt i = 0; i < m_Size; ++i) {
        for(UInt j = 0, jEnd = static_cast<UInt>(m_ColIndex[i].size()); j < jEnd; ++j) {
            for(int l1 = 1; l1 <= 3; ++l1) {
                for(int l2 = 1; l2 <= 3; ++l2) {
                    //                    file << 3 * i + l1 << "    " << 3 * m_ColIndex[i][j] + l2 << "    " << glm::to_string((*this)(i, j)) << std::endl;
                }
            }

            ++numProcessed;
            if((showPercentage > 0) && (numProcessed % (showPercentage * onePercent) == 0)) {
                printf("%u/%u...\n", numProcessed, numElements);
            }
        }
    }
    file.close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::writeBinaryFile(const char* fileName) const {
    FILE* fptr;
#ifdef __BANANA_WINDOWS__
    fopen_s(&fptr, fileName, "wb");
#else
    fptr = fopen(fileName, "wb");
#endif
    if(fptr == nullptr) {
        printf("Cannot open file for writing!\n");
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // write matrix size and size of matrix element
    {
        fwrite(&m_Size,      sizeof(UInt), 1, fptr);
        UInt elementSize = static_cast<UInt>(sizeof(MatrixType));
        fwrite(&elementSize, sizeof(UInt), 1, fptr);
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // write data, row by row
    UInt rowSize;
    for(UInt i = 0; i < m_Size; ++i) {
        __BNN_REQUIRE(m_ColIndex[i].size() == m_ColValue[i].size());
        rowSize = static_cast<UInt>(m_ColIndex[i].size());
        fwrite(&rowSize,             sizeof(UInt),       1,       fptr);
        fwrite(m_ColIndex[i].data(), sizeof(UInt),       rowSize, fptr);
        fwrite(m_ColValue[i].data(), sizeof(MatrixType), rowSize, fptr);
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // finalize
    fclose(fptr);
    printf("Matrix was written to file: %s\n", fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
bool BlockSparseMatrix<MatrixType>::loadFromBinaryFile(const char* fileName) {
    FILE* fptr;
#ifdef _WIN32
    fopen_s(&fptr, fileName, "rb");
#else
    fptr = fopen(fileName, "rb");
#endif
    if(fptr == nullptr) {
        printf("Cannot open file for reading!\n");
        return false;
    }

    clear();
    ////////////////////////////////////////////////////////////////////////////////
    // read matrix size and size of matrix element
    UInt elementSize;
    bool bConsistentSize = true;
    {
        UInt matrixSize;
        fread(&matrixSize,  sizeof(UInt), 1, fptr);
        fread(&elementSize, sizeof(UInt), 1, fptr);
        resize(matrixSize);
        if(elementSize != sizeof(MatrixType)) {
            bConsistentSize = false;
            Int N = MatrixType::length();
            if(elementSize > sizeof(MatrixType)) {
                __BNN_REQUIRE(sizeof(double) * N * N == elementSize);
                __BNN_REQUIRE(sizeof(float) * N * N == sizeof(MatrixType));
            } else {
                __BNN_REQUIRE(sizeof(float) * N * N == elementSize);
                __BNN_REQUIRE(sizeof(double) * N * N == sizeof(MatrixType));
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // read data, row by row
    UInt  rowSize;
    char* buffer = new char[elementSize];
    for(UInt i = 0; i < m_Size; ++i) {
        fread(&rowSize,             sizeof(UInt), 1,       fptr);
        m_ColIndex[i].resize(rowSize);
        fread(m_ColIndex[i].data(), sizeof(UInt), rowSize, fptr);

        m_ColValue[i].resize(rowSize);
        if(bConsistentSize) {
            fread(m_ColValue[i].data(), sizeof(MatrixType), rowSize, fptr);
        } else {
            for(UInt j = 0; j < rowSize; ++j) {
                fread(buffer, elementSize, 1, fptr);
                RealType* dst = glm::value_ptr(m_ColValue[i][j]);
                Int       N   = MatrixType::length();
                if(elementSize > sizeof(MatrixType)) {
                    double* src = reinterpret_cast<double*>(buffer);
                    for(Int k = 0, k_end = N * N; k < k_end; ++k) {
                        dst[k] = static_cast<RealType>(src[k]);
                    }
                } else {
                    float* src = reinterpret_cast<float*>(buffer);
                    for(Int k = 0, k_end = N * N; k < k_end; ++k) {
                        dst[k] = static_cast<RealType>(src[k]);
                    }
                }
            }
        }
    }
    delete[] buffer;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // finalize
    fclose(fptr);
    printf("Matrix was loaded from file: %s\n", fileName);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& matrix) {
    resize(matrix.size());
    m_RowStart[0] = 0;
    for(UInt i = 0; i < m_Size; ++i) {
        m_RowStart[i + 1] = m_RowStart[i] + static_cast<UInt>(matrix.getIndices(i).size());
    }

    m_ColValue.resize(m_RowStart[m_Size]);
    m_ColIndex.resize(m_RowStart[m_Size]);

    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i) {
                                memcpy(&m_ColIndex[m_RowStart[i]], matrix.getIndices(i).data(), matrix.getIndices(i).size() * sizeof(UInt));
                                memcpy(&m_ColValue[m_RowStart[i]], matrix.getValues(i).data(),  matrix.getValues(i).size() * sizeof(MatrixType));
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result) {
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());
    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i) {
                                VectorType tmpResult(0);
                                for(UInt j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j) {
                                    tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
                                }
                                result[i] = tmpResult;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class BlockSparseMatrix<Mat2x2f>;
template class BlockSparseMatrix<Mat3x3f>;

#define __BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(matrixType, indexType)                                                          \
    template const auto& BlockSparseMatrix<matrixType >::operator()(indexType i, indexType j) const;                                \
    template void BlockSparseMatrix<matrixType>::setElement<indexType>(indexType i, indexType j, const matrixType& newValue);       \
    template void BlockSparseMatrix<matrixType>::addElement<indexType>(indexType i, indexType j, const matrixType& incrementValue); \
    template void BlockSparseMatrix<matrixType>::eraseElement<indexType>(indexType i, indexType j);

__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat2x2f, Int);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat2x2f, UInt);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat2x2f, Int64);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat2x2f, UInt64);

__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat3x3f, Int);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat3x3f, UInt);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat3x3f, Int64);
__BNN_INSTANTIATE_BLOCK_SPARSE_MATRIX_FUNCS(Mat3x3f, UInt64);

template class FixedBlockSparseMatrix<Mat2x2f>;
template class FixedBlockSparseMatrix<Mat3x3f>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
