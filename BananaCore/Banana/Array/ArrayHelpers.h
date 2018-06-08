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

#pragma once
#include <Banana/Array/Array.h>
#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ArrayHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType> void getCoordinatesAndWeights(const Vec2<RealType>& point, const Vec2ui& size, std::array<Vec2i, 8>& indices, std::array<RealType, 8>& weights);
template<class RealType> void getCoordinatesAndWeights(const Vec3<RealType>& point, const Vec3ui& size, std::array<Vec3i, 8>& indices, std::array<RealType, 8>& weights);

template<class RealType> RealType interpolateValueLinear(const Vec2<RealType>& point, const Array2<RealType>& grid);
template<class RealType> RealType interpolateValueLinear(const Vec3<RealType>& point, const Array3<RealType>& grid);

template<class RealType> Vec2<RealType> grad_bilerp(RealType v00, RealType v10, RealType v01, RealType v11, RealType fi, RealType fj);

template<class RealType> Vec2<RealType> interpolateValueAffine(const Vec2<RealType>& point, const Array2<RealType>& grid);

template<class RealType> RealType interpolateValueCubicBSpline(const Vec2<RealType>& point, const Array2<RealType>& grid);
template<class RealType> RealType interpolateValueCubicBSpline(const Vec3<RealType>& point, const Array3<RealType>& grid);

template<class RealType> Vec2<RealType> interpolateGradient(const Vec2<RealType>& point, const Array2<RealType>& grid);
template<class RealType> Vec3<RealType> interpolateGradient(const Vec3<RealType>& point, const Array3<RealType>& grid);

template<class RealType> Vec2<RealType> interpolateGradientValue(const Vec2<RealType>& point, const Array2<RealType>& grid, RealType cellSize);
template<class RealType> Vec3<RealType> interpolateGradientValue(const Vec3<RealType>& point, const Array3<RealType>& grid, RealType cellSize);

template<class RealType> RealType interpolateValueAndGradient(Vec2<RealType>& gradient, const Vec2<RealType>& point, const Array2<RealType>& grid);
template<class RealType> RealType interpolateValueAndGradient(Vec3<RealType>& gradient, const Vec3<RealType>& point, const Array3<RealType>& grid);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ArrayHelpers
