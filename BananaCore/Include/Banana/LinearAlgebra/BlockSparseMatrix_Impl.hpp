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


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class MatrixType>
unsigned int BlockSparseMatrix<MatrixType>::size() const noexcept
{
    return m_Size;
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::resize(UInt32 newSize)
{
    m_Size = newSize;
    m_Index.resize(m_Size);
    m_Value.resize(m_Size);
}

template<class MatrixType>
void BlockSparseMatrix<MatrixType>::clear(void)
{
    for(UInt32 i = 0; i < m_Size; ++i)
    {
        m_Index[i].resize(0);
        m_Value[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
const std::vector<UInt32>& BlockSparseMatrix<MatrixType>::getIndices(UInt32 row) const
{
    assert(row < m_Size);
    return m_Index[row];
}

template<class MatrixType>
const std::vector<UInt32>& BlockSparseMatrix<MatrixType>::getValues(UInt32 row) const
{
    assert(row < m_Size);
    return m_Value[row];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
const MatrixType& BlockSparseMatrix<MatrixType>::operator()(UInt32 i, UInt32 j) const
{
    UInt32 k = 0;

    if(STLHelpers::Sorted::contain(m_Index[i], j, k))
    {
        return m_Value[i][k];
    }
    else
    {
        return MatrixType::Zero();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::setElement(UInt32 i, UInt32 j, const MatrixType& newValue)
{
    assert(i < m_Size && j < m_Size);

    UInt32 k = 0;

    if(STLHelpers::Sorted::contain(m_Index[i], j, k))
    {
        m_Value[i][k] = newValue;
    }
    else
    {
        STLHelpers::Sorted::insertPairSorted(m_Index[i], j, m_Value[i], newValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::addElement(UInt32 i, UInt32 j, const MatrixType& incrementValue)
{
    assert(i < m_Size && j < m_Size);

    UInt32 k = 0;

    if(STLHelpers::Sorted::contain(m_Index[i], j, k))
    {
        m_Value[i][k] += incrementValue;
    }
    else
    {
        STLHelpers::insertPairSorted(m_Index[i], j, m_Value[i], incrementValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::eraseElement(UInt32 i, UInt32 j)
{
    assert(i < m_Size && j < m_Size);

    UInt32 k = 0;

    if(STLHelpers::Sorted::contain(m_Index[i], j, k))
    {
        m_Index[i].erase(m_Index[i].begin() + k);
        m_Value[i].erase(m_Value[i].begin() + k);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::printDebug() const noexcept
{
    for(UInt32 i = 0; i < m_Size; ++i)
    {
        if(m_Index[i].size() == 0)
        {
            continue;
        }

        std::cout << "Line " << i << ": " << std::endl;

        for(UInt32 j = 0; j < m_Index[i].size(); ++j)
        {
            std::cout << m_Index[i][j] << "(" << glm::to_string(m_Value[i][j]) << "), " << std::endl;
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::checkSymmetry() const noexcept
{
    volatile bool check = true;
    std::cout << "============================== Checking Matrix Symmetry... ==============================" << std::endl;
    std::cout << "Matrix size: " << m_Size << std::endl;

    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_Size), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              for(UInt32 j = i + 1; j < m_Size; ++j)
                              {
                                  if(STLHelpers::Sorted::contain(m_Index[i], j))
                                  {
                                      auto err = glm::length2((*this)(i, j) - (*this)(j, i));

                                      if(err > 1e-5)
                                      {
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
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const
{
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open())
    {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }

    UInt32 onePercent   = static_cast<UInt32>(numElements / 100.0);
    UInt32 numProcessed = 0;
    UInt32 numElements  = 0;

    if(showPercentage > 0)
    {
        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = 0, jEnd = static_cast<UInt32>(m_Index[i].size()); j < jEnd; ++j)
            {
                UInt32 colIndex = m_Index[i][j];

                if(colIndex < i)
                {
                    continue;
                }

                ++numElements;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt32 i = 0; i < m_Size; ++i)
    {
        for(UInt32 j = 0, jEnd = static_cast<UInt32>(m_Index[i].size()); j < jEnd; ++j)
        {
            auto matElement = m_Value[i][j];

            for(int l1 = 1; l1 <= 3; ++l1)
            {
                for(int l2 = 1; l2 <= 3; ++l2)
                {
                    file << 3 * i + l1 << "    " << 3 * m_Index[i][j] + l2 << "    " << matElement(i, j) << std::endl;
                }
            }

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
template<class MatrixType>
void BlockSparseMatrix<MatrixType>::writeBinaryFile(const char* fileName, int showPercentage /*= -1*/) const
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    if(!file.is_open())
    {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }


    UInt32 numElements = 0;

    for(UInt32 i = 0; i < m_Size; ++i)
    {
        for(UInt32 j = 0, jEnd = static_cast<UInt32>(m_Index[i].size()); j < jEnd; ++j)
        {
            UInt32 colIndex = m_Index[i][j];

            if(colIndex < i)
            {
                continue;
            }

            ++numElements;
        }
    }


    UInt32              onePercent   = static_cast<UInt32>(numElements / 100.0);
    UInt32              numProcessed = 0;
    std::vector<double> data(9 * numElements);
    std::vector<UInt32> rowIndex(numElements);
    std::vector<UInt32> colIndex(numElements);

    for(UInt32 i = 0; i < m_Size; ++i)
    {
        for(UInt32 j = 0, jEnd = static_cast<UInt32>(m_Index[i].size()); j < jEnd; ++j)
        {
            UInt32 colIndex = m_Index[i][j];

            if(colIndex < i)
            {
                continue;
            }

            const auto* tmp_ptr = glm::value_ptr(m_Value[i][j]);
            for(int k = 0; k < 9; ++k)
                data[9 * numProcessed + k] = static_cast<double>(tmp_ptr[k]);

            rowIndex[numProcessed] = i;
            colIndex[numProcessed] = colIndex;

            ++numProcessed;

            if((showPercentage > 0) && (numProcessed % (showPercentage * onePercent) == 0))
            {
                printf("%u/%u...\n", numProcessed, numElements);
            }
        }
    }

    file.write(&m_Size,         sizeof(UInt32));
    file.write(&numElements,    sizeof(UInt32));
    file.write(rowIndex.data(), numElements * sizeof(UInt32));
    file.write(colIndex.data(), numElements * sizeof(UInt32));
    file.write(data.data(),     9 * numElements * sizeof(double));

    file.close();
    printf("File written, num. elements: %u, filename: %s\n", numElements, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
bool BlockSparseMatrix<MatrixType>::loadFromBinaryFile(const char* fileName, int showPercentage /*= -1*/)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        printf("Cannot open file for reading!\n");
        return false;
    }

    UInt32            fileSize = (UInt32)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), fileSize);
    file.close();


    ////////////////////////////////////////////////////////////////////////////////
    UInt32 newSize     = 0;
    UInt32 numElements = 0;

    memcpy(&newSize,     buffer.data(),                  sizeof(UInt32));
    memcpy(&numElements, &buffer.data()[sizeof(UInt32)], sizeof(UInt32));
    UInt32* row_ptr    = reinterpret_cast<UInt32*>(&buffer.data()[sizeof(UInt32) * 2]);
    UInt32* column_ptr = reinterpret_cast<UInt32*>(&buffer.data()[(numElements + 2) * sizeof(UInt32)]);
    double* data_ptr   = reinterpret_cast<double*>(&buffer.data()[(numElements * 2 + 2) * sizeof(UInt32)]);

    resize(newSize);
    UInt32 onePercent   = static_cast<UInt32>(numElements / 100.0);
    UInt32 numProcessed = 0;

    for(UInt32 k = 0; k < numElements; ++k)
    {
        MatrixType tmp;

        auto* data = glm::value_ptr(tmp);

        for(int l = 0; l < 9; ++l)
        {
            data[l] = data_ptr[9 * numProcessed + l];
        }

        setElement(row_ptr[numProcessed],    column_ptr[numProcessed], tmp);
        setElement(column_ptr[numProcessed], row_ptr[numProcessed],    tmp);

        ++numProcessed;

        if((showPercentage > 0) && (numProcessed % (showPercentage * onePercentage) == 0))
        {
            printf("%u/%u...\n", numProcessed, numElements);
        }
    }

    printf("File read, num. elements: %u, filename: %s\n", numElements, fileName);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType>
template<class VectorType>
void BlockSparseMatrix<MatrixType>::multiply(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = VectorType::Zero();

                              for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
                              {
                                  tmpResult += matrix.m_Value[i][j] * x[matrix.m_Index[i][j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=alpha*matrix*x
template<class MatrixType>
template<class VectorType, class ScalarType>
void BlockSparseMatrix<MatrixType>::multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, const ScalarType alpha, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = VectorType::Zero();

                              for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
                              {
                                  tmpResult += matrix.m_Value[i][j] * x[matrix.m_Index[i][j]] * alpha;
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result+=alpha*matrix*x
template<class MatrixType>
template<class VectorType, class ScalarType>
void BlockSparseMatrix<MatrixType>::add_multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, const ScalarType alpha, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = VectorType::Zero();

                              for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
                                  Z
                                  {
                                      tmpResult += matrix.m_Value[i][j] * x[matrix.m_Index[i][j]] * alpha;
                                  }

                                  result[i] += tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
template<class MatrixType>
template<class VectorType>
void BlockSparseMatrix<MatrixType>::multiply_and_subtract(const BlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = result[i];

                              for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
                              {
                                  tmpResult -= matrix.m_Value[i][j] * x[matrix.m_Index[i][j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform mat = A + alpha*B
template<class MatrixType>
template<class ScalarType>
void BlockSparseMatrix<MatrixType>::add_scaled(const BlockSparseMatrix<MatrixType>& A, const BlockSparseMatrix<MatrixType>& B, const ScalarType alpha, BlockSparseMatrix<MatrixType>& matrix)
{
    assert(A.size() == B.size());
    assert(A.size() == matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, A.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              assert(A.m_Index[i].size() == B.m_Index[i].size());

                              for(UInt32 j = 0, jEnd = A.m_Index[i].size(); j < jEnd; ++j)
                              {
                                  matrix.setElement(i, A.m_Index[i][j], A.m_Value[i][j] + B.m_Value[i][j] * alpha);
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::clear(void)
{
    m_Value.resize(0);
    m_Index.resize(0);
    m_RowStart.resize(0);
}

template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::resize(UInt32 newSize)
{
    m_Size = newSize;
    m_RowStart.resize(m_Size + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class MatrixType>
void FixedBlockSparseMatrix<MatrixType>::constructFromSparseMatrix(const BlockSparseMatrix<MatrixType>& matrix)
{
    resize(matrix.size());
    m_RowStart[0] = 0;

    for(UInt32 i = 0; i < m_Size; ++i)
    {
        m_RowStart[i + 1] = m_RowStart[i] + static_cast<UInt32>(matrix.getIndices(i).size());
    }

    m_Value.resize(m_RowStart[m_Size]);
    m_Index.resize(m_RowStart[m_Size]);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              memcpy(&m_Index[m_RowStart[i]], matrix.getIndices(i).data(), matrix.getIndices(i).size() * sizeof(UInt32));
                          }

                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              memcpy(&m_Value[m_RowStart[i]], matrix.getValues(i).data(), matrix.getValues(i).size() * sizeof(MatrixType));
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType>
template<class VectorType>
void FixedBlockSparseMatrix<MatrixType>::multiply(const FixedBlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32>& r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = VectorType(0);

                              for(UInt32 j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j)
                              {
                                  tmpResult += matrix.m_Value[j] * x[matrix.m_Index[j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
template<class MatrixType>
template<class VectorType>
void FixedBlockSparseMatrix<MatrixType>::multiply_and_subtract(const FixedBlockSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());


    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
                          {
                              VectorType tmpResult = result[i];

                              for(UInt32 j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j)
                              {
                                  tmpResult -= matrix.m_Value[j] * x[matrix.m_Index[j]];
                              }

                              result[i] = tmpResult;
                          }
                      }, ap); // end parallel_for
}
