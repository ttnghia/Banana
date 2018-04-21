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
    Int  i, j;
    Real fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(size[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(size[1]));

    indices[0] = Vec2i(i, j);
    indices[1] = Vec2i(i + 1, j);
    indices[2] = Vec2i(i, j + 1);
    indices[3] = Vec2i(i + 1, j + 1);

    weights[0] = (1.0_f - fi) * (1.0_f - fj);
    weights[1] = fi * (1.0_f - fj);
    weights[2] = (1.0_f - fi) * fj;
    weights[3] = fi * fj;
}

inline void getCoordinatesAndWeights(const Vec3r& point, const Vec3ui& size, std::array<Vec3i, 8>& indices, std::array<Real, 8>& weights)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(size[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(size[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(size[2]));

    indices[0] = Vec3i(i, j, k);
    indices[1] = Vec3i(i + 1, j, k);
    indices[2] = Vec3i(i, j + 1, k);
    indices[3] = Vec3i(i + 1, j + 1, k);
    indices[4] = Vec3i(i, j, k + 1);
    indices[5] = Vec3i(i + 1, j, k + 1);
    indices[6] = Vec3i(i, j + 1, k + 1);
    indices[7] = Vec3i(i + 1, j + 1, k + 1);

    weights[0] = (1 - fi) * (1 - fj) * (1 - fk);
    weights[1] = fi * (1 - fj) * (1 - fk);
    weights[2] = (1 - fi) * fj * (1 - fk);
    weights[3] = fi * fj * (1 - fk);
    weights[4] = (1 - fi) * (1 - fj) * fk;
    weights[5] = fi * (1 - fj) * fk;
    weights[6] = (1 - fi) * fj * fk;
    weights[7] = fi * fj * fk;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Real interpolateValueLinear(const Vec2r& point, const Array2r& grid)
{
    Int  i, j;
    Real fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fi, fj);
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueLinear(const Vec3r& point, const Array3r& grid)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

    return MathHelpers::trilerp(
        grid(i, j, k), grid(i + 1, j, k), grid(i, j + 1, k), grid(i + 1, j + 1, k),
        grid(i, j, k + 1), grid(i + 1, j, k + 1), grid(i, j + 1, k + 1), grid(i + 1, j + 1, k + 1),
        fi, fj, fk);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec2r grad_bilerp(Real v00, Real v10, Real v01, Real v11, Real fi, Real fj)
{
    return Vec2r(fj - 1.0, fi - 1.0) * v00 +
           Vec2r(1.0 - fj, -fi) * v10 +
           Vec2r(-fj,      1.0 - fi) * v01 +
           Vec2r(fj,       fi) * v11;
}

inline Vec2r interpolateValueAffine(const Vec2r& point, const Array2r& grid)
{
    Int  i, j;
    Real fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    return grad_bilerp(
        grid(i, j), grid(i + 1, j),
        grid(i, j + 1), grid(i + 1, j + 1),
        fi, fj);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Real interpolateValueCubicBSpline(const Vec2r& point, const Array2r& grid)
{
    Int  i, j;
    Real fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    Real sumW   = 0;
    Real sumVal = 0;

    for(Int lj = -1; lj <= 2; ++lj) {
        for(Int li = -1; li <= 2; ++li) {
            const Vec2i ind = Vec2i(i + li, j + lj);

            if(grid.isValidIndex(ind)) {
                const Real weight = MathHelpers::cubic_bspline_2d(fi - (Real)li, fj - (Real)lj);
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }

    if(sumW > 1e-30_f) {
        return sumVal / sumW;
    } else {
        return 0_f;
    }
}

////////////////////////////////////////////////////////////////////////////////
inline Real interpolateValueCubicBSpline(const Vec3r& point, const Array3r& grid)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

    Real sumW   = 0;
    Real sumVal = 0;

    for(Int lk = -1; lk <= 2; ++lk) {
        for(Int lj = -1; lj <= 2; ++lj) {
            for(Int li = -1; li <= 2; ++li) {
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
        return 0_f;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec2r interpolateGradient(const Vec2r& point, const Array2r& grid)
{
    Int  i, j;
    Real fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Real ddy0 = (v01 - v00);
    Real ddy1 = (v11 - v10);

    Real ddx0 = (v10 - v00);
    Real ddx1 = (v11 - v01);

    return Vec2r(MathHelpers::lerp(ddx0, ddx1, fj),
                 MathHelpers::lerp(ddy0, ddy1, fi));
}

inline Real interpolateValueAndGradient(Vec2r& gradient, const Vec2r& point, const Array2r& grid)
{
    Int  i, j;
    Real fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Real ddy0 = (v01 - v00);
    Real ddy1 = (v11 - v10);

    Real ddx0 = (v10 - v00);
    Real ddx1 = (v11 - v01);

    gradient[0] = MathHelpers::lerp(ddx0, ddx1, fj);
    gradient[1] = MathHelpers::lerp(ddy0, ddy1, fi);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fi, fj);
}

inline Vec2r interpolateGradientValue(const Vec2r& point, const Array2r& grid, Real cellSize)
{
    Int  i, j;
    Real fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    Real v00 = grid(i, j);
    Real v01 = grid(i, j + 1);
    Real v10 = grid(i + 1, j);
    Real v11 = grid(i + 1, j + 1);

    Vec2r grad = v00 * Vec2r(fj - 1.0_f, fi - 1.0_f) +
                 v10 * Vec2r(1.0_f - fj, -fi) +
                 v01 * Vec2r(-fj,            1.0_f - fi) +
                 v11 * Vec2r(fj,             fi);
    return grad / cellSize;
}

////////////////////////////////////////////////////////////////////////////////
inline Vec3r interpolateGradient(const Vec3r& point, const Array3r& grid)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

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
    Real dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fj, fk);

    Real ddy00 = (v010 - v000);
    Real ddy10 = (v110 - v100);
    Real ddy01 = (v011 - v001);
    Real ddy11 = (v111 - v101);
    Real dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fi, fk);

    Real ddz00 = (v001 - v000);
    Real ddz10 = (v101 - v100);
    Real ddz01 = (v011 - v010);
    Real ddz11 = (v111 - v110);
    Real dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fi, fj);

    return Vec3r(dv_dx, dv_dy, dv_dz);
}

inline Real interpolateValueAndGradient(Vec3r& gradient, const Vec3r& point, const Array3r& grid)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

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
    Real dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fj, fk);

    Real ddy00 = (v010 - v000);
    Real ddy10 = (v110 - v100);
    Real ddy01 = (v011 - v001);
    Real ddy11 = (v111 - v101);
    Real dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fi, fk);

    Real ddz00 = (v001 - v000);
    Real ddz10 = (v101 - v100);
    Real ddz01 = (v011 - v010);
    Real ddz11 = (v111 - v110);
    Real dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fi, fj);

    gradient[0] = dv_dx;
    gradient[1] = dv_dy;
    gradient[2] = dv_dz;

    //return value for good measure.
    return MathHelpers::trilerp(v000, v100,
                                v010, v110,
                                v001, v101,
                                v011, v111,
                                fi, fj, fk);
}

////////////////////////////////////////////////////////////////////////////////

inline Vec3r interpolateGradientValue(const Vec3r& point, const Array3r& grid, Real cellSize)
{
    Int  i, j, k;
    Real fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

    Real v000 = grid(i, j, k);
    Real v001 = grid(i, j, k + 1);
    Real v010 = grid(i, j + 1, k);
    Real v011 = grid(i, j + 1, k + 1);
    Real v100 = grid(i + 1, j, k);
    Real v101 = grid(i + 1, j, k + 1);
    Real v110 = grid(i + 1, j + 1, k);
    Real v111 = grid(i + 1, j + 1, k + 1);

    Vec3r grad = v000 * Vec3r(-(1.0_f - fj) * (1.0_f - fk), -(1.0_f - fi) * (1.0_f - fk), -(1.0_f - fi) * (1.0_f - fj)) +
                 v001 * Vec3r(-(1.0_f - fj) * fk, -(1.0_f - fi) * fk, (1.0_f - fi) * (1.0_f - fj)) +
                 v010 * Vec3r(-fj * (1.0_f - fk), (1.0_f - fi) * (1.0_f - fk), -(1.0_f - fi) * fj) +
                 v011 * Vec3r(-fj * fk, (1.0_f - fi) * fk, (1.0_f - fi) * fj) +
                 v100 * Vec3r((1.0_f - fj) * (1.0_f - fk), -fi * (1.0_f - fk), -fi * (1.0_f - fj)) +
                 v101 * Vec3r((1.0_f - fj) * fk, -fi * fk, fi * (1.0_f - fj)) +
                 v110 * Vec3r(fj * (1.0_f - fk), fi * (1.0_f - fk), -fi * fj) +
                 v111 * Vec3r(fj * fk, fi * fk, fi * fj);
    return grad / cellSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void write_matlab_array(std::ostream& output, Array2r& a, const char* variable_name, bool transpose = false)
{
    output << variable_name << "=[";

    for(Int j = 0; j < a.vsize()[1]; ++j) {
        for(Int i = 0; i < a.vsize()[0]; ++i) {
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