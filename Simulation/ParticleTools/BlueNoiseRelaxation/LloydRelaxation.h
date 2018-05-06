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
#include <Banana/Utils/Logger.h>
#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Utils/Timer.h>

#include <tbb/tbb.h>

#include <limits>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class LloydRelaxation
{
public:
    LloydRelaxation(const Vec3<Real>& domainBMin, const Vec3<Real>& domainBMax, Real particleRadius);
    ~LloydRelaxation() = default;

    void relaxPositions(std::vector<Vec3<Real> >& denseParticles,
                        std::vector<Vec3<Real> >& particles,
                        int                       minIterations        = 10,
                        int                       maxIterations        = 1000,
                        bool                      bUseCandidateCenters = false);

    void relaxParticlesWeighted(const std::vector<Real>&  weights,
                                std::vector<Vec3<Real> >& denseParticles,
                                std::vector<Vec3<Real> >& particles,
                                int                       minIterations        = 10,
                                int                       maxIterations        = 1000,
                                bool                      bUseCandidateCenters = false);

    void setMovingThreshold(Real movingThreshold) { m_MovingThreshold = movingThreshold; }
    void setOverlapThreshold(Real overlapThreshold) { m_OverlapThreshold = overlapThreshold; }
    void setRemovingThreshold(Real removingThreshold) { m_RemovingThreshold = removingThreshold; }
    void setNumCheckIterations(int numCheckIterations) { m_NumCheckIterations = numCheckIterations; }

private:
    Real         m_MovingThreshold;
    Real         m_OverlapThreshold;
    Real         m_RemovingThreshold;
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

    void computeLloydClusters(std::vector<Vec3<Real> >& samples,
                              std::vector<Vec3<Real> >& clusterCenters,
                              int                       minIterations        = 10,
                              int                       maxIterations        = 1000,
                              bool                      bUseCandidateCenters = false);

    void computeWeightedLloydClusters(const std::vector<Real>&  weights,
                                      std::vector<Vec3<Real> >& samples,
                                      std::vector<Vec3<Real> >& clusterCenters,
                                      int                       minIterations        = 10,
                                      int                       maxIterations        = 1000,
                                      bool                      bUseCandidateCenters = false);

    ////////////////////////////////////////////////////////////////////////////////
    ///	Clusters a set of samples by assigning each sample to its closest cluster.
    ///	\c clusterCenter contains the centers of the cluster.
    ///	On return, \c samplesInCluster will contain for each cluster the indices of the samples in the cluster.
    void collectSampleToCluster(const std::vector<Vec3<Real> >& clusterCenters, const std::vector<Vec3<Real> >& samples, Vec_VecUInt& samplesInCluster);

    size_t computeMedian(const std::vector<Vec3<Real> >& samples, const Vec_UInt& subsetIndices);
    size_t computeWeightedMedian(const std::vector<Vec3<Real> >& samples, const std::vector<Real>& weights, const Vec_UInt& subsetIndices);
    void   computeMean(const std::vector<Vec3<Real> >& samples, const Vec_UInt& subsetIndices, Vec3<Real>& mean);
    void   computeWeightedMean(const std::vector<Vec3<Real> >& samples, const std::vector<Real>& weights, const Vec_UInt& subsetIndices, Vec3<Real>& mean);

    Real computeMinDistance(const std::vector<Vec3<Real> >& clusterCenters);
    Real computeMinDistance(const std::vector<Vec3<Real> >& clusterCenters, UInt& numOverlappedParticles);

    UInt removeOverlappedParticles(std::vector<Vec3<Real> >& clusterCenters);
    void sortSamples(std::vector<Vec3<Real> >& samples, const Vec_VecUInt& samplesInCluster);
    void collectClustersToCells(const std::vector<Vec3<Real> >& clusterCenters);


    ////////////////////////////////////////////////////////////////////////////////
    Vec3<Real>     m_DomainBMin;
    Vec3<Real>     m_DomainBMax;
    Real           m_ParticleRadius;
    Vec_UInt       m_ClosestCluster;
    Array3_VecUInt m_CellParticles;
    Grid3r         m_Grid3D;

    Logger m_Logger;
    Timer  m_Timer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana