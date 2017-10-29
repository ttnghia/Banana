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
template<Int N, class RealType>
void BlockSparseMatrix<N, RealType >::resize(UInt newSize)
{
    m_Size = newSize;
    m_ColIndex.resize(m_Size);
    m_ColValue.resize(m_Size);
}

template<Int N, class RealType>
void BlockSparseMatrix<N, RealType >::clear(void)
{
    for(UInt i = 0; i < m_Size; ++i) {
        m_ColIndex[i].resize(0);
        m_ColValue[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class IndexType>
const MatXxX<N, RealType>& BlockSparseMatrix<N, RealType>::operator ()(IndexType i, IndexType j) const
{
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        return m_ColValue[i][k];
    } else {
        return MatXxX<N, RealType>(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class IndexType>
void BlockSparseMatrix<N, RealType >::setElement(IndexType i, IndexType j, const MatXxX<N, RealType>& newValue)
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
template<Int N, class RealType>
template<class IndexType>
void BlockSparseMatrix<N, RealType >::addElement(IndexType i, IndexType j, const MatXxX<N, RealType>& incrementValue)
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
template<Int N, class RealType>
template<class IndexType>
void BlockSparseMatrix<N, RealType >::eraseElement(IndexType i, IndexType j)
{
    assert(static_cast<UInt>(i) < m_Size && static_cast<UInt>(j) < m_Size);

    UInt k = 0;
    if(STLHelpers::Sorted::contain(m_ColIndex[i], static_cast<UInt>(j), k)) {
        m_ColIndex[i].erase(m_ColIndex[i].begin() + k);
        m_ColValue[i].erase(m_ColValue[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlockSparseMatrix<N, RealType >::printDebug() const noexcept
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
template<Int N, class RealType>
void BlockSparseMatrix<N, RealType >::checkSymmetry() const noexcept
{
    volatile bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << m_Size << std::endl;

    ParallelFuncs::parallel_for(ParallelFuncs::blocked_range<UInt>(0, m_Size), [&](ParallelFuncs::blocked_range<UInt> r)
                                {
                                    for(UInt i = r.begin(), iEnd = r.end(); i != iEnd; ++i) {
                                        for(UInt j = i + 1; j < m_Size; ++j) {
                                            if(STLHelpers::Sorted::contain(m_ColIndex[i], j)) {
                                                auto err = glm::length2((*this)(i, j) - (*this)(j, i));

                                                if(err > 1e-5) {
                                                    check = false;
                                                    std::cout << "Invalid matrix element at index " << i << ", " << j
                                                              << ", err = " << err << ": "
                                                              << "matrix(" << i << ", " << j << ") = " << glm::to_string((*this)(i, j)) << " != "
                                                              << "matrix(" << j << ", " << i << ") = " << glm::to_string((*this)(j, i)) << std::endl;
                                                }
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
template<Int N, class RealType>
void BlockSparseMatrix<N, RealType >::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const
{
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open()) {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }

    UInt onePercent   = static_cast<UInt>(numElements / 100.0);
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
            auto matElement = m_ColValue[i][j];

            for(int l1 = 1; l1 <= 3; ++l1) {
                for(int l2 = 1; l2 <= 3; ++l2) {
                    file << 3 * i + l1 << "    " << 3 * m_ColIndex[i][j] + l2 << "    " << matElement(i, j) << std::endl;
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
template<Int N, class RealType>
void FixedBlockSparseMatrix<N, RealType >::constructFromSparseMatrix(const BlockSparseMatrix<N, RealType>& matrix)
{
    resize(matrix.size());

    m_RowStart[0] = 0;
    for(UInt i = 0; i < m_Size; ++i) {
        m_RowStart[i + 1] = m_RowStart[i] + static_cast<UInt>(matrix.getIndices(i).size());
    }

    m_ColValue.resize(m_RowStart[m_Size]);
    m_ColIndex.resize(m_RowStart[m_Size]);

    ParallelFuncs::parallel_for(matrix.size(),
                                [&](UInt i)
                                {
                                    memcpy(&m_ColIndex[m_RowStart[i]], matrix.getIndices(i).data(), matrix.getIndices(i).size() * sizeof(UInt));
                                    memcpy(&m_ColValue[m_RowStart[i]], matrix.getValues(i).data(),  matrix.getValues(i).size() * sizeof(MatXxX<N, RealType> ));
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<Int N, class RealType>
void FixedBlockSparseMatrix<N, RealType >::multiply(const FixedBlockSparseMatrix<N, RealType>& matrix, const Vector<VecX<N, RealType> >& x, Vector<VecX<N, RealType> >& result)
{
    assert(matrix.size() == static_cast<UInt>(x.size()));
    result.resize(matrix.size());

    ParallelFuncs::parallel_for(matrix.size(),
                                [&](UInt i)
                                {
                                    VecX<N, RealType> tmpResult(0);
                                    for(UInt j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j) {
                                        tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
                                    }
                                    result[i] = tmpResult;
                                });
}
