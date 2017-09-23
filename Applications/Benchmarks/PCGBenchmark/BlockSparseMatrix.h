#ifndef BLOCK_SPARSE_MATRIX_H
#define BLOCK_SPARSE_MATRIX_H

#include <tbb/tbb.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "./Setup.h"

using Real = real;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template< typename MatrixType >
inline typename std::vector<MatrixType>::iterator
block_insert_sorted(std::vector<MatrixType>& vec, const MatrixType& item)
{
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class MatrixType>
struct BlockSparseMatrix
{
    // dimension
    UInt size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt> > index;

    // values corresponding to index
    std::vector<std::vector<MatrixType> > value;

    explicit BlockSparseMatrix(UInt n_ = 0)
        : size(n_), index(n_), value(n_) {}

    void clear(void)
    {
        size = 0;

        for(size_t i = 0; i < index.size(); ++i)
        {
            index[i].clear();
        }

        index.clear();

        for(size_t i = 0; i < value.size(); ++i)
        {
            value[i].clear();
        }

        value.clear();
    }

    void zero(void)
    {
        for(UInt i = 0; i < size; ++i)
        {
            index[i].resize(0);
            value[i].resize(0);
        }
    }

    void resize(UInt n_)
    {
        size = n_;
        index.resize(size);
        value.resize(size);
    }

    const MatrixType operator()(UInt i, UInt j) const
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);

        if((iter != index[i].end()) && (*iter == j))
        {
            return value[i][iter - index[i].begin()];
        }
        else
        {
#ifdef __Using_Eigen_Lib__
            return MatrixType::Zero();
#else
            MatrixType tmp;
            tmp[i][0] = 0;;
            tmp[i][1] = 0;;
            tmp[i][2] = 0;;
#endif
        }
    }

    void set_element(UInt i, UInt j, const MatrixType& new_value)
    {
        assert(i < size && j < size);

        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);

        if((iter != index[i].end()) && (*iter == j))
        {
            value[i][iter - index[i].begin()] = new_value;
        }
        else
        {
            auto iter = block_insert_sorted(index[i], j);
            size_t k = iter - index[i].begin();
            value[i].insert(value[i].begin() + k, new_value);
        }
    }


    void add_to_element(UInt i, UInt j, const MatrixType& increment_value)
    {
        assert(i < size && j < size);

        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);

        if((iter != index[i].end()) && (*iter == j))
        {
            value[i][iter - index[i].begin()] += increment_value;
            return;
        }
        else
        {
            auto iter = block_insert_sorted(index[i], j);
            size_t k = iter - index[i].begin();
            value[i].insert(value[i].begin() + k, increment_value);
            return;
        }
    }

    void erase_element(UInt i, UInt j)
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);
        UInt k = iter - index[i].begin();

        if((iter != index[i].end()) && (*iter == j))
        {
            index[i].erase(iter);
            value[i].erase(value[i].begin() + k);
        }
    }

    bool has_data(UInt i, UInt j)
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);
        //        UInt k = iter - index[i].begin();

        return ((iter != index[i].end()) && (*iter == j));
    }


    // assumes matrix has symmetric structure - so the indices in row i tell us which columns to delete i from
    void symmetric_remove_row_and_column(UInt i)
    {
        for(UInt a = 0; a < index[i].size(); ++a)
        {
            UInt j = index[i][a]; //

            for(UInt b = 0; b < index[j].size(); ++b)
            {
                if(index[j][b] == i)
                {
                    MathUtils::erase(index[j], b);
                    MathUtils::erase(value[j], b);
                    break;
                }
            }
        }

        index[i].resize(0);
        value[i].resize(0);
    }

    void write_matlab(std::ostream& output, const char* variable_name)
    {
        output << variable_name << "=sparse([";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = 0; j < index[i].size(); ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = 0; j < index[i].size(); ++j)
            {
                output << index[i][j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = 0; j < value[i].size(); ++j)
            {
                output << value[i][j] << " ";
            }
        }

        output << "], " << size << ", " << size << ");" << std::endl;
    }

    void print_debug()
    {
        for(UInt i = 0; i < size; ++i)
        {
            if(index[i].size() == 0)
            {
                continue;
            }

            std::cout << "Line " << i << ": " << std::endl;

            for(UInt j = 0; j < index[i].size(); ++j)
            {
                std::cout << index[i][j] << "(" << value[i][j] << "), " << std::endl;
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
            for(size_t i = r.begin(); i != r.end(); ++i)
            {
                for(size_t j = i + 1; j < size; ++j)
                {
                    if(has_data(i, j))
                    {
                        auto err = ((*this)(i, j) - (*this)(j, i)).squaredNorm();

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

    void write_to_file(const char* fileName)
    {
        FILE* fptr = fopen(fileName, "wb");

        if(!fptr)
        {
            printf("Cannot open file for writing!\n");
            return;
        }

        UInt number_elements = 0;

        for(size_t i = 0; i < size; ++i)
        {
            for(size_t j = i; j < size; ++j)
            {
                if(has_data(i, j))
                {
                    ++number_elements;
                }
            }
        }


        UInt one_percentage = (UInt)(number_elements / 100);
        UInt num_processed = 0;
        float* data_ptr = new float[9 * number_elements];
        UInt* row_ptr = new UInt[number_elements];
        UInt* column_ptr = new UInt[number_elements];

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = i; j < size; ++j)
            {
                if(has_data(i, j))
                {
                    const MatrixType& tmp = (*this)(i, j);

#ifdef __Using_Eigen_Lib__
                    data_ptr[9 * num_processed + 0] = tmp(0, 0);
                    data_ptr[9 * num_processed + 1] = tmp(0, 1);
                    data_ptr[9 * num_processed + 2] = tmp(0, 2);

                    data_ptr[9 * num_processed + 3] = tmp(1, 0);
                    data_ptr[9 * num_processed + 4] = tmp(1, 1);
                    data_ptr[9 * num_processed + 5] = tmp(1, 2);

                    data_ptr[9 * num_processed + 6] = tmp(2, 0);
                    data_ptr[9 * num_processed + 7] = tmp(2, 1);
                    data_ptr[9 * num_processed + 8] = tmp(2, 2);

#else
#ifdef __Using_GLM_Lib__
                    Real* tmp_ptr = glm::value_ptr(tmp);
                    data_ptr[9 * num_processed + 0] = tmp_ptr[0];
                    data_ptr[9 * num_processed + 1] = tmp_ptr[1];
                    data_ptr[9 * num_processed + 2] = tmp_ptr[2];

                    data_ptr[9 * num_processed + 3] = tmp_ptr[3];
                    data_ptr[9 * num_processed + 4] = tmp_ptr[4];
                    data_ptr[9 * num_processed + 5] = tmp_ptr[5];

                    data_ptr[9 * num_processed + 6] = tmp_ptr[6];
                    data_ptr[9 * num_processed + 7] = tmp_ptr[7];
                    data_ptr[9 * num_processed + 8] = tmp_ptr[8];
#else
                    data_ptr = new Real[9];

                    data_ptr[9 * num_processed + 0] = tmp[0][0];
                    data_ptr[9 * num_processed + 1] = tmp[0][1];
                    data_ptr[9 * num_processed + 2] = tmp[0][2];

                    data_ptr[9 * num_processed + 3] = tmp[1][0];
                    data_ptr[9 * num_processed + 4] = tmp[1][1];
                    data_ptr[9 * num_processed + 5] = tmp[1][2];

                    data_ptr[9 * num_processed + 6] = tmp[2][0];
                    data_ptr[9 * num_processed + 7] = tmp[2][1];
                    data_ptr[9 * num_processed + 8] = tmp[2][2];
#endif
#endif

                    row_ptr[num_processed] = i;
                    column_ptr[num_processed] = j;

                    ++num_processed;

                    if(num_processed % one_percentage == 0)
                    {
                        printf("%u/%u...\n", num_processed, number_elements);
                    }

                }
            }
        }

        fwrite(&number_elements, 1, sizeof(UInt), fptr);
        fwrite(row_ptr, 1, number_elements * sizeof(UInt), fptr);
        fwrite(column_ptr, 1, number_elements * sizeof(UInt), fptr);
        fwrite(data_ptr, 1, 9 * number_elements * sizeof(float), fptr);
        fclose(fptr);
        printf("File written, num. elements: %u, filename: %s\n", number_elements, fileName);

        delete[] row_ptr;
        delete[] column_ptr;
        delete[] data_ptr;
    }




    bool load_from_file(const char* fileName)
    {
        zero();

        FILE* fptr = fopen(fileName, "rb");

        if(!fptr)
        {
            printf("Cannot open file %s for reading!\n", fileName);
            return false;
        }

        UInt number_elements = 0;

        fread(&number_elements, 1, sizeof(UInt), fptr);
        float* data_ptr = new float[9 * number_elements];
        UInt* row_ptr = new UInt[number_elements];
        UInt* column_ptr = new UInt[number_elements];

        fread(row_ptr, 1, number_elements * sizeof(UInt), fptr);
        fread(column_ptr, 1, number_elements * sizeof(UInt), fptr);
        fread(data_ptr, 1, 9 * number_elements * sizeof(float), fptr);

        UInt one_percentage = (UInt)(number_elements / 100);
        UInt num_processed = 0;

        for(UInt k = 0; k < number_elements; ++k)
        {
            MatrixType tmp;

#if defined(__Using_Eigen_Lib__) || defined(__Using_Cem_Lib__)
            tmp(0, 0) = data_ptr[9 * num_processed + 0];
            tmp(0, 1) = data_ptr[9 * num_processed + 1];
            tmp(0, 2) = data_ptr[9 * num_processed + 2];

            tmp(1, 0) = data_ptr[9 * num_processed + 3];
            tmp(1, 1) = data_ptr[9 * num_processed + 4];
            tmp(1, 2) = data_ptr[9 * num_processed + 5];

            tmp(2, 0) = data_ptr[9 * num_processed + 6];
            tmp(2, 1) = data_ptr[9 * num_processed + 7];
            tmp(2, 2) = data_ptr[9 * num_processed + 8];

#else
#ifdef __Using_GLM_Lib__
            Real* data_mat = glm::value_ptr(tmp);

            for(int l = 0; l < 9; ++l)
            {
                data_mat[l] = data_ptr[9 * num_processed + l];
            }

#else
            tmp[0][0] = data_ptr[9 * num_processed + 0];
            tmp[0][1] = data_ptr[9 * num_processed + 1];
            tmp[0][2] = data_ptr[9 * num_processed + 2];

            tmp[1][0] = data_ptr[9 * num_processed + 3];
            tmp[1][1] = data_ptr[9 * num_processed + 4];
            tmp[1][2] = data_ptr[9 * num_processed + 5];

            tmp[2][0] = data_ptr[9 * num_processed + 6];
            tmp[2][1] = data_ptr[9 * num_processed + 7];
            tmp[2][2] = data_ptr[9 * num_processed + 8];
#endif
#endif

            set_element(row_ptr[num_processed], column_ptr[num_processed], tmp);
            set_element(column_ptr[num_processed], row_ptr[num_processed], tmp);

            ++num_processed;

            //if(num_processed % one_percentage == 0)
            //{
            //    printf("%u/%u...\n", num_processed, number_elements);
            //}

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
              const std::vector<VectorType>& x,
              std::vector<VectorType>& result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult += matrix.value[i][j] * x[matrix.index[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for

}

// perform result=alpha*matrix*x
template<class MatrixType, class VectorType, class RealType>
void multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix,
                     const std::vector<VectorType>& x,
                     const RealType alpha,
                     std::vector<VectorType>& result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult += matrix.value[i][j] * x[matrix.index[i][j]] * alpha;
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for

}

// perform result+=alpha*matrix*x
template<class MatrixType, class VectorType, class RealType>
void add_multiply_scaled(const BlockSparseMatrix<MatrixType>& matrix,
                         const std::vector<VectorType>& x,
                         const RealType alpha,
                         std::vector<VectorType>& result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult = VectorType::Zero();

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult += matrix.value[i][j] * x[matrix.index[i][j]] * alpha;
            }

            result[i] += tmpResult;
        }
    }, ap); // end parallel_for

}

// perform result=result-matrix*x
template<class MatrixType, class VectorType>
void multiply_and_subtract(const BlockSparseMatrix<MatrixType>& matrix,
                           const std::vector<VectorType>& x,
                           std::vector<VectorType>& result)
{
    __NOODLE_ASSERT(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult = result[i];

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult -= matrix.value[i][j] * x[matrix.index[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform mat = A + alpha*B
template<class MatrixType, class VectorType, class RealType>
void add_scaled(const BlockSparseMatrix<MatrixType>& A,
                const BlockSparseMatrix<MatrixType>& B,
                const RealType alpha,
                BlockSparseMatrix<MatrixType>& matrix)
{
    __NOODLE_ASSERT(A.size == B.size);
    __NOODLE_ASSERT(A.size == matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            __NOODLE_ASSERT(A.index[i].size() == B.index[i].size());

            for(size_t j = 0; j < A.index[i].size(); ++j)
            {
                matrix.set_element(i, A.index[i][j], A.value[i][j] + B.value[i][j] * alpha);
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
    UInt size;

    // nonzero values row by row
    std::vector<MatrixType> value;

    // corresponding column indices
    std::vector<UInt> colindex;

    // where each row starts in value and colindex (and last entry is one past the end, the number of nonzeros)
    std::vector<UInt> rowstart;

    explicit BlockFixedSparseMatrix(UInt n_ = 0)
        : size(n_), value(0), colindex(0), rowstart(n_ + 1) {}

    void clear(void)
    {
        size = 0;
        value.clear();
        colindex.clear();
        rowstart.clear();
    }

    void resize(int n_)
    {
        size = n_;
        rowstart.resize(size + 1);
    }

    void construct_from_matrix(const BlockSparseMatrix<MatrixType>& matrix)
    {
        resize(matrix.size);
        rowstart[0] = 0;

        for(UInt i = 0; i < size; ++i)
        {
            rowstart[i + 1] = rowstart[i] + matrix.index[i].size();
        }

        value.resize(rowstart[size]);
        colindex.resize(rowstart[size]);

        static tbb::affinity_partitioner ap;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t i = r.begin(); i != r.end(); ++i)
            {
                for(UInt k = 0; k < matrix.index[i].size(); ++k)
                {
                    value[rowstart[i] + k] = matrix.value[i][k];
                    colindex[rowstart[i] + k] = matrix.index[i][k];
                }
            }
        }, ap); // end parallel_for
    }

    void write_matlab(std::ostream& output, const char* variable_name)
    {
        output << variable_name << "=sparse([";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << colindex[j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt i = 0; i < size; ++i)
        {
            for(UInt j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << value[j] << " ";
            }
        }

        output << "], " << size << ", " << size << ");" << std::endl;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class MatrixType, class VectorType>
void multiply(const BlockFixedSparseMatrix<MatrixType>& matrix,
              const std::vector<VectorType>& x,
              std::vector<VectorType>& result)
{
    assert(matrix.size == x.size());
    //    result.resize(matrix.n);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult;
            tmpResult[0] = 0;;
            tmpResult[1] = 0;;
            tmpResult[2] = 0;;

            const UInt rowend = matrix.rowstart[i + 1];

            for(UInt j = matrix.rowstart[i]; j < rowend; ++j)
            {
                tmpResult += matrix.value[j] * x[matrix.colindex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}

// perform result=result-matrix*x
template<class MatrixType, class VectorType>
void multiply_and_subtract(const BlockFixedSparseMatrix<MatrixType>& matrix,
                           const std::vector<VectorType>& x,
                           std::vector<VectorType>& result)
{
    assert(matrix.size == x.size());
    //    result.resize(matrix.n);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            VectorType tmpResult = result[i];

            const UInt rowend = matrix.rowstart[i + 1];

            for(UInt j = matrix.rowstart[i]; j < rowend; ++j)
            {
                tmpResult -= matrix.value[j] * x[matrix.colindex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for

}

#endif
