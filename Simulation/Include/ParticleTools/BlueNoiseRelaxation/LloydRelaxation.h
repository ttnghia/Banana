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

#include <Banana/Logger.h>
#include <Grid/Grid3D.h>


#include <tbb/tbb.h>

//#include <Noodle/Core/Global/Constants.h>
//#include <Noodle/Core/Global/Enums.h>
//#include <Noodle/Core/Global/Macros.h>
//#include <Noodle/Core/Global/Parameters.h>
//#include <Noodle/Core/Global/TypeNames.h>
//#include <Noodle/Core/Math/MathUtils.h>
//#include <Noodle/Core/Monitor/Timer.h>
//#include <Noodle/Core/Parallel/ParallelObjects.h>
//#include <Noodle/Core/Monitor/MemoryUsage.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
class LloydRelaxation
{
public:
    LloydRelaxation(const VectorType& domainBMin, const VectorType& domainBMax, ScalarType particleRadius);
    ~LloydRelaxation() = default;

    void relaxParticles(std::vector<VectorType>& denseParticles,
                        std::vector<VectorType>& particles,
                        int                      minIterations = 10,
                        int                      maxIterations = 1000,
                        bool                     bUseCandidateCenters = false);

    void relaxParticlesWeighted(const std::vector<ScalarType>& weights,
                                std::vector<VectorType>&       denseParticles,
                                std::vector<VectorType>&       particles,
                                int                            minIterations = 10,
                                int                            maxIterations = 1000,
                                bool                           bUseCandidateCenters = false);

    void setMovingThreshold(ScalarType movingThreshold) { m_MovingThreshold = movingThreshold; }
    void setOverlapThreshold(ScalarType overlapThreshold) { m_OverlapThreshold = overlapThreshold; }
    void setRemovingThreshold(ScalarType removingThreshold) { m_RemovingThreshold = removingThreshold; }
    void setNumCheckIterations(int numCheckIterations) { m_NumCheckIterations = numCheckIterations; }

private:
    ScalarType   m_MovingThreshold;
    ScalarType   m_OverlapThreshold;
    ScalarType   m_RemovingThreshold;
    unsigned int m_NumCheckIterations;


    ////////////////////////////////////////////////////////////////////////////////
    /// Given a set of points in \c samples and a set of cluster centers, clusters
    ///     the samples using Lloyd's algorithm with \c numIterations iterations.
    ///     \c clusterCenters contains the initial position of each cluster's center.
    ///     After the call, \c clusterCenters will contain the adjusted cluster center positions,
    ///     and \c samplesPerCluster will contain for every cluster the indices of the samples
    ///     assigned to the cluster.
    ///     If \c useCandidateCenters is set to \c true, the cluster centers will be a subset
    ///     of the samples positions.

    void computeLloydClusters(std::vector<VectorType>& samples,
                              std::vector<VectorType>& clusterCenters,
                              int                      minIterations = 10,
                              int                      maxIterations = 1000,
                              bool                     bUseCandidateCenters = false);

    void computeWeightedLloydClusters(const std::vector<ScalarType>& weights,
                                      std::vector<VectorType>&       samples,
                                      std::vector<VectorType>&       clusterCenters,
                                      int                            minIterations = 10,
                                      int                            maxIterations = 1000,
                                      bool                           bUseCandidateCenters = false);

    ////////////////////////////////////////////////////////////////////////////////
    ///	Clusters a set of samples by assigning each sample to its closest cluster.
    ///	\c clusterCenter contains the centers of the cluster.
    ///	On return, \c samplesPerCluster will contain for each cluster the indices of the samples in the cluster.
    void collectSampleToCluster(const std::vector<VectorType>& clusterCenters, const std::vector<VectorType>& samples, Vec_VecUInt& samplesPerCluster);

    ////////////////////////////////////////////////////////////////////////////////
    // static functions
    static size_t computeMedian(const std::vector<VectorType>& samples, const Vec_UInt& subsetIndices);
    static void   computeMean(const std::vector<VectorType>& samples, const Vec_UInt& subsetIndices, VectorType& mean);
    static size_t compute_weighted_median(const std::vector<VectorType>& samples, const std::vector<ScalarType>& weights, const Vec_UInt& subsetIndices);
    static void   compute_weighted_mean(const std::vector<VectorType>& samples, const std::vector<ScalarType>& weights, const Vec_UInt& subsetIndices, Vec3<ScalarType>& mean);
    ScalarType    check_vector_distance(const std::vector<VectorType>& clusterCenters);
    ScalarType    check_min_distance(const std::vector<VectorType>& clusterCenters,
                                     UInt32&                        num_overlapped_particles);
    UInt32 remove_overlapped_particles(std::vector<VectorType>& clusterCenters);
    void   sort_sample(std::vector<VectorType>& samples, const Vec_VecUInt& samples_per_cluster);
    void   collect_clusters_to_cells(const std::vector<VectorType>& clusterCenters);
    Vec3i  get_small_cellId(const Vec3<ScalarType>& pos);
    bool   is_valid_small_cell(const Vec3i& cellId);

    ////////////////////////////////////////////////////////////////////////////////
    VectorType                    m_DomainBMin;
    VectorType                    m_DomainBMax;
    ScalarType                    m_ParticleRadius;
    Vec_UInt                      m_ClosestCluster;
    Array3_VecUInt                m_CellParticles;
    Grid3D<ArrayType, ScalarType> m_Grid3D;

    Logger m_Logger;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation_Impl.hpp>