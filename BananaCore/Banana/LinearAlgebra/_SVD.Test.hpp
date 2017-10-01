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

#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/LinearAlgebra/TensorHelpers.h>

#include <catch.hpp>
#include <chrono>
#include <random>

using namespace Banana;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class RandomNumber {
public:
    std::mt19937 generator;

    RandomNumber(unsigned s = 123)
        : generator(s)
    {}

    ~RandomNumber()
    {}

    /**
       Random real number from an interval
     */
    T randReal(T a, T b)
    {
        std::uniform_real_distribution<T> distribution(a, b);
        return distribution(generator);
    }

    /**
       Fill with uniform random numbers
     */
    void fill(Mat3x3<T>& x, T a, T b)
    {
        for(Int i = 0; i < 3; ++i)
            for(Int j = 0; j < 3; ++j)
                x[i][j] = randReal(a, b);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Timer {
public:
    Timer() {}

    ~Timer() {}

    /**
       \brief Start timing
     */
    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    /**
       \return time elapsed since last click in seconds
     */
    double click()
    {
        to_time         = std::chrono::steady_clock::now();
        elapsed_seconds = to_time - start_time;
        start_time      = to_time;
        return elapsed_seconds.count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> to_time;
    std::chrono::duration<double>                      elapsed_seconds;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T maxAbs(const Mat3x3<T>& A)
{
    T mabs = 0;
    for(Int i = 0; i < 3; ++i)
        for(Int j = 0; j < 3; ++j)
            mabs = MathHelpers::max(mabs, fabs(A[i][j]));

    return mabs;
}

template<class T>
inline T norm2(const Mat3x3<T>& m)
{
    T prod = T(0);

    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            prod += m[i][j] * m[i][j];
        }
    }
    return sqrt(prod);
}

template<class T, class S>
inline void fill(Mat3x3<T>& A, S x)
{
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            A[i][j] = T(x);
        }
    }
}

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
        error           = maxAbs(U * glm::transpose(U) - Mat3x3<T>(1.0));
        max_UUt_error   = (error > max_UUt_error) ? error : max_UUt_error;
        ave_UUt_error  += fabs(error);
        error           = maxAbs(V * glm::transpose(V) - Mat3x3<T>(1.0));
        max_VVt_error   = (error > max_VVt_error) ? error : max_VVt_error;
        ave_VVt_error  += fabs(error);
        error           = fabs(fabs(glm::determinant(U)) - (T)1);
        max_detU_error  = (error > max_detU_error) ? error : max_detU_error;
        ave_detU_error += fabs(error);
        error           = fabs(fabs(glm::determinant(V)) - (T)1);
        max_detV_error  = (error > max_detV_error) ? error : max_detV_error;
        ave_detV_error += fabs(error);
        error           = maxAbs(U * Mat3x3<T>(S[0], 0,    0,
                                               0,    S[1], 0,
                                               0,    0,    S[2]) * glm::transpose(V) - M);
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
void runImplicitQRSVD(const int repeat, const Vector<Mat3x3<T> >& tests, const bool accuracy_test)
{
    Vector<Mat3x3<T> > UU, VV;
    Vector<Vec3<T> >   SS;
    Timer              timer;
    timer.start();
    double total_time = 0;
    for(int test_iter = 0; test_iter < repeat; test_iter++) {
        timer.click();
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
        double this_time = timer.click();
        total_time += this_time;
        std::cout << std::setprecision(10) << "impQR time: " << this_time << std::endl;
    }
    std::cout << std::setprecision(10) << "impQR Average time: " << total_time / (double)(repeat) << std::endl;
    if(accuracy_test)
        testAccuracy(tests, UU, SS, VV);
}

template<class T>
void addRandomCases(Vector<Mat3x3<T> >& tests, const T random_range, const int N)
{
    int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding random test cases with range " << -random_range << " to " << random_range << std::endl;
    RandomNumber<T> random_gen(123);
    for(int t = 0; t < N; t++) {
        Mat3x3<T> Z;
        random_gen.fill(Z, -random_range, random_range);
        tests.push_back(Z);
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addIntegerCases(Vector<Mat3x3<T> >& tests, const int int_range)
{
    int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding integer test cases with range " << -int_range << " to " << int_range << std::endl;
    Mat3x3<T> Z;
    fill(Z, -int_range);
    Int i = 0;
    tests.push_back(Z);
    while(i < 9) {
        if((glm::value_ptr(Z))[i] < int_range) {
            (glm::value_ptr(Z))[i]++;
            tests.push_back(Z);
            i = 0;
        }
        else {
            (glm::value_ptr(Z))[i] = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addPerturbationFromIdentityCases(Vector<Mat3x3<T> >& tests, const int num_perturbations, const T perturb)
{
    int                old_count = tests.size();
    Vector<Mat3x3<T> > tests_tmp;
    Mat3x3<T>          Z = Mat3x3<T>(1.0);
    tests_tmp.push_back(Z);
    std::cout << std::setprecision(10) << "Adding perturbed identity test cases with perturbation " << perturb << std::endl;
    RandomNumber<T> random_gen(123);
    size_t          special_cases = tests_tmp.size();
    for(size_t t = 0; t < special_cases; t++) {
        for(int i = 0; i < num_perturbations; i++) {
            random_gen.fill(Z, -perturb, perturb);
            tests.push_back(tests_tmp[t] + Z);
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template<class T>
void addPerturbationCases(Vector<Mat3x3<T> >& tests, const int int_range, const int num_perturbations, const T perturb)
{
    int                old_count = tests.size();
    Vector<Mat3x3<T> > tests_tmp;
    Mat3x3<T>          Z;
    fill(Z, -int_range);
    Int i = 0;
    tests_tmp.push_back(Z);
    while(i < 3) {
        if((glm::value_ptr(Z))[i] < int_range) {
            (glm::value_ptr(Z))[i]++;
            tests_tmp.push_back(Z);
            i = 0;
        }
        else {
            (glm::value_ptr(Z))[i] = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << "Adding perturbed integer test cases with perturbation " << perturb << " and range " << -int_range << " to " << int_range << std::endl;
    RandomNumber<T> random_gen(123);
    size_t          special_cases = tests_tmp.size();
    for(size_t t = 0; t < special_cases; t++) {
        for(int i = 0; i < num_perturbations; i++) {
            random_gen.fill(Z, -perturb, perturb);
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
    int         number_of_repeated_experiments;
    bool        test_random;
    int         random_range;
    int         number_of_random_cases;
    bool        test_integer;
    int         integer_range;
    bool        test_perturbation;
    int         perturbation_count;
    float       float_perturbation;
    double      double_perturbation;
    bool        test_perturbation_from_identity;
    int         perturbation_from_identity_count;
    float       float_perturbation_identity;
    double      double_perturbation_identity;
    std::string title;

    // Finalized options
    run_qr = true;

    test_float       = true;
    test_double      = true;
    normalize_matrix = false;
    int number_of_repeated_experiments_for_timing = 2;

    for(int test_number = 1; test_number <= 10; test_number++) {
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
            if(test_integer)
                addIntegerCases(tests, integer_range);
            if(test_perturbation)
                addPerturbationCases(tests, integer_range, perturbation_count, float_perturbation);
            if(test_perturbation_from_identity)
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, float_perturbation_identity);
            if(test_random)
                addRandomCases(tests, (float)random_range, number_of_random_cases);
            if(normalize_matrix) {
                for(size_t i = 0; i < tests.size(); i++) {
                    float norm = norm2(tests[i]);
                    if(norm > (float)8 * std::numeric_limits<float>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if(run_qr)
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);
        }

        std::cout << std::setprecision(10) << "\n--- double test ---\n" << std::endl;
        if(test_double) {
            Vector<Mat3x3d> tests;
            if(test_integer)
                addIntegerCases(tests, integer_range);
            if(test_perturbation)
                addPerturbationCases(tests, integer_range, perturbation_count, double_perturbation);
            if(test_perturbation_from_identity)
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, double_perturbation_identity);
            if(test_random)
                addRandomCases(tests, (double)random_range, number_of_random_cases);
            if(normalize_matrix) {
                for(size_t i = 0; i < tests.size(); i++) {
                    float norm = norm2(tests[i]);
                    if(norm > (double)8 * std::numeric_limits<double>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if(run_qr)
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test QRSVD", "[Test QRSVD]")
{
    runBenchmark();
}
