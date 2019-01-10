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

#include <Banana/Utils/MathHelpers.h>
#include <ParticleTools/PoissonDiskSampler.h>

#include <algorithm>
#include <limits>

#include <iostream>
#include <fstream>
#include <string>

#define _USE_MATH_DEFINES
#include "math.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PoissonDiskSampler::PoissonDiskSampler() :
    m_uniform_distribution1(0.0, 1.0)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::sampleMesh(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces,
                                    const Real minRadius, const UInt numTrials,
                                    UInt distanceNorm, Vector<Vec3r>& samples)
{
    m_r            = minRadius;
    m_numTrials    = numTrials;
    m_distanceNorm = distanceNorm;

    m_cellSize = m_r / std::sqrt(3.0);

    // Init sampling
    m_maxArea = Tiny<Real>();
    determineMinX(numVertices, vertices);

    determineTriangleAreas(numVertices, vertices, numFaces, faces);

    const Real circleArea = M_PI * minRadius * minRadius;
    m_numTestpointsPerFace = (UInt)(40.0 * (m_maxArea / circleArea));

    const UInt numInitialPoints = (numFaces * m_numTestpointsPerFace);
    //cout << "# Initial points: " << numInitialPoints << endl;

    m_initialInfoVec.resize(numInitialPoints);
    m_phaseGroups.resize(27);

    computeFaceNormals(numVertices, vertices, numFaces, faces);

    // Generate initial set of candidate points
    generateInitialPointSet(numVertices, vertices, numFaces, faces);

    // Find minimal coordinates of object

    // Calculate CellIndices
    const Real factor = 1.0 / m_cellSize;

        #pragma omp parallel for schedule(static)
    for(Int i = 0; i < (Int)m_initialInfoVec.size(); i++) {
        const Vec3r& v        = m_initialInfoVec[i].pos;
        const Int    cellPos1 = PoissonDiskSampler::floor((v[0] - m_minVec[0]) * factor) + 1;
        const Int    cellPos2 = PoissonDiskSampler::floor((v[1] - m_minVec[1]) * factor) + 1;
        const Int    cellPos3 = PoissonDiskSampler::floor((v[2] - m_minVec[2]) * factor) + 1;
        m_initialInfoVec[i].cP = Vec3i(cellPos1, cellPos2, cellPos3);
    }

    // Sort Initial points for CellID
    quickSort(0, (Int)m_initialInfoVec.size() - 1);

    // PoissonSampling
    parallelUniformSurfaceSampling(samples);

    // release data
    m_initialInfoVec.clear();
    for(Int i = 0; i < m_phaseGroups.size(); i++) {
        m_phaseGroups[i].clear();
    }
    m_phaseGroups.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::determineTriangleAreas(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces)
{
    m_areas.resize(numFaces);
    Real totalArea  = 0.0;
    Real tmpMaxArea = Tiny<Real>();

        #pragma omp parallel default(shared)
    {
        // Compute area of each triangle
                #pragma omp for reduction(+:totalArea) schedule(static)
        for(Int i = 0; i < (Int)numFaces; i++) {
            const Vec3r& a = vertices[faces[3 * i]];
            const Vec3r& b = vertices[faces[3 * i + 1]];
            const Vec3r& c = vertices[faces[3 * i + 2]];

            const Vec3r d1 = b - a;
            const Vec3r d2 = c - a;

            const Real area = (glm::length(glm::cross(d1, d2))) / 2.0;
            m_areas[i] = area;
            totalArea += area;
            tmpMaxArea = MathHelpers::max(area, tmpMaxArea);
        }
    }
    m_maxArea   = MathHelpers::max(tmpMaxArea, m_maxArea);
    m_totalArea = totalArea;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::generateInitialPointSet(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces)
{
    m_totalArea = 0.0;

        #pragma omp parallel default(shared)
    {
        // Generating the surface points
                #pragma omp for schedule(static)
        for(Int i = 0; i < (Int)m_initialInfoVec.size(); i++) {
            // Drawing random barycentric coordinates
            Real rn1 = std::sqrt(m_uniform_distribution1(m_generator));
            Real bc1 = 1.0 - rn1;
            Real bc2 = m_uniform_distribution1(m_generator) * rn1;
            Real bc3 = 1.0 - bc1 - bc2;

            // Triangle selection with probability proportional to area
            const UInt randIndex = getAreaIndex(m_areas, m_totalArea);

            // Calculating point coordinates
            const Vec3r& v1 = vertices[faces[3 * randIndex]];
            const Vec3r& v2 = vertices[faces[3 * randIndex + 1]];
            const Vec3r& v3 = vertices[faces[3 * randIndex + 2]];

            m_initialInfoVec[i].pos = bc1 * v1 + bc2 * v2 + bc3 * v3;
            m_initialInfoVec[i].ID  = randIndex;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt PoissonDiskSampler::getAreaIndex(const Vector<Real>& areas, const Real totalArea)
{
    // see https://en.wikipedia.org/wiki/Fitness_proportionate_selection
    //// Linear Version O(n)
    //Real rn = m_uniform_distribution1(m_generator)*totalArea;

    //for (UInt i = 0; i<areas.size(); i++)
    //{
    //	rn -= areas[i];
    //	if (rn <= 0)
    //		return i;
    //}
    //return (Int)areas.size() - 1;

    // Stochastic acceptance version O(1)
    bool notaccepted = true;
    UInt index;
    while(notaccepted) {
        index = (Int)((Real)areas.size() * m_uniform_distribution1(m_generator));
        if(m_uniform_distribution1(m_generator) < areas[index] / m_maxArea) {
            notaccepted = false;
        }
    }
    return index;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::parallelUniformSurfaceSampling(Vector<Vec3r>& samples)
{
    // Sort initial points into HashMap storing only the index of the first point of cell
    // and build phase groups
    std::unordered_map<Vec3i, HashEntry, CellPosHasher> hMap(2 * m_initialInfoVec.size());
    samples.clear();
    samples.reserve(m_initialInfoVec.size());

    // Already insert first Initial point as start of first cell in hashmap
    {
        const Vec3i& cell  = m_initialInfoVec[0].cP;
        HashEntry&   entry = hMap[cell];
        entry.startIndex = 0;
        entry.samples.reserve(5);
        Int index = cell[0] % 3 + 3 * (cell[1] % 3) + 9 * (cell[2] % 3);
        m_phaseGroups[index].push_back(cell);
    }

    for(Int i = 1; i < (Int)m_initialInfoVec.size(); i++) {
        const Vec3i& cell = m_initialInfoVec[i].cP;
        if(cell != m_initialInfoVec[i - 1].cP) {
            HashEntry& entry = hMap[cell];
            entry.startIndex = i;
            entry.samples.reserve(5);
            Int index = cell[0] % 3 + 3 * (cell[1] % 3) + 9 * (cell[2] % 3);
            m_phaseGroups[index].push_back(cell);
        }
    }
    // Loop over number of tries to find a sample in a cell
    for(Int k = 0; k < (Int)m_numTrials; k++) {
        // Loop over the 27 cell groups
        for(Int pg = 0; pg < m_phaseGroups.size(); pg++) {
            const Vector<Vec3i>& cells = m_phaseGroups[pg];
            // Loop over the cells in each cell group
                        #pragma omp parallel for schedule(static)
            for(Int i = 0; i < (Int)cells.size(); i++) {
                const auto entryIt = hMap.find(cells[i]);
                // Check if cell exists
                if(entryIt != hMap.end()) {
                    // Check if max Index is not exceeded
                    HashEntry& entry = entryIt->second;
                    if(entry.startIndex + k < m_initialInfoVec.size()) {
                        if(m_initialInfoVec[entry.startIndex].cP == m_initialInfoVec[entry.startIndex + k].cP) {
                            // choose kth point from cell
                            const InitialPointInfo& test = m_initialInfoVec[entry.startIndex + k];
                            // Assign sample
                            if(!nbhConflict(hMap, test)) {
                                const Int index = entry.startIndex + k;
                                                                #pragma omp critical
                                {
                                    entry.samples.push_back(index);
                                    samples.push_back(m_initialInfoVec[index].pos);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PoissonDiskSampler::nbhConflict(const std::unordered_map<Vec3i, HashEntry, CellPosHasher>& hMap, const InitialPointInfo& iPI)
{
    Vec3i nbPos = iPI.cP;

    // check neighboring cells inside to outside
    if(checkCell(hMap, nbPos, iPI)) {
        return true;
    }
    for(Int level = 1; level < 3; level++) {
        for(Int ud = -level; ud < level + 1; ud += 2 * level) {
            for(Int i = -level + 1; i < level; i++) {
                for(Int j = -level + 1; j < level; j++) {
                    nbPos = Vec3i(i, ud, j) + iPI.cP;
                    if(checkCell(hMap, nbPos, iPI)) {
                        return true;
                    }
                }
            }

            for(Int i = -level; i < level + 1; i++) {
                for(Int j = -level + 1; j < level; j++) {
                    nbPos = Vec3i(j, i, ud) + iPI.cP;
                    if(checkCell(hMap, nbPos, iPI)) {
                        return true;
                    }
                }

                for(Int j = -level; j < level + 1; j++) {
                    nbPos = Vec3i(ud, i, j) + iPI.cP;
                    if(checkCell(hMap, nbPos, iPI)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PoissonDiskSampler::checkCell(const std::unordered_map<Vec3i, HashEntry, CellPosHasher>& hMap, const Vec3i& cell, const InitialPointInfo& iPI)
{
    const auto nbEntryIt = hMap.find(cell);
    if(nbEntryIt != hMap.end()) {
        const HashEntry& nbEntry = nbEntryIt->second;
        for(UInt i = 0; i < nbEntry.samples.size(); i++) {
            const InitialPointInfo& info = m_initialInfoVec[nbEntry.samples[i]];
            Real                    dist;
            if(m_distanceNorm == 0 || iPI.ID == info.ID) {
                dist = glm::length(iPI.pos - info.pos);
            } else if(m_distanceNorm == 1) {
                Vec3r v  = glm::normalize(info.pos - iPI.pos);
                Real  c1 = glm::dot(m_faceNormals[iPI.ID], v);
                Real  c2 = glm::dot(m_faceNormals[info.ID], v);

                dist = glm::length(iPI.pos - info.pos);
                if(std::abs(c1 - c2) > 0.00001f) {
                    dist *= (asin(c1) - asin(c2)) / (c1 - c2);
                } else {
                    dist /= (sqrt(1.0 - c1 * c1));
                }
            } else {
                return true;
            }

            if(dist < m_r) {
                return true;
            }
        }
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::determineMinX(const UInt numVertices, const Vec3r* vertices)
{
    m_minVec = Vec3r(Huge<Real>());

    for(Int i = 0; i < (Int)numVertices; i++) {
        const Vec3r& v = vertices[i];
        m_minVec[0] = std::min(m_minVec[0], v[0]);
        m_minVec[1] = std::min(m_minVec[1], v[1]);
        m_minVec[2] = std::min(m_minVec[2], v[2]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::quickSort(Int left, Int right)
{
    if(left < right) {
        Int index = partition(left, right);
        quickSort(left,  index - 1);
        quickSort(index, right);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int PoissonDiskSampler::partition(Int left, Int right)
{
    Int              i = left;
    Int              j = right;
    Vec3r            tmpPos;
    Vec3i            tmpCell;
    InitialPointInfo tmpInfo;
    Vec3i            pivot = m_initialInfoVec[left + (right - left) / 2].cP;

    while(i <= j) {
        while(compareCellID(m_initialInfoVec[i].cP, pivot)) {
            i++;
        }

        while(compareCellID(pivot, m_initialInfoVec[j].cP)) {
            j--;
        }

        if(i <= j) {
            tmpInfo             = m_initialInfoVec[i];
            m_initialInfoVec[i] = m_initialInfoVec[j];
            m_initialInfoVec[j] = tmpInfo;
            i++;
            j--;
        }
    }
    return i;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PoissonDiskSampler::compareCellID(Vec3i& a, Vec3i& b)
{
    for(UInt i = 0; i < 3; i++) {
        if(a[i] < b[i]) { return true; }
        if(a[i] > b[i]) { return false; }
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PoissonDiskSampler::computeFaceNormals(const UInt numVertices, const Vec3r* vertices, const UInt numFaces, const UInt* faces)
{
    m_faceNormals.resize(numFaces);

        #pragma omp parallel default(shared)
    {
                #pragma omp for schedule(static)
        for(Int i = 0; i < (Int)numFaces; i++) {
            // Get first three points of face
            const Vec3r& a = vertices[faces[3 * i]];
            const Vec3r& b = vertices[faces[3 * i + 1]];
            const Vec3r& c = vertices[faces[3 * i + 2]];

            // Create normal
            Vec3r v1 = b - a;
            Vec3r v2 = c - a;

            m_faceNormals[i] = glm::cross(v1, v2);
            m_faceNormals[i] = glm::normalize(m_faceNormals[i]);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
