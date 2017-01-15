//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/12/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>


//#define __Using_Eigen_Lib__
//#define __Using_GLM_Lib__
//#define __Using_Real_Number__


#define USING_TBB
#define NUM_TEST_LOOP 10

//#define LOAD_SMALL_DATA
#define LOAD_MEDIUM_DATA

#define HIGH_PRECISION

#define SOLVER_TOLERANCE 1e-20

#include <Banana/TypeNames.h>
#include <Banana/Timer.h>      
#include <spdlog/spdlog.h>
#include <ProgressBar.hpp>


#ifdef USING_TBB
#include <tbb/tbb.h>
#endif


#include <glm/gtc/type_ptr.hpp>

#ifdef HIGH_PRECISION
using real = double;
#else
using real = float;
#endif // HIGH_PRECISION

#pragma warning(disable:4996)
#include "./BlockSparseMatrix.h"
#include "./BlockCGSolver.h"
#include "./SparseMatrix.h"
#include "./CGSolver.h"

using Vec3D = Vec3<real>;
using Mat3x3D = Mat3x3<real>;

//------------------------------------------------------------------------------------------
void load_data(BlockSparseMatrix<Mat3x3D>& mat, std::vector<Vec3D>& rhs)
{
#ifdef LOAD_SMALL_DATA
    const char* rhsFileName = "D:/Programming/WorkingData/Data/rhs_small.dat";
#else
#ifdef LOAD_MEDIUM_DATA
    const char* rhsFileName = "D:/Programming/WorkingData/Data/rhs_medium.dat";
#else
#ifdef LOAD_SLOW_DATA
    const char* rhsFileName = "D:/Programming/WorkingData/Data/rhs_slow.dat";
#else
    const char* rhsFileName = "D:/Programming/WorkingData/Data/rhs_fast.dat";
#endif
#endif
#endif
    FILE* fptr = NULL;
    fptr = fopen(rhsFileName, "rb");
    if(!fptr)
    {
        printf("Cannot open file %s for reading!\n", rhsFileName);
        exit(-1);
    }

    UInt32 size;
    fread(&size, 1, sizeof(UInt32), fptr);
    float *data_ptr = new float[3 * size];
    size_t num_read = fread(data_ptr, 1, size * 3 * sizeof(float), fptr);
    assert(num_read == size * 3 * sizeof(float));

    rhs.resize(size);
    for(auto i = 0; i < size; ++i)
    {
        Vec3D tmp;

        tmp[0] = data_ptr[3 * i + 0];
        tmp[1] = data_ptr[3 * i + 1];
        tmp[2] = data_ptr[3 * i + 2];

        //printf("read: %f, %f, %f\n",
        //       tmp[0], tmp[1], tmp[2]);

        rhs[i] = tmp;
    }

    fclose(fptr);
    printf("File read, num. elements: %lu, filename: %s\n", rhs.size(), rhsFileName);

    ////////////////////////////////////////////////////////////////////////////////
    // load matrix
    mat.resize(rhs.size());
#ifdef LOAD_SMALL_DATA
    mat.load_from_file("D:/Programming/WorkingData/Data/mat_small.dat");
#else
#ifdef LOAD_MEDIUM_DATA
    mat.load_from_file("D:/Programming/WorkingData/Data/mat_medium.dat");
#else
#ifdef LOAD_SLOW_DATA
    mat.load_from_file("D:/Programming/WorkingData/Data/mat_slow.dat");
#else
    mat.load_from_file("D:/Programming/WorkingData/Data/mat_fast.dat");
#endif
#endif
#endif
}

//------------------------------------------------------------------------------------------
void copy_data(BlockSparseMatrix<Mat3x3D>& mat_src, const std::vector<Vec3D>& rhs_src,
               SparseMatrix<real>& mat_dst, std::vector<real>& rhs_dst)
{
    mat_dst.zero();
    mat_dst.resize(mat_src.size * 3);
    for(UInt32 i = 0; i < mat_src.size; ++i)
    {
        for(UInt32 j = 0; j < 3; ++j)
        {
            mat_dst.index[i * 3 + j].resize(mat_src.index[i].size() * 3);
            mat_dst.value[i * 3 + j].resize(mat_src.value[i].size() * 3);
        }
    }

    for(UInt32 i = 0; i < mat_src.size; ++i)
    {
        for(UInt32 j = 0; j < mat_src.index[i].size(); ++j)
        {
            const Mat3x3D& tmp = mat_src.value[i][j];

#ifdef __Using_GLM_Lib__
            const real* tmp_ptr = glm::value_ptr(tmp);
#endif

            for(UInt32 l1 = 0; l1 < 3; ++l1)
            {
                for(UInt32 l2 = 0; l2 < 3; ++l2)
                {
                    mat_dst.index[i * 3 + l1][j * 3 + l2] = mat_src.index[i][j] * 3 + l2;

#ifdef __Using_Eigen_Lib__
                    mat_dst.value[i * 3 + l1][j * 3 + l2] = tmp(l1, l2);
#else
#ifdef __Using_GLM_Lib__

                    mat_dst.value[i * 3 + l1][j * 3 + l2] = tmp_ptr[l2 * 3 + l1];
#else
                    mat_dst.value[i * 3 + l1][j * 3 + l2] = tmp[l1][l2];
#endif
#endif
                }
            }

        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    rhs_dst.resize(rhs_src.size() * 3);
    for(size_t i = 0; i < rhs_src.size(); ++i)
    {
        const Vec3D& tmp = rhs_src[i];

        rhs_dst[i * 3 + 0] = tmp[0];
        rhs_dst[i * 3 + 1] = tmp[1];
        rhs_dst[i * 3 + 2] = tmp[2];
    }

}
//------------------------------------------------------------------------------------------
TEST_CASE("Tested conjugate gradient solver performance")
{
    Timer timer;
    auto console_logger = spdlog::stdout_color_mt("console");
#ifdef __Using_Eigen_Lib__
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "log_eigen.txt");
#ifdef HIGH_PRECISION
    console_logger->info("Test by eigen, real = double");
    file_logger->info("Test by eigen, real = double");
#else
    console_logger->info("Test by eigen, real = float");
    file_logger->info("Test by eigen, real = float");
#endif // HIGH_PRECISION
#else
#ifdef __Using_GLM_Lib__
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "log_glm.txt");
#ifdef HIGH_PRECISION
    console_logger->info("Test by glm, real = double");
    file_logger->info("Test by glm, real = double");
#else
    console_logger->info("Test by glm, real = float");
    file_logger->info("Test by glm, real = float");
#endif // HIGH_PRECISION
#else
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "log_yocto.txt");
#ifdef HIGH_PRECISION
    console_logger->info("Test by yocto, real = double");
    file_logger->info("Test by yocto, real = double");
#else
    console_logger->info("Test by yocto, real = float");
    file_logger->info("Test by yocto, real = float");
#endif // HIGH_PRECISION
#endif
#endif

    BlockConjugateGradientSolver<Mat3x3D, Vec3D, real> solver_3D;
    solver_3D.set_solver_parameters(SOLVER_TOLERANCE, 10000);

    BlockSparseMatrix<Mat3x3D> mat3D;
    std::vector<Vec3D> rhs_3D;
    std::vector<Vec3D> result_3D;

    ConjugateGradientSolver<real> solver_1D;
    solver_1D.set_solver_parameters(SOLVER_TOLERANCE, 10000);

    SparseMatrix<real> mat1D;
    std::vector<real> rhs_1D;
    std::vector<real> result_1D;

    double time_test_total = 0;
    double time_test;

    ////////////////////////////////////////////////////////////////////////////////
    {
        load_data(mat3D, rhs_3D);
        console_logger->info("System 3D data initialized");
        file_logger->info("System 3D data initialized");

        result_3D.resize(rhs_3D.size());

    }
    console_logger->info("\n\n");
    file_logger->info("\n\n");

    ////////////////////////////////////////////////////////////////////////////////
    {
        ProgressBar progressBar(NUM_TEST_LOOP, 70, '#', '-');
        real tolerance;
        int iterations;
        for(auto i = 0; i < NUM_TEST_LOOP; ++i)
        {
            timer.tick();
            solver_3D.solve(mat3D, rhs_3D, result_3D, tolerance, iterations);
            time_test = timer.tock();
            console_logger->info("Test BlockCG, time = {} ms, tolerance = {}, iterations = {}",
                                 NumberHelpers::format_with_commas(time_test), tolerance, iterations);
            file_logger->info("Test BlockCG, avg time = {} ms, tolerance = {}, iterations = {}",
                              NumberHelpers::format_with_commas(time_test), tolerance, iterations);
            time_test_total += time_test;

            /////////////////////////////////////////////////////////////////////////////////
            ++progressBar; // record the tick
            //if(i % 10 == 0)
            progressBar.display();
        }
        progressBar.done();

    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        console_logger->info("\n\n");
        file_logger->info("\n\n");
        console_logger->info("Finished test BlockCG, avg time = {} ms\n\n",
                             NumberHelpers::format_with_commas(time_test_total / (double)NUM_TEST_LOOP));
        file_logger->info("Finished test BlockCG, avg time = {} ms\n\n",
                          NumberHelpers::format_with_commas(time_test_total / (double)NUM_TEST_LOOP));
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        copy_data(mat3D, rhs_3D, mat1D, rhs_1D);
        console_logger->info("Copied block data to typical data");
        file_logger->info("Copied block data to typical data");

        result_1D.resize(rhs_1D.size());

    }

#ifdef __Using_Real_Number__
    console_logger->info("\n\n");
    file_logger->info("\n\n");
    time_test_total = 0;

    ////////////////////////////////////////////////////////////////////////////////
    {
        ProgressBar progressBar(NUM_TEST_LOOP, 70, '#', '-');
        real tolerance;
        int iterations;
        for(auto i = 0; i < NUM_TEST_LOOP; ++i)
        {
            timer.tick();
            solver_1D.solve(mat1D, rhs_1D, result_1D, tolerance, iterations);
            time_test = timer.tock();
            console_logger->info("Test CG, time = {} ms, tolerance = {}, iterations = {}",
                                 NumberHelpers::format_with_commas(time_test), tolerance, iterations);
            file_logger->info("Test CG, avg time = {} ms, tolerance = {}, iterations = {}",
                              NumberHelpers::format_with_commas(time_test), tolerance, iterations);
            time_test_total += time_test;

            /////////////////////////////////////////////////////////////////////////////////
            ++progressBar; // record the tick
            //if(i % 10 == 0)
            progressBar.display();
        }
        progressBar.done();
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        console_logger->info("\n\n");
        file_logger->info("\n\n");
        console_logger->info("Finished test CG, avg time = {} ms\n\n",
                             NumberHelpers::format_with_commas(time_test_total / (double)NUM_TEST_LOOP));
        file_logger->info("Finished test CG, avg time = {} ms\n\n",
                          NumberHelpers::format_with_commas(time_test_total / (double)NUM_TEST_LOOP));
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////

    spdlog::drop_all();

}