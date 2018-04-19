//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

#pragma once

#include <Banana/Setup.h>

#include <algorithm>
#include <climits>
#include <vector>
#include <iostream>
#include <cmath>

#undef min
#undef max

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::MathHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline constexpr T sqr(T x)
{
    return x * x;
}

template<class T>
inline constexpr T cube(T x)
{
    return x * x * x;
}

template<class T>
inline constexpr T pow3(T x)
{
    return x * x * x;
}

template<class T>
inline constexpr T pow4(T x)
{
    return sqr(sqr(x));
}

template<class T>
inline constexpr T pow5(T x)
{
    return pow4(x) * x;
}

template<class T>
inline constexpr T pow6(T x)
{
    return pow3(sqr(x));
}

template<class T>
inline constexpr T pow7(T x)
{
    return pow6(x) * x;
}

template<class T>
inline constexpr T pow8(T x)
{
    return sqr(pow4(x));
}

template<class T, class I>
inline constexpr T pow(T x, I n)
{
    T result = T(1);
    for(I i = 0; i < n; ++i) {
        result *= x;
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float approx_rsqrt(float x)
{
    const float xhalf = 0.5f * x;
    int32_t     i     = *(int32_t*)&x; // View x as an int.
    i = 0x5f37599e - (i >> 1);         // Initial guess.
    x = *(float*)&i;                   // View i as float.
    x = x * (1.5f - xhalf * x * x);    // Newton step.
    x = x * (1.5f - xhalf * x * x);    // Newton step again.
    x = x * (1.5f - xhalf * x * x);    // Newton step again.
    return x;
}

inline double approx_rsqrt(double x)
{
    const double xhalf = 0.5 * x;
    int64_t      i     = *(int64_t*)&x; // View x as an int.
    i = 0x5fe6eb50c7b537a9 - (i >> 1);  // Initial guess.
    x = *(double*)&i;                   // View i as float.
    x = x * (1.5 - xhalf * x * x);      // Newton step.
    x = x * (1.5 - xhalf * x * x);      // Newton step again.
    x = x * (1.5 - xhalf * x * x);      // Newton step again.
    return x;
}

template<class T>
inline constexpr T approx_sqrt(T x)
{
    return x * approx_rsqrt(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline constexpr T norm2(T x1, T x2)
{
    return std::sqrt(sqr(x1) + sqr(x2));
}

template<class T>
inline constexpr T norm2(T x1, T x2, T x3)
{
    return std::sqrt(sqr(x1) + sqr(x2) + sqr(x3));
}

template<class T>
inline constexpr T norm3(T x1, T x2)
{
    return std::pow(pow3(x1) + pow3(x2), T(1.0 / 3.0));
}

template<class T>
inline constexpr T norm3(T x1, T x2, T x3)
{
    return std::pow(pow3(x1) + pow3(x2) + pow3(x3), T(1.0 / 3.0));
}

template<class T>
inline constexpr T norm4(T x1, T x2)
{
    return std::pow(pow4(x1) + pow4(x2), T(1.0 / 4.0));
}

template<class T>
inline constexpr T norm4(T x1, T x2, T x3)
{
    return std::pow(pow4(x1) + pow4(x2) + pow4(x3), T(1.0 / 4.0));
}

template<class T>
inline constexpr T norm5(T x1, T x2)
{
    return std::pow(pow5(x1) + pow5(x2), T(1.0 / 5.0));
}

template<class T>
inline constexpr T norm5(T x1, T x2, T x3)
{
    return std::pow(pow5(x1) + pow5(x2) + pow5(x3), T(1.0 / 5.0));
}

template<class T>
inline constexpr T norm6(T x1, T x2)
{
    return std::pow(pow6(x1) + pow6(x2), T(1.0 / 6.0));
}

template<class T>
inline constexpr T norm6(T x1, T x2, T x3)
{
    return std::pow(pow6(x1) + pow6(x2) + pow6(x3), T(1.0 / 6.0));
}

template<class T>
inline constexpr T norm7(T x1, T x2)
{
    return std::pow(pow7(x1) + pow7(x2), T(1.0 / 7.0));
}

template<class T>
inline constexpr T norm7(T x1, T x2, T x3)
{
    return std::pow(pow7(x1) + pow7(x2) + pow7(x3), T(1.0 / 7.0));
}

template<class T>
inline constexpr T norm8(T x1, T x2)
{
    return std::pow(pow8(x1) + pow8(x2), T(1.0 / 8.0));
}

template<class T>
inline constexpr T norm8(T x1, T x2, T x3)
{
    return std::pow(pow8(x1) + pow8(x2) + pow8(x3), T(1.0 / 8.0));
}

template<class T>
inline constexpr T norm_inf(T x1, T x2)
{
    return max(std::abs(x1), std::abs(x2));
}

template<class T>
inline constexpr T norm_inf(T x1, T x2, T x3)
{
    return max(std::abs(x1), std::abs(x2), std::abs(x3));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline constexpr T min(T a1, T a2)
{
    return std::min(a1, a2);
}

template<class T>
inline constexpr T min(T a1, T a2, T a3)
{
    return std::min(a1, std::min(a2, a3));
}

template<class T>
inline constexpr T min(T a1, T a2, T a3, T a4)
{
    return std::min(std::min(a1, a2), std::min(a3, a4));
}

template<class T>
inline constexpr T min(T a1, T a2, T a3, T a4, T a5)
{
    return min(std::min(a1, a2), std::min(a3, a4), a5);
}

template<class T>
inline constexpr T min(T a1, T a2, T a3, T a4, T a5, T a6)
{
    return min(std::min(a1, a2), std::min(a3, a4), std::min(a5, a6));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// exponential smooth min (k = 32);
template<class T>
inline constexpr T smin_exp(T a, T b, T k = T(32.0))
{
    T res = exp(-k * a) + exp(-k * b);
    return -log(res) / k;
}

// polynomial smooth min (k = 0.1);
template<class T>
inline constexpr T smin_poly(T a, T b, T k = T(0.1))
{
    T h = clamp(T(0.5) + T(0.5) * (b - a) / k, T(0), T(1.0));
    return lerp(b, a, h) - k * h * (T(1.0) - h);
}

// power smooth min (k = 8);
template<class T>
inline constexpr T smin_pow(T a, T b, T k = T(8.0))
{
    a = pow(a, k); b = pow(b, k);
    return pow((a * b) / (a + b), T(1.0) / k);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
constexpr VecX<N, T> min(const VecX<N, T>& a, const VecX<N, T>& b)
{
    VecX<N, T> result;
    for(Int i = 0; i < N; ++i) {
        result[i] = min(a[i], b[i]);
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline constexpr T max(T a1, T a2)
{
    return std::max(a1, a2);
}

template<class T>
inline constexpr T max(T a1, T a2, T a3)
{
    return std::max(a1, std::max(a2, a3));
}

template<class T>
inline constexpr T max(T a1, T a2, T a3, T a4)
{
    return std::max(std::max(a1, a2), std::max(a3, a4));
}

template<class T>
inline constexpr T max(T a1, T a2, T a3, T a4, T a5)
{
    return max(std::max(a1, a2), std::max(a3, a4), a5);
}

template<class T>
inline constexpr T max(T a1, T a2, T a3, T a4, T a5, T a6)
{
    return max(std::max(a1, a2), std::max(a3, a4), std::max(a5, a6));
}

template<Int N, class T>
VecX<N, T> max(const VecX<N, T>& a, const VecX<N, T>& b)
{
    VecX<N, T> result;
    for(Int i = 0; i < N; ++i) {
        result[i] = max(a[i], b[i]);
    }
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void minmax(T a1, T a2, T& amin, T& amax)
{
    if(a1 < a2) {
        amin = a1;
        amax = a2;
    } else {
        amin = a2;
        amax = a1;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void minmax(T a1, T a2, T a3, T& amin, T& amax)
{
    amin = min(a1, a2, a3);
    amax = max(a1, a2, a3);
}

template<class T>
inline void minmax(T a1, T a2, T a3, T a4, T& amin, T& amax)
{
    amin = min(a1, a2, a3, a4);
    amax = max(a1, a2, a3, a4);
}

template<class T>
inline void minmax(T a1, T a2, T a3, T a4, T a5, T& amin, T& amax)
{
    amin = min(a1, a2, a3, a4, a5);
    amax = max(a1, a2, a3, a4, a5);
}

template<class T>
inline void minmax(T a1, T a2, T a3, T a4, T a5, T a6, T& amin, T& amax)
{
    amin = min(a1, a2, a3, a4, a5, a6);
    amax = max(a1, a2, a3, a4, a5, a6);
}

template<class T>
inline void update_minmax(T a1, T& amin, T& amax)
{
    if(a1 < amin) {
        amin = a1;
    } else if(a1 > amax) {
        amax = a1;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void sort(T& a, T& b, T& c)
{
    T temp;

    if(a < b) {
        if(a < c) {
            if(c < b) {   // a<c<b
                temp = c;
                c    = b;
                b    = temp;
            }     // else: a<b<c
        } else {  // c<a<b
            temp = c;
            c    = b;
            b    = a;
            a    = temp;
        }
    } else {
        if(b < c) {
            if(a < c) {   //b<a<c
                temp = b;
                b    = a;
                a    = temp;
            } else {         // b<c<a
                temp = b;
                b    = c;
                c    = a;
                a    = temp;
            }
        } else {     // c<b<a
            temp = c;
            c    = a;
            a    = temp;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline constexpr T clamp(T a, T lower, T upper)
{
    if(a < lower) {
        return lower;
    } else if(a > upper) {
        return upper;
    } else {
        return a;
    }
}

template<class T>
inline constexpr T clamp01(T a)
{
    if(a < T(0)) {
        return T(0);
    } else if(a > T(1.0)) {
        return T(1.0);
    } else {
        return a;
    }
}

template<class T, class S>
inline constexpr T clamp(T a, S lower, S upper)
{
    if(a < T(lower)) {
        return T(lower);
    } else if(a > T(upper)) {
        return T(upper);
    } else {
        return a;
    }
}

template<Int N, class T>
inline VecX<N, T> clamp(const VecX<N, T>& x, const VecX<N, T>& a, const VecX<N, T>& b)
{
    VecX<N, T> result;
    for(Int i = 0; i < N; ++i) {
        result[i] = clamp(x[i], a[i], b[i]);
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// only makes sense with T=float or double
template<class T>
inline constexpr T smooth_step(T r)
{
    if(r < 0) {
        return 0;
    } else if(r > T(1.0)) {
        return T(1.0);
    }

    return r * r * r * (T(10.0) + r * (T(-15.0) + r * T(6.0)));
}

// only makes sense with T=float or double
template<class T>
inline constexpr T smooth_step(T r, T r_lower, T r_upper, T value_lower, T value_upper)
{
    return value_lower + smooth_step((r - r_lower) / (r_upper - r_lower)) * (value_upper - value_lower);
}

// only makes sense with T=float or double
template<class T>
inline constexpr T ramp(T r)
{
    return smooth_step((r + T(1.0)) / T(2.0)) * T(2.0) - T(1.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline int lround(T x)
{
    if(x > 0) {
        return (x - floor(x) < T(0.5)) ? (int)floor(x) : (int)ceil(x);
    } else {
        return (x - floor(x) <= T(0.5)) ? (int)floor(x) : (int)ceil(x);
    }
}

template<class T>
inline double remainder(T x, T y)
{
    return x - std::floor(x / y + T(0.5)) * y;
}

inline unsigned int round_up_to_power_of_two(unsigned int n)
{
    int exponent = 0;
    --n;

    while(n) {
        ++exponent;
        n >>= 1;
    }

    return 1 << exponent;
}

inline unsigned int round_down_to_power_of_two(unsigned int n)
{
    int exponent = 0;

    while(n > 1) {
        ++exponent;
        n >>= 1;
    }

    return 1 << exponent;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Transforms even the sequence 0,1,2,3,... into reasonably good random numbers
// Challenge: improve on this in speed and "randomness"!
// This seems to pass several statistical tests, and is a bijective map (of 32-bit unsigned ints)
inline unsigned int randhash(unsigned int seed)
{
    unsigned int i = (seed ^ 0xA3C59AC3u) * 2654435769u;
    i ^= (i >> 16);
    i *= 2654435769u;
    i ^= (i >> 16);
    i *= 2654435769u;
    return i;
}

// the inverse of randhash
inline unsigned int unhash(unsigned int h)
{
    h *= 340573321u;
    h ^= (h >> 16);
    h *= 340573321u;
    h ^= (h >> 16);
    h *= 340573321u;
    h ^= 0xA3C59AC3u;
    return h;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T frand()
{
    return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

template<class T>
inline T frand(T a, T b)
{
    return (static_cast<T>(rand()) / static_cast<T>(RAND_MAX)) * (b - a) + a;
}

template<class Vector>
inline Vector vrand()
{
    Vector tmp;
    for(Int i = 0; i < tmp.length(); ++i) {
        tmp[i] = frand<Vector::value_type>();
    }
    return tmp;
}

template<class T>
inline T frand11()
{
    return frand<T>(T(-1.0), T(1.0));
}

template<class Vector>
inline Vector vrand11()
{
    Vector tmp;
    for(Int i = 0; i < tmp.length(); ++i) {
        tmp[i] = frand(Vector::value_type(-1.0), Vector::value_type(1.0));
    }
    return tmp;
}

// returns repeatable stateless pseudo-random number in [0,1]
template<class T>
inline T frandhash(unsigned int seed)
{
    return T(randhash(seed)) / static_cast<T>(UINT_MAX);
}

// returns repeatable stateless pseudo-random number in [a,b]
template<class T>
inline T frandhash(T a, T b, unsigned int seed)
{
    return (b - a) * (randhash(seed) / static_cast<T>(UINT_MAX)) + a;
}

template<class T>
inline T frandhash11(unsigned int seed)
{
    return frandhash(T(-1.0), T(1.0), seed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline int intlog2(int x)
{
    int exp = -1;

    while(x) {
        x >>= 1;
        ++exp;
    }

    return exp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void get_barycentric(T x, int& i, T& f)
{
    T s = std::floor(x);
    i = static_cast<int>(s);
    f = (T)(x - s);
}

template<Int N, class T>
inline void get_barycentric(const VecX<N, T>& x, VecX<N, int>& i, VecX<N, T>& f)
{
    for(Int j = 0; j < N; ++j) {
        T s = std::floor(x[j]);
        i[j] = static_cast<int>(s);
        f[j] = (T)(x[j] - s);
    }
}

template<class T>
inline void get_barycentric(T x, int& i, T& f, int i_low, int i_high)
{
    T s = std::floor(x);
    i = static_cast<int>(s);

    if(i < i_low) {
        i = i_low;
        f = 0;
    } else if(i > i_high - 2) {
        i = i_high - 2;
        f = 1;
    } else {
        f = (T)(x - s);
    }
}

template<class T>
inline void get_bary_below(T x, int& i, T& f, int i_low, int i_high)
{
    T s = std::floor(x - T(0.5));
    i = static_cast<int>(s);

    if(i < i_low) {
        i = i_low;
        f = 0;
    } else if(i > i_high - 2) {
        i = i_high - 2;
        f = T(1.0);
    } else {
        f = T(x - T(0.5) - s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class S, class T>
inline S lerp(const S& value0, const S& value1, T f)
{
    return (T(1.0) - f) * value0 + f * value1;
}

template<class S, class T>
inline S bilerp(const S& v00, const S& v10,
                const S& v01, const S& v11,
                T fx, T fy)
{
    return lerp(
        lerp(v00, v10, fx),
        lerp(v01, v11, fx),
        fy);
}

template<class S, class T>
inline S trilerp(const S& v000, const S& v100,
                 const S& v010, const S& v110,
                 const S& v001, const S& v101,
                 const S& v011, const S& v111,
                 T fx, T fy, T fz)
{
    return lerp(
        bilerp(v000, v100, v010, v110, fx, fy),
        bilerp(v001, v101, v011, v111, fx, fy),
        fz);
}

template<class S, class T>
inline S quadlerp(const S& v0000, const S& v1000,
                  const S& v0100, const S& v1100,
                  const S& v0010, const S& v1010,
                  const S& v0110, const S& v1110,
                  const S& v0001, const S& v1001,
                  const S& v0101, const S& v1101,
                  const S& v0011, const S& v1011,
                  const S& v0111, const S& v1111,
                  T fx, T fy, T fz, T ft)
{
    return lerp(
        trilerp(v0000, v1000, v0100, v1100, v0010, v1010, v0110, v1110, fx, fy, fz),
        trilerp(v0001, v1001, v0101, v1101, v0011, v1011, v0111, v1111, fx, fy, fz),
        ft);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// f should be between 0 and 1, with f=0.5 corresponding to balanced weighting between w0 and w2
template<class T>
inline void quadratic_bspline_weights(T f, T& w0, T& w1, T& w2)
{
    w0 = T(0.5) * sqr(f - T(1.0));
    w1 = T(0.75) - sqr(f - T(0.5));;
    w2 = T(0.5) * sqr(f);
}

// f should be between 0 and 1
template<class T>
inline void cubic_interp_weights(T f, T& wneg1, T& w0, T& w1, T& w2)
{
    T f2(f * f), f3(f2 * f);
    wneg1 = -T(1.0 / 3.0) * f + T(1.0 / 2.0) * f2 - T(1.0 / 6.0) * f3;
    w0    = T(1.0) - f2 + T(1.0 / 2.0) * (f3 - f);
    w1    = f + T(1.0 / 2.0) * (f2 - f3);
    w2    = T(1.0 / 6.0) * (f3 - f);
}

template<class S, class T>
inline S cubic_interp(const S& value_neg1, const S& value0, const S& value1,
                      const S& value2, T f)
{
    T wneg1, w0, w1, w2;
    cubic_interp_weights(f, wneg1, w0, w1, w2);
    return wneg1 * value_neg1 + w0 * value0 + w1 * value1 + w2 * value2;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T sharp_kernel(T r2, T h)
{
    if(r2 > h * h) {
        return 0;
    }

    return fmax(h * h / fmax(r2, T(1.0e-5)) - T(1.0), T(0));
}

template<class T>
T smooth_kernel(T r2, T h2)
{
    return pow(fmax(T(1.0) - r2 / h2, T(0)), T(3.0));
}

template<class T>
inline constexpr T smooth_kernel_laplacian(T r2, T h2)
{
    T x2 = T(sqrt(r2 / h2));
    return x2 > T(1.0) ? 0 : (T(1.0) - x2);
}

template<class T>
inline constexpr T bilinear_kernel(T dx_, T dy_)
{
    T dx = dx_ > 0 ? dx_ : -dx_;
    T dy = dy_ > 0 ? dy_ : -dy_;

    if(dx > T(1.0) || dy > T(1.0)) {
        return 0;
    }

    return (T(1.0) - dx) * (T(1.0) - dy);
}

template<class T>
inline constexpr T trilinear_kernel(T dx_, T dy_, T dz_)
{
    T dx = dx_ > 0 ? dx_ : -dx_;
    T dy = dy_ > 0 ? dy_ : -dy_;
    T dz = dz_ > 0 ? dz_ : -dz_;

    if(dx > T(1.0) || dy > T(1.0) || dz > T(1.0)) {
        return 0;
    }

    return (T(1.0) - dx) * (T(1.0) - dy) * (T(1.0) - dz);
}

template<class T>
inline constexpr T spiky_kernel(T r, T h)
{
    if(r > h) {
        return 0;
    }

    T rh = r / h;
    rh = rh * rh;

    T val = T(315.0 / 64.0 / M_PI) * (T(1.0) - rh) * (T(1.0) - rh) * (T(1.0) - rh);

    return val;
}

template<class T>
inline constexpr T quad_bspline_kernel(T f)
{
    T x  = f > 0 ? f : -f;
    T x2 = x * x;

    if(x >= T(1.5)) {
        return T(0);
    }

    if(x >= T(0.5)) {
        return max(T(0), (T(0.5) * x2 - T(1.5) * x + T(9.0 / 8.0)));
    }

    return (T(0.75) - x2);
}

template<class T>
inline constexpr T quad_bspline_grad(T f)
{
    T x  = f > 0 ? f : -f;
    T x2 = x * x;

    if(x >= T(1.5)) {
        return T(0);
    }

    if(x >= T(0.5)) {
        if(f < 0) {
            return fabs(x - T(1.5));
        } else {
            return (x - T(1.5));
        }
    }

    return (-T(2.0) * f);
}

template<class T>
inline constexpr T cubic_bspline_kernel(T f)
{
    T x = f > 0 ? f : -f;

    if(x >= T(2.0)) {
        return T(0);
    }

    if(x >= T(1.0)) {
        return max(T(0), x * (x * (-x / T(6.0) + T(1.0)) - T(2.0)) + T(4.0 / 3.0));
    }

    // else, x < 1.0
    return x * x * (x * T(0.5) - T(1.0)) + T(2.0 / 3.0);
}

template<class T>
inline constexpr T cubic_bspline_2d(T x, T y)
{
    return cubic_bspline_kernel(x) * cubic_bspline_kernel(y);
}

template<class T>
inline constexpr T cubic_bspline_3d(T x, T y, T z)
{
    return cubic_bspline_kernel(x) * cubic_bspline_kernel(y) * cubic_bspline_kernel(z);
}

template<class T>
inline constexpr T cubic_bspline_grad(T x)
{
    T abs_x = x > 0 ? x : -x;

    if(abs_x >= T(2.0)) {
        return T(0);
    }

    if(abs_x >= T(1.0)) {
        return -x* abs_x* T(0.5) + T(2.0) * x - T(2.0) * x / abs_x;
    }

    // else, x < 1.0
    return T(1.5) * x * abs_x - T(2.0) * x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void cycle_array(T* arr, int size)
{
    T t = arr[0];

    for(int i = 0; i < size - 1; ++i) {
        arr[i] = arr[i + 1];
    }

    arr[size - 1] = t;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// value < 0 is inside
template<class T>
inline constexpr T fraction_inside(T left_val, T right_val)
{
    if(left_val < 0 && right_val < 0) {
        return T(1.0);
    }

    if(left_val < 0 && right_val >= 0) {
        return left_val / (left_val - right_val);
    }

    if(left_val >= 0 && right_val < 0) {
        return right_val / (right_val - left_val);
    }

    return T(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Given four signed distance values (square corners), determine what fraction of the square is "inside"
template<class T>
T fraction_inside(T phi_bl, T phi_br, T phi_tl, T phi_tr)
{
    int inside_count = (phi_bl < 0 ? 1 : 0) + (phi_tl < 0 ? 1 : 0) + (phi_br < 0 ? 1 : 0) + (phi_tr < 0 ? 1 : 0);
    T   list[]       = { phi_bl, phi_br, phi_tr, phi_tl };

    if(inside_count == 4) {
        return T(1.0);
    } else if(inside_count == 3) {
        //rotate until the positive value is in the first position
        while(list[0] < 0) {
            MathHelpers::cycle_array(list, 4);
        }

        //Work out the area of the exterior triangle
        T side0 = T(1.0) - fraction_inside(list[0], list[3]);
        T side1 = T(1.0) - fraction_inside(list[0], list[1]);
        return T(1.0) - T(0.5) * side0 * side1;
    } else if(inside_count == 2) {
        //rotate until a negative value is in the first position, and the next negative is in either slot 1 or 2.
        while(list[0] >= 0 || !(list[1] < 0 || list[2] < 0)) {
            MathHelpers::cycle_array(list, 4);
        }

        if(list[1] < 0) { //the matching signs are adjacent
            T side_left  = fraction_inside(list[0], list[3]);
            T side_right = fraction_inside(list[1], list[2]);
            return T(0.5) * (side_left + side_right);
        } else {     //matching signs are diagonally opposite
            //determine the centre point's sign to disambiguate this case
            T middle_point = T(0.25) * (list[0] + list[1] + list[2] + list[3]);

            if(middle_point < 0) {
                T area = T(0);

                //first triangle (top left)
                T side1 = T(1.0) - fraction_inside(list[0], list[3]);
                T side3 = T(1.0) - fraction_inside(list[2], list[3]);

                area += T(0.5) * side1 * side3;

                //second triangle (top right)
                T side2 = T(1.0) - fraction_inside(list[2], list[1]);
                T side0 = T(1.0) - fraction_inside(list[0], list[1]);
                area += T(0.5) * side0 * side2;

                return T(1.0) - area;
            } else {
                T area = T(0);

                //first triangle (bottom left)
                T side0 = fraction_inside(list[0], list[1]);
                T side1 = fraction_inside(list[0], list[3]);
                area += T(0.5) * side0 * side1;

                //second triangle (top right)
                T side2 = fraction_inside(list[2], list[1]);
                T side3 = fraction_inside(list[2], list[3]);
                area += T(0.5) * side2 * side3;
                return area;
            }
        }
    } else if(inside_count == 1) {
        //rotate until the negative value is in the first position
        while(list[0] >= 0) {
            MathHelpers::cycle_array(list, 4);
        }

        //Work out the area of the interior triangle, and subtract from 1.
        T side0 = fraction_inside(list[0], list[3]);
        T side1 = fraction_inside(list[0], list[1]);
        return T(0.5) * side0 * side1;
    } else {
        return T(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec3<RealType> EulerToAxisAngle(Vec2<RealType> EulerAngles, bool bRadiansIn = true, bool bRadiansOut = true)
{
    __BNN_UNUSED(EulerAngles);
    __BNN_UNUSED(bRadiansIn);
    __BNN_UNUSED(bRadiansOut);
    // This function is a fake function for 2D rotation, so the result is not correct
    return Vec3<RealType>(Vec2<RealType>(1), 0);
}

template<class RealType>
Vec4<RealType> EulerToAxisAngle(Vec3<RealType> EulerAngles, bool bRadiansIn = true, bool bRadiansOut = true)
{
    if(glm::length2(EulerAngles) < Tiny) {
        return Vec4<RealType>(Vec3<RealType>(1.0), 0);
    }

    if(!bRadiansIn) {
        EulerAngles.x = glm::radians(EulerAngles.x);
        EulerAngles.y = glm::radians(EulerAngles.y);
        EulerAngles.z = glm::radians(EulerAngles.z);
    }

    RealType c1 = cos(EulerAngles[1] * RealType(0.5));
    RealType s1 = sin(EulerAngles[1] * RealType(0.5));
    RealType c2 = cos(EulerAngles[2] * RealType(0.5));
    RealType s2 = sin(EulerAngles[2] * RealType(0.5));
    RealType c3 = cos(EulerAngles[0] * RealType(0.5));
    RealType s3 = sin(EulerAngles[0] * RealType(0.5));

    RealType c1c2  = c1 * c2;
    RealType s1s2  = s1 * s2;
    RealType w     = c1c2 * c3 - s1s2 * s3;
    RealType x     = c1c2 * s3 + s1s2 * c3;
    RealType y     = s1 * c2 * c3 + c1 * s2 * s3;
    RealType z     = c1 * s2 * c3 - s1 * c2 * s3;
    RealType angle = RealType(2.0) * acos(w);
    if(!bRadiansIn && !bRadiansOut) {
        angle = glm::degrees(angle);
    }

    return Vec4<RealType>(glm::normalize(Vec3<RealType>(x, y, z)), angle);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::MathHelpers
