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

namespace SignDistanceField
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBox(const Vec3<T>& pos, const Vec3<T>& bMin, const Vec3<T>& bMax)
{
    if(pos[0] >= bMin[0] && pos[0] <= bMax[0] &&
       pos[1] >= bMin[1] && pos[1] <= bMax[1] &&
       pos[2] >= bMin[2] && pos[2] <= bMax[2])
    {
        T dists[6] = { pos[0] - bMin[0], bMax[0] - pos[0],
                       pos[1] - bMin[1], bMax[1] - pos[1],
                       pos[2] - bMin[2], bMax[2] - pos[2] };

        T minDist = dists[0];

        for(int i = 1; i < 6; ++i)
        {
            if(minDist > dists[i])
            {
                minDist = dists[i];
            }
        }

        return -minDist;
    }
    else
    {
        Vec3<T> cp = Vec3<T>(fmax(fmin(pos[0], bMax[0]), bMin[0]), fmax(fmin(pos[1], bMax[1]), bMin[1]), fmax(fmin(pos[2], bMax[2]), bMin[2]));

        return glm::length(pos - cp);
    }
}

template<class T>
void SDFBox(const Vec3<T>& bMin, const Vec3<T>& bMax, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative /*= true*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToSphere(const Vec3<T>& pos, const Vec3<T>& sphereCenter, T sphereRadius)
{
    return (glm::length(pos - sphereCenter) - sphereRadius);
}

template<class T>
void SDFSphere(const Vec3<T>& sphereCenter, T sphereRadius, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative /*= true*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToHollowSphere(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius)
{
    T d1 = (glm::length(pos - center) - outerRadius);
    T d2 = (innerRadius - glm::length(pos - center));

    return (d1 > d2 ? d1 : d2);
}

template<class T>
void SDFHollowSphere(const Vec3<T>& sphereCenter, T innerRadius, T outerRadius, const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk,
                     Array3<T>& SDF, bool bInsideNegative /*= true*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToBowl(const Vec3<T>& pos, const Vec3<T>& center, T innerRadius, T outerRadius, T thickness, T max_y)
{
    return fmax(fmax(fmax(
                         fmin(-distanceToSphere(pos, center, innerRadius), pos[1] - thickness),
                         distanceToSphere(pos, center, outerRadius)),
                     pos[1] - max_y),
                -pos[1]);
}

template<class T>
void SDFBowl(const Vec3<T>& bowlCenter, T innerRadius, T outerRadius, T thickness, T max_y,
             const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative /*= true*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
T distanceToCylinder(const Vec3<T>& pos, const Vec3<T>& cylinderBase, const Vec3<T>& cylinderDirection, T cylinderRadius, T cylinderLength)
{
    Vec3<T> u  = cylinderDirection;
    Vec3<T> c  = cylinderBase + u * lhalf;
    Vec3<T> cp = c - pos;

    T l     = cylinderLength;
    T lhalf = static_cast<T>(0.5) * l;
    T x     = glm::dot(cp, u);
    T n2    = glm::dot(cp, cp);
    T y2    = n2 - x * x;

    assert(y2 >= 0);
    T y = sqrt(y2);


    if(fabs(x) < lhalf)
    {
        if(y < cylinderRadius)    // inside cylinder, distance < 0
        {
            Vec3<T> h = c + x * u;
            return -fmin(fmin(cylinderRadius - y, glm::length(h - cylinderBase)), glm::length(h - cylinderBase - l * u));
        }
        else
        {
            return (y - cylinderRadius);
        }
    }
    else
    {
        if(y < cylinderRadius)
            return (fabs(x) - lhalf);
        else
            return sqrt((y - cylinderRadius) * (y - cylinderRadius) + (fabs(x) - lhalf) * (fabs(x) - lhalf));
    }
}

template<class T>
void SDFCylinder(const Vec3<T>& cylinderBase, const Vec3<T>& cylinderDirection, T cylinderRadius, T cylinderLength,
                 const Vec3<T>& origin, T cellSize, UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative /*= true*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sign distance field for triangle mesh
template<class T>
void SDFMesh(const std::vector<Vec3ui>& faces, const std::vector<Vec3<T> >& vertices, const Vec3<T>& origin, T cellSize,
             UInt32 ni, UInt32 nj, UInt32 nk, Array3<T>& SDF, bool bInsideNegative, UInt32 exactBand /*= 1*/)
{
    SDF.resize(ni, nj, nk);
    SDF.assign((ni + nj + nk) * cellSize); // upper bound on distance
    Array3i closest_tri(ni, nj, nk, -1);
    Array3i intersection_count(ni, nj, nk,
                               0); // intersection_count(i,j,k) is # of tri intersections in (i-1,i]x{j}x{k}
    // we begin by initializing distances near the mesh, and figuring out intersection counts
    //    Vec3 ijkmin, ijkmax;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, faces.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          // for(UInt32 t = 0; t < tri.size(); ++t)
                          for(size_t face = r.begin(); face != r.end(); ++face)
                          {
                              UInt32 p, q, r;
                              VecUtils::assign(faces[face], p, q, r);
                              // coordinates in grid to high precision
                              T fip = ((T)vertices[p][0] - origin[0]) / cellSize,
                              fjp = ((T)vertices[p][1] - origin[1]) / cellSize,
                              fkp = ((T)vertices[p][2] - origin[2]) / cellSize;

                              T fiq = ((T)vertices[q][0] - origin[0]) / cellSize,
                              fjq = ((T)vertices[q][1] - origin[1]) / cellSize,
                              fkq = ((T)vertices[q][2] - origin[2]) / cellSize;

                              T fir = ((T)vertices[r][0] - origin[0]) / cellSize,
                              fjr = ((T)vertices[r][1] - origin[1]) / cellSize,
                              fkr = ((T)vertices[r][2] - origin[2]) / cellSize;
                              // do distances nearby
                              int i0 = MathHelpers::clamp(int(MathHelpers::min(fip, fiq, fir)) - exactBand, 0, ni - 1);
                              int i1 = MathHelpers::clamp(int(MathHelpers::max(fip, fiq, fir)) + exactBand + 1, 0, ni - 1);
                              int j0 = MathHelpers::clamp(int(MathHelpers::min(fjp, fjq, fjr)) - exactBand, 0, nj - 1);
                              int j1 = MathHelpers::clamp(int(MathHelpers::max(fjp, fjq, fjr)) + exactBand + 1, 0, nj - 1);
                              int k0 = MathHelpers::clamp(int(MathHelpers::min(fkp, fkq, fkr)) - exactBand, 0, nk - 1);
                              int k1 = MathHelpers::clamp(int(MathHelpers::max(fkp, fkq, fkr)) + exactBand + 1, 0, nk - 1);

                              for(int k = k0; k <= k1; ++k)
                                  for(int j = j0; j <= j1; ++j)
                                      for(int i = i0; i <= i1; ++i)
                                      {
                                          Vec3 gx(i * cellSize + origin[0], j * cellSize + origin[1],
                                                  k * cellSize + origin[2]);
                                          T d = point_triangle_distance(gx, vertices[p], vertices[q], vertices[r]);

                                          if(d < SDF(i, j, k))
                                          {
                                              SDF(i, j, k) = d;
                                              closest_tri(i, j, k) = face;
                                          }
                                      }

                              // and do intersection counts
                              j0 = MathHelpers::clamp((int)std::ceil(MathHelpers::min(fjp, fjq, fjr)), 0, nj - 1);
                              j1 = MathHelpers::clamp((int)std::floor(MathHelpers::max(fjp, fjq, fjr)), 0, nj - 1);
                              k0 = MathHelpers::clamp((int)std::ceil(MathHelpers::min(fkp, fkq, fkr)), 0, nk - 1);
                              k1 = MathHelpers::clamp((int)std::floor(MathHelpers::max(fkp, fkq, fkr)), 0, nk - 1);

                              for(int k = k0; k <= k1; ++k)
                                  for(int j = j0; j <= j1; ++j)
                                  {
                                      T a, b, c;

                                      if(point_in_triangle_2d(j, k, fjp, fkp, fjq, fkq, fjr, fkr, a, b, c))
                                      {
                                          T fi = a * fip + b * fiq + c * fir;  // intersection i coordinate
                                          int i_interval = int(std::ceil(fi)); // intersection is in (i_interval-1,i_interval]

                                          if(i_interval < 0)
                                          {
                                              ++intersection_count(0, j,
                                                                   k); // we enlarge the first interval to include everything to the -x direction
                                          }
                                          else if(i_interval < ni)
                                          {
                                              ++intersection_count(i_interval, j, k);
                                          }

                                          // we ignore intersections that are beyond the +x side of the grid
                                      }
                                  }
                          } // end for t
                      }); // end parallel_for


    // and now we fill in the rest of the distances with fast sweeping
    for(UInt32 pass = 0; pass < 2; ++pass)
    {
#if 1
        tbb::parallel_invoke([&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, +1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, -1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, -1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, +1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, +1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, -1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, -1);
            },
                             [&]
            {
                sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, +1);
            }
                             );
#else
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, +1);
#endif
    }

    // then figure out signs (inside/outside) from intersection counts
    for(int k = 0; k < nk; ++k)
    {
        for(int j = 0; j < nj; ++j)
        {
            int total_count = 0;

            for(int i = 0; i < ni; ++i)
            {
                total_count += intersection_count(i, j, k);

                if(total_count % 2 == 1)          // if parity of intersections so far is odd,
                {
                    SDF(i, j, k) = -SDF(i, j, k); // we are inside the mesh
                }

                if(!bInsideNegative)
                {
                    SDF(i, j, k) = -SDF(i, j, k);
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Given two signed distance values (line endpoints), determine what fraction of a connecting segment is "inside"
template<class T>
T fraction_inside(T phi_left, T phi_right)
{
    if(phi_left < 0 && phi_right < 0)
    {
        return 1;
    }

    if(phi_left < 0 && phi_right >= 0)
    {
        return phi_left / (phi_left - phi_right);
    }

    if(phi_left >= 0 && phi_right < 0)
    {
        return phi_right / (phi_right - phi_left);
    }
    else
    {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Given four signed distance values (square corners), determine what fraction of the square is "inside"
template<class T>
T fraction_inside(T phi_bl, T phi_br, T phi_tl, T phi_tr)
{
    int inside_count = (phi_bl < 0 ? 1 : 0) + (phi_tl < 0 ? 1 : 0) + (phi_br < 0 ? 1 : 0) +
                       (phi_tr < 0 ? 1 : 0);
    T list[] = { phi_bl, phi_br, phi_tr, phi_tl };

    if(inside_count == 4)
    {
        return 1;
    }
    else if(inside_count == 3)
    {
        //rotate until the positive value is in the first position
        while(list[0] < 0)
        {
            cycle_array(list, 4);
        }

        //Work out the area of the exterior triangle
        T side0 = 1 - fraction_inside(list[0], list[3]);
        T side1 = 1 - fraction_inside(list[0], list[1]);
        return 1 - 0.5f * side0 * side1;
    }
    else if(inside_count == 2)
    {
        //rotate until a negative value is in the first position, and the next negative is in either slot 1 or 2.
        while(list[0] >= 0 || !(list[1] < 0 || list[2] < 0))
        {
            cycle_array(list, 4);
        }

        if(list[1] < 0)   //the matching signs are adjacent
        {
            T side_left  = fraction_inside(list[0], list[3]);
            T side_right = fraction_inside(list[1], list[2]);
            return 0.5f * (side_left + side_right);
        }
        else   //matching signs are diagonally opposite
        {
            //determine the centre point's sign to disambiguate this case
            T middle_point = 0.25f * (list[0] + list[1] + list[2] + list[3]);

            if(middle_point < 0)
            {
                T area = 0;

                //first triangle (top left)
                T side1 = 1 - fraction_inside(list[0], list[3]);
                T side3 = 1 - fraction_inside(list[2], list[3]);

                area += 0.5f * side1 * side3;

                //second triangle (top right)
                T side2 = 1 - fraction_inside(list[2], list[1]);
                T side0 = 1 - fraction_inside(list[0], list[1]);
                area += 0.5f * side0 * side2;

                return 1 - area;
            }
            else
            {
                T area = 0;

                //first triangle (bottom left)
                T side0 = fraction_inside(list[0], list[1]);
                T side1 = fraction_inside(list[0], list[3]);
                area += 0.5f * side0 * side1;

                //second triangle (top right)
                T side2 = fraction_inside(list[2], list[1]);
                T side3 = fraction_inside(list[2], list[3]);
                area += 0.5f * side2 * side3;
                return area;
            }
        }
    }
    else if(inside_count == 1)
    {
        //rotate until the negative value is in the first position
        while(list[0] >= 0)
        {
            cycle_array(list, 4);
        }

        //Work out the area of the interior triangle, and subtract from 1.
        T side0 = fraction_inside(list[0], list[3]);
        T side1 = fraction_inside(list[0], list[1]);
        return 0.5f * side0 * side1;
    }
    else
    {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from segment x1-x2
template<class T>
T point_segment_distance(const Vec3<T>& x0, const Vec3<T>& x1, const Vec3<SignDistanceField::T>& x2)
{
    Vec3<T> dx(x2 - x1);

#ifdef __Using_Eigen_Lib__
    T m2 = dx.squaredNorm();
    // find parameter value of closest point on segment
    T s12 = (T)(dx.dot(x2 - x0) / m2);
#else
    T m2 = glm::length2(dx);
    // find parameter value of closest point on segment
    T s12 = (T)(glm::dot(dx, x2 - x0) / m2);
#endif

    if(s12 < 0)
    {
        s12 = 0;
    }
    else if(s12 > 1)
    {
        s12 = 1;
    }

    // and find the distance
#ifdef __Using_Eigen_Lib__
    return (x0 - s12 * x1 + (1 - s12) * x2).norm();
#else
    return glm::length(x0 - s12 * x1 + (1 - s12) * x2);
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from triangle x1-x2-x3
template<class T>
T point_triangle_distance(const Vec3<T>& x0, const Vec3<T>& x1, const Vec3<T>& x2, const Vec3<T>& x3)
{
    // first find barycentric coordinates of closest point on infinite plane
    Vec3 x13(x1 - x3), x23(x2 - x3), x03(x0 - x3);
#ifdef __Using_Eigen_Lib__
    T m13 = x13.squaredNorm(), m23 = x23.squaredNorm(), d = x13.dot(x23);
#else
    T m13 = glm::length2(x13), m23 = glm::length2(x23), d = glm::dot(x13, x23);
#endif

    T invdet = 1.f / fmax(m13 * m23 - d * d, 1e-30f);
#ifdef __Using_Eigen_Lib__
    T a = x13.dot(x03), b = x23.dot(x03);
#else
    T a = glm::dot(x13, x03), b = glm::dot(x23, x03);
#endif
    // the barycentric coordinates themselves
    T w23 = invdet * (m23 * a - d * b);
    T w31 = invdet * (m13 * b - d * a);
    T w12 = 1 - w23 - w31;

    if(w23 >= 0 && w31 >= 0 && w12 >= 0)                      // if we're inside the triangle
    {
#ifdef __Using_Eigen_Lib__
        return (x0 - w23 * x1 + w31 * x2 + w12 * x3).norm();
#else
        return glm::length(x0 - w23 * x1 + w31 * x2 + w12 * x3);
#endif
    }
    else                        // we have to clamp to one of the edges
    {
        if(w23 > 0)             // this rules out edge 2-3 for us
        {
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x1, x3));
        }
        else if(w31 > 0)                      // this rules out edge 1-3
        {
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x2, x3));
        }
        else                      // w12 must be >0, ruling out edge 1-2
        {
            return std::min(point_segment_distance(x0, x1, x3), point_segment_distance(x0, x2, x3));
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void check_neighbour(const std::vector<Vec3ui>& tri,
                     const std::vector<Vec3<T> >& x,
                     Array3<T>& phi, Array3i& closest_tri,
                     const Vec3<T>& gx, UInt32 i0, UInt32 j0, UInt32 k0,
                     int i1, int j1, int k1)
{
    if(closest_tri(i1, j1, k1) >= 0)
    {
        UInt32 p, q, r;
        VecUtils::assign(tri[closest_tri(i1, j1, k1)], p, q, r);
        T d = point_triangle_distance(gx, x[p], x[q], x[r]);

        if(d < phi(i0, j0, k0))
        {
            phi(i0, j0, k0)         = d;
            closest_tri(i0, j0, k0) = closest_tri(i1, j1, k1);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void sweep(const std::vector<Vec3ui>& tri,
           const std::vector<Vec3<SignDistanceField::T> >& x,
           Array3<T>& phi, Array3i& closest_tri, const Vec3<T>& origin, T dx,
           int di, int dj, int dk)
{
    int i0, i1;
    int j0, j1;
    int k0, k1;

    if(di > 0)
    {
        i0 = 1;
        i1 = phi.m_SizeX;
    }
    else
    {
        i0 = phi.m_SizeX - 2;
        i1 = -1;
    }


    if(dj > 0)
    {
        j0 = 1;
        j1 = phi.m_SizeY;
    }
    else
    {
        j0 = phi.m_SizeY - 2;
        j1 = -1;
    }



    if(dk > 0)
    {
        k0 = 1;
        k1 = phi.m_SizeZ;
    }
    else
    {
        k0 = phi.m_SizeZ - 2;
        k1 = -1;
    }

    for(int k = k0; k != k1; k += dk)
    {
        for(int j = j0; j != j1; j += dj)
        {
            for(int i = i0; i != i1; i += di)
            {
                Vec3 gx(i* dx + origin[0], j* dx + origin[1], k* dx + origin[2]);

#if 1
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k - dk);
#else
                tbb::parallel_invoke(
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j, k);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i, j - dj, k);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i, j, k - dk);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j, k - dk);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i, j - dj, k - dk);
                    },
                    [&]
                    {
                        check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k - dk);
                    }
                    );
#endif
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// calculate twice signed area of triangle (0,0)-(x1,y1)-(x2,y2)
// return an SOS-determined sign (-1, +1, or 0 only if it's a truly degenerate triangle)
template<class T>
int orientation(T x1, T y1, T x2, T y2,
                T& twice_signed_area)
{
    twice_signed_area = y1 * x2 - x1 * y2;

    if(twice_signed_area > 0)
    {
        return 1;
    }
    else if(twice_signed_area < 0)
    {
        return -1;
    }
    else if(y2 > y1)
    {
        return 1;
    }
    else if(y2 < y1)
    {
        return -1;
    }
    else if(x1 > x2)
    {
        return 1;
    }
    else if(x1 < x2)
    {
        return -1;
    }
    else
    {
        return 0;    // only true when x1==x2 and y1==y2
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// robust test of (x0,y0) in the triangle (x1,y1)-(x2,y2)-(x3,y3)
// if true is returned, the barycentric coordinates are set in a,b,c.
template<class T>
bool point_in_triangle_2d(T x0, T y0,
                          T x1, T y1, T x2, T y2, T x3, T y3,
                          T& a, T& b, T& c)
{
    x1 -= x0;
    x2 -= x0;
    x3 -= x0;
    y1 -= y0;
    y2 -= y0;
    y3 -= y0;
    int signa = orientation(x2, y2, x3, y3, a);

    if(signa == 0)
    {
        return false;
    }

    int signb = orientation(x3, y3, x1, y1, b);

    if(signb != signa)
    {
        return false;
    }

    int signc = orientation(x1, y1, x2, y2, c);

    if(signc != signa)
    {
        return false;
    }

    T sum = a + b + c;
    __NOODLE_ASSERT(sum !=
                    0); // if the SOS signs match and are nonkero, there's no way all of a, b, and c are zero.
    a /= sum;
    b /= sum;
    c /= sum;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void cycle_array(T* arr, int size)
{
    T t = arr[0];

    for(int i = 0; i < size - 1; ++i)
    {
        arr[i] = arr[i + 1];
    }

    arr[size - 1] = t;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace SignDistanceField