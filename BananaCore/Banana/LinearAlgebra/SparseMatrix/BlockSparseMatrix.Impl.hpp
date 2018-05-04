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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::reserve(UInt size)
{
    m_ColIndex.reserve(m_Size);
    m_ColValue.reserve(m_Size);
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::resize(UInt newSize)
{
    m_Size = newSize;
    m_ColIndex.resize(m_Size);
    m_ColValue.resize(m_Size);
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::clear()
{
    for(UInt i = 0; i < m_Size; ++i) {
        m_ColIndex[i].resize(0);
        m_ColValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
const auto& BlockSparseMatrix<MatrixType>::operator()(IndexType i, IndexType j) const
{
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        return m_ColValue[i][k];
    } else {
        return MatrixType(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
template<class IndexType>
void BlockSparseMatrix<MatrixType>::setElement(IndexType i, IndexType j, const MatrixType& newValue)
{
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
void BlockSparseMatrix<MatrixType>::addElement(IndexType i, IndexType j, const MatrixType& incrementValue)
{
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
void BlockSparseMatrix<MatrixType>::eraseElement(IndexType i, IndexType j)
{
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);
    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        m_ColIndex[i].erase(m_ColIndex[i].begin() + k);
        m_ColValue[i].erase(m_ColValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::printDebug() const noexcept
{
    for(UInt i = 0; i < m_Size; ++i) {
        if(m_ColIndex[i].size() == 0) {
            continue;
        }

        std::cout << "Line " << i << ": " << std::endl;
        for(UInt j = 0; j < m_ColIndex[i].size(); ++j) {
            std::cout << m_ColIndex[i][j] << "(" << glm::to_string(m_ColValue[i][j]) << "), " << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::checkSymmetry(RealType threshold /* = RealType(1e-8) */) const noexcept
{
    bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << m_Size << std::endl;

    Scheduler::parallel_for(m_Size, [&](UInt i)
                            {
                                for(UInt j = i + 1; j < m_Size; ++j) {
                                    if(STLHelpers::Sorted::contain(m_ColIndex[i], j)) {
                                        auto errM    = (*this)(i, j) - (*this)(j, i);
                                        RealType err = 0;
                                        for(Int l = 0; l < N; ++l) {
                                            err += glm::length2(errM[l]);
                                        }
                                        err = sqrt(err);

                                        if(err > threshold) {
                                            check = false;
                                            std::cout << "Invalid matrix element at index " << i << ", " << j
                                                      << ", err = " << err << ": "
                                                      << "matrix(" << i << ", " << j << ") = " << glm::to_string((*this)(i, j)) << " != "
                                                      << "matrix(" << j << ", " << i << ") = " << glm::to_string((*this)(j, i)) << std::endl;
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
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const
{
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
                    file << 3 * i + l1 << "    " << 3 * m_ColIndex[i][j] + l2 << "    " << glm::to_string((*this)(i, j)) << std::endl;
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& matrix)
{
    resize(matrix.size());
    m_RowStart[0] = 0;
    for(UInt i = 0; i < m_Size; ++i) {
        m_RowStart[i + 1] = m_RowStart[i] + static_cast<UInt>(matrix.getIndices(i).size());
    }

    m_ColValue.resize(m_RowStart[m_Size] + 1);
    m_ColIndex.resize(m_RowStart[m_Size] + 1);

    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i)
                            {
                                memcpy(&m_ColIndex[m_RowStart[i]], matrix.getIndices(i).data(), matrix.getIndices(i).size() * sizeof(UInt));
                                memcpy(&m_ColValue[m_RowStart[i]], matrix.getValues(i).data(),  matrix.getValues(i).size() * sizeof(MatrixType));
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const Vector<VectorType>& x, Vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());
    Scheduler::parallel_for(matrix.size(),
                            [&](UInt i)
                            {
                                VectorType tmpResult(0);
                                for(UInt j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j) {
                                    tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
                                }
                                result[i] = tmpResult;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana