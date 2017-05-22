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

#include <BananaCore/Logger.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
class LloydRelaxation
{
public:
    LloydRelaxation(DomainParameters* domainParams_, ScalarType particle_radius_);
    ~LloydRelaxation();

    void relax_particles(std::vector<VectorType>& material_points,
                         std::vector<VectorType>& particles,
                         int                      min_iterations = 10,
                         int                      max_iterations = 100,
                         bool                     use_candidate_center = false);

    void relax_particles_weighted(const Vec_ScalarType     weights,
                                  std::vector<VectorType>& material_points,
                                  std::vector<VectorType>& particles,
                                  int                      min_iterations = 10,
                                  int                      max_iterations = 100,
                                  bool                     use_candidate_center = false);
    ScalarType moving_percentage_threshold;
    ScalarType overlap_ratio_threshold;
    ScalarType remove_ratio_threshold;
    int        check_per_iterations;

private:
    /// Given a set of points in \c samples and a set of cluster centers, clusters
    ///     the samples using Lloyd's algorithm with \c numIterations iterations.
    ///     \c clusterCenters contains the initial position of each cluster's center.
    ///     After the call, \c clusterCenters will contain the adjusted cluster center positions,
    ///     and \c samplesPerCluster will contain for every cluster the indices of the samples
    ///     assigned to the cluster.
    ///     If \c useCandidateCenters is set to \c true, the cluster centers will be a subset
    ///     of the samples positions.

    void compute_lloyd_clusters(std::vector<VectorType>& samples,
                                std::vector<VectorType>& cluster_centers,
                                int                      min_iterations = 10,
                                int                      max_iterations = 100,
                                bool                     use_candidate_center = false);

    void compute_weighted_lloyd_clusters(const Vec_ScalarType&    weights,
                                         std::vector<VectorType>& samples,
                                         std::vector<VectorType>& cluster_centers,
                                         int                      min_iterations = 10,
                                         int                      max_iterations = 100,
                                         bool                     use_candidate_center = false);

    ///	Clusters a set of samples by assigning each sample to its closest cluster.
    ///	\c clusterCenter contains the centers of the cluster.
    ///	On return, \c samplesPerCluster will contain for each cluster the indices of the samples in the cluster.

    void collect_sample_to_cluster(const Vec_Vec3& cluster_centers,
                                   const Vec_Vec3& samples,
                                   Vec_VecUInt&    samples_per_cluster);

    ////////////////////////////////////////////////////////////////////////////////
    // static functions
    static size_t compute_median(const std::vector<VectorType>& samples,
                                 const Vec_UInt&                subset_indices);
    static void compute_mean(const std::vector<VectorType>& samples,
                             const Vec_UInt&                subset_indices,
                             VectorType&                    mean);
    static size_t compute_weighted_median(const std::vector<VectorType>& samples,
                                          const Vec_ScalarType&          weights,
                                          const Vec_UInt&                subset_indices);
    static void compute_weighted_mean(const std::vector<VectorType>& samples,
                                      const Vec_ScalarType&          weights,
                                      const Vec_UInt&                subset_indices,
                                      Vec3&                          mean);
    ScalarType check_vector_distance(const std::vector<VectorType>& cluster_centers);
    ScalarType check_min_distance(const std::vector<VectorType>& cluster_centers,
                                  UInt32&                        num_overlapped_particles);
    UInt32 remove_overlapped_particles(std::vector<VectorType>& cluster_centers);
    void   sort_sample(Vec_Vec3& samples, const Vec_VecUInt& samples_per_cluster);
    void   collect_clusters_to_cells(const Vec_Vec3& cluster_centers);
    Vec3i  get_small_cellId(const Vec3& pos);
    bool   is_valid_small_cell(const Vec3i& cellId);

    ////////////////////////////////////////////////////////////////////////////////
    DomainParameters* domainParams;
    ScalarType        particle_radius;
    Array3_VecUInt    cell3D_particles;
    Vec3ui            num_small_cells;
    ScalarType        small_cell_size;
    Vec_UInt          closest_cluster;

    Logger logger;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation_Imp.hpp>