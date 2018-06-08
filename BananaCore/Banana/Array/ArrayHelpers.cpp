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

#include <Banana/Utils/MathHelpers.h>
#include <Banana/Array/ArrayHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ArrayHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void getCoordinatesAndWeights(const Vec2<RealType>& point, const Vec2ui& size, std::array<Vec2i, 8>& indices, std::array<RealType, 8>& weights)
{
    Int      i, j;
    RealType fi, fj;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(size[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(size[1]));

    indices[0] = Vec2i(i, j);
    indices[1] = Vec2i(i + 1, j);
    indices[2] = Vec2i(i, j + 1);
    indices[3] = Vec2i(i + 1, j + 1);

    weights[0] = (RealType(1.0) - fi) * (RealType(1.0) - fj);
    weights[1] = fi * (RealType(1.0) - fj);
    weights[2] = (RealType(1.0) - fi) * fj;
    weights[3] = fi * fj;
}

template<class RealType>
void getCoordinatesAndWeights(const Vec3<RealType>& point, const Vec3ui& size, std::array<Vec3i, 8>& indices, std::array<RealType, 8>& weights)
{
    Int      i, j, k;
    RealType fi, fj, fk;

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

    weights[0] = (RealType(1.0) - fi) * (RealType(1.0) - fj) * (RealType(1.0) - fk);
    weights[1] = fi * (RealType(1.0) - fj) * (RealType(1.0) - fk);
    weights[2] = (RealType(1.0) - fi) * fj * (RealType(1.0) - fk);
    weights[3] = fi * fj * (RealType(1.0) - fk);
    weights[4] = (RealType(1.0) - fi) * (RealType(1.0) - fj) * fk;
    weights[5] = fi * (RealType(1.0) - fj) * fk;
    weights[6] = (RealType(1.0) - fi) * fj * fk;
    weights[7] = fi * fj * fk;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType interpolateValueLinear(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    return MathHelpers::bilerp(grid(i, j), grid(i + 1, j), grid(i, j + 1), grid(i + 1, j + 1), fi, fj);
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
RealType interpolateValueLinear(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    Int      i, j, k;
    RealType fi, fj, fk;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));
    return MathHelpers::trilerp(
        grid(i, j, k), grid(i + 1, j, k), grid(i, j + 1, k), grid(i + 1, j + 1, k),
        grid(i, j, k + 1), grid(i + 1, j, k + 1), grid(i, j + 1, k + 1), grid(i + 1, j + 1, k + 1),
        fi, fj, fk);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec2<RealType> grad_bilerp(RealType v00, RealType v10, RealType v01, RealType v11, RealType fi, RealType fj)
{
    return Vec2<RealType>(fj - RealType(1.0), fi - RealType(1.0)) * v00 +
           Vec2<RealType>(RealType(1.0) - fj, -fi) * v10 +
           Vec2<RealType>(-fj,                RealType(1.0) - fi) * v01 +
           Vec2<RealType>(fj,                 fi) * v11;
}

template<class RealType>
Vec2<RealType> interpolateValueAffine(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    return grad_bilerp(
        grid(i, j), grid(i + 1, j),
        grid(i, j + 1), grid(i + 1, j + 1),
        fi, fj);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType interpolateValueCubicBSpline(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    RealType sumW   = 0;
    RealType sumVal = 0;
    for(Int lj = -1; lj <= 2; ++lj) {
        for(Int li = -1; li <= 2; ++li) {
            const Vec2i ind = Vec2i(i + li, j + lj);
            if(grid.isValidIndex(ind)) {
                const RealType weight = MathHelpers::cubic_bspline_2d(fi - static_cast<RealType>(li), fj - static_cast<RealType>(lj));
                sumW   += weight;
                sumVal += weight * grid(ind);
            }
        }
    }
    if(sumW > 0) {
        return sumVal / sumW;
    } else {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
RealType interpolateValueCubicBSpline(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    Int      i, j, k;
    RealType fi, fj, fk;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

    RealType sumW   = 0;
    RealType sumVal = 0;
    for(Int lk = -1; lk <= 2; ++lk) {
        for(Int lj = -1; lj <= 2; ++lj) {
            for(Int li = -1; li <= 2; ++li) {
                const Vec3i ind = Vec3i(i + li, j + lj, k + lk);
                if(grid.isValidIndex(ind)) {
                    const RealType weight = MathHelpers::cubic_bspline_3d(fi - static_cast<RealType>(li),
                                                                          fj - static_cast<RealType>(lj),
                                                                          fk - static_cast<RealType>(lk));
                    sumW   += weight;
                    sumVal += weight * grid(ind);
                }
            }
        }
    }
    if(sumW > 0) {
        return sumVal / sumW;
    } else {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec2<RealType> interpolateGradient(const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    RealType v00 = grid(i, j);
    RealType v01 = grid(i, j + 1);
    RealType v10 = grid(i + 1, j);
    RealType v11 = grid(i + 1, j + 1);

    RealType ddy0 = (v01 - v00);
    RealType ddy1 = (v11 - v10);

    RealType ddx0 = (v10 - v00);
    RealType ddx1 = (v11 - v01);

    return Vec2<RealType>(MathHelpers::lerp(ddx0, ddx1, fj),
                          MathHelpers::lerp(ddy0, ddy1, fi));
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
Vec3<RealType> interpolateGradient(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    Int      i, j, k;
    RealType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

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
    RealType dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fj, fk);

    RealType ddy00 = (v010 - v000);
    RealType ddy10 = (v110 - v100);
    RealType ddy01 = (v011 - v001);
    RealType ddy11 = (v111 - v101);
    RealType dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fi, fk);

    RealType ddz00 = (v001 - v000);
    RealType ddz10 = (v101 - v100);
    RealType ddz01 = (v011 - v010);
    RealType ddz11 = (v111 - v110);
    RealType dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fi, fj);

    return Vec3<RealType>(dv_dx, dv_dy, dv_dz);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec2<RealType> interpolateGradientValue(const Vec2<RealType>& point, const Array2<RealType>& grid, RealType cellSize)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    RealType v00 = grid(i, j);
    RealType v01 = grid(i, j + 1);
    RealType v10 = grid(i + 1, j);
    RealType v11 = grid(i + 1, j + 1);

    Vec2<RealType> grad = v00 * Vec2<RealType>(fj - RealType(1.0), fi - RealType(1.0)) +
                          v10 * Vec2<RealType>(RealType(1.0) - fj, -fi) +
                          v01 * Vec2<RealType>(-fj, RealType(1.0) - fi) +
                          v11 * Vec2<RealType>(fj, fi);
    return grad / cellSize;
}

template<class RealType>
Vec3<RealType> interpolateGradientValue(const Vec3<RealType>& point, const Array3<RealType>& grid, RealType cellSize)
{
    Int      i, j, k;
    RealType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

    RealType v000 = grid(i, j, k);
    RealType v001 = grid(i, j, k + 1);
    RealType v010 = grid(i, j + 1, k);
    RealType v011 = grid(i, j + 1, k + 1);
    RealType v100 = grid(i + 1, j, k);
    RealType v101 = grid(i + 1, j, k + 1);
    RealType v110 = grid(i + 1, j + 1, k);
    RealType v111 = grid(i + 1, j + 1, k + 1);

    Vec3<RealType> grad = v000 * Vec3<RealType>(-(RealType(1.0) - fj) * (RealType(1.0) - fk), -(RealType(1.0) - fi) * (RealType(1.0) - fk), -(RealType(1.0) - fi) * (RealType(1.0) - fj)) +
                          v001 * Vec3<RealType>(-(RealType(1.0) - fj) * fk, -(RealType(1.0) - fi) * fk, (RealType(1.0) - fi) * (RealType(1.0) - fj)) +
                          v010 * Vec3<RealType>(-fj * (RealType(1.0) - fk), (RealType(1.0) - fi) * (RealType(1.0) - fk), -(RealType(1.0) - fi) * fj) +
                          v011 * Vec3<RealType>(-fj * fk, (RealType(1.0) - fi) * fk, (RealType(1.0) - fi) * fj) +
                          v100 * Vec3<RealType>((RealType(1.0) - fj) * (RealType(1.0) - fk), -fi * (RealType(1.0) - fk), -fi * (RealType(1.0) - fj)) +
                          v101 * Vec3<RealType>((RealType(1.0) - fj) * fk, -fi * fk, fi * (RealType(1.0) - fj)) +
                          v110 * Vec3<RealType>(fj * (RealType(1.0) - fk), fi * (RealType(1.0) - fk), -fi * fj) +
                          v111 * Vec3<RealType>(fj * fk, fi * fk, fi * fj);
    return grad / cellSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType interpolateValueAndGradient(Vec2<RealType>& gradient, const Vec2<RealType>& point, const Array2<RealType>& grid)
{
    Int      i, j;
    RealType fi, fj;
    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));

    RealType v00 = grid(i, j);
    RealType v01 = grid(i, j + 1);
    RealType v10 = grid(i + 1, j);
    RealType v11 = grid(i + 1, j + 1);

    RealType ddy0 = (v01 - v00);
    RealType ddy1 = (v11 - v10);

    RealType ddx0 = (v10 - v00);
    RealType ddx1 = (v11 - v01);

    gradient[0] = MathHelpers::lerp(ddx0, ddx1, fj);
    gradient[1] = MathHelpers::lerp(ddy0, ddy1, fi);

    //may as well return value too
    return MathHelpers::bilerp(v00, v10, v01, v11, fi, fj);
}

template<class RealType>
RealType interpolateValueAndGradient(Vec3<RealType>& gradient, const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    Int      i, j, k;
    RealType fi, fj, fk;

    MathHelpers::get_barycentric(point[0], i, fi, 0, static_cast<Int>(grid.vsize()[0]));
    MathHelpers::get_barycentric(point[1], j, fj, 0, static_cast<Int>(grid.vsize()[1]));
    MathHelpers::get_barycentric(point[2], k, fk, 0, static_cast<Int>(grid.vsize()[2]));

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
    RealType dv_dx = MathHelpers::bilerp(ddx00, ddx10, ddx01, ddx11, fj, fk);

    RealType ddy00 = (v010 - v000);
    RealType ddy10 = (v110 - v100);
    RealType ddy01 = (v011 - v001);
    RealType ddy11 = (v111 - v101);
    RealType dv_dy = MathHelpers::bilerp(ddy00, ddy10, ddy01, ddy11, fi, fk);

    RealType ddz00 = (v001 - v000);
    RealType ddz10 = (v101 - v100);
    RealType ddz01 = (v011 - v010);
    RealType ddz11 = (v111 - v110);
    RealType dv_dz = MathHelpers::bilerp(ddz00, ddz10, ddz01, ddz11, fi, fj);

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template void getCoordinatesAndWeights<Real>(const Vec2<Real>& point, const Vec2ui& size, std::array<Vec2i, 8>& indices, std::array<Real, 8>& weights);
template void getCoordinatesAndWeights<Real>(const Vec3<Real>& point, const Vec3ui& size, std::array<Vec3i, 8>& indices, std::array<Real, 8>& weights);

template Real interpolateValueLinear<Real>(const Vec2<Real>& point, const Array2<Real>& grid);
template Real interpolateValueLinear<Real>(const Vec3<Real>& point, const Array3<Real>& grid);

template Vec2<Real> grad_bilerp<Real>(Real v00, Real v10, Real v01, Real v11, Real fi, Real fj);

template Vec2<Real> interpolateValueAffine<Real>(const Vec2<Real>& point, const Array2<Real>& grid);

template Real interpolateValueCubicBSpline<Real>(const Vec2<Real>& point, const Array2<Real>& grid);
template Real interpolateValueCubicBSpline<Real>(const Vec3<Real>& point, const Array3<Real>& grid);

template Vec2<Real> interpolateGradient<Real>(const Vec2<Real>& point, const Array2<Real>& grid);
template Vec3<Real> interpolateGradient<Real>(const Vec3<Real>& point, const Array3<Real>& grid);

template Vec2<Real> interpolateGradientValue<Real>(const Vec2<Real>& point, const Array2<Real>& grid, Real cellSize);
template Vec3<Real> interpolateGradientValue<Real>(const Vec3<Real>& point, const Array3<Real>& grid, Real cellSize);

template Real interpolateValueAndGradient<Real>(Vec2<Real>& gradient, const Vec2<Real>& point, const Array2<Real>& grid);
template Real interpolateValueAndGradient<Real>(Vec3<Real>& gradient, const Vec3<Real>& point, const Array3<Real>& grid);
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ArrayHelpers
