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

#ifndef LLOYD_ALGORITHM_H
#define LLOYD_ALGORITHM_H

#include <tbb/tbb.h>

#include <Noodle/Core/Global/Constants.h>
#include <Noodle/Core/Global/Enums.h>
#include <Noodle/Core/Global/Macros.h>
#include <Noodle/Core/Global/Parameters.h>
#include <Noodle/Core/Global/TypeNames.h>
#include <Noodle/Core/Array/Array2.h>
#include <Noodle/Core/Array/Array3.h>
#include <Noodle/Core/Math/MathUtils.h>
#include <Noodle/Core/Monitor/Timer.h>
#include <Noodle/Core/Parallel/ParallelObjects.h>
#include <Noodle/Core/Monitor/MemoryUsage.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class LloydRelaxation
{
public:
    LloydRelaxation(DomainParameters* domainParams_, Real particle_radius_) :
        domainParams(domainParams_),
        particle_radius(particle_radius_),
        moving_percentage_threshold(3.0),
        overlap_ratio_threshold(1.55),
        remove_ratio_threshold(1.55),
        check_per_iterations(1),
        monitor(MonitorSource::ParticleRelaxation)
    {
        small_cell_size = particle_radius * 2.5;
        num_small_cells = domainParams->getGridSize(domainParams->domainBMin,
                                                    domainParams->domainBMax,
                                                    small_cell_size);
        cell3D_particles.resize(num_small_cells);
    }

    ~LloydRelaxation()
    {
        cell3D_particles.clear();
        closest_cluster.clear();
    }

    template<class VectorType>
    void relax_particles(std::vector<VectorType>& material_points,
                         std::vector<VectorType>&              particles,
                         int                                   min_iterations = 10,
                         int                                   max_iterations = 100,
                         bool                                  use_candidate_center = false)
    {
        __NOODLE_ASSERT(material_points.size() > 4 * particles.size());
        compute_lloyd_clusters(material_points, particles, min_iterations,
                               max_iterations, use_candidate_center);
    }


    template<class VectorType>
    void relax_particles_weighted(const Vec_Real weights,
                                  std::vector<VectorType>&             material_points,
                                  std::vector<VectorType>&             particles,
                                  int                                  min_iterations = 10,
                                  int                                  max_iterations = 100,
                                  bool                                 use_candidate_center = false)
    {
        __NOODLE_ASSERT(material_points.size() > 4 * particles.size());
        compute_weighted_lloyd_clusters(weights, material_points, particles, min_iterations,
                                        max_iterations, use_candidate_center);
    }

    Real moving_percentage_threshold;
    Real overlap_ratio_threshold;
    Real remove_ratio_threshold;
    int  check_per_iterations;
private:
    /// Given a set of points in \c samples and a set of cluster centers, clusters
    ///     the samples using Lloyd's algorithm with \c numIterations iterations.
    ///     \c clusterCenters contains the initial position of each cluster's center.
    ///     After the call, \c clusterCenters will contain the adjusted cluster center positions,
    ///     and \c samplesPerCluster will contain for every cluster the indices of the samples
    ///     assigned to the cluster.
    ///     If \c useCandidateCenters is set to \c true, the cluster centers will be a subset
    ///     of the samples positions.

    template<class VectorType>
    void compute_lloyd_clusters(std::vector<VectorType>& samples,
                                std::vector<VectorType>&                     cluster_centers,
                                int                                          min_iterations = 10,
                                int                                          max_iterations = 100,
                                bool                                         use_candidate_center = false)
    {
        Timer                   timer;
        Real                    total_time;

        Vec_VecUInt             samples_per_cluster;
        std::vector<VectorType> cluster_backup;
        std::vector<Real>       move_distance;
        Real                    min_cluster_distance = BIG_NUMBER;
        Real                    max_moved_percentage = BIG_NUMBER;
        UInt32                  num_overlapped_particles = 1;

        bool                    converged = false;

        ////////////////////////////////////////////////////////////////////////////////

        cluster_backup.resize(cluster_centers.size());
        move_distance.resize(cluster_centers.size());

        for(int iter = 0; iter < max_iterations; ++iter)
        {
            monitor.printLog("Relaxation iteration: " + std::to_string(iter));
            size_t num_clusters = cluster_centers.size();

            timer.tick();
            collect_sample_to_cluster(cluster_centers, samples, samples_per_cluster);
            __NOODLE_ASSERT(samples_per_cluster.size() == num_clusters);
            total_time = timer.tock();
            monitor.printLog(timer.getRunTime("Collect samples to clusters total: "));

            ////////////////////////////////////////////////////////////////////////////////
            std::copy(cluster_centers.begin(), cluster_centers.end(), cluster_backup.begin());

            timer.tick();
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_clusters),
                              [&](tbb::blocked_range<size_t> r)
            {
                for(size_t cluster_id = r.begin(); cluster_id != r.end(); ++cluster_id)
                {
                    __NOODLE_ASSERT(samples_per_cluster[cluster_id].size() > 0);

                    VectorType& center = cluster_centers[cluster_id];

                    if(use_candidate_center)
                    {
                        size_t medianIdx = compute_median(samples, samples_per_cluster[cluster_id]);
                        center = samples[samples_per_cluster[cluster_id][medianIdx]];
                    }
                    else
                    {
                        compute_mean(samples, samples_per_cluster[cluster_id], center);
                    }

                    move_distance[cluster_id] = glm::length2(center - cluster_backup[cluster_id]);
                }
            }); // end parallel for

            total_time += timer.tock();
            monitor.printLog(timer.getRunTime("Compute new cluster positions: "));

            VectorMaxElement<Real> md2(move_distance);
            tbb::parallel_reduce(tbb::blocked_range<size_t>(0, move_distance.size()), md2);
            max_moved_percentage = sqrt(md2.result) / particle_radius * 100.0;
            monitor.printLog("Max moving distance in this iteration: " + NumberHelpers::formatWithCommas(max_moved_percentage) + "% of particle radius");

            if(iter % check_per_iterations == 0)
            {
                min_cluster_distance = check_min_distance(cluster_centers, num_overlapped_particles);
                monitor.newLine();
                monitor.printLog("Min distance: " + NumberHelpers::formatToScientific(
                    min_cluster_distance) +
                    ". Min distance/particle_radius = " +
                    NumberHelpers::formatWithCommas(min_cluster_distance / particle_radius));
                monitor.printLog("Number of overlapped particles: " +
                                 NumberHelpers::formatWithCommas(num_overlapped_particles));
                monitor.newLine();
            }

            if((iter + 1) % 10 == 0 && max_moved_percentage > 10)
            {
                sort_sample(samples, samples_per_cluster);
            }

            monitor.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(
                total_time) + "ms");
            monitor.printLog("Memory usage: " + NumberHelpers::formatWithCommas(
                getCurrentRSS() / 1048576.0) +
                " MB(s). Peak: " + NumberHelpers::formatWithCommas(getPeakRSS() / 1048576.0) + " MB(s).");
            monitor.newLine();

            if(iter > min_iterations && max_moved_percentage < moving_percentage_threshold)
            {
                if(num_overlapped_particles == 0)
                {
                    converged = true;
                    break;
                }

                if(num_overlapped_particles > 0)
                {
                    UInt32 num_removed_particles = remove_overlapped_particles(cluster_centers);
                    monitor.printLog("Removed particles: " +
                                     NumberHelpers::formatWithCommas(num_removed_particles) +
                                     " =======================================");
                    monitor.newLine();

                    if(num_removed_particles == 0)
                    {
                        // if cannot remove any particles, exit...
                        break;
                    }
                }
            }
        } // end iterations


        min_cluster_distance = check_min_distance(cluster_centers, num_overlapped_particles);
        monitor.printLog("Finished relaxation. Min distance: " +
                         NumberHelpers::formatToScientific(min_cluster_distance) +
                         ". Min distance/particle_radius = " +
                         NumberHelpers::formatWithCommas(min_cluster_distance / particle_radius));
        monitor.printLog("Max moving distance in last iteration: " +
                         NumberHelpers::formatWithCommas(max_moved_percentage) +
                         "% of particle radius");
        monitor.printLog("Number of overlapped particles: " +
                         NumberHelpers::formatWithCommas(num_overlapped_particles));

        if(converged)
        {
            monitor.printLog("Relaxation converged.");
        }
        else
        {
            monitor.printLog("Relaxation did NOT converge.");
        }

        monitor.newLine();
        __NOODLE_ASSERT(min_cluster_distance > overlap_ratio_threshold * particle_radius);
    }

    template<class VectorType>
    void compute_weighted_lloyd_clusters(const Vec_Real& weights,
                                         std::vector<VectorType>&                     samples,
                                         std::vector<VectorType>&                     cluster_centers,
                                         int                                          min_iterations = 10,
                                         int                                          max_iterations = 100,
                                         bool                                         use_candidate_center = false)
    {
        __NOODLE_ASSERT(samples.size() == weights.size());

        Timer                   timer;
        Real                    total_time;

        Vec_VecUInt             samples_per_cluster;
        std::vector<VectorType> cluster_backup;
        std::vector<Real>       move_distance;
        size_t                  num_clusters = cluster_centers.size();
        Real                    min_cluster_distance = BIG_NUMBER;
        Real                    max_moved_percentage = BIG_NUMBER;
        UInt32                  num_overlapped_particles;

        bool                    converged = false;

        ////////////////////////////////////////////////////////////////////////////////
        cluster_backup.resize(cluster_centers.size());
        move_distance.resize(cluster_centers.size());

        for(int iter = 0; iter < max_iterations; ++iter)
        {
            monitor.printLog("Relaxation iteration: " + std::to_string(iter));

            timer.tick();
            collect_sample_to_cluster(cluster_centers, samples, samples_per_cluster);
            __NOODLE_ASSERT(samples_per_cluster.size() == num_clusters);
            total_time = timer.tock();
            monitor.printLog(timer.getRunTime("Collect samples to clusters total: "));

            ////////////////////////////////////////////////////////////////////////////////
            std::copy(cluster_centers.begin(), cluster_centers.end(), cluster_backup.begin());

            timer.tick();
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_clusters),
                              [&](tbb::blocked_range<size_t> r)
            {
                for(size_t cluster_id = r.begin(); cluster_id != r.end(); ++cluster_id)
                {
                    __NOODLE_ASSERT(samples_per_cluster[cluster_id].size() > 0);
                    VectorType& center = cluster_centers[cluster_id];

                    if(use_candidate_center)
                    {
                        int medianIdx = compute_weighted_median(samples, weights,
                                                                samples_per_cluster[cluster_id]);
                        center = samples[samples_per_cluster[cluster_id][medianIdx]];
                    }
                    else
                    {
                        compute_weighted_mean(samples, weights, samples_per_cluster[cluster_id], center);
                    }

                    move_distance[cluster_id] = glm::length2(center - cluster_backup[cluster_id]);
                }
            }); // end parallel for

            total_time += timer.tock();
            monitor.printLog(timer.getRunTime("Compute new cluster positions: "));

            VectorMaxElement<Real> md2(move_distance);
            tbb::parallel_reduce(tbb::blocked_range<size_t>(0, move_distance.size()), md2);
            max_moved_percentage = sqrt(md2.result) / particle_radius * 100.0;
            monitor.printLog("Max moving distance in this iteration: " +
                             NumberHelpers::formatWithCommas(max_moved_percentage) +
                             "% of particle radius");

            if(iter % check_per_iterations == 0)
            {
                min_cluster_distance = check_min_distance(cluster_centers, num_overlapped_particles);
                monitor.newLine();
                monitor.printLog("Min distance: " + NumberHelpers::formatToScientific(
                    min_cluster_distance) +
                    ". Min distance/particle_radius = " +
                    NumberHelpers::formatWithCommas(min_cluster_distance / particle_radius));
                monitor.printLog("Number of overlapped particles: " +
                                 NumberHelpers::formatWithCommas(num_overlapped_particles));
                monitor.newLine();
            }

            if((iter + 1) % 10 == 0 && max_moved_percentage > 10)
            {
                sort_sample(samples, samples_per_cluster);
            }

            monitor.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(
                total_time) + "ms");
            monitor.printLog("Memory usage: " + NumberHelpers::formatWithCommas(
                getCurrentRSS() / 1048576.0) +
                " MB(s). Peak: " + NumberHelpers::formatWithCommas(getPeakRSS() / 1048576.0) + " MB(s).");
            monitor.newLine();

            if(max_moved_percentage < moving_percentage_threshold && iter > min_iterations)
            {
                converged = true;
                break;
            }
        } // end iterations

        min_cluster_distance = check_min_distance(cluster_centers, num_overlapped_particles);
        monitor.printLog("Finished relaxation. Min distance: " +
                         NumberHelpers::formatWithCommas(min_cluster_distance) +
                         ". Min distance/particle_radius = " +
                         NumberHelpers::formatWithCommas(min_cluster_distance / particle_radius));
        monitor.printLog("Max moving distance in last iteration: " +
                         NumberHelpers::formatWithCommas(max_moved_percentage) +
                         "% of particle radius");
        monitor.printLog("Number of overlapped particles: " +
                         NumberHelpers::formatWithCommas(num_overlapped_particles));

        if(converged)
        {
            monitor.printLog("Relaxation converged.");
        }
        else
        {
            monitor.printLog("Relaxation did NOT converge.");
        }

        monitor.newLine();
        __NOODLE_ASSERT(min_cluster_distance > overlap_ratio_threshold * particle_radius);
    }



    ///	Clusters a set of samples by assigning each sample to its closest cluster.
    ///	\c clusterCenter contains the centers of the cluster.
    ///	On return, \c samplesPerCluster will contain for each cluster the indices of the samples in the cluster.

    void collect_sample_to_cluster(const Vec_Vec3& cluster_centers,
                                   const Vec_Vec3&                        samples,
                                   Vec_VecUInt&                           samples_per_cluster)
    {
        static Timer timer;
        size_t       num_clusters = cluster_centers.size();
        size_t       num_samples = samples.size();

        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();
        collect_clusters_to_cells(cluster_centers);
        timer.tock();
        monitor.printLogIndent(timer.getRunTime("Collect clusters to cells: "));

        closest_cluster.resize(num_samples);
        samples_per_cluster.resize(num_clusters);
        __NOODLE_CLEAR_VECVEC(samples_per_cluster);


        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, num_samples),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t sample_id = r.begin(); sample_id != r.end(); ++sample_id)
            {
                const Vec3& sp = samples[sample_id];
                const Vec3i& scellId = get_small_cellId(sp);
                UInt32 closest_cluster_id = 0;
                Real closest_cluster_d2 = BIG_NUMBER;
                bool has_close_cluster = false;

                for(Int32 lk = -1; lk <= 1; ++lk)
                {
                    for(Int32 lj = -1; lj <= 1; ++lj)
                    {
                        for(Int32 li = -1; li <= 1; ++li)
                        {
                            const Vec3i cellId = scellId + Vec3i(li, lj, lk);

                            if(!is_valid_small_cell(cellId))
                            {
                                continue;
                            }

                            for(const UInt32 cluster_id : cell3D_particles(cellId))
                            {
                                const Vec3& cp = cluster_centers[cluster_id];
                                const Real d2 = glm::length2(sp - cp);

                                if(d2 < closest_cluster_d2)
                                {
                                    has_close_cluster = true;
                                    closest_cluster_d2 = d2;
                                    closest_cluster_id = cluster_id;
                                }
                            }
                        }
                    }
                } // end loop over neighbor cells

                __NOODLE_ASSERT(has_close_cluster);
                closest_cluster[sample_id] = closest_cluster_id;
            }
        }); // end parallel for
        timer.tock();
        monitor.printLogIndent(timer.getRunTime("Find nearest cluster for each sample: "));

        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();

        for(size_t sample_id = 0; sample_id < num_samples; ++sample_id)
        {
            samples_per_cluster[closest_cluster[sample_id]].push_back(sample_id);
        }

        timer.tock();
        monitor.printLogIndent(timer.getRunTime("Write nearest cluster to sample array: "));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // static functions

    template<class VectorType>
    static size_t compute_median(const std::vector<VectorType>& samples,
                                 const Vec_UInt&                                     subset_indices)
    {
        Vec_Real dist2(subset_indices.size(), 0);

        for(size_t i = 0; i < subset_indices.size(); ++i)
        {
            const VectorType& si = samples[subset_indices[i]];

            for(size_t j = 0; j < i; ++j)
            {
                const VectorType& sj = samples[subset_indices[j]];
                const Real        d2 = glm::length2(si - sj);
                dist2[i] += d2;
                dist2[j] += d2;
            }
        }

        Real   distMin = dist2[0];
        size_t idxMin = 0;

        for(size_t i = 1; i < dist2.size(); ++i)
        {
            if(dist2[i] < distMin)
            {
                distMin = dist2[i];
                idxMin = i;
            }
        }

        return idxMin;
    }

    template<class VectorType>
    static void compute_mean(const std::vector<VectorType>& samples,
                             const Vec_UInt&                                 subset_indices,
                             VectorType&                                     mean)
    {
        mean = VectorType(0);

        for(size_t i = 0; i < subset_indices.size(); ++i)
        {
            mean += samples[subset_indices[i]];
        }

        mean /= (Real)subset_indices.size();
    }

    template<class VectorType>
    static size_t compute_weighted_median(const std::vector<VectorType>& samples,
                                          const Vec_Real&                                              weights,
                                          const Vec_UInt&                                              subset_indices)
    {
        __NOODLE_ASSERT(samples.size() == weights.size());

        Vec_Real dist2(subset_indices.size(), 0);

        for(size_t i = 0; i < subset_indices.size(); ++i)
        {
            const VectorType& si = samples[subset_indices[i]];
            const Real        wi = weights[subset_indices[i]];

            for(size_t j = 0; j < i; ++j)
            {
                const VectorType& sj = samples[subset_indices[j]];
                const Real        wj = weights[subset_indices[j]];
                Real              d2 = glm::length2(si - sj);

                dist2[i] += d2 * wj;
                dist2[j] += d2 * wi;
            }
        }

        Real   distMin = dist2[0];
        size_t idxMin = 0;

        for(size_t i = 1; i < dist2.size(); ++i)
        {
            if(dist2[i] < distMin)
            {
                distMin = dist2[i];
                idxMin = i;
            }
        }

        return idxMin;
    }

    template<class VectorType>
    static void compute_weighted_mean(const std::vector<VectorType>& samples,
                                      const Vec_Real&                                          weights,
                                      const Vec_UInt&                                          subset_indices,
                                      Vec3&                                                    mean)
    {
        mean = VectorType::Zero();
        __NOODLE_ASSERT(samples.size() == weights.size());

        Real sumW = 0;

        for(size_t i = 0; i < subset_indices.size(); ++i)
        {
            const VectorType& sp = samples[subset_indices[i]];
            const Real        w = weights[subset_indices[i]];
            mean += sp * w;
            sumW += w;
        }

        __NOODLE_ASSERT(sumW != 0);

        mean = mean / sumW;
    }


    template<class VectorType>
    Real check_vector_distance(const std::vector<VectorType>& cluster_centers)
    {
        Vec_Real dist2(cluster_centers.size(), 0);

        tbb::parallel_for(tbb::blocked_range<size_t>(0, cluster_centers.size()),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t p = r.begin(); p != r.end(); ++p)
            {
                const VectorType& ppos = cluster_centers[p];
                const Vec3i& pcellId = get_small_cellId(ppos);
                Real min_d2 = BIG_NUMBER;

                for(Int32 lk = -1; lk <= 1; ++lk)
                {
                    for(Int32 lj = -1; lj <= 1; ++lj)
                    {
                        for(Int32 li = -1; li <= 1; ++li)
                        {
                            const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                            if(!is_valid_small_cell(cellId))
                            {
                                continue;
                            }

                            for(const UInt32 cluster_id : cell3D_particles(cellId))
                            {
                                if(p == cluster_id)
                                {
                                    continue;
                                }

                                const Vec3& qpos = cluster_centers[cluster_id];
                                const Real d2 = glm::length2(ppos - qpos);

                                if(d2 < min_d2)
                                {
                                    min_d2 = d2;
                                }
                            }
                        }
                    } // end loop over neighbor cells
                }

                dist2[p] = min_d2;
            }
        });

        VectorMinElement<Real> md2(dist2);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

        return sqrt(md2.result);
    }


    template<class VectorType>
    Real check_min_distance(const std::vector<VectorType>& cluster_centers,
                            UInt32&                                        num_overlapped_particles)
    {
        Vec_Real dist2(cluster_centers.size(), 0);

        tbb::parallel_for(tbb::blocked_range<size_t>(0, cluster_centers.size()),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t p = r.begin(); p != r.end(); ++p)
            {
                const VectorType& ppos = cluster_centers[p];
                const Vec3i& pcellId = get_small_cellId(ppos);
                Real min_d2 = BIG_NUMBER;

                for(Int32 lk = -1; lk <= 1; ++lk)
                {
                    for(Int32 lj = -1; lj <= 1; ++lj)
                    {
                        for(Int32 li = -1; li <= 1; ++li)
                        {
                            const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                            if(!is_valid_small_cell(cellId))
                            {
                                continue;
                            }

                            for(const UInt32 cluster_id : cell3D_particles(cellId))
                            {
                                if(p == cluster_id)
                                {
                                    continue;
                                }

                                const Vec3& qpos = cluster_centers[cluster_id];
                                const Real d2 = glm::length2(ppos - qpos);

                                if(d2 < min_d2)
                                {
                                    min_d2 = d2;
                                }
                            }
                        }
                    } // end loop over neighbor cells
                }

                dist2[p] = min_d2;
            }
        });

        num_overlapped_particles = 0;
        const Real threshold = MathHelpers::sqr(overlap_ratio_threshold * particle_radius);

        for(size_t p = 0; p < dist2.size(); ++p)
        {
            if(dist2[p] < threshold)
            {
                ++num_overlapped_particles;
            }
        }

        VectorMinElement<Real> md2(dist2);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

        return sqrt(md2.result);
    }

    template<class VectorType>
    UInt32 remove_overlapped_particles(std::vector<VectorType>& cluster_centers)
    {
        Vec_Char   check_remove;
        check_remove.assign(cluster_centers.size(), 0);
        const Real threshold = MathHelpers::sqr(remove_ratio_threshold * particle_radius);

        tbb::parallel_for(tbb::blocked_range<size_t>(0, cluster_centers.size()),
                          [&](tbb::blocked_range<size_t> r)
        {
            for(size_t p = r.begin(); p != r.end(); ++p)
            {
                const VectorType& ppos = cluster_centers[p];
                const Vec3i& pcellId = get_small_cellId(ppos);
                Real min_d2 = BIG_NUMBER;

                for(Int32 lk = -1; lk <= 1; ++lk)
                {
                    for(Int32 lj = -1; lj <= 1; ++lj)
                    {
                        for(Int32 li = -1; li <= 1; ++li)
                        {
                            const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                            if(!is_valid_small_cell(cellId))
                            {
                                continue;
                            }

                            for(const UInt32 q : cell3D_particles(cellId))
                            {
                                if(p <= q)
                                {
                                    continue;
                                }

                                const Vec3& qpos = cluster_centers[q];
                                const Real d2 = glm::length2(ppos - qpos);

                                if(d2 < min_d2)
                                {
                                    min_d2 = d2;
                                }
                            }
                        }
                    }
                } // end loop over neighbor cells


                if(min_d2 < threshold)
                {
                    check_remove[p] = 1;
                }
            }
        });

        UInt32 num_removed = 0;

        for(size_t i = check_remove.size(); i-- > 0; )
        {
            if(check_remove[i] > 0)
            {
                ++num_removed;
                cluster_centers.erase(cluster_centers.begin() + i);
            }
        }

        return num_removed;
    }


    void sort_sample(Vec_Vec3& samples, const Vec_VecUInt& samples_per_cluster)
    {
        static Timer timer;
        size_t       num_samples = samples.size();

        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();
        static Vec_Vec3 tmp_samples;
        tmp_samples.resize(samples.size());

        size_t index = 0;

        for(size_t i = 0; i < samples_per_cluster.size(); ++i)
        {
            size_t cluster_samples = samples_per_cluster[i].size();

            for(size_t j = 0; j < cluster_samples; ++j)
            {
                tmp_samples[index] = samples[samples_per_cluster[i][j]];
                ++index;
            }
        }

        __NOODLE_ASSERT(index == num_samples);

        std::copy(tmp_samples.begin(), tmp_samples.end(), samples.begin());
        tmp_samples.clear();

        timer.tock();
        monitor.printLog(timer.getRunTime("Sort samples: ") + " =======================================");
    }


    void collect_clusters_to_cells(const Vec_Vec3& cluster_centers)
    {
        __NOODLE_CLEAR_VECVEC(cell3D_particles);

        // cannot run in parallel....
        for(UInt32 p = 0; p < cluster_centers.size(); ++p)
        {
            const Vec3&  pos = cluster_centers[p];
            const Vec3i& cellId = get_small_cellId(pos);

            cell3D_particles(cellId).push_back(p);
        }
    }

    Vec3i get_small_cellId(const Vec3& pos)
    {
        int i = (int)floor((pos[0] - domainParams->domainBMin[0]) / small_cell_size);
        int j = (int)floor((pos[1] - domainParams->domainBMin[1]) / small_cell_size);
        int k = (int)floor((pos[2] - domainParams->domainBMin[2]) / small_cell_size);

        return Vec3i(i, j, k);
    }

    bool is_valid_small_cell(const Vec3i& cellId)
    {
        for(int i = 0; i < 3; ++i)
        {
            if(cellId[i] < 0 || (UInt32)cellId[i] > num_small_cells[i] - 1)
            {
                return false;
            }
        }

        return true;
    }


    ////////////////////////////////////////////////////////////////////////////////
    DomainParameters* domainParams;
    Real              particle_radius;
    Array3_VecUInt    cell3D_particles;
    Vec3ui            num_small_cells;
    Real              small_cell_size;
    Vec_UInt          closest_cluster;

    Monitor monitor;
};

#endif // LLOYD_ALGORITHM_H