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
#include <locale>
#include <iomanip>

//#define __Using_Eigen_Lib__
#define __Using_GLM_Lib__
//#define __Using_Cem_Lib__


#define USING_TBB
#define NUM_TEST_LOOP 2
#define DATA_SIZE     10000000
//#define HIGH_PRECISION

#include <spdlog/spdlog.h>
#include <ProgressBar.hpp>
#include "./TypeNames.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#ifdef USING_TBB
#include <tbb/tbb.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#ifdef HIGH_PRECISION
using real = double;
#else
using real = float;
#endif // HIGH_PRECISION

using Vec3D   = Vec3<real>;
using Vec4D   = Vec4<real>;
using Mat3x3D = Mat3x3<real>;
using Mat4x4D = Mat4x4<real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
std::string formatWithCommas(RealType value, int precision = 2)
{
    struct Numpunct : public std::numpunct<char>
    {
protected:
        virtual char do_thousands_sep() const
        {
            return ',';
        }

        virtual std::string do_grouping() const
        {
            return "\03";
        }
    };

    std::stringstream ss;
    ss.str("");
    ss.imbue({ std::locale(), new Numpunct });
    ss << std::setprecision(precision) << std::fixed << value;
    return ss.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Timer
{
    using Clock = std::chrono::high_resolution_clock;
public:
    Timer() : m_TimerTicked(false) {}

    void tick()
    {
        assert(!m_TimerTicked);
        m_StartTime   = Clock::now();
        m_TimerTicked = true;
    }

    double tock()
    {
        assert(m_TimerTicked);
        m_EndTime     = Clock::now();
        m_TimerTicked = false;
        m_ElapsedTime = std::chrono::duration<double, std::milli>(m_EndTime - m_StartTime).count();

        return m_ElapsedTime;
    }

    std::string getRunTime()
    {
        if(m_TimerTicked) {
            tock();
        }

        m_StrBuilder.str("");
        m_StrBuilder << formatWithCommas(m_ElapsedTime);
        m_StrBuilder << "ms";

        return m_StrBuilder.str();
    }

    std::string getRunTime(const char* caption)
    {
        if(m_TimerTicked) {
            tock();
        }

        m_StrBuilder.str("");
        m_StrBuilder << caption;
        m_StrBuilder << formatWithCommas(m_ElapsedTime);
        m_StrBuilder << "ms";

        return m_StrBuilder.str();
    }

    template<class Function>
    static std::string getRunTime(const char* caption, const Function& function)
    {
        Timer timer;

        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();
        function();
        timer.tock();
        ////////////////////////////////////////////////////////////////////////////////

        return timer.getRunTime(caption);
    }

private:
    Clock::time_point m_StartTime;
    Clock::time_point m_EndTime;
    std::stringstream m_StrBuilder;
    double            m_ElapsedTime;
    bool              m_TimerTicked;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void init_vec_data(std::vector<Vec3D>& vec)
{
    vec.resize(DATA_SIZE);

    for(size_t i = 0; i < vec.size(); ++i) {
        Vec3D tmp;
        tmp[0] = (real)rand();
        tmp[1] = (real)rand();
        tmp[2] = (real)rand();

        vec[i] = tmp;
    }
}

void init_vec_data(std::vector<Vec4D>& vec)
{
    vec.resize(DATA_SIZE);

    for(size_t i = 0; i < vec.size(); ++i) {
        Vec4D tmp;
        tmp[0] = (real)rand();
        tmp[1] = (real)rand();
        tmp[2] = (real)rand();
        tmp[3] = (real)rand();

        vec[i] = tmp;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void compute_work(Vec3D& vec)
{
#if 0
    auto  v = vec;
    Vec3D sum(0);

    for(int i = 0; i < 100; ++i) {
        v    = glm::normalize(v);
        sum += v;
    }

    vec = sum;
#else
    Vec4D v(vec, 0);
    Vec4D sum(0);

    for(int i = 0; i < 1; ++i) {
        v    = glm::normalize(v);
        sum += (v);
    }

    memcpy(&vec, &sum, sizeof(Vec3D));
#endif
}

inline void compute_work(Vec4D& vec)
{
    auto  v = vec;
    Vec4D sum(0);

    for(int i = 0; i < 1; ++i) {
        v    = glm::normalize(v);
        sum += v;
    }

    vec = sum;
    /*   real tmp = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3];

       if(tmp > 0) {
           vec /= std::sqrt(tmp);
       }*/
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void init_matrix_data(std::vector<Mat3x3D>& vec)
{
    vec.resize(DATA_SIZE);

    for(size_t i = 0; i < vec.size(); ++i) {
        Mat3x3D tmp;
#if defined(__Using_Eigen_Lib__) || defined(__Using_Cem_Lib__)
        tmp(0, 0) = (real)rand();
        tmp(0, 1) = (real)rand();
        tmp(0, 2) = (real)rand();

        tmp(1, 0) = (real)rand();
        tmp(1, 1) = (real)rand();
        tmp(1, 2) = (real)rand();

        tmp(2, 0) = (real)rand();
        tmp(2, 1) = (real)rand();
        tmp(2, 2) = (real)rand();
#else
#ifdef __Using_GLM_Lib__
        real* value_ptr = glm::value_ptr(tmp);
        for(int i = 0; i < 9; ++i) {
            value_ptr[i] = (real)rand();
        }
#else
        tmp[0][0] = (real)rand();
        tmp[0][1] = (real)rand();
        tmp[0][2] = (real)rand();

        tmp[1][0] = (real)rand();
        tmp[1][1] = (real)rand();
        tmp[1][2] = (real)rand();

        tmp[2][0] = (real)rand();
        tmp[2][1] = (real)rand();
        tmp[2][2] = (real)rand();
#endif
#endif

        vec[i] = tmp;
    }
}

void init_matrix_data(std::vector<Mat4x4D>& vec)
{
    vec.resize(DATA_SIZE);

    for(size_t i = 0; i < vec.size(); ++i) {
        Mat4x4D tmp;
#if defined(__Using_Eigen_Lib__) || defined(__Using_Cem_Lib__)
        tmp(0, 0) = (real)rand();
        tmp(0, 1) = (real)rand();
        tmp(0, 2) = (real)rand();
        tmp(0, 3) = (real)rand();

        tmp(1, 0) = (real)rand();
        tmp(1, 1) = (real)rand();
        tmp(1, 2) = (real)rand();
        tmp(1, 3) = (real)rand();

        tmp(2, 0) = (real)rand();
        tmp(2, 1) = (real)rand();
        tmp(2, 2) = (real)rand();
        tmp(2, 3) = (real)rand();

        tmp(3, 0) = (real)rand();
        tmp(3, 1) = (real)rand();
        tmp(3, 2) = (real)rand();
        tmp(3, 3) = (real)rand();
#else
#ifdef __Using_GLM_Lib__
        real* value_ptr = glm::value_ptr(tmp);
        for(int i = 0; i < 16; ++i) {
            value_ptr[i] = (real)rand();
        }
#else
        tmp[0][0] = (real)rand();
        tmp[0][1] = (real)rand();
        tmp[0][2] = (real)rand();
        tmp[0][3] = (real)rand();

        tmp[1][0] = (real)rand();
        tmp[1][1] = (real)rand();
        tmp[1][2] = (real)rand();
        tmp[1][3] = (real)rand();

        tmp[2][0] = (real)rand();
        tmp[2][1] = (real)rand();
        tmp[2][2] = (real)rand();
        tmp[2][3] = (real)rand();

        tmp[3][0] = (real)rand();
        tmp[3][1] = (real)rand();
        tmp[3][2] = (real)rand();
        tmp[3][3] = (real)rand();
#endif
#endif
        vec[i] = tmp;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void compute_work(Mat3x3D& mat)
{
#if defined(__Using_Eigen_Lib__) || defined(__Using_Cem_Lib__)
    real tmp = mat(0, 0) * mat(0, 0) + mat(0, 1) * mat(0, 1) + mat(0, 2) * mat(0, 2) +
               mat(1, 0) * mat(1, 0) + mat(1, 1) * mat(1, 1) + mat(1, 2) * mat(1, 2) +
               mat(2, 0) * mat(2, 0) + mat(2, 1) * mat(2, 1) + mat(2, 2) * mat(2, 2);
#else
#ifdef __Using_GLM_Lib__
    real* value_ptr = glm::value_ptr(mat);
    real  tmp       = 0;
    for(int i = 0; i < 9; ++i) {
        tmp += value_ptr[i] * value_ptr[i];
    }
#else
    real tmp = mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2] +
               mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1] + mat[1][2] * mat[1][2] +
               mat[2][0] * mat[2][0] + mat[2][1] * mat[2][1] + mat[2][2] * mat[2][2];
#endif
#endif

    if(tmp > 0) {
        mat /= std::sqrt(tmp);
    }
}

inline void compute_work(Mat4x4D& mat)
{
#if defined(__Using_Eigen_Lib__) || defined(__Using_Cem_Lib__)
    real tmp = mat(0, 0) * mat(0, 0) + mat(0, 1) * mat(0, 1) + mat(0, 2) * mat(0, 2) + mat(0, 3) * mat(0, 3) +
               mat(1, 0) * mat(1, 0) + mat(1, 1) * mat(1, 1) + mat(1, 2) * mat(1, 2) + mat(1, 3) * mat(1, 3) +
               mat(2, 0) * mat(2, 0) + mat(2, 1) * mat(2, 1) + mat(2, 2) * mat(2, 2) + mat(2, 3) * mat(2, 3) +
               mat(3, 0) * mat(3, 0) + mat(3, 1) * mat(3, 1) + mat(3, 2) * mat(3, 2) + mat(3, 3) * mat(3, 3);
#else
#ifdef __Using_GLM_Lib__
    real* value_ptr = glm::value_ptr(mat);
    real  tmp       = 0;
    for(int i = 0; i < 16; ++i) {
        tmp += value_ptr[i] * value_ptr[i];
    }
#else
    real tmp = mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1] + mat[0][2] * mat[0][2] + mat[0][3] * mat[0][3] +
               mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1] + mat[1][2] * mat[1][2] + mat[1][3] * mat[1][3] +
               mat[2][0] * mat[2][0] + mat[2][1] * mat[2][1] + mat[2][2] * mat[2][2] + mat[2][3] * mat[2][3] +
               mat[3][0] * mat[3][0] + mat[3][1] * mat[3][1] + mat[3][2] * mat[3][2] + mat[3][3] * mat[3][3];

#endif
#endif

    if(tmp > 0) {
        mat /= std::sqrt(tmp);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void test_operations(std::vector<T>& vec)
{
#ifdef USING_TBB
    tbb::parallel_for(tbb::blocked_range<size_t>(0, vec.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t i = r.begin(); i != r.end(); ++i) {
                              compute_work(vec[i]);
                          }
                      });
#else
    for(size_t i = 0; i < vec.size(); ++i) {
        compute_work(vec[i]);
    }
#endif
}

template<class MatrixType, class VectorType>
void test_mat_vec_multiplication(const std::vector<MatrixType>& mat,
                                 const std::vector<VectorType>& vec,
                                 std::vector<VectorType>&       result)
{
#ifdef USING_TBB
    tbb::parallel_for(tbb::blocked_range<size_t>(0, vec.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t i = r.begin(); i != r.end(); ++i) {
                              result[i] = mat[i] * vec[i];
                          }
                      });
#else
    for(size_t i = 0; i < vec.size(); ++i) {
        result[i] = mat[i] * vec[i];
    }
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Tested vector type performance")
{
    Timer timer;
    auto  console_logger = spdlog::stdout_color_mt("console");
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
#ifdef __Using_Cem_Lib__
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "log_cem.txt");
#ifdef HIGH_PRECISION
    console_logger->info("Test by cy, real = double");
    file_logger->info("Test by cy, real = double");
#else
    console_logger->info("Test by cy, real = float");
    file_logger->info("Test by cy, real = float");
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
#endif

    ProgressBar progressBar(NUM_TEST_LOOP, 70, '#', '-');

    std::vector<Vec3D>   vec_vec3;
    std::vector<Vec4D>   vec_vec4;
    std::vector<Mat3x3D> vec_mat3x3;
    std::vector<Mat4x4D> vec_mat4x4;
    std::vector<Vec3D>   result_vec3;
    std::vector<Vec4D>   result_vec4;

    double time_test_vec3        = 0;
    double time_test_vec4        = 0;
    double time_test_mat3x3      = 0;
    double time_test_mat4x4      = 0;
    double time_test_mat3x3_vec3 = 0;
    double time_test_mat4x4_vec4 = 0;

    ////////////////////////////////////////////////////////////////////////////////
    {
        init_vec_data(vec_vec3);
        console_logger->info("Vector 3D data initialized");
        file_logger->info("Vector 3D data initialized");

        init_vec_data(vec_vec4);
        console_logger->info("Vector 4D data initialized");
        file_logger->info("Vector 4D data initialized");

        init_matrix_data(vec_mat3x3);
        console_logger->info("Mat3x3 data initialized");
        file_logger->info("Mat3x3 data initialized");

        init_matrix_data(vec_mat4x4);
        console_logger->info("Mat4x4 data initialized");
        file_logger->info("Mat4x4 data initialized");

        result_vec3.resize(vec_vec3.size());
        result_vec4.resize(vec_vec4.size());
    }
    console_logger->info("\n\n");
    file_logger->info("\n\n");

    ////////////////////////////////////////////////////////////////////////////////
    {
        for(auto i = 0; i < NUM_TEST_LOOP; ++i) {
            timer.tick();
            test_operations<Vec3D>(vec_vec3);
            time_test_vec3 += timer.tock();


            timer.tick();
            test_operations<Vec4D>(vec_vec4);
            time_test_vec4 += timer.tock();

            timer.tick();
            test_operations<Mat3x3D>(vec_mat3x3);
            time_test_mat3x3 += timer.tock();

            timer.tick();
            test_operations<Mat4x4D>(vec_mat4x4);
            time_test_mat4x4 += timer.tock();



            timer.tick();
            test_mat_vec_multiplication<Mat3x3D, Vec3D>(vec_mat3x3, vec_vec3, result_vec3);
            time_test_mat3x3_vec3 += timer.tock();


            timer.tick();
            test_mat_vec_multiplication<Mat4x4D, Vec4D>(vec_mat4x4, vec_vec4, result_vec4);
            time_test_mat4x4_vec4 += timer.tock();

            /////////////////////////////////////////////////////////////////////////////////
            ++progressBar; // record the tick
            //if(i % 10 == 0)
            progressBar.display();
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    {
        console_logger->info("Test vec3 operation, avg time = {} ms",
                             formatWithCommas(time_test_vec3 / (double)NUM_TEST_LOOP));
        file_logger->info("Test vec3 operation, avg time = {} ms",
                          formatWithCommas(time_test_vec3 / (double)NUM_TEST_LOOP));

        console_logger->info("Test vec4 operation, avg time = {} ms",
                             formatWithCommas(time_test_vec4 / (double)NUM_TEST_LOOP));
        file_logger->info("Test vec4 operation, avg time = {} ms",
                          formatWithCommas(time_test_vec4 / (double)NUM_TEST_LOOP));


        console_logger->info("Test mat3x3 operation, avg time = {} ms",
                             formatWithCommas(time_test_mat3x3 / (double)NUM_TEST_LOOP));
        file_logger->info("Test mat3x3 operation, avg time = {} ms",
                          formatWithCommas(time_test_mat3x3 / (double)NUM_TEST_LOOP));


        console_logger->info("Test mat4x4 operation, avg time = {} ms",
                             formatWithCommas(time_test_mat4x4 / (double)NUM_TEST_LOOP));
        file_logger->info("Test mat4x4 operation, avg time = {} ms",
                          formatWithCommas(time_test_mat4x4 / (double)NUM_TEST_LOOP));


        console_logger->info("Test mat3x3 x vec3 multiplication, avg time = {} ms",
                             formatWithCommas(time_test_mat3x3_vec3 / (double)NUM_TEST_LOOP));
        file_logger->info("Test mat3x3 x vec3 multiplication, avg time = {} ms",
                          formatWithCommas(time_test_mat3x3_vec3 / (double)NUM_TEST_LOOP));


        console_logger->info("Test mat4x4 x vec4 multiplication, avg time = {} ms",
                             formatWithCommas(time_test_mat4x4_vec4 / (double)NUM_TEST_LOOP));
        file_logger->info("Test mat4x4 x vec4 multiplication, avg time = {} ms",
                          formatWithCommas(time_test_mat4x4_vec4 / (double)NUM_TEST_LOOP));
    }
    ////////////////////////////////////////////////////////////////////////////////

    console_logger->info("Finished.\n\n\n");
    file_logger->info("Finished.\n\n\n");
    spdlog::drop_all();
}