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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sign distance field for triangle mesh
template<class RealType>
void computeSDFMesh(const std::vector<Vec3ui>& faces, const std::vector<Vec3<RealType> >& vertices, const Vec3<RealType>& origin, RealType cellSize,
                    UInt32 ni, UInt32 nj, UInt32 nk, Array3<RealType>& SDF, int exactBand = 1)
{
    __BNN_ASSERT(ni > 0 && nj > 0 && nk > 0);

    SDF.resize(ni, nj, nk);
    SDF.assign(RealType(ni + nj + nk) * cellSize);       // upper bound on distance
    Array3ui closest_tri(ni, nj, nk, 0xffffffff);

    // intersection_count(i,j,k) is # of tri intersections in (i-1,i]x{j}x{k}
    // we begin by initializing distances near the mesh, and figuring out intersection counts
    Array3ui intersectionCount(ni, nj, nk, 0u);

    for(UInt32 face = 0, faceEnd = static_cast<UInt32>(faces.size()); face < faceEnd; ++face)
    {
        UInt32 p = faces[face][0];
        UInt32 q = faces[face][1];
        UInt32 r = faces[face][2];

        // coordinates in grid to high precision
        Vec3<RealType> fp = (vertices[p] - origin) / cellSize;
        Vec3<RealType> fq = (vertices[q] - origin) / cellSize;
        Vec3<RealType> fr = (vertices[r] - origin) / cellSize;

        // do distances nearby
        Int32 i0 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::min(fp[0], fq[0], fr[0])) - exactBand, 0, static_cast<Int32>(ni - 1));
        Int32 i1 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::max(fp[0], fq[0], fr[0])) + exactBand + 1, 0, static_cast<Int32>(ni - 1));
        Int32 j0 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::min(fp[1], fq[1], fr[1])) - exactBand, 0, static_cast<Int32>(nj - 1));
        Int32 j1 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::max(fp[1], fq[1], fr[1])) + exactBand + 1, 0, static_cast<Int32>(nj - 1));
        Int32 k0 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::min(fp[2], fq[2], fr[2])) - exactBand, 0, static_cast<Int32>(nk - 1));
        Int32 k1 = MathHelpers::clamp(static_cast<Int32>(MathHelpers::max(fp[2], fq[2], fr[2])) + exactBand + 1, 0, static_cast<Int32>(nk - 1));

        ParallelFuncs::parallel_for<Int32>(i0, i1 + 1, j0, j1 + 1, k0, k1 + 1,
                                           [&](Int32 i, Int32 j, Int32 k)
                                           {
                                               Vec3<RealType> gx = Vec3<RealType>(i, j, k) * cellSize + origin;
                                               RealType d = point_triangle_distance(gx, vertices[p], vertices[q], vertices[r]);

                                               if(d < SDF(i, j, k))
                                               {
                                                   SDF(i, j, k) = d;
                                                   closest_tri(i, j, k) = face;
                                               }
                                           });

        // and do intersection counts
        j0 = MathHelpers::clamp(static_cast<Int32>(std::ceil(MathHelpers::min(fp[1], fq[1], fr[1]))) - 10, 0, static_cast<Int32>(nj - 1));
        j1 = MathHelpers::clamp(static_cast<Int32>(std::floor(MathHelpers::max(fp[1], fq[1], fr[1]))) + 10, 0, static_cast<Int32>(nj - 1));
        k0 = MathHelpers::clamp(static_cast<Int32>(std::ceil(MathHelpers::min(fp[2], fq[2], fr[2]))) - 10, 0, static_cast<Int32>(nk - 1));
        k1 = MathHelpers::clamp(static_cast<Int32>(std::floor(MathHelpers::max(fp[2], fq[2], fr[2]))) + 10, 0, static_cast<Int32>(nk - 1));

        for(Int32 k = k0; k <= k1; ++k)
        {
            for(Int32 j = j0; j <= j1; ++j)
            {
                RealType a, b, c;

                if(point_in_triangle_2d(static_cast<RealType>(j), static_cast<RealType>(k), fp[1], fp[2], fq[1], fq[2], fr[1], fr[2], a, b, c))
                {
                    // intersection i coordinate
                    RealType fi = a * fp[0] + b * fq[0] + c * fr[0];

                    // intersection is in (i_interval-1,i_interval]
                    Int32 i_interval = MathHelpers::max(static_cast<Int32>(std::ceil(fi)), 0);

                    // we enlarge the first interval to include everything to the -x direction
                    // we ignore intersections that are beyond the +x side of the grid
                    if(i_interval < static_cast<Int32>(ni))
                        ++intersectionCount(i_interval, j, k);
                }
            }
        }
    } // end loop face


#if 1
    // and now we fill in the rest of the distances with fast sweeping
    for(UInt32 pass = 0; pass < 2; ++pass)
    {
#if 0
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
#endif
    // then figure out signs (inside/outside) from intersection counts
    ParallelFuncs::parallel_for<UInt32>(0, nk,
                                        [&](UInt32 k)
                                        {
                                            for(UInt32 j = 0; j < nj; ++j)
                                            {
                                                UInt32 total_count = 0;

                                                for(UInt32 i = 0; i < ni; ++i)
                                                {
                                                    total_count += intersectionCount(i, j, k);

                                                    if(total_count & 1)                // if parity of intersections so far is odd,
                                                        SDF(i, j, k) = -SDF(i, j, k);  // we are inside the mesh
                                                }
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from segment x1-x2
template<class RealType>
RealType point_segment_distance(const Vec3<RealType>& x0, const Vec3<RealType>& x1, const Vec3<RealType>& x2)
{
    Vec3<RealType> dx(x2 - x1);

    RealType m2 = glm::length2(dx);
    // find parameter value of closest point on segment
    RealType s12 = (glm::dot(dx, x2 - x0) / m2);

    if(s12 < 0)
    {
        s12 = 0;
    }
    else if(s12 > 1)
    {
        s12 = 1;
    }

    return glm::length(x0 - s12 * x1 + (1 - s12) * x2);
}

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

    if(w23 >= 0 && w31 >= 0 && w12 >= 0)                      // if we're inside the triangle
    {
        return glm::length(x0 - w23 * x1 + w31 * x2 + w12 * x3);
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
template<class RealType>
void check_neighbour(const std::vector<Vec3ui>& tri,
                     const std::vector<Vec3<RealType> >& x,
                     Array3<RealType>& phi, Array3ui& closest_tri,
                     const Vec3<RealType>& gx,
                     Int32 i0, Int32 j0, Int32 k0,
                     Int32 i1, Int32 j1, Int32 k1)
{
    if(closest_tri(i1, j1, k1) != 0xffffffff)
    {
        UInt32 p = tri[closest_tri(i1, j1, k1)][0];
        UInt32 q = tri[closest_tri(i1, j1, k1)][1];
        UInt32 r = tri[closest_tri(i1, j1, k1)][2];

        RealType d = point_triangle_distance(gx, x[p], x[q], x[r]);

        if(d < phi(i0, j0, k0))
        {
            phi(i0, j0, k0)         = d;
            closest_tri(i0, j0, k0) = closest_tri(i1, j1, k1);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void sweep(const std::vector<Vec3ui>& tri,
           const std::vector<Vec3<RealType> >& x,
           Array3<RealType>& phi, Array3ui& closest_tri, const Vec3<RealType>& origin, RealType dx,
           int di, int dj, int dk)
{
    Int32 i0, i1;
    Int32 j0, j1;
    Int32 k0, k1;

    if(di > 0)
    {
        i0 = 1;
        i1 = static_cast<Int32>(phi.sizeX());
    }
    else
    {
        i0 = static_cast<Int32>(phi.sizeX()) - 2;
        i1 = -1;
    }


    if(dj > 0)
    {
        j0 = 1;
        j1 = static_cast<Int32>(phi.sizeY());
    }
    else
    {
        j0 = static_cast<Int32>(phi.sizeY()) - 2;
        j1 = -1;
    }

    if(dk > 0)
    {
        k0 = 1;
        k1 = static_cast<Int32>(phi.sizeZ());
    }
    else
    {
        k0 = static_cast<Int32>(phi.sizeZ()) - 2;
        k1 = -1;
    }

//    ParallelFuncs::parallel_for<Int32>(i0, i1 + 1, j0, j1 + 1, k0, k1 + 1,
//                                       [&](Int32 i, Int32 j, Int32 k)

    for(Int32 k = k0; k != k1; k += dk)
    {
        for(Int32 j = j0; j != j1; j += dj)
        {
            for(Int32 i = i0; i != i1; i += di)
            {
                Vec3<RealType> gx = Vec3<RealType>(i, j, k) * dx + origin;

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
template<class RealType>
int orientation(RealType x1, RealType y1, RealType x2, RealType y2,
                RealType& twice_signed_area)
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

    RealType sum = a + b + c;
    __BNN_ASSERT(sum != 0);                     // if the SOS signs match and are nonkero, there's no way all of a, b, and c are zero.
    a /= sum;
    b /= sum;
    c /= sum;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void cycle_array(RealType* arr, int size)
{
    RealType t = arr[0];

    for(int i = 0; i < size - 1; ++i)
    {
        arr[i] = arr[i + 1];
    }

    arr[size - 1] = t;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::GeometryObject3D::TriMeshObject<RealType>::makeSDF()
{
    ////////////////////////////////////////////////////////////////////////////////
    // Load mesh
    MeshLoader<RealType> meshLoader;
    __BNN_ASSERT(meshLoader.loadMesh(m_TriMeshFile));

    Vec3<RealType> bbmin = meshLoader.getAABBMin();
    Vec3<RealType> bbmax = meshLoader.getAABBMax();

    Vec3<RealType> diff    = bbmax - bbmin;
    RealType       maxSize = fmaxf(fmaxf(fabs(diff[0]), fabs(diff[1])), fabs(diff[2]));
    RealType       scale   = RealType(1.0) / maxSize;

    // multiply all vertices by scale to make the mesh having max(w, h, d) = 1
    bbmin *= scale;
    bbmax *= scale;

    // expand the bounding box
    Vec3<RealType> meshCenter = (bbmax + bbmin) * RealType(0.5);
    auto           cmin       = bbmin - meshCenter;
    auto           cmax       = bbmax - meshCenter;

    bbmin = meshCenter + glm::normalize(cmin) * glm::length(cmin) * RealType(1.0 + m_Expanding);
    bbmax = meshCenter + glm::normalize(cmax) * glm::length(cmax) * RealType(1.0 + m_Expanding);

    // to move the mesh center to origin
    bbmin -= meshCenter;
    bbmax -= meshCenter;

    std::vector<Vec3<RealType> > vertexList(meshLoader.getFaceVertices().size() / 3);
    std::vector<Vec3ui>          faceList(meshLoader.getFaces().size() / 3);

    std::memcpy(vertexList.data(), meshLoader.getFaceVertices().data(), meshLoader.getFaceVertices().size() * sizeof(RealType));
    std::memcpy(faceList.data(),   meshLoader.getFaces().data(),        meshLoader.getFaces().size() * sizeof(UInt32));

    for(auto& vertex : vertexList)
    {
        vertex *= scale;
        vertex -= meshCenter;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Init grid and array of data
    m_Grid3D.setGrid(bbmin, bbmax, m_Step);

    ////////////////////////////////////////////////////////////////////////////////
    // Compute SDF data
    computeSDFMesh(faceList, vertexList, bbmin, m_Step, m_Grid3D.getNumCellX(), m_Grid3D.getNumCellY(), m_Grid3D.getNumCellZ(), m_SDFData);
    m_bSDFReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::GeometryObject3D::TriMeshObject<RealType>::signedDistance(const Vec3<RealType>& ppos)
{
    if(!m_bSDFReady)
        makeSDF();

    if(!m_Grid3D.isInsideGrid(ppos))
        return MAX_ABS_SIGNED_DISTANCE;

    auto cellIdx = m_Grid3D.getCellIdx<UInt32>(ppos);
    return ArrayHelpers::interpolateValueLinear((ppos - m_Grid3D.getBMin()) / m_Step, m_SDFData);
}