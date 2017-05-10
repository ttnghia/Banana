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

#include <algorithm>
#include <iostream>
#include <vector>

#include <Noodle/Core/Global/TypeNames.h>
#include <Noodle/Core/Math/MathUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename MatrixType>
inline typename std::vector<MatrixType>::iterator insert_sorted(std::vector<MatrixType>& vec, const MatrixType& item)
{
    return vec.insert(std::upperBound(vec.begin(), vec.end(), item), item);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class MatrixType>
struct BlockSparseMatrix
{
    // dimension
    UInt32 size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt32> > m_ColIndex;

    // values corresponding to index
    std::vector<std::vector<MatrixType> > m_ColValue;

    explicit BlockSparseMatrix(UInt32 n = 0) : size(n), m_ColIndex(n), m_ColValue(n)
    {}

    void clear(void)
    {
        size = 0;

        for(size_t i = 0; i < m_ColIndex.size(); ++i)
        {
            m_ColIndex[i].clear();
        }

        m_ColIndex.clear();

        for(size_t i = 0; i < m_ColValue.size(); ++i)
        {
            m_ColValue[i].clear();
        }

        m_ColValue.clear();
    }

    void zero(void)
    {
        for(UInt32 i = 0; i < size; ++i)
        {
            m_ColIndex[i].resize(0);
            m_ColValue[i].resize(0);
        }
    }

    void resize(UInt32 n)
    {
        size = n;
        m_ColIndex.resize(size);
        m_ColValue.resize(size);
    }

    const MatrixType operator()(UInt32 i, UInt32 j) const
    {
        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            return m_ColValue[i][iter - m_ColIndex[i].begin()];
        }
        else
        {
            return MatrixType::Zero();
        }
    }

    void set_element(UInt32 i, UInt32 j, const MatrixType& new_value)
    {
        assert(i < size && j < size);

        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            m_ColValue[i][iter - m_ColIndex[i].begin()] = new_value;
        }
        else
        {
            auto   iter = insert_sorted(m_ColIndex[i], j);
            size_t k = iter - m_ColIndex[i].begin();
            m_ColValue[i].insert(m_ColValue[i].begin() + k, new_value);
        }
    }


    void add_to_element(UInt32 i, UInt32 j, const MatrixType& increment_value)
    {
        assert(i < size && j < size);

        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            m_ColValue[i][iter - m_ColIndex[i].begin()] += increment_value;
            return;
        }
        else
        {
            auto   iter = insert_sorted(m_ColIndex[i], j);
            size_t k = iter - m_ColIndex[i].begin();
            m_ColValue[i].insert(m_ColValue[i].begin() + k, increment_value);
            return;
        }
    }

    void erase_element(UInt32 i, UInt32 j)
    {
        auto   iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);
        UInt32 k = iter - m_ColIndex[i].begin();

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            m_ColIndex[i].erase(iter);
            m_ColValue[i].erase(m_ColValue[i].begin() + k);
        }
    }

    bool has_data(UInt32 i, UInt32 j)
    {
        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);
        //        UInt32 k = iter - index[i].begin();

        return ((iter != m_ColIndex[i].end()) && (*iter == j));
    }


    // assumes matrix has symmetric structure - so the indices in row i tell us which columns to delete i from
    void symmetric_remove_row_and_column(UInt32 i)
    {
        for(UInt32 a = 0; a < m_ColIndex[i].size(); ++a)
        {
            UInt32 j = m_ColIndex[i][a]; //

            for(UInt32 b = 0; b < m_ColIndex[j].size(); ++b)
            {
                if(m_ColIndex[j][b] == i)
                {
                    MathHelpers::erase(m_ColIndex[j], b);
                    MathHelpers::erase(m_ColValue[j], b);
                    break;
                }
            }
        }

        m_ColIndex[i].resize(0);
        m_ColValue[i].resize(0);
    }

    void write_matlab(std::ostream& output, const char* variable_name)
    {
        output << variable_name << "=sparse([";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                output << m_ColIndex[i][j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColValue[i].size(); ++j)
            {
                output << m_ColValue[i][j] << " ";
            }
        }

        output << "], " << size << ", " << size << ");" << std::endl;
    }



    void print_debug()
    {
        for(UInt32 i = 0; i < size; ++i)
        {
            if(m_ColIndex[i].size() == 0)
            {
                continue;
            }

            std::cout << "Line " << i << ": " << std::endl;

            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                std::cout << m_ColIndex[i][j] << "(" << m_ColValue[i][j] << "), " << std::endl;
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    void check_symmetry()
    {
        volatile bool check = true;
        std::cout <<
            "============================== Checking Matrix Symmetry... =============================="
            <<
            std::endl;
        std::cout << "Matrix size: " << size << std::endl;

        tbb::parallel_for(tbb::blocked_range<size_t>(0, size),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
            {
                for(size_t j = i + 1; j < size; ++j)
                {
                    if(has_data(i, j))
                    {
                        auto err = glm::length2((*this)(i, j) - (*this)(j, i));

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

    void write_sparse_matrix_file(const char* fileName)
    {
        std::ofstream file;
        file.open(fileName);
        if(!file.is_open())
        {
            printf("Cannot open file for writing!\n");
            return;
        }

        UInt32 one_percentage = (UInt32)(size / 100);
        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                UInt32            colIndex = m_ColIndex[i][j];
                const MatrixType& tmp = m_ColValue[i][j];
                const Real*       tmp_ptr = glm::value_ptr(tmp);

                for(UInt32 k = 0; k < 9; ++k)
                {
                    file << (i + 1) << "    " << (colIndex + 1) << "    " << tmp_ptr[k] << std::endl;
                }
            }

            if(i % one_percentage == 0)
            {
                printf("%u/%u...\n", i, size);
            }
        }

        file.close();
    }

    void write_to_file(const char* fileName)
    {
        FILE* fptr = fopen(fileName, "wb");

        if(!fptr)
        {
            printf("Cannot open file for writing!\n");
            return;
        }

        UInt32 number_elements = 0;

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                UInt32 colIndex = m_ColIndex[i][j];

                if(colIndex < i)
                {
                    continue;
                }

                ++number_elements;
            }
        }


        UInt32  one_percentage = (UInt32)(number_elements / 100);
        UInt32  num_processed = 0;
        float*  data_ptr = new float[9 * number_elements];
        UInt32* row_ptr = new UInt32[number_elements];
        UInt32* column_ptr = new UInt32[number_elements];

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                UInt32 colIndex = m_ColIndex[i][j];

                if(colIndex < i)
                {
                    continue;
                }

                const MatrixType& tmp = m_ColValue[i][j];

                const Real*       tmp_ptr = glm::value_ptr(tmp);
                data_ptr[9 * num_processed + 0] = tmp_ptr[0];
                data_ptr[9 * num_processed + 1] = tmp_ptr[1];
                data_ptr[9 * num_processed + 2] = tmp_ptr[2];

                data_ptr[9 * num_processed + 3] = tmp_ptr[3];
                data_ptr[9 * num_processed + 4] = tmp_ptr[4];
                data_ptr[9 * num_processed + 5] = tmp_ptr[5];

                data_ptr[9 * num_processed + 6] = tmp_ptr[6];
                data_ptr[9 * num_processed + 7] = tmp_ptr[7];
                data_ptr[9 * num_processed + 8] = tmp_ptr[8];

                row_ptr[num_processed] = i;
                column_ptr[num_processed] = colIndex;

                ++num_processed;

                //                if(num_processed % one_percentage == 0)
                //                {
                //                    printf("%u/%u...\n", num_processed, number_elements);
                //                }
            }
        }

        fwrite(&number_elements, 1, sizeof(UInt32), fptr);
        fwrite(row_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fwrite(column_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fwrite(data_ptr, 1, 9 * number_elements * sizeof(float), fptr);
        fclose(fptr);
        printf("File written, num. elements: %u, filename: %s\n", number_elements, fileName);

        delete[] row_ptr;
        delete[] column_ptr;
        delete[] data_ptr;
    }


    bool load_from_file(const char* fileName, int showPercentage = -1)
    {
        zero();

        FILE* fptr;
#ifdef _WIN32
        fopen_s(&fptr, fileName, "rb");
#else
        fptr = fopen(fileName, "rb");
#endif
        if(!fptr)
        {
            printf("Cannot open file for reading!\n");
            return false;
        }

        UInt32 number_elements = 0;

        fread(&number_elements, 1, sizeof(UInt32), fptr);
        float*  data_ptr = new float[9 * number_elements];
        UInt32* row_ptr = new UInt32[number_elements];
        UInt32* column_ptr = new UInt32[number_elements];

        fread(row_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fread(column_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fread(data_ptr, 1, 9 * number_elements * sizeof(float), fptr);

        UInt32 onePercentage = (UInt32)(number_elements / 100);
        UInt32 num_processed = 0;

        for(UInt32 k = 0; k < number_elements; ++k)
        {
            MatrixType tmp;

            Real*      data_mat = glm::value_ptr(tmp);

            for(int l = 0; l < 9; ++l)
            {
                data_mat[l] = data_ptr[9 * num_processed + l];
            }

            set_element(row_ptr[num_processed], column_ptr[num_processed], tmp);
            set_element(column_ptr[num_processed], row_ptr[num_processed], tmp);

            ++num_processed;

            if((showPercentage > 0) && (num_processed % (showPercentage * onePercentage) == 0))
            {
                printf("%u/%u...\n", num_processed, number_elements);
            }
        }

        fclose(fptr);
        printf("File read, num. elements: %u, filename: %s\n", number_elements, fileName);

        delete[] row_ptr;
        delete[] column_ptr;
        delete[] data_ptr;

        return true;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x

template<class MatrixType, class VectorType>
void multiply(const BlockSparseMatrix<MatrixType>& matrix,
              const std::vector<VectorType>&       x,
              std::vector<VectorType>&             result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0, jend = matrix.m_ColIndex[i].size(); j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result=alpha*matrix*x
template<class MatrixType, class VectorType, class ScalarType>
void multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix,
                     const std::vector<VectorType>&       x,
                     const ScalarType                     alpha,
                     std::vector<VectorType>&             result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0, jend = matrix.m_ColIndex[i].size(); j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]] * alpha;
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result+=alpha*matrix*x
template<class MatrixType, class VectorType, class ScalarType>
void add_multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix,
                         const std::vector<VectorType>&       x,
                         const ScalarType                     alpha,
                         std::vector<VectorType>&             result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0, jend = matrix.m_ColIndex[i].size(); j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]] * alpha;
            }

            result[i] += tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result=result-matrix*x
template<class MatrixType, class VectorType>
void multiply_and_subtract(const BlockSparseMatrix<MatrixType>& matrix,
                           const std::vector<VectorType>&       x,
                           std::vector<VectorType>&             result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = result[i];

            for(size_t j = 0, jend = matrix.m_ColIndex[i].size(); j < jend; ++j)
            {
                tmpResult -= matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform mat = A + alpha*B
template<class MatrixType, class VectorType, class ScalarType>
void add_scaled(const BlockSparseMatrix<MatrixType>& A,
                const BlockSparseMatrix<MatrixType>& B,
                const ScalarType                     alpha,
                BlockSparseMatrix<MatrixType>&       matrix)
{
    __NOODLE_ASSERT(A.size == B.size);
    __NOODLE_ASSERT(A.size == matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            __NOODLE_ASSERT(A.m_ColIndex[i].size() == B.m_ColIndex[i].size());

            for(size_t j = 0, jend = A.m_ColIndex[i].size(); j < jend; ++j)
            {
                matrix.set_element(i, A.m_ColIndex[i][j], A.m_ColValue[i][j] + B.m_ColValue[i][j] * alpha);
            }
        }
    }, ap); // end parallel_for
}

//============================================================================
// Fixed version of SparseMatrix. This is not a good structure for dynamically
// modifying the matrix, but can be significantly faster for matrix-vector
// multiplies due to better data locality.

template<class MatrixType>
struct BlockFixedSparseMatrix
{
    // dimension
    UInt32 m_Size;

    // nonzero values row by row
    std::vector<MatrixType> m_ColValue;

    // corresponding column indices
    std::vector<UInt32> m_ColIndex;

    // where each row starts in value and colindex (and last entry is one past the end, the number of nonzeros)
    std::vector<UInt32> m_RowStart;

    explicit BlockFixedSparseMatrix(UInt32 n_ = 0) : m_Size(n_), m_ColValue(0), m_ColIndex(0), m_RowStart(n_ + 1)
    {}

    void clear(void)
    {
        m_Size = 0;
        m_ColValue.clear();
        m_ColIndex.clear();
        m_RowStart.clear();
    }

    void resize(int n_)
    {
        m_Size = n_;
        m_RowStart.resize(m_Size + 1);
    }

    void construct_from_matrix(const BlockSparseMatrix<MatrixType>& matrix)
    {
        resize(matrix.size);
        m_RowStart[0] = 0;

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            m_RowStart[i + 1] = m_RowStart[i] + matrix.m_ColIndex[i].size();
        }

        m_ColValue.resize(m_RowStart[m_Size]);
        m_ColIndex.resize(m_RowStart[m_Size]);

        static tbb::affinity_partitioner ap;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size), [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
            {
                for(UInt32 k = 0, ks = matrix.m_ColIndex[i].size(); k < ks; ++k)
                {
                    m_ColValue[m_RowStart[i] + k] = matrix.m_ColValue[i][k];
                    m_ColIndex[m_RowStart[i] + k] = matrix.m_ColIndex[i][k];
                }
            }
        }, ap); // end parallel_for
    }

    void write_matlab(std::ostream& output, const char* variable_name)
    {
        output << variable_name << "=sparse([";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = m_RowStart[i]; j < m_RowStart[i + 1]; ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = m_RowStart[i]; j < m_RowStart[i + 1]; ++j)
            {
                output << m_ColIndex[j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = m_RowStart[i]; j < m_RowStart[i + 1]; ++j)
            {
                output << m_ColValue[j] << " ";
            }
        }

        output << "], " << m_Size << ", " << m_Size << ");" << std::endl;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType, class VectorType>
void multiply(const BlockFixedSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    assert(matrix.m_Size == x.size());
    //    result.resize(matrix.n);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = VectorType(0);

            for(UInt32 j = matrix.m_RowStart[i], jend = matrix.m_RowStart[i + 1]; j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result=result-matrix*x
template<class MatrixType, class VectorType>
void multiply_and_subtract(const BlockFixedSparseMatrix<MatrixType>& matrix, const std::vector<VectorType>& x, std::vector<VectorType>& result)
{
    __NOODLE_ASSERT(matrix.m_Size == x.size());
    //    result.resize(matrix.n);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            VectorType tmpResult = result[i];

            for(UInt32 j = matrix.m_RowStart[i], jend = matrix.m_RowStart[i + 1]; j < jend; ++j)
            {
                tmpResult -= matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}
