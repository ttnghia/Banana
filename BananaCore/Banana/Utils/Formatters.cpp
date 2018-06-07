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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Utils/Formatters.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Formatters
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<FormatType formatType, Int precision, bool bMultiThreaded>
void Formatter<formatType, precision, bMultiThreaded>::init()
{
    if constexpr(bMultiThreaded) { s_lock.lock(); }
    s_ss.str("");
    if constexpr(formatType == FormatType::CommaSeparated)
    {
        s_ss.imbue({ std::locale(), new Numpunct });
        s_ss << std::setprecision(precision) << std::fixed;
    } else {
        s_ss << std::setprecision(precision) << std::fixed << std::scientific;
    }
    s_bInit = true;
    if constexpr(bMultiThreaded) { s_lock.unlock(); }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<FormatType formatType, Int precision, bool bMultiThreaded>
template<class T>
String Formatter<formatType, precision, bMultiThreaded>::format(T x)
{
    if(!s_bInit) { init(); }
    if constexpr(bMultiThreaded) { s_lock.lock(); }
    s_ss.str("");
    s_ss << x;
    String result = s_ss.str();
    if constexpr(bMultiThreaded) { s_lock.unlock(); }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<FormatType formatType, Int precision, bool bMultiThreaded>
template<Int N, class T>
String Formatter<formatType, precision, bMultiThreaded>::format(const VecX<N, T>& vec)
{
    if(!s_bInit) { init(); }
    if constexpr(bMultiThreaded) { s_lock.lock(); }
    s_ss.str("");
    s_ss << "[";
    for(Int i = 0; i < N - 1; ++i) {
        s_ss << vec[i] << ", ";
    }
    s_ss << vec[N - 1] << "]";
    String result = s_ss.str();
    if constexpr(bMultiThreaded) { s_lock.unlock(); }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<FormatType formatType, Int precision, bool bMultiThreaded>
template<Int N, class T>
String Formatter<formatType, precision, bMultiThreaded>::format(const MatXxX<N, T>& mat, bool breakLine)
{
    if(!s_bInit) { init(); }
    if constexpr(bMultiThreaded) { s_lock.lock(); }
    s_ss.str("");
    s_ss << "[";

    // N-1 line
    for(Int i = 0; i < N - 1; ++i) {
        s_ss << "[";
        for(Int j = 0; j < N - 1; ++j) {
            s_ss << mat[j][i] << ", ";
        }
        s_ss << mat[N - 1][i] << "]";

        if(breakLine) {
            s_ss << String(",\n");
        } else {
            s_ss << ", ";
        }
    }

    // last line
    {
        s_ss << "[";
        for(Int j = 0; j < N - 1; ++j) {
            s_ss << mat[j][N - 1] << ", ";
        }
        s_ss << mat[N - 1][N - 1] << "]";
    }

    s_ss << "]";
    String result = s_ss.str();
    if constexpr(bMultiThreaded) { s_lock.unlock(); }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// instatiation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNN_INSTANTIATE_FORMATTER_SCALAR(formatType, precision, multithreaded)                           \
    template class Formatters::Formatter<formatType, precision, multithreaded>;                            \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<Int>(Int x);       \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<Int64>(Int64 x);   \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<UInt>(UInt x);     \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<UInt64>(UInt64 x); \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<float>(float x);   \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<double>(double x);

#define __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, precision) \
    __BNN_INSTANTIATE_FORMATTER_SCALAR(formatType, precision, true);          \
    __BNN_INSTANTIATE_FORMATTER_SCALAR(formatType, precision, false);

#define __BNN_INSTANTIATE_FORMATTER_VECTOR(formatType, precision, multithreaded, N)                                                          \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, Int>(const VecX<N, Int>&v);                       \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, UInt>(const VecX<N, UInt>&v);                     \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, float>(const VecX<N, float>&v);                   \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, double>(const VecX<N, double>&v);                 \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, Int>(const MatXxX<N, Int>&m, bool breakLine);     \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, UInt>(const MatXxX<N, UInt>&m, bool breakLine);   \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, float>(const MatXxX<N, float>&m, bool breakLine); \
    template String Formatters::Formatter<formatType, precision, multithreaded>::format<N, double>(const MatXxX<N, double>&m, bool breakLine);

#define __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, precision, N) \
    __BNN_INSTANTIATE_FORMATTER_VECTOR(formatType, precision, true,  N);         \
    __BNN_INSTANTIATE_FORMATTER_VECTOR(formatType, precision, false, N);

#define __BNN_INSTANTIATE_FORMATTER_SCALAR_COMMON_PRECISION(formatType) \
    __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, 2);      \
    __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, 3);      \
    __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, 5);      \
    __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, 7);      \
    __BNN_INSTANTIATE_FORMATTER_SCALAR_THREAD_SAFE(formatType, 10);

#define __BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION(formatType, N) \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, 2,  N);     \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, 3,  N);     \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, 5,  N);     \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, 7,  N);     \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_THREAD_SAFE(formatType, 10, N);

#define __BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION_COMMON_VEC_SIZE(formatType) \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION(formatType, 2);                 \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION(formatType, 3);                 \
    __BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION(formatType, 4);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INSTANTIATE_FORMATTER_SCALAR_COMMON_PRECISION(FormatType::CommaSeparated);
__BNN_INSTANTIATE_FORMATTER_SCALAR_COMMON_PRECISION(FormatType::Scientific);

__BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION_COMMON_VEC_SIZE(FormatType::CommaSeparated);
__BNN_INSTANTIATE_FORMATTER_VECTOR_COMMON_PRECISION_COMMON_VEC_SIZE(FormatType::Scientific);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace namespace Banana::Formatters
