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

#include <BananaCore/TypeNames.h>
#include <BananaCore/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
inline typename std::vector<T>::iterator insert_sorted_vec(std::vector<T>& vec, T const& item)
{
    return vec.insert(std::upperBound(vec.begin(), vec.end(), item), item);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class T>
struct SparseMatrix
{
    // dimension
    UInt32 m_Size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt32> > m_ColIndex;

    // values corresponding to index
    std::vector<std::vector<T> > m_ColValue;

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    explicit SparseMatrix(UInt32 size_ = 0, UInt32 nonzeros_per_row_ = 0) : m_Size(size_), m_ColIndex(size_), m_ColValue(size_)
    {
        if(nonzeros_per_row_ > 0)
        {
            for(UInt32 i = 0; i < m_Size; ++i)
            {
                m_ColIndex[i].reserve(nonzeros_per_row_);
                m_ColValue[i].reserve(nonzeros_per_row_);
            }
        }
    }

    void clear()
    {
        m_Size = 0;

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

    void zero()
    {
        for(UInt32 i = 0; i < m_Size; ++i)
        {
            m_ColIndex[i].resize(0);
            m_ColValue[i].resize(0);
        }
    }

    void resize(int size_)
    {
        m_Size = size_;
        m_ColIndex.resize(m_Size);
        m_ColValue.resize(m_Size);
    }

    T operator()(UInt32 i, UInt32 j) const
    {
        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            return m_ColValue[i][iter - m_ColIndex[i].begin()];
        }
        else
        {
            return 0;
        }
    }

    //    void set_element_symmetric(UInt32 i, UInt32 j, T new_value)
    //    {
    //        auto iter = std::lowerBound(index[i].begin(), index[i].end(), j);

    //        if((iter != index[i].end()) && (*iter == j))
    //        {
    //            value[i][iter - index[i].begin()] = new_value;
    //        }
    //        else
    //        {
    //            auto iter = insert_sorted_vec(index[i], j);
    //            size_t k = iter - index[i].begin();
    //            value[i].insert(value[i].begin() + k, new_value);
    //        }

    //        if(i < j)
    //        {
    //            set_element_symmetric(j, i, new_value);
    //        }
    //    }

    //    void set_element_allcase(UInt32 i, UInt32 j, T new_value)
    void set_element(UInt32 i, UInt32 j, T new_value)
    {
        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            m_ColValue[i][iter - m_ColIndex[i].begin()] = new_value;
        }
        else
        {
            auto   iter = insert_sorted_vec(m_ColIndex[i], j);
            size_t k = iter - m_ColIndex[i].begin();
            m_ColValue[i].insert(m_ColValue[i].begin() + k, new_value);
        }
    }

    void add_to_element(UInt32 i, UInt32 j, T increment_value)
    {
        //        if(i < j) // only add to the element below the main diagonal
        //        {
        //            return;
        //        }

        auto iter = std::lowerBound(m_ColIndex[i].begin(), m_ColIndex[i].end(), j);

        if((iter != m_ColIndex[i].end()) && (*iter == j))
        {
            m_ColValue[i][iter - m_ColIndex[i].begin()] += increment_value;
            return;
        }
        else
        {
            auto   iter = insert_sorted_vec(m_ColIndex[i], j);
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

    // assumes indices is already sorted
    void add_sparse_row(UInt32 i, const std::vector<UInt32>& indices,
                        const std::vector<T>& values)
    {
        UInt32 j = 0, k = 0;

        while(j < indices.size() && k < m_ColIndex[i].size())
        {
            if(m_ColIndex[i][k] < indices[j])
            {
                ++k;
            }
            else if(m_ColIndex[i][k] > indices[j])
            {
                MathHelpers::insert(m_ColIndex[i], k, indices[j]);
                MathHelpers::insert(m_ColValue[i], k, values[j]);
                ++j;
            }
            else
            {
                m_ColValue[i][k] += values[j];
                ++j;
                ++k;
            }
        }

        for(; j < indices.size(); ++j)
        {
            m_ColIndex[i].push_back(indices[j]);
            m_ColValue[i].push_back(values[j]);
        }
    }

    void make_symmetry()
    {
        for(UInt32 i = 0; i < m_ColIndex.size(); ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                if(m_ColIndex[i][j] < i)
                {
                    set_element_allcase(m_ColIndex[i][j], i, m_ColValue[i][j]);
                }
                else
                {
                    break;
                }
            }
        }
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

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                output << m_ColIndex[i][j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            for(UInt32 j = 0; j < m_ColValue[i].size(); ++j)
            {
                output << m_ColValue[i][j] << " ";
            }
        }

        output << "], " << m_Size << ", " << m_Size << ");" << std::endl;
    }

    void write_debug()
    {
        for(UInt32 i = 0; i < m_Size; ++i)
        {
            if(m_ColIndex[i].size() == 0)
            {
                continue;
            }

            std::cout << "Line " << i << ": " << std::endl;

            for(UInt32 j = 0; j < m_ColIndex[i].size(); ++j)
            {
                std::cout << m_ColIndex[i][j] << "(" << m_ColValue[i][j] << "), ";
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
        std::cout << "Matrix size: " << m_Size << std::endl;

        tbb::parallel_for(tbb::blocked_range<size_t>(0, m_Size),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
            {
                for(size_t j = i + 1; j < m_Size; ++j)
                {
                    if(fabs((*this)(i, j) - (*this)(j, i)) > 1e-10)
                    {
                        check = false;
                        std::cout << "Invalid matrix element at index " << i << ", " << j << ": "
                            << "matrix(" << i << ", " << j << " = " << (*this)(i, j) << " != "
                            << "matrix(" << j << ", " << i << " = " << (*this)(j, i) << std::endl;
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
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef SparseMatrix<float>    SparseMatrixf;
typedef SparseMatrix<double>   SparseMatrixd;

// perform result=matrix*x
template<class T>
void multiply(const SparseMatrix<T>& matrix, const std::vector<T>& x,
              std::vector<T>& result)
{
    __NOODLE_ASSERT(matrix.m_Size == x.size());
    result.resize(matrix.m_Size);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            double tmpResult = 0;

            for(UInt32 j = matrix.rowstart[i], jend = matrix.rowstart[i + 1]; j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
            }

            result[i] = tmpResult;
        }
    }); // end parallel_for
}

// perform result=result-matrix*x
template<class T>
void multiply_and_subtract(const SparseMatrix<T>& matrix, const std::vector<T>& x, std::vector<T>& result)
{
    __NOODLE_ASSERT(matrix.m_Size == x.size());
    result.resize(matrix.m_Size);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            double tmpResult = result[i];

            for(UInt32 j = matrix.rowstart[i], jend = matrix.rowstart[i + 1]; j < jend; ++j)
            {
                tmpResult -= matrix.m_ColValue[i][j] * x[matrix.m_ColIndex[i][j]];
            }

            result[i] = tmpResult;
        }
    }); // end parallel_for
}

//============================================================================
// Fixed version of SparseMatrix. This is not a good structure for dynamically
// modifying the matrix, but can be significantly faster for matrix-vector
// multiplies due to better data locality.

template<class T>
struct FixedSparseMatrix
{
    UInt32              m_Size;     // dimension
    std::vector<T>      m_ColValue; // nonzero values row by row
    std::vector<UInt32> m_ColIndex; // corresponding column indices
    std::vector<UInt32> m_RowStart; // where each row starts in value and colindex (and last entry is one past the end, the number of nonzeros)

    explicit FixedSparseMatrix(UInt32 n_ = 0) : m_Size(n_), m_ColValue(0), m_ColIndex(0), m_RowStart(n_ + 1)
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

    void construct_from_matrix(const SparseMatrix<T>& matrix)
    {
        resize(matrix.m_Size);
        m_RowStart[0] = 0;

        for(UInt32 i = 0; i < m_Size; ++i)
        {
            m_RowStart[i + 1] = m_RowStart[i] + matrix.m_ColIndex[i].size();
        }

        m_ColValue.resize(m_RowStart[m_Size]);
        m_ColIndex.resize(m_RowStart[m_Size]);

        static tbb::affinity_partitioner ap;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
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

typedef FixedSparseMatrix<float>    FixedSparseMatrixf;
typedef FixedSparseMatrix<double>   FixedSparseMatrixd;

// perform result=matrix*x
template<class T>
void multiply(const FixedSparseMatrix<T>& matrix, const std::vector<T>& x, std::vector<T>& result)
{
    __NOODLE_ASSERT(matrix.m_Size == x.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            double tmpResult = 0;

            for(UInt32 j = matrix.m_RowStart[i], jend = matrix.m_RowStart[i + 1]; j < jend; ++j)
            {
                tmpResult += matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result=result-matrix*x
template<class T>
void multiply_and_subtract(const FixedSparseMatrix<T>& matrix, const std::vector<T>& x, std::vector<T>& result)
{
    __NOODLE_ASSERT(matrix.m_Size == x.size());

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.m_Size), [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            double tmpResult = result[i];

            for(UInt32 j = matrix.m_RowStart[i], jend = matrix.m_RowStart[i + 1]; j < jend; ++j)
            {
                tmpResult -= matrix.m_ColValue[j] * x[matrix.m_ColIndex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}
