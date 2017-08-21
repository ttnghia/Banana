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
#include <Banana/Utils/Logger.h>
#include <Banana/Array/Array3.h>
#include <Banana/Grid/Grid3D.h>
#include <Banana/Utils/Timer.h>

#include <tbb/tbb.h>

#include <limits>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class LloydRelaxation
{
public:
    LloydRelaxation(const Vec3<RealType>& domainBMin, const Vec3<RealType>& domainBMax, RealType particleRadius);
    ~LloydRelaxation() = default;

    void relaxParticles(std::vector<Vec3<RealType> >& denseParticles,
                        std::vector<Vec3<RealType> >& particles,
                        int                           minIterations = 10,
                        int                           maxIterations = 1000,
                        bool                          bUseCandidateCenters = false);

    void relaxParticlesWeighted(const std::vector<RealType>&  weights,
                                std::vector<Vec3<RealType> >& denseParticles,
                                std::vector<Vec3<RealType> >& particles,
                                int                           minIterations = 10,
                                int                           maxIterations = 1000,
                                bool                          bUseCandidateCenters = false);

    void setMovingThreshold(RealType movingThreshold) { m_MovingThreshold = movingThreshold; }
    void setOverlapThreshold(RealType overlapThreshold) { m_OverlapThreshold = overlapThreshold; }
    void setRemovingThreshold(RealType removingThreshold) { m_RemovingThreshold = removingThreshold; }
    void setNumCheckIterations(int numCheckIterations) { m_NumCheckIterations = numCheckIterations; }

private:
    RealType     m_MovingThreshold;
    RealType     m_OverlapThreshold;
    RealType     m_RemovingThreshold;
    unsigned int m_NumCheckIterations;


    ////////////////////////////////////////////////////////////////////////////////
    /// Given a set of points in \c samples and a set of cluster centers, clusters
    ///     the samples using Lloyd's algorithm with \c numIterations iterations.
    ///     \c clusterCenters contains the initial position of each cluster's64 center.
    ///     After the call, \c clusterCenters will contain the adjusted cluster center positions,
    ///     and \c samplesInCluster will contain for every cluster the indices of the samples
    ///     assigned to the cluster.
    ///     If \c useCandidateCenters is set to \c true, the cluster centers will be a subset
    ///     of the samples positions.

    void computeLloydClusters(std::vector<Vec3<RealType> >& samples,
                              std::vector<Vec3<RealType> >& clusterCenters,
                              int                           minIterations = 10,
                              int                           maxIterations = 1000,
                              bool                          bUseCandidateCenters = false);

    void computeWeightedLloydClusters(const std::vector<RealType>&  weights,
                                      std::vector<Vec3<RealType> >& samples,
                                      std::vector<Vec3<RealType> >& clusterCenters,
                                      int                           minIterations = 10,
                                      int                           maxIterations = 1000,
                                      bool                          bUseCandidateCenters = false);

    ////////////////////////////////////////////////////////////////////////////////
    ///	Clusters a set of samples by assigning each sample to its closest cluster.
    ///	\c clusterCenter contains the centers of the cluster.
    ///	On return, \c samplesInCluster will contain for each cluster the indices of the samples in the cluster.
    void collectSampleToCluster(const std::vector<Vec3<RealType> >& clusterCenters, const std::vector<Vec3<RealType> >& samples, Vec_VecUInt& samplesInCluster);

    size_t computeMedian(const std::vector<Vec3<RealType> >& samples, const Vec_UInt& subsetIndices);
    size_t computeWeightedMedian(const std::vector<Vec3<RealType> >& samples, const std::vector<RealType>& weights, const Vec_UInt& subsetIndices);
    void   computeMean(const std::vector<Vec3<RealType> >& samples, const Vec_UInt& subsetIndices, Vec3<RealType>& mean);
    void   computeWeightedMean(const std::vector<Vec3<RealType> >& samples, const std::vector<RealType>& weights, const Vec_UInt& subsetIndices, Vec3<RealType>& mean);

    RealType computeMinDistance(const std::vector<Vec3<RealType> >& clusterCenters);
    RealType computeMinDistance(const std::vector<Vec3<RealType> >& clusterCenters, UInt32& numOverlappedParticles);

    UInt32 removeOverlappedParticles(std::vector<Vec3<RealType> >& clusterCenters);
    void   sortSamples(std::vector<Vec3<RealType> >& samples, const Vec_VecUInt& samplesInCluster);
    void   collectClustersToCells(const std::vector<Vec3<RealType> >& clusterCenters);


    ////////////////////////////////////////////////////////////////////////////////
    Vec3<RealType>   m_DomainBMin;
    Vec3<RealType>   m_DomainBMax;
    RealType         m_ParticleRadius;
    Vec_UInt         m_ClosestCluster;
    Array3_VecUInt   m_CellParticles;
    Grid3D<RealType> m_Grid3D;

    Logger m_Logger;
    Timer  m_Timer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana