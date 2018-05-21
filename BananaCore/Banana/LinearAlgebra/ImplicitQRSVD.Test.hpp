//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/Timer.h>

#include <catch.hpp>
#include <chrono>
#include <random>

using namespace Banana;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void testAccuracy(const Vector<Mat3x3<T> >& AA,
                  const Vector<Mat3x3<T> >& UU,
                  const Vector<Vec3<T> >&   SS,
                  const Vector<Mat3x3<T> >& VV)
{
    T max_UUt_error = 0, max_VVt_error = 0, max_detU_error = 0, max_detV_error = 0, max_reconstruction_error = 0;
    T ave_UUt_error = 0, ave_VVt_error = 0, ave_detU_error = 0, ave_detV_error = 0, ave_reconstruction_error = 0;
    for(size_t i = 0; i < AA.size(); i++) {
        Mat3x3<T> M = AA[i];
        Vec3<T>   S = SS[i];
        Mat3x3<T> U = UU[i];
        Mat3x3<T> V = VV[i];
        T         error;
        error           = LinaHelpers::maxAbs(U * glm::transpose(U) - Mat3x3<T>(1.0));
        max_UUt_error   = (error > max_UUt_error) ? error : max_UUt_error;
        ave_UUt_error  += fabs(error);
        error           = LinaHelpers::maxAbs(V * glm::transpose(V) - Mat3x3<T>(1.0));
        max_VVt_error   = (error > max_VVt_error) ? error : max_VVt_error;
        ave_VVt_error  += fabs(error);
        error           = fabs(fabs(glm::determinant(U)) - (T)1);
        max_detU_error  = (error > max_detU_error) ? error : max_detU_error;
        ave_detU_error += fabs(error);
        error           = fabs(fabs(glm::determinant(V)) - (T)1);
        max_detV_error  = (error > max_detV_error) ? error : max_detV_error;
        ave_detV_error += fabs(error);
        error           = LinaHelpers::maxAbs(U * LinaHelpers::diagMatrix(S) * glm::transpose(V) - M);


        if(error > T(1.0)) {
            printf("err : m : %s, u = %s, S= %s, v = %s\n", NumberHelpers::toString(M).c_str(),
                   NumberHelpers::toString(U).c_str(),
                   NumberHelpers::toString(S).c_str(),
                   NumberHelpers::toString(V).c_str());
        }


        max_reconstruction_error  = (error > max_reconstruction_error) ? error : max_reconstruction_error;
        ave_reconstruction_error += fabs(error);
    }
    ave_UUt_error            /= (T)(AA.size());
    ave_VVt_error            /= (T)(AA.size());
    ave_detU_error           /= (T)(AA.size());
    ave_detV_error           /= (T)(AA.size());
    ave_reconstruction_error /= (T)(AA.size());
    std::cout << std::setprecision(10) << " UUt max error: " << max_UUt_error
              << " VVt max error: " << max_VVt_error
              << " detU max error:" << max_detU_error
              << " detV max error:" << max_detV_error
              << " recons max error:" << max_reconstruction_error << std::endl;
    std::cout << std::setprecision(10) << " UUt ave error: " << ave_UUt_error
              << " VVt ave error: " << ave_VVt_error
              << " detU ave error:" << ave_detU_error
              << " detV ave error:" << ave_detV_error
              << " recons ave error:" << ave_reconstruction_error << std::endl;
}

template<class T>
void runImplicitQRSVD(const Int repeat, const Vector<Mat3x3<T> >& tests, const bool accuracy_test)
{
    Vector<Mat3x3<T> > UU, VV;
    Vector<Vec3<T> >   SS;
    Timer              timer;
    double             total_time = 0;
    for(Int test_iter = 0; test_iter < repeat; test_iter++) {
        timer.tick();
        for(size_t i = 0; i < tests.size(); i++) {
            Mat3x3<T> M = tests[i];
            Vec3<T>   S;
            Mat3x3<T> U;
            Mat3x3<T> V;
            QRSVD::svd(M, U, S, V);
            if(accuracy_test && test_iter == 0) {
                UU.push_back(U);
                SS.push_back(S);
                VV.push_back(V);
            }
        }
        double this_time = timer.tock();
        total_time += this_time;
        std::cout << std::setprecision(10) << "impQR time: " << this_time << std::endl;
    }
    std::cout << std::setprecision(10) << "impQR Average time: " << total_time / (double)(repeat) << std::endl;
    if(accuracy_test) {
        testAccuracy(tests, UU, SS, VV);
    }
}

template<class T>
void addRandomCases(Vector<Mat3x3<T> >& tests, const T random_range, const Int N)
{
    Int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding random test cases with range " << -random_range << " to " << random_range << std::endl;
    for(Int t = 0; t < N; t++) {
        Mat3x3<T> Z;
        Z = LinaHelpers::randMatrix<3>(-random_range, random_range);
        tests.push_back(Z);
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addIntegerCases(Vector<Mat3x3<T> >& tests, const Int int_range)
{
    Int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding integer test cases with range " << -int_range << " to " << int_range << std::endl;
    Mat3x3<T> Z;
    LinaHelpers::fill(Z, -int_range);
    Int i = 0;
    tests.push_back(Z);
    while(i < 9) {
        if((glm::value_ptr(Z))[i] < int_range) {
            (glm::value_ptr(Z))[i]++;
            tests.push_back(Z);
            i = 0;
        } else {
            (glm::value_ptr(Z))[i] = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addPerturbationFromIdentityCases(Vector<Mat3x3<T> >& tests, const Int num_perturbations, const T perturb)
{
    Int                old_count = tests.size();
    Vector<Mat3x3<T> > tests_tmp;
    Mat3x3<T>          Z = Mat3x3<T>(1.0);
    tests_tmp.push_back(Z);
    std::cout << std::setprecision(10) << "Adding perturbed identity test cases with perturbation " << perturb << std::endl;
    size_t special_cases = tests_tmp.size();
    for(size_t t = 0; t < special_cases; t++) {
        for(Int i = 0; i < num_perturbations; i++) {
            Z = LinaHelpers::randMatrix<3>(-perturb, perturb);
            tests.push_back(tests_tmp[t] + Z);
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addPerturbationCases(Vector<Mat3x3<T> >& tests, const Int int_range, const Int num_perturbations, const T perturb)
{
    Int                old_count = tests.size();
    Vector<Mat3x3<T> > tests_tmp;
    Mat3x3<T>          Z;
    LinaHelpers::fill(Z, -int_range);
    Int i = 0;
    tests_tmp.push_back(Z);
    while(i < 3) {
        if((glm::value_ptr(Z))[i] < int_range) {
            (glm::value_ptr(Z))[i]++;
            tests_tmp.push_back(Z);
            i = 0;
        } else {
            (glm::value_ptr(Z))[i] = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << "Adding perturbed integer test cases with perturbation " << perturb << " and range " << -int_range << " to " << int_range << std::endl;
    size_t special_cases = tests_tmp.size();
    for(size_t t = 0; t < special_cases; t++) {
        for(Int i = 0; i < num_perturbations; i++) {
            Z = LinaHelpers::randMatrix<3>(-perturb, perturb);
            tests.push_back(tests_tmp[t] + Z);
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

void runBenchmark()
{
    bool run_qr;

    bool        test_float;
    bool        test_double;
    bool        accuracy_test;
    bool        normalize_matrix;
    Int         number_of_repeated_experiments;
    bool        test_random;
    Int         random_range;
    Int         number_of_random_cases;
    bool        test_integer;
    Int         integer_range;
    bool        test_perturbation;
    Int         perturbation_count;
    float       float_perturbation;
    double      double_perturbation;
    bool        test_perturbation_from_identity;
    Int         perturbation_from_identity_count;
    float       float_perturbation_identity;
    double      double_perturbation_identity;
    std::string title;

    // Finalized options
    run_qr = true;

    test_float       = true;
    test_double      = true;
    normalize_matrix = false;
    Int number_of_repeated_experiments_for_timing = 2;

    for(Int test_number = 1; test_number <= 10; test_number++) {
        if(test_number == 1) {
            title                           = "random timing test";
            number_of_repeated_experiments  = number_of_repeated_experiments_for_timing;
            accuracy_test                   = false;
            test_random                     = true, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                                          // random test
            test_integer                    = false;                                                                                                                                                                                                 // integer test
            integer_range                   = 2;                                                                                                                                                                                                     // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, integer_range = 3, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                                        // perturbed itentity test
        }
        if(test_number == 2) {
            title                           = "integer timing test";
            number_of_repeated_experiments  = number_of_repeated_experiments_for_timing;
            accuracy_test                   = false;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                      // random test
            test_integer                    = true;                                                                                                                                                                               // integer test
            integer_range                   = 2;                                                                                                                                                                                  // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                     // perturbed itentity test
        }
        if(test_number == 3) {
            title                           = "integer-perturbation timing test: 256 eps";
            number_of_repeated_experiments  = number_of_repeated_experiments_for_timing;
            accuracy_test                   = false;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                     // random test
            test_integer                    = false;                                                                                                                                                                             // integer test
            integer_range                   = 2;                                                                                                                                                                                 // this variable is used by both integer test and perturbed integer test
            test_perturbation               = true, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                    // perturbed itentity test
        }
        if(test_number == 4) {
            title                           = "identity-perturbation timing test: 1e-3";
            number_of_repeated_experiments  = number_of_repeated_experiments_for_timing;
            accuracy_test                   = false;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                      // random test
            test_integer                    = false;                                                                                                                                                                              // integer test
            integer_range                   = 2;                                                                                                                                                                                  // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                      // perturbed itentity test
        }
        if(test_number == 5) {
            title                           = "identity-perturbation timing test: 256 eps";
            number_of_repeated_experiments  = number_of_repeated_experiments_for_timing;
            accuracy_test                   = false;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                                                               // random test
            test_integer                    = false;                                                                                                                                                                                                                       // integer test
            integer_range                   = 2;                                                                                                                                                                                                                           // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon();                                          // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation_identity = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed itentity test
        }

        if(test_number == 6) {
            title                           = "random accuracy test";
            number_of_repeated_experiments  = 1;
            accuracy_test                   = true;
            test_random                     = true, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                                          // random test
            test_integer                    = false;                                                                                                                                                                                                 // integer test
            integer_range                   = 2;                                                                                                                                                                                                     // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, integer_range = 3, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                                        // perturbed itentity test
        }
        if(test_number == 7) {
            title                           = "integer accuracy test";
            number_of_repeated_experiments  = 1;
            accuracy_test                   = true;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                      // random test
            test_integer                    = true;                                                                                                                                                                               // integer test
            integer_range                   = 2;                                                                                                                                                                                  // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                     // perturbed itentity test
        }
        if(test_number == 8) {
            title                           = "integer-perturbation accuracy test: 256 eps";
            number_of_repeated_experiments  = 1;
            accuracy_test                   = true;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                     // random test
            test_integer                    = false;                                                                                                                                                                             // integer test
            integer_range                   = 2;                                                                                                                                                                                 // this variable is used by both integer test and perturbed integer test
            test_perturbation               = true, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                    // perturbed itentity test
        }
        if(test_number == 9) {
            title                           = "identity-perturbation accuracy test: 1e-3";
            number_of_repeated_experiments  = 1;
            accuracy_test                   = true;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                      // random test
            test_integer                    = false;                                                                                                                                                                              // integer test
            integer_range                   = 2;                                                                                                                                                                                  // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3;                                                      // perturbed itentity test
        }
        if(test_number == 10) {
            title                           = "identity-perturbation accuracy test: 256 eps";
            number_of_repeated_experiments  = 1;
            accuracy_test                   = true;
            test_random                     = false, random_range = 3, number_of_random_cases = 1024 * 1024;                                                                                                                                                               // random test
            test_integer                    = false;                                                                                                                                                                                                                       // integer test
            integer_range                   = 2;                                                                                                                                                                                                                           // this variable is used by both integer test and perturbed integer test
            test_perturbation               = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon();                                          // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation_identity = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed itentity test
        }

        std::cout << " \n========== RUNNING BENCHMARK TEST == " << title << "=======" << std::endl;
        std::cout << " run_qr " << run_qr << std::endl;
        std::cout << " test_float " << test_float << std::endl;
        std::cout << " test_double " << test_double << std::endl;
        std::cout << " accuracy_test " << accuracy_test << std::endl;
        std::cout << " normalize_matrix " << normalize_matrix << std::endl;
        std::cout << " number_of_repeated_experiments " << number_of_repeated_experiments << std::endl;
        std::cout << " test_random " << test_random << std::endl;
        std::cout << " random_range " << random_range << std::endl;
        std::cout << " number_of_random_cases " << number_of_random_cases << std::endl;
        std::cout << " test_integer " << test_integer << std::endl;
        std::cout << " integer_range " << integer_range << std::endl;
        std::cout << " test_perturbation " << test_perturbation << std::endl;
        std::cout << " perturbation_count " << perturbation_count << std::endl;
        std::cout << " float_perturbation " << float_perturbation << std::endl;
        std::cout << " double_perturbation " << double_perturbation << std::endl;
        std::cout << " test_perturbation_from_identity " << test_perturbation_from_identity << std::endl;
        std::cout << " perturbation_from_identity_count " << perturbation_from_identity_count << std::endl;
        std::cout << " float_perturbation_identity " << float_perturbation_identity << std::endl;
        std::cout << " double_perturbation_identity " << double_perturbation_identity << std::endl;

        std::cout << std::setprecision(10) << "\n--- float test ---\n" << std::endl;
        if(test_float) {
            Vector<Mat3x3f> tests;
            if(test_integer) {
                addIntegerCases(tests, integer_range);
            }
            if(test_perturbation) {
                addPerturbationCases(tests, integer_range, perturbation_count, float_perturbation);
            }
            if(test_perturbation_from_identity) {
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, float_perturbation_identity);
            }
            if(test_random) {
                addRandomCases(tests, (float)random_range, number_of_random_cases);
            }
            if(normalize_matrix) {
                for(size_t i = 0; i < tests.size(); i++) {
                    float norm = LinaHelpers::norm2(tests[i]);
                    if(norm > (float)8 * std::numeric_limits<float>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if(run_qr) {
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);
            }
        }

        std::cout << std::setprecision(10) << "\n--- double test ---\n" << std::endl;
        if(test_double) {
            Vector<Mat3x3d> tests;
            if(test_integer) {
                addIntegerCases(tests, integer_range);
            }
            if(test_perturbation) {
                addPerturbationCases(tests, integer_range, perturbation_count, double_perturbation);
            }
            if(test_perturbation_from_identity) {
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, double_perturbation_identity);
            }
            if(test_random) {
                addRandomCases(tests, (double)random_range, number_of_random_cases);
            }
            if(normalize_matrix) {
                for(size_t i = 0; i < tests.size(); i++) {
                    float norm = LinaHelpers::norm2(tests[i]);
                    if(norm > (double)8 * std::numeric_limits<double>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if(run_qr) {
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

TEST_CASE("Test QRSVD", "[Test QRSVD]")
{
    //runBenchmark();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test 1 matrix", "[Test 1 matrix]")
{
    Mat3x3f M(1, 2, 3,
              4, 5, 6,
              7, 8, 9);
    M = glm::transpose(M);


    Vec3f   S;
    Mat3x3f U;
    Mat3x3f V;
    QRSVD::svd(M, U, S, V);

    std::cout << "M: " << NumberHelpers::toString(M, true) << std::endl << std::endl;

    std::cout << "S: " << NumberHelpers::toString(S, 7) << std::endl << std::endl;
    std::cout << "U: " << NumberHelpers::toString(U, true, 7) << std::endl << std::endl;
    std::cout << "V: " << NumberHelpers::toString(V, true, 7) << std::endl << std::endl;

    std::cout << "Recon M1: " << NumberHelpers::toString(U * LinaHelpers::diagMatrix(S) * glm::transpose(V), true) << std::endl << std::endl;


    float error = LinaHelpers::maxAbs(U * LinaHelpers::diagMatrix(S) * glm::transpose(V) - M);
    std::cout << "error: " << error << std::endl << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define NUM_TEST 1'000'000
TEST_CASE("Test time", "[Test time]")
{
    Mat3x3f M;

    Vec3f   Sv;
    Mat3x3f U;
    Mat3x3f V;

    Timer  timer;
    float  error     = 0;
    double totalTime = 0;
    for(int i = 0; i < NUM_TEST; ++i) {
        M = LinaHelpers::randMatrix<3, float>();
        timer.tick();
        QRSVD::svd(M, U, Sv, V);
        totalTime += timer.tock();
        error     += LinaHelpers::maxAbs(U * LinaHelpers::diagMatrix(Sv) * glm::transpose(V) - M);
    }

    printf("New SVD: Time = %s, avg time = %s,    error = %s, avg error = %s\n", NumberHelpers::formatWithCommas(totalTime),
           NumberHelpers::formatToScientific(totalTime / NUM_TEST),
           NumberHelpers::formatToScientific(error),
           NumberHelpers::formatToScientific(error / NUM_TEST));

    ////////////////////////////////////////////////////////////////////////////////
    //Mat3x3r S;
    //totalTime = 0;
    //error     = 0;
    //for(int i = 0; i < NUM_TEST; ++i) {
    //    M = LinaHelpers::randMatrix<3, float>();
    //    timer.tick();

    //    SVDDecomposition::svd(M[0][0], M[0][1], M[0][2], M[1][0], M[1][1], M[1][2], M[2][0], M[2][1], M[2][2],
    //                          U[0][0], U[0][1], U[0][2], U[1][0], U[1][1], U[1][2], U[2][0], U[2][1], U[2][2],
    //                          S[0][0], S[0][1], S[0][2], S[1][0], S[1][1], S[1][2], S[2][0], S[2][1], S[2][2],
    //                          V[0][0], V[0][1], V[0][2], V[1][0], V[1][1], V[1][2], V[2][0], V[2][1], V[2][2]);
    //    totalTime += timer.tock();
    //    error     += LinaHelpers::maxAbs(glm::transpose(U) * LinaHelpers::diagMatrix(Sv) * V - M);
    //}

    //printf("Old SVD: Time = %s, avg time = %s,    error = %s, avg error = %s\n", NumberHelpers::formatWithCommas(totalTime),
    //       NumberHelpers::formatToScientific(totalTime / NUM_TEST),
    //       NumberHelpers::formatToScientific(error),
    //       NumberHelpers::formatToScientific(error / NUM_TEST));
}
