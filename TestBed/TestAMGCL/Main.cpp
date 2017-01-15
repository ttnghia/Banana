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


#define __Using_GLM_Lib__


#define USING_TBB
#define NUM_TEST_LOOP 10
#define HIGH_PRECISION



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

//------------------------------------------------------------------------------------------
void load_data(BlockSparseMatrix<Mat3x3D>& mat, std::vector<Vec3D>& rhs)
{
    FILE* fptr = NULL;
    fptr = fopen("rhs.dat", "rb");
    if(!fptr)
    {
        printf("Error: Cannot open file for reading!\n");
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

    ////////////////////////////////////////////////////////////////////////////////
    // load matrix
    mat.resize(rhs.size());
    mat.load_from_file("mat.dat");
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

    ProgressBar progressBar(NUM_TEST_LOOP, 70, '#', '-');

    ConjugateGradientSolver<Mat3x3D, Vec3D, real> solver_3D;
    solver_3D.set_solver_parameters(1e-30, 10000);

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

}