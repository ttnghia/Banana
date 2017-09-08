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

#include <Banana/Setup.h>
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
inline Real interpolateValueLinear(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueLinear(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.sizeZ());

    return MathHelpers::trilerp(
        grid(i, j, k), grid(i + 1, j, k), grid(i, j + 1, k), grid(i + 1, j + 1, k),
        grid(i, j, k + 1), grid(i + 1, j, k + 1), grid(i, j + 1, k + 1), grid(i + 1, j + 1, k + 1),
        fi, fj, fk);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec2r grad_bilerp(Real v00, Real v10, Real v01, Real v11, Real fx, Real fy)
{
    return Vec2r(fy - 1.0, fx - 1.0) * v00 +
           Vec2r(1.0 - fy, -fx) * v10 +
           Vec2r(-fy,      1.0 - fx) * v01 +
           Vec2r(fy,       fx) * v11;
}

inline Vec2r interpolateValueAffine(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    return grad_bilerp(
        grid(i, j), grid(i + 1, j),
        grid(i, j + 1), grid(i + 1, j + 1),
        fx, fy);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Real interpolateValueCubicBSpline(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());

    Real sumW   = 0;
    Real sumVal = 0;

    for(int lj = -1; lj <= 2; ++lj)
    {
        for(int li = -1; li <= 2; ++li)
        {
            const Vec2i ind = Vec2i(i + li, j + lj);

            if(grid.isValidIndex(ind))
            {
                const Real weight = MathHelpers::cubic_bspline_2d(fi - (Real)li, fj - (Real)lj);
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }

    if(sumW > Real(1e-30))
        return sumVal / sumW;
    else
        return Real(0);
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueCubicBSpline(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.sizeZ());

    Real sumW   = 0;
    Real sumVal = 0;

    for(int lk = -1; lk <= 2; ++lk)
    {
        for(int lj = -1; lj <= 2; ++lj)
        {
            for(int li = -1; li <= 2; ++li)
            {
                const Vec3i ind = Vec3i(i + li, j + lj, k + lk);

                if(grid.isValidIndex(ind))
                {
                    const Real weight = MathHelpers::cubic_bspline_3d(fi - (Real)li, fj - (Real)lj, fk - (Real)lk);
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
        return Real(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec2r interpolateGradient(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Real ddy0 = (v01 - v00);
    Real ddy1 = (v11 - v10);

    Real ddx0 = (v10 - v00);
    Real ddx1 = (v11 - v01);

    return Vec2r(MathHelpers::lerp(ddx0, ddx1, fy),
                 MathHelpers::lerp(ddy0, ddy1, fx));
}

template<class Real>
inline Real interpolateValueGradient(Vec2r& gradient, const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Real ddy0 = (v01 - v00);
    Real ddy1 = (v11 - v10);

    Real ddx0 = (v10 - v00);
    Real ddx1 = (v11 - v01);

    gradient[0] = lerp(ddx0, ddx1, fy);
    gradient[1] = lerp(ddy0, ddy1, fx);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
inline Vec3r interpolateGradient(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.sizeZ());

    Real v000 = grid(i, j, k);
    Real v001 = grid(i, j, k + 1);
    Real v010 = grid(i, j + 1, k);
    Real v011 = grid(i, j + 1, k + 1);
    Real v100 = grid(i + 1, j, k);
    Real v101 = grid(i + 1, j, k + 1);
    Real v110 = grid(i + 1, j + 1, k);
    Real v111 = grid(i + 1, j + 1, k + 1);

    Real ddx00 = (v100 - v000);
    Real ddx10 = (v110 - v010);
    Real ddx01 = (v101 - v001);
    Real ddx11 = (v111 - v011);
    Real dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fy, fz);

    Real ddy00 = (v010 - v000);
    Real ddy10 = (v110 - v100);
    Real ddy01 = (v011 - v001);
    Real ddy11 = (v111 - v101);
    Real dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fx, fz);

    Real ddz00 = (v001 - v000);
    Real ddz10 = (v101 - v100);
    Real ddz01 = (v011 - v010);
    Real ddz11 = (v111 - v110);
    Real dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fx, fy);

    return Vec3r(dv_dx, dv_dy, dv_dz);
}

inline Real interpolateValueGradient(Vec3r& gradient, const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.sizeX());
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.sizeY());
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.sizeZ());

    Real v000 = grid(i, j, k);
    Real v001 = grid(i, j, k + 1);
    Real v010 = grid(i, j + 1, k);
    Real v011 = grid(i, j + 1, k + 1);
    Real v100 = grid(i + 1, j, k);
    Real v101 = grid(i + 1, j, k + 1);
    Real v110 = grid(i + 1, j + 1, k);
    Real v111 = grid(i + 1, j + 1, k + 1);

    Real ddx00 = (v100 - v000);
    Real ddx10 = (v110 - v010);
    Real ddx01 = (v101 - v001);
    Real ddx11 = (v111 - v011);
    Real dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fy, fz);

    Real ddy00 = (v010 - v000);
    Real ddy10 = (v110 - v100);
    Real ddy01 = (v011 - v001);
    Real ddy11 = (v111 - v101);
    Real dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fx, fz);

    Real ddz00 = (v001 - v000);
    Real ddz10 = (v101 - v100);
    Real ddz01 = (v011 - v010);
    Real ddz11 = (v111 - v110);
    Real dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fx, fy);

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
inline void write_matlab_array(std::ostream& output, Array2r& a, const char* variable_name, bool transpose = false)
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