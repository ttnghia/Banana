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
template<class RealType>
unsigned int SparseMatrix<RealType>::size() const noexcept
{
    return m_Size;
}

template<class RealType>
void SparseMatrix<RealType>::resize(UInt32 newSize)
{
    m_Size = newSize;
    m_Index.resize(m_Size);
    m_Value.resize(m_Size);
}

template<class RealType>
void SparseMatrix<RealType>::clear(void)
{
    for(UInt32 i = 0; i < m_Size; ++i)
    {
        m_Index[i].resize(0);
        m_Value[i].resize(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
const std::vector<UInt32>& SparseMatrix<RealType>::getIndices(UInt32 row) const
{
    assert(row < m_Size);
    return m_Index[row];
}

template<class RealType>
const std::vector<UInt32>& SparseMatrix<RealType>::getValues(UInt32 row) const
{
    assert(row < m_Size);
    return m_Value[row];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType SparseMatrix<RealType>::operator()(UInt32 i, UInt32 j) const
{
    UInt32 k = 0;

    if(STLHelpers::Sorted::contain(m_Index[i], j, k))
    {
        return m_Value[i][k];
    }
    else
    {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseMatrix<RealType>::setElement(UInt32 i, UInt32 j, RealType newValue)
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
template<class RealType>
void SparseMatrix<RealType>::addElement(UInt32 i, UInt32 j, RealType incrementValue)
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
template<class RealType>
void SparseMatrix<RealType>::eraseElement(UInt32 i, UInt32 j)
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
template<class RealType>
void SparseMatrix<RealType>::printDebug() const noexcept
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
            std::cout << m_Index[i][j] << "(" << m_Value[i][j] << "), " << std::endl;
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SparseMatrix<RealType>::checkSymmetry() const noexcept
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
template<class RealType>
void SparseMatrix<RealType>::writeMatlabFile(const char* fileName, int showPercentage /*= -1*/) const
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
            file << i + 1 << "    " << m_Index[i][j] + 1 << "    " << m_Value[i][j] << std::endl;

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
template<class RealType>
void SparseMatrix<RealType>::writeBinaryFile(const char* fileName, int showPercentage /*= -1*/) const
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
    std::vector<UInt32> rowIndex(numElements);
    std::vector<UInt32> colIndex(numElements);
    std::vector<double> data(numElements);

    for(UInt32 i = 0; i < m_Size; ++i)
    {
        for(UInt32 j = 0, jEnd = static_cast<UInt32>(m_Index[i].size()); j < jEnd; ++j)
        {
            UInt32 colIndex = m_Index[i][j];

            if(colIndex < i)
            {
                continue;
            }

            rowIndex[numProcessed] = i;
            colIndex[numProcessed] = colIndex;
            data[numProcessed]     = static_cast<double>(m_Value[i][j]);

            ++numProcessed;

            if((showPercentage > 0) && (numProcessed % (showPercentage * onePercentage) == 0))
            {
                printf("%u/%u...\n", numProcessed, numElements);
            }
        }
    }

    file.write(&m_Size,         sizeof(UInt32));
    file.write(&numElements,    sizeof(UInt32));
    file.write(rowIndex.data(), numElements * sizeof(UInt32));
    file.write(colIndex.data(), numElements * sizeof(UInt32));
    file.write(data.data(),     numElements * sizeof(double));

    file.close();
    printf("File written, num. elements: %u, filename: %s\n", numElements, fileName);
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool SparseMatrix<RealType>::loadFromBinaryFile(const char* fileName, int showPercentage /*= -1*/)
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
template<class RealType>
void SparseMatrix<RealType>::multiply(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
    {
        for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmpResult = 0;

            for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
            {
                tmpResult += matrix.m_Value[i][j] * x[matrix.m_Index[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=result-matrix*x
template<class RealType>
void SparseMatrix<RealType>::multiply_and_subtract(const SparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
    {
        for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmpResult = result[i];

            for(UInt32 j = 0, jEnd = static_cast<UInt32>(matrix.m_Index[i].size()); j < jEnd; ++j)
            {
                tmpResult -= matrix.m_Value[i][j] * x[matrix.m_Index[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Fixed version of SparseMatrix
//
template<class RealType>
void FixedSparseMatrix<RealType>::clear(void)
{
    m_Value.resize(0);
    m_Index.resize(0);
    m_RowStart.resize(0);
}

template<class RealType>
void FixedSparseMatrix<RealType>::resize(UInt32 newSize)
{
    m_Size = newSize;
    m_RowStart.resize(m_Size + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void FixedSparseMatrix<RealType>::constructFromSparseMatrix(const SparseMatrix<RealType>& matrix)
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
            memcpy(&m_Value[m_RowStart[i]], matrix.getValues(i).data(), matrix.getValues(i).size() * sizeof(RealType));
        }
    }, ap);     // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class RealType>
template<class RealType>
void FixedSparseMatrix<RealType>::multiply(const FixedSparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32>& r)
    {
        for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmpResult = 0;

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
template<class RealType>
template<class RealType>
void FixedSparseMatrix<RealType>::multiply_and_subtract(const FixedSparseMatrix<RealType>& matrix, const std::vector<RealType>& x, std::vector<RealType>& result)
{
    assert(matrix.size() == static_cast<UInt32>(x.size()));
    result.resize(matrix.size());


    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, matrix.size()), [&](tbb::blocked_range<UInt32> r)
    {
        for(UInt32 i = r.begin(), iEnd = r.end(); i != iEnd; ++i)
        {
            RealType tmpResult = result[i];

            for(UInt32 j = matrix.m_RowStart[i], jEnd = matrix.m_RowStart[i + 1]; j < jEnd; ++j)
            {
                tmpResult -= matrix.m_Value[j] * x[matrix.m_Index[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}
