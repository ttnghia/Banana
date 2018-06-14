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

#include <Banana/Geometry/GeometryHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::GeometryHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType point_line_distance(const VecX<N, RealType>& p, const VecX<N, RealType>& x0, const VecX<N, RealType>& x1)
{
    auto x01 = x1 - x0;
    auto x0p = p - x0;
    auto prj = glm::dot(x01, x0p) * glm::normalize(x01);
    return glm::length(x0p - prj);
}

template Real point_line_distance<2, Real>(const VecX<2, Real>& p, const VecX<2, Real>& x0, const VecX<2, Real>& x1);
template Real point_line_distance<3, Real>(const VecX<3, Real>& p, const VecX<3, Real>& x0, const VecX<3, Real>& x1);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from segment x1-x2
template<class RealType>
RealType point_segment_distance(const Vec3<RealType>& x0, const Vec3<RealType>& x1, const Vec3<RealType>& x2)
{
    Vec3<RealType> dx(x2 - x1);

    RealType m2 = glm::length2(dx);
    // find parameter value of closest point on segment
    RealType s12 = glm::dot(dx, x2 - x0) / m2;

    if(s12 < 0) {
        s12 = 0;
    } else if(s12 > 1) {
        s12 = 1;
    }

    return glm::length(x0 - s12 * x1 + (1 - s12) * x2);
}

template Real point_segment_distance<Real>(const Vec3<Real>& x0, const Vec3<Real>& x1, const Vec3<Real>& x2);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from triangle x1-x2-x3
template<class RealType>
RealType point_triangle_distance(const Vec3<RealType>& x0, const Vec3<RealType>& x1, const Vec3<RealType>& x2, const Vec3<RealType>& x3)
{
    // first find barycentric coordinates of closest point on infinite plane
    Vec3<RealType> x13(x1 - x3), x23(x2 - x3), x03(x0 - x3);
    RealType       m13 = glm::length2(x13), m23 = glm::length2(x23), d = glm::dot(x13, x23);

    RealType invdet = 1.f / fmax(m13 * m23 - d * d, 1e-30f);
    RealType a = glm::dot(x13, x03), b = glm::dot(x23, x03);

    // the barycentric coordinates themselves
    RealType w23 = invdet * (m23 * a - d * b);
    RealType w31 = invdet * (m13 * b - d * a);
    RealType w12 = 1 - w23 - w31;

    if(w23 >= 0 && w31 >= 0 && w12 >= 0) {     // if we're inside the triangle
        return glm::length(x0 - w23 * x1 + w31 * x2 + w12 * x3);
    } else {                                   // we have to clamp to one of the edges
        if(w23 > 0) {                          // this rules out edge 2-3 for us
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x1, x3));
        } else if(w31 > 0) {                   // this rules out edge 1-3
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x2, x3));
        } else {                               // w12 must be >0, ruling out edge 1-2
            return std::min(point_segment_distance(x0, x1, x3), point_segment_distance(x0, x2, x3));
        }
    }
}

template Real point_triangle_distance<Real>(const Vec3<Real>& x0, const Vec3<Real>& x1, const Vec3<Real>& x2, const Vec3<Real>& x3);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void check_neighbour(const Vector<Vec3ui>& tri, const Vec_Vec3<RealType>& x, Array<3, RealType>& phi, Array3ui& closest_tri,
                     const Vec3<RealType>& gx,
                     Int i0, Int j0, Int k0,
                     Int i1, Int j1, Int k1)
{
    if(closest_tri(i1, j1, k1) != 0xffffffff) {
        UInt p = tri[closest_tri(i1, j1, k1)][0];
        UInt q = tri[closest_tri(i1, j1, k1)][1];
        UInt r = tri[closest_tri(i1, j1, k1)][2];

        RealType d = point_triangle_distance(gx, x[p], x[q], x[r]);

        if(d < phi(i0, j0, k0)) {
            phi(i0, j0, k0)         = RealType(d);
            closest_tri(i0, j0, k0) = closest_tri(i1, j1, k1);
        }
    }
}

template void check_neighbour<Real>(const Vector<Vec3ui>& tri, const Vec_Vec3<Real>& x, Array<3, Real>& phi, Array3ui& closest_tri,
                                    const Vec3<Real>& gx,
                                    Int i0, Int j0, Int k0,
                                    Int i1, Int j1, Int k1);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<class RealType>
void sweep(const Vector<Vec3ui>& tri, const Vec_Vec3<RealType>& x,
           Array<3, RealType>& phi, Array3ui& closest_tri, const Vec3<RealType>& origin, RealType dx,
           Int di, Int dj, Int dk)
{
    Int i0, i1;
    Int j0, j1;
    Int k0, k1;

    if(di > 0) {
        i0 = 1;
        i1 = static_cast<Int>(phi.vsize()[0]);
    } else {
        i0 = static_cast<Int>(phi.vsize()[0]) - 2;
        i1 = -1;
    }

    if(dj > 0) {
        j0 = 1;
        j1 = static_cast<Int>(phi.vsize()[1]);
    } else {
        j0 = static_cast<Int>(phi.vsize()[1]) - 2;
        j1 = -1;
    }

    if(dk > 0) {
        k0 = 1;
        k1 = static_cast<Int>(phi.vsize()[2]);
    } else {
        k0 = static_cast<Int>(phi.vsize()[2]) - 2;
        k1 = -1;
    }

    //    Scheduler::parallel_for<Int>(i0, i1 + 1, j0, j1 + 1, k0, k1 + 1,
    //                                       [&](Int i, Int j, Int k)

    for(Int k = k0; k != k1; k += dk) {
        for(Int j = j0; j != j1; j += dj) {
            for(Int i = i0; i != i1; i += di) {
                Vec3<RealType> gx = Vec3<RealType>(i, j, k) * dx + origin;

                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k - dk);
            }
        }
    }
}

template void sweep<Real>(const Vector<Vec3ui>& tri,
                          const Vec_Vec3<Real>& x,
                          Array<3, Real>& phi, Array3ui& closest_tri, const Vec3<Real>& origin, Real dx,
                          Int di, Int dj, Int dk);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// calculate twice signed area of triangle (0,0)-(x1,y1)-(x2,y2)
// return an SOS-determined sign (-1, +1, or 0 only if it's a truly degenerate triangle)
template<class RealType>
Int orientation(RealType x1, RealType y1, RealType x2, RealType y2, RealType& twice_signed_area)
{
    twice_signed_area = y1 * x2 - x1 * y2;

    if(twice_signed_area > 0) {
        return 1;
    } else if(twice_signed_area < 0) {
        return -1;
    } else if(y2 > y1) {
        return 1;
    } else if(y2 < y1) {
        return -1;
    } else if(x1 > x2) {
        return 1;
    } else if(x1 < x2) {
        return -1;
    } else {
        return 0;                    // only true when x1==x2 and y1==y2
    }
}

template Int orientation<Real>(Real x1, Real y1, Real x2, Real y2, Real& twice_signed_area);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// robust test of (x0,y0) in the triangle (x1,y1)-(x2,y2)-(x3,y3)
// if true is returned, the barycentric coordinates are set in a,b,c.
template<class RealType>
bool point_in_triangle_2d(RealType x0, RealType y0,
                          RealType x1, RealType y1, RealType x2, RealType y2, RealType x3, RealType y3,
                          RealType& a, RealType& b, RealType& c)
{
    x1 -= x0;
    x2 -= x0;
    x3 -= x0;
    y1 -= y0;
    y2 -= y0;
    y3 -= y0;
    Int signa = orientation(x2, y2, x3, y3, a);

    if(signa == 0) {
        return false;
    }

    Int signb = orientation(x3, y3, x1, y1, b);

    if(signb != signa) {
        return false;
    }

    Int signc = orientation(x1, y1, x2, y2, c);

    if(signc != signa) {
        return false;
    }

    RealType sum = a + b + c;
    __BNN_REQUIRE(sum != 0);                                 // if the SOS signs match and are nonkero, there's no way all of a, b, and c are zero.
    a /= sum;
    b /= sum;
    c /= sum;
    return true;
}

template<class RealType>
bool point_in_triangle_2d(RealType x0, RealType y0, RealType x1, RealType y1, RealType x2, RealType y2, RealType x3, RealType y3)
{
    x1 -= x0;
    x2 -= x0;
    x3 -= x0;
    y1 -= y0;
    y2 -= y0;
    y3 -= y0;

    RealType a;
    Int      signa = orientation(x2, y2, x3, y3, a);
    if(signa == 0) {
        return false;
    }

    RealType b;
    Int      signb = orientation(x3, y3, x1, y1, b);
    if(signb != signa) {
        return false;
    }

    RealType c;
    Int      signc = orientation(x1, y1, x2, y2, c);
    if(signc != signa) {
        return false;
    }

    return true;
}

template bool point_in_triangle_2d<Real>(Real x0, Real y0,
                                         Real x1, Real y1, Real x2, Real y2, Real x3, Real y3,
                                         Real& a, Real& b, Real& c);

template bool point_in_triangle_2d<Real>(Real x0, Real y0, Real x1, Real y1, Real x2, Real y2, Real x3, Real y3);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::GeometryHelpers
