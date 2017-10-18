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

#include <random>
#include <unordered_map>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/** \brief This class implements a Poisson disk sampling for the surface
 * of 3D models.
 */
class PoissonDiskSampler
{
    struct CellPosHasher
    {
        std::size_t operator ()(const Vec3i& k) const
        {
            const Int p1 = 73856093 * k[0];
            const Int p2 = 19349663 * k[1];
            const Int p3 = 83492791 * k[2];
            return static_cast<size_t>(p1 + p2 + p3);
        }
    };

public:
    PoissonDiskSampler();

    /** \brief Struct to store the information of the initial points
     */
    struct InitialPointInfo
    {
        Vec3i cP;
        Vec3r pos;
        UInt  ID;
    };

    /** \brief Struct to store the hash entry (spatial hashing)
     */
    struct HashEntry
    {
        HashEntry() {};
        Vector<UInt> samples;
        UInt         startIndex;
    };

    static Int floor(const Real v)
    {
        return (Int)(v + Real(32768.0)) - Real(32768.0); // Shift to get positive values
    }

    /** Performs the poisson sampling with the
     * respective parameters. Compare
     * http://graphics.cs.umass.edu/pubs/sa_2010.pdf
     *
     * @param mesh mesh data of sampled body
     * @param vertices vertex data of sampled data
     * @param sampledVertices sampled vertices that will be returned
     * @param minRadius minimal distance of sampled vertices
     * @param numTestpointsPerFace # of generated test points per face of body
     * @param distanceNorm 0: euclidean norm, 1: approx geodesic distance
     * @param numTrials # of iterations used to find samples
     */
    void sampleMesh(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces,
                    const Real minRadius, const UInt numTrials,
                    UInt distanceNorm, Vector<Vec3r>& samples);

private:
    Real          m_r;
    UInt          m_numTrials;
    UInt          m_numTestpointsPerFace;
    UInt          m_distanceNorm;
    Vector<Vec3r> m_faceNormals;
    Vector<Real>  m_areas;
    Real          m_totalArea;

    Real  m_cellSize;
    Vec3r m_minVec;
    Vec3r m_maxVec;

    Vector<InitialPointInfo> m_initialInfoVec;
    Vector<Vector<Vec3i> >   m_phaseGroups;

    std::default_random_engine           m_generator;
    std::uniform_real_distribution<Real> m_uniform_distribution1;
    Real                                 m_maxArea;

    void computeFaceNormals(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces);
    void determineTriangleAreas(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces);
    void generateInitialPointSet(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces);
    UInt getAreaIndex(const Vector<Real>& areas, const Real totalArea);
    void parallelUniformSurfaceSampling(Vector<Vec3r>& samples);

    void quickSort(Int left, Int right);
    Int  partition(Int left, Int right);
    bool compareCellID(Vec3i& a, Vec3i& b);

    void determineMinX(const UInt numVertices, const Vec3r* vertices);

    bool nbhConflict(const std::unordered_map<Vec3i, HashEntry, CellPosHasher>& kvMap, const InitialPointInfo& iPI);
    bool checkCell(const std::unordered_map<Vec3i, HashEntry, CellPosHasher>& kvMap, const Vec3i& cell, const InitialPointInfo& iPI);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
