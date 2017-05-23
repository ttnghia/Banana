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
#include <Banana/MathHelpers.h>
#include <Banana/Array/Array2.h>
#include <Banana/Array/Array3.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
inline ScalarType interpolateLinear(const Vec2& point, const Array2<ScalarType>& grid)
{
    int        i, j;
    ScalarType fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.m_SizeY);

    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
template<class ScalarType>
inline ScalarType interpolateLinear(const Vec3& point, const Array3<ScalarType>& grid)
{
    int        i, j, k;
    ScalarType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.m_SizeY);
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.m_SizeZ);

    return MathHelpers::trilerp(
        grid(i, j, k), grid(i + 1, j, k), grid(i, j + 1, k), grid(i + 1, j + 1, k),
        grid(i, j, k + 1), grid(i + 1, j, k + 1), grid(i, j + 1, k + 1), grid(i + 1, j + 1, k + 1),
        fi, fj, fk);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
inline ScalarType interpolateCubicBSpline(const Vec2& point, const Array2<ScalarType>& grid)
{
    int        i, j;
    ScalarType fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.m_SizeY);

    ScalarType sumW   = 0;
    ScalarType sumVal = 0;

    for(int lj = -1; lj <= 2; ++lj)
    {
        for(int li = -1; li <= 2; ++li)
        {
            const Vec2i ind = Vec2i(i + li, j + lj);

            if(grid.isValidIndex(ind))
            {
                const ScalarType weight = MathHelpers::cubic_spline_kernel_2d(fi - (ScalarType)li, fj - (ScalarType)lj);
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }

    if(sumW > ScalarType(1e-30))
        return sumVal / sumW;
    else
        return ScalarType(0);
}

////////////////////////////////////////////////////////////////////////////////
template<class ScalarType>
inline ScalarType interpolateCubicBSpline(const Vec3& point, const Array3<ScalarType>& grid)
{
    int        i, j, k;
    ScalarType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.m_SizeY);
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.m_SizeZ);

    ScalarType sumW   = 0;
    ScalarType sumVal = 0;

    for(int lk = -1; lk <= 2; ++lk)
    {
        for(int lj = -1; lj <= 2; ++lj)
        {
            for(int li = -1; li <= 2; ++li)
            {
                const Vec3i ind = Vec3i(i + li, j + lj, k + lk);

                if(grid.isValidIndex(ind))
                {
                    const ScalarType weight = MathHelpers::cubic_spline_kernel_3d(fi - (ScalarType)li, fj - (ScalarType)lj, fk - (ScalarType)lk);
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
        return ScalarType(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
inline float interpolate_gradient(Vec2& gradient, const Vec2& point, const Array2<ScalarType>& grid)
{
    int        i, j;
    ScalarType fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.m_SizeY);

    ScalarType v00 = grid(i, j);
    ScalarType v01 = grid(i, j + 1);
    ScalarType v10 = grid(i + 1, j);
    ScalarType v11 = grid(i + 1, j + 1);

    ScalarType ddy0 = (v01 - v00);
    ScalarType ddy1 = (v11 - v10);

    ScalarType ddx0 = (v10 - v00);
    ScalarType ddx1 = (v11 - v01);

    gradient[0] = lerp(ddx0, ddx1, fy);
    gradient[1] = lerp(ddy0, ddy1, fx);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
template<class ScalarType>
inline ScalarType interpolate_gradient(Vec3& gradient, const Vec3& point, const Array3<ScalarType>& grid)
{
    int        i, j, k;
    ScalarType fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.m_SizeX);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.m_SizeY);
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.m_SizeZ);

    ScalarType v000 = grid(i, j, k);
    ScalarType v001 = grid(i, j, k + 1);
    ScalarType v010 = grid(i, j + 1, k);
    ScalarType v011 = grid(i, j + 1, k + 1);
    ScalarType v100 = grid(i + 1, j, k);
    ScalarType v101 = grid(i + 1, j, k + 1);
    ScalarType v110 = grid(i + 1, j + 1, k);
    ScalarType v111 = grid(i + 1, j + 1, k + 1);

    ScalarType ddx00 = (v100 - v000);
    ScalarType ddx10 = (v110 - v010);
    ScalarType ddx01 = (v101 - v001);
    ScalarType ddx11 = (v111 - v011);
    ScalarType dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fy, fz);

    ScalarType ddy00 = (v010 - v000);
    ScalarType ddy10 = (v110 - v100);
    ScalarType ddy01 = (v011 - v001);
    ScalarType ddy11 = (v111 - v101);
    ScalarType dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fx, fz);

    ScalarType ddz00 = (v001 - v000);
    ScalarType ddz10 = (v101 - v100);
    ScalarType ddz01 = (v011 - v010);
    ScalarType ddz11 = (v111 - v110);
    ScalarType dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fx, fy);

    gradient[0] = dv_dx;
    gradient[1] = dv_dy;
    gradient[2] = dv_dz;

    //return value for good measure.
    return MathHelpers::trilerp(
        v000, v100,
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

    for(int j = 0; j < a.m_SizeY; ++j)
    {
        for(int i = 0; i < a.m_SizeX; ++i)
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
