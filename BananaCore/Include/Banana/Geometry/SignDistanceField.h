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
#include <Banana/Array/Array3.h>

namespace SignDistanceField
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBox(const Vec3<T>& pos, const Vec3<T>& bMin, const Vec3<T>& bMax);

template<class T>
void SDFBox(const Vec3<T>& bMin, const Vec3<T>& bMax, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative = true);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToSphere(const Vec3<T>& pos, const Vec3<T>& sphereCenter, T sphereRadius);

template<class T>
void SDFSphere(const Vec3<T>& sphereCenter, T sphereRadius, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative = true);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToHollowSphere(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius);

template<class T>
void SDFHollowSphere(const Vec3<T>& sphereCenter, T innerRadius, T outerRadius, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk,
                     Array3<T>& SDF, bool bInsideNegative = true);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBowl(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius, T thickness, T max_y);

template<class T>
void SDFBowl(const Vec3<T>& bowlCenter, T innerRadius, T outerRadius, T thickness, T max_y,
             const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative = true);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToCylinder(const Vec3<T>& pos, const Vec3<T>& cylinderBase, const Vec3<T>& cylinderDirection, T cylinderRadius, T cylinderLength);

template<class T>
void SDFCylinder(const Vec3<T>& cylinderBase, const Vec3<T>& cylinderDirection, T cylinderRadius, T cylinderLength,
                 const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative = true);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sign distance field for triangle mesh
template<class T>
void SDFMesh(const std::vector<Vec3ui>& faces, const std::vector<Vec3<T> >& vertices, const Vec3<T>& origin, T cellSize,
             UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative = true, UInt32 exactBand = 1);

////////////////////////////////////////////////////////////////////////////////
// Helpers for distanceFieldMesh
template<class T>
T fractionInside(T phi_left, T phi_right);

template<class T>
T fractionInside(T phi_bl, T phi_br, T phi_tl, T phi_tr);

template<class T>
T point_segment_distance(const Vec3& x0, const Vec3& x1, const Vec3& x2);

template<class T>
T point_triangle_distance(const Vec3& x0, const Vec3& x1, const Vec3& x2, const Vec3& x3);

template<class T>
void check_neighbour(const std::vector<Vec3ui>& tri, const std::vector<Vec3>& x,
                     Array3_T& phi, Array3i& closest_tri, const Vec3& gx, UInt32 i0, UInt32 j0, UInt32 k0, int i1, int j1, int k1);

template<class T>
void sweep(const std::vector<Vec3ui>& tri, const std::vector<Vec3>& x, Array3_T& phi, Array3i& closest_tri, const Vec3& origin, T dx,
           int di, int dj, int dk);

template<class T>
int orientation(T x1, T y1, T x2, T y2, T& twice_signed_area);

template<class T>
bool point_in_triangle_2d(T x0, T y0, T x1, T y1, T x2, T y2, T x3, T y3, T& a, T& b, T& c);

template<class T>
void cycle_array(T* arr, int size);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace SignDistanceField

#include <Banana/Geometry/SignDistanceField_Impl.hpp>