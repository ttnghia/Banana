#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <tbb/tbb.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include <Banana/TypeNames.h>

using Real = real;

#define SQR(x) (x)*(x)
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template< typename ScalarType >
inline typename std::vector<ScalarType>::iterator
insert_sorted(std::vector<ScalarType>& vec, ScalarType item)
{
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Dynamic compressed sparse row matrix.
//
template<class ScalarType>
struct SparseMatrix
{
    // dimension
    UInt32 size;

    // for each row, a list of all column indices (sorted)
    std::vector<std::vector<UInt32> > index;

    // values corresponding to index
    std::vector<std::vector<ScalarType> > value;

    explicit SparseMatrix(UInt32 n_ = 0)
        : size(n_), index(n_), value(n_)
    {}

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
        for(UInt32 i = 0; i < size; ++i)
        {
            index[i].resize(0);
            value[i].resize(0);
        }
    }

    void resize(UInt32 n_)
    {
        size = n_;
        index.resize(size);
        value.resize(size);
    }

    const ScalarType operator()(UInt32 i, UInt32 j) const
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);

        if((iter != index[i].end()) && (*iter == j))
        {
            return value[i][iter - index[i].begin()];
        }
        else
        {
            return 0;
        }
    }

    void set_element(UInt32 i, UInt32 j, ScalarType new_value)
    {
        assert(i < size && j < size);

        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);

        if((iter != index[i].end()) && (*iter == j))
        {
            value[i][iter - index[i].begin()] = new_value;
        }
        else
        {
            auto iter = insert_sorted(index[i], j);
            size_t k = iter - index[i].begin();
            value[i].insert(value[i].begin() + k, new_value);
        }
    }


    void add_to_element(UInt32 i, UInt32 j, ScalarType increment_value)
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
            auto iter = insert_sorted(index[i], j);
            size_t k = iter - index[i].begin();
            value[i].insert(value[i].begin() + k, increment_value);
            return;
        }
    }

    void erase_element(UInt32 i, UInt32 j)
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);
        UInt32 k = iter - index[i].begin();

        if((iter != index[i].end()) && (*iter == j))
        {
            index[i].erase(iter);
            value[i].erase(value[i].begin() + k);
        }
    }

    bool has_data(UInt32 i, UInt32 j)
    {
        auto iter = std::lower_bound(index[i].begin(), index[i].end(), j);
        //        UInt32 k = iter - index[i].begin();

        return ((iter != index[i].end()) && (*iter == j));
    }


    // assumes matrix has symmetric structure - so the indices in row i tell us which columns to delete i from
    void symmetric_remove_row_and_column(UInt32 i)
    {
        for(UInt32 a = 0; a < index[i].size(); ++a)
        {
            UInt32 j = index[i][a]; //

            for(UInt32 b = 0; b < index[j].size(); ++b)
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

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < index[i].size(); ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < index[i].size(); ++j)
            {
                output << index[i][j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = 0; j < value[i].size(); ++j)
            {
                output << value[i][j] << " ";
            }
        }

        output << "], " << size << ", " << size << ");" << std::endl;
    }

    void print_debug()
    {
        for(UInt32 i = 0; i < size; ++i)
        {
            if(index[i].size() == 0)
            {
                continue;
            }

            std::cout << "Line " << i << ": " << std::endl;

            for(UInt32 j = 0; j < index[i].size(); ++j)
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
                        auto err = SQR((*this)(i, j) - (*this)(j, i));

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

        UInt32 number_elements = 0;

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


        UInt32 one_percentage = (UInt32)(number_elements / 100);
        UInt32 num_processed = 0;
        float* data_ptr = new float[number_elements];
        UInt32* row_ptr = new UInt32[number_elements];
        UInt32* column_ptr = new UInt32[number_elements];

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = i; j < size; ++j)
            {
                if(has_data(i, j))
                {
                    data_ptr[num_processed] = (*this)(i, j);
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

        fwrite(&number_elements, 1, sizeof(UInt32), fptr);
        fwrite(row_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fwrite(column_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fwrite(data_ptr, 1, number_elements * sizeof(float), fptr);
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
            printf("Cannot open file for reading!\n");
            return false;
        }

        UInt32 number_elements = 0;

        fread(&number_elements, 1, sizeof(UInt32), fptr);
        float* data_ptr = new float[number_elements];
        UInt32* row_ptr = new UInt32[number_elements];
        UInt32* column_ptr = new UInt32[number_elements];

        fread(row_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fread(column_ptr, 1, number_elements * sizeof(UInt32), fptr);
        fread(data_ptr, 1, number_elements * sizeof(float), fptr);

        UInt32 one_percentage = (UInt32)(number_elements / 100);
        UInt32 num_processed = 0;

        for(UInt32 k = 0; k < number_elements; ++k)
        {
            const ScalarType tmp = data_ptr[num_processed];

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x

template<class ScalarType>
void multiply(const SparseMatrix<ScalarType>& matrix,
              const std::vector<ScalarType>& x,
              std::vector<ScalarType>& result)
{
    assert(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            ScalarType tmpResult = 0;

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult += matrix.value[i][j] * x[matrix.index[i][j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for

}

// perform result=alpha*matrix*x
template<class ScalarType>
void multiply_scaled(const SparseMatrix<ScalarType>& matrix,
                     const std::vector<ScalarType>& x,
                     const ScalarType alpha,
                     std::vector<ScalarType>& result)
{
    assert(matrix.size == x.size());
    result.resize(matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            ScalarType tmpResult = 0;

            for(size_t j = 0; j < matrix.index[i].size(); ++j)
            {
                tmpResult += matrix.value[i][j] * x[matrix.index[i][j]] * alpha;
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for

}


// perform mat = A + alpha*B
template<class ScalarType>
void add_scaled(const SparseMatrix<ScalarType>& A,
                const SparseMatrix<ScalarType>& B,
                const ScalarType alpha,
                SparseMatrix<ScalarType>& matrix)
{
    assert(A.size == B.size);
    assert(A.size == matrix.size);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            assert(A.index[i].size() == B.index[i].size());

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

template<class ScalarType>
struct FixedSparseMatrix
{
    // dimension
    UInt32 size;

    // nonzero values row by row
    std::vector<ScalarType> value;

    // corresponding column indices
    std::vector<UInt32> colindex;

    // where each row starts in value and colindex (and last entry is one past the end, the number of nonzeros)
    std::vector<UInt32> rowstart;

    explicit FixedSparseMatrix(UInt32 n_ = 0)
        : size(n_), value(0), colindex(0), rowstart(n_ + 1)
    {}

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

    void construct_from_matrix(const SparseMatrix<ScalarType>& matrix)
    {
        resize(matrix.size);
        rowstart[0] = 0;

        for(UInt32 i = 0; i < size; ++i)
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
                for(UInt32 k = 0; k < matrix.index[i].size(); ++k)
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

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << i + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << colindex[j] + 1 << " ";
            }
        }

        output << "],...\n  [";

        for(UInt32 i = 0; i < size; ++i)
        {
            for(UInt32 j = rowstart[i]; j < rowstart[i + 1]; ++j)
            {
                output << value[j] << " ";
            }
        }

        output << "], " << size << ", " << size << ");" << std::endl;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// perform result=matrix*x
template<class ScalarType>
void multiply(const FixedSparseMatrix<ScalarType>& matrix,
              const std::vector<ScalarType>& x,
              std::vector<ScalarType>& result)
{
    assert(matrix.size == x.size());
    //    result.resize(matrix.n);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, matrix.size),
                      [&](tbb::blocked_range<size_t>& r)
    {
        for(size_t i = r.begin(); i != r.end(); ++i)
        {
            ScalarType tmpResult = 0;

            const UInt32 rowend = matrix.rowstart[i + 1];

            for(UInt32 j = matrix.rowstart[i]; j < rowend; ++j)
            {
                tmpResult += matrix.value[j] * x[matrix.colindex[j]];
            }

            result[i] = tmpResult;
        }
    }, ap); // end parallel_for
}



#endif
