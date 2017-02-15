//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>


#define __Using_GLM_Lib__


#define USING_TBB
#define NUM_TEST_LOOP 2

//#define LOAD_SMALL_DATA
#define LOAD_MEDIUM_DATA
#define LOAD_SLOW_DATA

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
#include "./CGSolver.h"

#include <amgcl/amg.hpp>

using Vec3D = Vec3<real>;
using Mat3x3D = Mat3x3<real>;






#include <iostream>
#include <cstdlib>
#include <utility>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include <amgcl/amg.hpp>
#include <amgcl/make_solver.hpp>
#include <amgcl/backend/builtin.hpp>
#include <amgcl/adapter/ublas.hpp>
#include <amgcl/coarsening/smoothed_aggregation.hpp>
#include <amgcl/coarsening/ruge_stuben.hpp>
#include <amgcl/coarsening/aggregation.hpp>
#include <amgcl/coarsening/smoothed_aggr_emin.hpp>
#include <amgcl/relaxation/spai0.hpp>
#include <amgcl/relaxation/spai1.hpp>
#include <amgcl/relaxation/gauss_seidel.hpp>
#include <amgcl/relaxation/multicolor_gauss_seidel.hpp>
#include <amgcl/relaxation/parallel_ilu0.hpp>
#include <amgcl/relaxation/ilut.hpp>
#include <amgcl/relaxation/damped_jacobi.hpp>
#include <amgcl/relaxation/chebyshev.hpp>
#include <amgcl/solver/bicgstabl.hpp>
#include <amgcl/solver/cg.hpp>
#include <amgcl/profiler.hpp>

#include <complex>
#include <boost/type_traits.hpp>
#include <amgcl/value_type/interface.hpp>

typedef boost::numeric::ublas::compressed_matrix<
    real, boost::numeric::ublas::row_major
> ublas_matrix;

typedef boost::numeric::ublas::vector<real> ublas_vector;

namespace amgcl
{
profiler<> prof;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
    fread(data_ptr, 1, size * 3 * sizeof(float), fptr);

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void copy_data(BlockSparseMatrix<Mat3x3D>& mat_src, const std::vector<Vec3D>& rhs_src,
               ublas_matrix& mat_dst, std::vector<real>& rhs_dst)
{
    size_t num_elements = 0;
    for(UInt32 i = 0; i < mat_src.size; ++i)
    {
        for(UInt32 j = 0; j < mat_src.index[i].size(); ++j)
        {
            num_elements += mat_src.index[i].size() * 3;
        }
    }

    printf("Num total element: %lu\n", num_elements);
    mat_dst.reserve(num_elements);

    for(UInt32 i = 0; i < mat_src.size; ++i)
    {
        for(UInt32 j = 0; j < mat_src.index[i].size(); ++j)
        {
            const Mat3x3D& tmp = mat_src.value[i][j];
            const UInt32 colIndex = mat_src.index[i][j];

#ifdef __Using_GLM_Lib__
            const real* tmp_ptr = glm::value_ptr(tmp);
#endif

            for(UInt32 l1 = 0; l1 < 3; ++l1)
            {
                for(UInt32 l2 = 0; l2 < 3; ++l2)
                {

#ifdef __Using_Eigen_Lib__
                    mat_dst(i * 3 + l1, colIndex * 3 + l2) = tmp(l1, l2);
#else
#ifdef __Using_GLM_Lib__
                    mat_dst(i * 3 + l1, colIndex * 3 + l2) = tmp_ptr[l2 * 3 + l1];
#else
                    mat_dst(i * 3 + l1, colIndex * 3 + l2) = tmp[l1][l2];
#endif
#endif
                }
            }

        }
    }

    printf("mat copied\n");
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

    ProgressBar progressBar(NUM_TEST_LOOP, 70, '#', '-');

    ConjugateGradientSolver<Mat3x3D, Vec3D, real> solver_3D;
    solver_3D.set_solver_parameters(SOLVER_TOLERANCE, 10000);

    BlockSparseMatrix<Mat3x3D> mat3D;
    std::vector<Vec3D> rhs_3D;
    std::vector<Vec3D> result_3D;

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
        real tolerance;
        int iterations;
        for(auto i = 0; i < NUM_TEST_LOOP; ++i)
        {
            timer.tick();
            solver_3D.solve(mat3D, rhs_3D, result_3D, tolerance, iterations);
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
    ////////////////////////////////////////////////////////////////////////////////

    spdlog::drop_all();
















#ifdef _OPENMP
    printf("Has openmp\n");
#endif










    using amgcl::prof;

    //std::vector<int>    ptr;
    //std::vector<int>    col;
    //std::vector<real> val;

    std::vector<real> rhs;

    prof.tic("assemble");
    //int m = 3 * mat3D.size;
    //int n = sample_problem(m, val, col, ptr, rhs);



    // Create ublas matrix with the data.
    ublas_matrix A(mat3D.size * 3, mat3D.size * 3);
    copy_data(mat3D, rhs_3D, A, rhs);










    {
        console_logger->info("Copied block data to typical data");
        file_logger->info("Copied block data to typical data");


    }



    prof.toc("assemble");


    for(auto i = 0; i < NUM_TEST_LOOP; ++i)
    {
        amgcl::make_solver<
            amgcl::amg<
            amgcl::backend::builtin<real>,
            amgcl::coarsening::smoothed_aggregation,
            amgcl::relaxation::spai0
            >,
            amgcl::solver::bicgstabl<
            amgcl::backend::builtin<real>
            >
        >::params prm;
        prm.solver.tol = SOLVER_TOLERANCE;
        prm.solver.maxiter = 10000;

        prof.tic("build" + std::to_string(i));

        amgcl::make_solver<
            amgcl::amg<
            amgcl::backend::builtin<real>,
            amgcl::coarsening::smoothed_aggregation,
            amgcl::relaxation::spai0
            >,
            amgcl::solver::bicgstabl<
            amgcl::backend::builtin<real>
            >
        > solve(amgcl::backend::map(A), prm);


        prof.toc("build" + std::to_string(i));

        std::cout << solve.precond() << std::endl;

        ublas_vector x(rhs.size(), 0);

        prof.tic("solve" + std::to_string(i));
        size_t iters;
        real resid;
        boost::tie(iters, resid) = solve(rhs, x);
        prof.toc("solve" + std::to_string(i));

        std::cout << "Iterations: " << iters << std::endl
            << "Error:      " << resid << std::endl
            << std::endl << prof << std::endl << std::endl;

    }





}