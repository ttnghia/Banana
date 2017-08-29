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

#pragma once

#include <Banana/TypeNames.h>
#include <Banana/Array/Array2.h>
#include <Banana/Array/Array3.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ArrayHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline RealType interpolateValueLinear(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    int      i, j;
    RealType fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
inline RealType interpolateValueLinear(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    int      i, j, k;
    RealType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.sizeZ());

    return MathHelpers::trilerp(
        grid(i, j, k), grid(i + 1, j, k), grid(i, j + 1, k), grid(i + 1, j + 1, k),
        grid(i, j, k + 1), grid(i + 1, j, k + 1), grid(i, j + 1, k + 1), grid(i + 1, j + 1, k + 1),
        fi, fj, fk);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec2<RealType> grad_bilerp(const RealType& v00, const RealType& v10,
                           const RealType& v01, const RealType& v11,
                           RealType fx, RealType fy)
{
    return Vec2<RealType>(fy - 1.0, fx - 1.0) * v00 +
           Vec2<RealType>(1.0 - fy, -fx) * v10 +
           Vec2<RealType>(-fy,      1.0 - fx) * v01 +
           Vec2<RealType>(fy,       fx) * v11;
}

template<class RealType>
Vec2<RealType> interpolateValueAffine(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    int      i, j;
    RealType fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    return grad_bilerp(
        grid(i, j), grid(i + 1, j),
        grid(i, j + 1), grid(i + 1, j + 1),
        fx, fy);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline RealType interpolateValueCubicBSpline(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    int      i, j;
    RealType fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());

    RealType sumW   = 0;
    RealType sumVal = 0;

    for(int lj = -1; lj <= 2; ++lj)
    {
        for(int li = -1; li <= 2; ++li)
        {
            const Vec2i ind = Vec2i(i + li, j + lj);

            if(grid.isValidIndex(ind))
            {
                const RealType weight = MathHelpers::cubic_spline_kernel_2d(fi - (RealType)li, fj - (RealType)lj);
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }

    if(sumW > RealType(1e-30))
        return sumVal / sumW;
    else
        return RealType(0);
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
inline RealType interpolateValueCubicBSpline(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    int      i, j, k;
    RealType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.sizeZ());

    RealType sumW   = 0;
    RealType sumVal = 0;

    for(int lk = -1; lk <= 2; ++lk)
    {
        for(int lj = -1; lj <= 2; ++lj)
        {
            for(int li = -1; li <= 2; ++li)
            {
                const Vec3i ind = Vec3i(i + li, j + lj, k + lk);

                if(grid.isValidIndex(ind))
                {
                    const RealType weight = MathHelpers::cubic_spline_kernel_3d(fi - (RealType)li, fj - (RealType)lj, fk - (RealType)lk);
                    sumW   += weight;
                    sumVal += weight * grid(ind);
                }
            }
        }
    }

    if(sumW > 0)
    {
        return sumVal / sumW;
    }
    else
    {
        return RealType(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
inline Vec2<RealType> interpolateGradient(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    int      i, j;
    RealType fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    RealType v00 = grid(i, j);
    RealType v01 = grid(i, j + 1);
    RealType v10 = grid(i + 1, j);
    RealType v11 = grid(i + 1, j + 1);

    RealType ddy0 = (v01 - v00);
    RealType ddy1 = (v11 - v10);

    RealType ddx0 = (v10 - v00);
    RealType ddx1 = (v11 - v01);

    return Vec2<RealType>(MathHelpers::lerp(ddx0, ddx1, fy),
                          MathHelpers::lerp(ddy0, ddy1, fx));
}

template<class RealType>
inline RealType interpolateValueGradient(Vec2<RealType>& gradient, const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    int      i, j;
    RealType fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    RealType v00 = grid(i, j);
    RealType v01 = grid(i, j + 1);
    RealType v10 = grid(i + 1, j);
    RealType v11 = grid(i + 1, j + 1);

    RealType ddy0 = (v01 - v00);
    RealType ddy1 = (v11 - v10);

    RealType ddx0 = (v10 - v00);
    RealType ddx1 = (v11 - v01);

    gradient[0] = lerp(ddx0, ddx1, fy);
    gradient[1] = lerp(ddy0, ddy1, fx);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
inline Vec3<RealType> interpolateGradient(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    int      i, j, k;
    RealType fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.sizeZ());

    RealType v000 = grid(i, j, k);
    RealType v001 = grid(i, j, k + 1);
    RealType v010 = grid(i, j + 1, k);
    RealType v011 = grid(i, j + 1, k + 1);
    RealType v100 = grid(i + 1, j, k);
    RealType v101 = grid(i + 1, j, k + 1);
    RealType v110 = grid(i + 1, j + 1, k);
    RealType v111 = grid(i + 1, j + 1, k + 1);

    RealType ddx00 = (v100 - v000);
    RealType ddx10 = (v110 - v010);
    RealType ddx01 = (v101 - v001);
    RealType ddx11 = (v111 - v011);
    RealType dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fy, fz);

    RealType ddy00 = (v010 - v000);
    RealType ddy10 = (v110 - v100);
    RealType ddy01 = (v011 - v001);
    RealType ddy11 = (v111 - v101);
    RealType dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fx, fz);

    RealType ddz00 = (v001 - v000);
    RealType ddz10 = (v101 - v100);
    RealType ddz01 = (v011 - v010);
    RealType ddz11 = (v111 - v110);
    RealType dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fx, fy);

    return Vec3<RealType>(dv_dx, dv_dy, dv_dz);
}

template<class RealType>
inline RealType interpolateValueGradient(Vec3<RealType>& gradient, const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    int      i, j, k;
    RealType fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.sizeZ());

    RealType v000 = grid(i, j, k);
    RealType v001 = grid(i, j, k + 1);
    RealType v010 = grid(i, j + 1, k);
    RealType v011 = grid(i, j + 1, k + 1);
    RealType v100 = grid(i + 1, j, k);
    RealType v101 = grid(i + 1, j, k + 1);
    RealType v110 = grid(i + 1, j + 1, k);
    RealType v111 = grid(i + 1, j + 1, k + 1);

    RealType ddx00 = (v100 - v000);
    RealType ddx10 = (v110 - v010);
    RealType ddx01 = (v101 - v001);
    RealType ddx11 = (v111 - v011);
    RealType dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fy, fz);

    RealType ddy00 = (v010 - v000);
    RealType ddy10 = (v110 - v100);
    RealType ddy01 = (v011 - v001);
    RealType ddy11 = (v111 - v101);
    RealType dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fx, fz);

    RealType ddz00 = (v001 - v000);
    RealType ddz10 = (v101 - v100);
    RealType ddz01 = (v011 - v010);
    RealType ddz11 = (v111 - v110);
    RealType dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fx, fy);

    gradient[0] = dv_dx;
    gradient[1] = dv_dy;
    gradient[2] = dv_dz;

    //return value for good measure.
    return MathHelpers::trilerp(v000, v100,
                                v010, v110,
                                v001, v101,
                                v011, v111,
                                fx, fy, fz);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline void write_matlab_array(std::ostream& output, Array2<T>& a, const char* variable_name, bool transpose = false)
{
    output << variable_name << "=[";

    for(int j = 0; j < a.sizeY(); ++j)
    {
        for(int i = 0; i < a.sizeX(); ++i)
        {
            output << a(i, j) << " ";
        }

        output << ";";
    }

    output << "]";

    if(transpose)
    {
        output << "'";
    }

    output << ";" << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ArrayHelpers
  //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana