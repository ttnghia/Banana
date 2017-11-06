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
#include <Banana/Array/Array.h>
#include <Banana/Utils/MathHelpers.h>

#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ArrayHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void getCoordinatesAndWeights(const Vec2r& point, const Vec2ui& size, std::array<Vec2i, 8>& indices, std::array<Real, 8>& weights)
{
    int  i, j;
    Real fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)size[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)size[1]);

    indices[0] = Vec2i(i, j);
    indices[1] = Vec2i(i + 1, j);
    indices[2] = Vec2i(i, j + 1);
    indices[3] = Vec2i(i + 1, j + 1);

    weights[0] = (Real(1.0) - fx) * (Real(1.0) - fy);
    weights[1] = fx * (Real(1.0) - fy);
    weights[2] = (Real(1.0) - fx) * fy;
    weights[3] = fx * fy;
}

inline void getCoordinatesAndWeights(const Vec3r& point, const Vec3ui& size, std::array<Vec3i, 8>& indices, std::array<Real, 8>& weights)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)size[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)size[1]);
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)size[2]);

    indices[0] = Vec3i(i, j, k);
    indices[1] = Vec3i(i + 1, j, k);
    indices[2] = Vec3i(i, j + 1, k);
    indices[3] = Vec3i(i + 1, j + 1, k);
    indices[4] = Vec3i(i, j, k + 1);
    indices[5] = Vec3i(i + 1, j, k + 1);
    indices[6] = Vec3i(i, j + 1, k + 1);
    indices[7] = Vec3i(i + 1, j + 1, k + 1);

    weights[0] = (1 - fx) * (1 - fy) * (1 - fz);
    weights[1] = fx * (1 - fy) * (1 - fz);
    weights[2] = (1 - fx) * fy * (1 - fz);
    weights[3] = fx * fy * (1 - fz);
    weights[4] = (1 - fx) * (1 - fy) * fz;
    weights[5] = fx * (1 - fy) * fz;
    weights[6] = (1 - fx) * fy * fz;
    weights[7] = fx * fy * fz;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Real interpolateValueLinear(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);

    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fx, fy);
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueLinear(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.size()[1]);
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.size()[2]);

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

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);

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

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.size()[1]);

    Real sumW   = 0;
    Real sumVal = 0;

    for(int lj = -1; lj <= 2; ++lj) {
        for(int li = -1; li <= 2; ++li) {
            const Vec2i ind = Vec2i(i + li, j + lj);

            if(grid.isValidIndex(ind)) {
                const Real weight = MathHelpers::cubic_bspline_2d(fi - (Real)li, fj - (Real)lj);
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }

    if(sumW > Real(1e-30)) {
        return sumVal / sumW;
    } else {
        return Real(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueCubicBSpline(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fj, 0, (int)grid.size()[1]);
    MathHelpers::get_barycentric(point[2], k, fk, 0, (int)grid.size()[2]);

    Real sumW   = 0;
    Real sumVal = 0;

    for(int lk = -1; lk <= 2; ++lk) {
        for(int lj = -1; lj <= 2; ++lj) {
            for(int li = -1; li <= 2; ++li) {
                const Vec3i ind = Vec3i(i + li, j + lj, k + lk);

                if(grid.isValidIndex(ind)) {
                    const Real weight = MathHelpers::cubic_bspline_3d(fi - (Real)li, fj - (Real)lj, fk - (Real)lk);
                    sumW   += weight;
                    sumVal += weight * grid(ind);
                }
            }
        }
    }

    if(sumW > 0) {
        return sumVal / sumW;
    } else {
        return Real(0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec2r interpolateGradient(const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);

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

inline Real interpolateValueAndGradient(Vec2r& gradient, const Vec2r& point, const Array2r& grid)
{
    int  i, j;
    Real fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Real ddy0 = (v01 - v00);
    Real ddy1 = (v11 - v10);

    Real ddx0 = (v10 - v00);
    Real ddx1 = (v11 - v01);

    gradient[0] = MathHelpers::lerp(ddx0, ddx1, fy);
    gradient[1] = MathHelpers::lerp(ddy0, ddy1, fx);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fx, fy);
}

inline Vec2r interpolateGradientValue(const Vec2r& point, const Array2r& grid, Real cellSize)
{
    int  i, j;
    Real fx, fy;
    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Vec2r grad = v00 * Vec2r(fy - Real(1.0), fx - Real(1.0)) +
                 v10 * Vec2r(Real(1.0) - fy, -fx) +
                 v01 * Vec2r(-fy,            Real(1.0) - fx) +
                 v11 * Vec2r(fy,             fx);
    return grad / cellSize;
}

////////////////////////////////////////////////////////////////////////////////
inline Vec3r interpolateGradient(const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.size()[2]);

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

inline Real interpolateValueAndGradient(Vec3r& gradient, const Vec3r& point, const Array3r& grid)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.size()[2]);

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

////////////////////////////////////////////////////////////////////////////////

inline Vec3r interpolateGradientValue(const Vec3r& point, const Array3r& grid, Real cellSize)
{
    int  i, j, k;
    Real fx, fy, fz;

    MathHelpers::get_barycentric(point[0], i, fx, 0, (int)grid.size()[0]);
    MathHelpers::get_barycentric(point[1], j, fy, 0, (int)grid.size()[1]);
    MathHelpers::get_barycentric(point[2], k, fz, 0, (int)grid.size()[2]);

    Real v000 = grid(i, j, k);
    Real v001 = grid(i, j, k + 1);
    Real v010 = grid(i, j + 1, k);
    Real v011 = grid(i, j + 1, k + 1);
    Real v100 = grid(i + 1, j, k);
    Real v101 = grid(i + 1, j, k + 1);
    Real v110 = grid(i + 1, j + 1, k);
    Real v111 = grid(i + 1, j + 1, k + 1);

    Vec3r grad = v000 * Vec3r(-(Real(1.0) - fy) * (Real(1.0) - fz), -(Real(1.0) - fx) * (Real(1.0) - fz), -(Real(1.0) - fx) * (Real(1.0) - fy)) +
                 v001 * Vec3r(-(Real(1.0) - fy) * fz, -(Real(1.0) - fx) * fz, (Real(1.0) - fx) * (Real(1.0) - fy)) +
                 v010 * Vec3r(-fy * (Real(1.0) - fz), (Real(1.0) - fx) * (Real(1.0) - fz), -(Real(1.0) - fx) * fy) +
                 v011 * Vec3r(-fy * fz, (Real(1.0) - fx) * fz, (Real(1.0) - fx) * fy) +
                 v100 * Vec3r((Real(1.0) - fy) * (Real(1.0) - fz), -fx * (Real(1.0) - fz), -fx * (Real(1.0) - fy)) +
                 v101 * Vec3r((Real(1.0) - fy) * fz, -fx * fz, fx * (Real(1.0) - fy)) +
                 v110 * Vec3r(fy * (Real(1.0) - fz), fx * (Real(1.0) - fz), -fx * fy) +
                 v111 * Vec3r(fy * fz, fx * fz, fx * fy);
    return grad / cellSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void write_matlab_array(std::ostream& output, Array2r& a, const char* variable_name, bool transpose = false)
{
    output << variable_name << "=[";

    for(int j = 0; j < a.size()[1]; ++j) {
        for(int i = 0; i < a.size()[0]; ++i) {
            output << a(i, j) << " ";
        }

        output << ";";
    }

    output << "]";

    if(transpose) {
        output << "'";
    }

    output << ";" << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace ArrayHelpers
  //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana