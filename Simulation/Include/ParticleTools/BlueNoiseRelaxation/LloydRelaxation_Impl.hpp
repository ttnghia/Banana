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
template<class VectorType, class ScalarType>
LloydRelaxation<VectorType, ScalarType>::LloydRelaxation(const VectorType& domainBMin, const VectorType& domainBMax, ScalarType particleRadius) :
    m_DomainBMin(m_DomainBMin), m_DomainBMax(m_DomainBMax), m_ParticleRadius(particleRadius),
    m_MovingThreshold(3.0),
    m_OverlapThreshold(1.55),
    m_RemovingThreshold(1.55),
    m_NumCheckIterations(1),
    m_Grid3D(domainBMin, domainBMax, particleRadius * 2.5),
    m_Logger(static_cast<int>(time(nullptr)), std::string("LloydRelaxation"))
{
    m_CellParticles.resize(m_Grid3D.getNumTotalCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::relaxParticles(std::vector<VectorType>& denseParticles,
                                                             std::vector<VectorType>& particles,
                                                             int                      minIterations /*= 10*/,
                                                             int                      maxIterations /*= 1000*/,
                                                             bool                     bUseCandidateCenters = false)
{
    __BNN_ASSERT(denseParticles.size() > 4 * particles.size());
    computeLloydClusters(denseParticles, particles, minIterations, maxIterations, bUseCandidateCenters);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::relaxParticlesWeighted(const std::vector<ScalarType>& weights,
                                                                     std::vector<VectorType>&       denseParticles,
                                                                     std::vector<VectorType>&       particles,
                                                                     int                            minIterations /*= 10*/,
                                                                     int                            maxIterations /*= 1000*/,
                                                                     bool                           bUseCandidateCenters = false)
{
    __BNN_ASSERT(denseParticles.size() > 4 * particles.size());
    computeWeightedLloydClusters(weights, denseParticles, particles, minIterations, maxIterations, bUseCandidateCenters);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::computeLloydClusters(std::vector<VectorType>& samples,
                                                                   std::vector<VectorType>& clusterCenters,
                                                                   int                      minIterations /*= 10*/,
                                                                   int                      maxIterations /*= 1000*/,
                                                                   bool                     bUseCandidateCenters = false)
{
    Timer      timer;
    ScalarType total_time;

    Vec_VecUInt             samples_per_cluster;
    std::vector<VectorType> cluster_backup;
    std::vector<ScalarType> move_distance;
    ScalarType              min_cluster_distance     = BIG_NUMBER;
    ScalarType              max_moved_percentage     = BIG_NUMBER;
    UInt32                  num_overlapped_particles = 1;

    bool converged = false;

    ////////////////////////////////////////////////////////////////////////////////

    cluster_backup.resize(clusterCenters.size());
    move_distance.resize(clusterCenters.size());

    for(int iter = 0; iter < maxIterations; ++iter)
    {
        m_Logger.printLog("Relaxation iteration: " + std::to_string(iter));
        size_t num_clusters = clusterCenters.size();

        timer.tick();
        collectSampleToCluster(clusterCenters, samples, samples_per_cluster);
        __BNN_ASSERT(samples_per_cluster.size() == num_clusters);
        total_time = timer.tock();
        m_Logger.printLog(timer.getRunTime("Collect samples to clusters total: "));

        ////////////////////////////////////////////////////////////////////////////////
        std::copy(clusterCenters.begin(), clusterCenters.end(), cluster_backup.begin());

        timer.tick();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, num_clusters),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              for(size_t cluster_id = r.begin(); cluster_id != r.end(); ++cluster_id)
                              {
                                  __BNN_ASSERT(samples_per_cluster[cluster_id].size() > 0);

                                  VectorType& center = clusterCenters[cluster_id];

                                  if(bUseCandidateCenters)
                                  {
                                      size_t medianIdx = compute_median(samples, samples_per_cluster[cluster_id]);
                                      center = samples[samples_per_cluster[cluster_id][medianIdx]];
                                  }
                                  else
                                  {
                                      computeMean(samples, samples_per_cluster[cluster_id], center);
                                  }

                                  move_distance[cluster_id] = glm::length2(center - cluster_backup[cluster_id]);
                              }
                          });     // end parallel for

        total_time += timer.tock();
        m_Logger.printLog(timer.getRunTime("Compute new cluster positions: "));

        VectorMaxElement<ScalarType> md2(move_distance);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, move_distance.size()), md2);
        max_moved_percentage = sqrt(md2.result) / m_ParticleRadius * 100.0;
        m_Logger.printLog("Max moving distance in this iteration: " + NumberHelpers::formatWithCommas(max_moved_percentage) + "% of particle radius");

        if(iter % m_NumCheckIterations == 0)
        {
            min_cluster_distance = check_min_distance(clusterCenters, num_overlapped_particles);
            m_Logger.newLine();
            m_Logger.printLog("Min distance: " + NumberHelpers::formatToScientific(
                                  min_cluster_distance) +
                              ". Min distance/particle_radius = " +
                              NumberHelpers::formatWithCommas(min_cluster_distance / m_ParticleRadius));
            m_Logger.printLog("Number of overlapped particles: " +
                              NumberHelpers::formatWithCommas(num_overlapped_particles));
            m_Logger.newLine();
        }

        if((iter + 1) % 10 == 0 && max_moved_percentage > 10)
        {
            sort_sample(samples, samples_per_cluster);
        }

        m_Logger.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(
                              total_time) + "ms");
        m_Logger.printLog("Memory usage: " + NumberHelpers::formatWithCommas(
                              getCurrentRSS() / 1048576.0) +
                          " MB(s). Peak: " + NumberHelpers::formatWithCommas(getPeakRSS() / 1048576.0) + " MB(s).");
        m_Logger.newLine();

        if(iter > minIterations && max_moved_percentage < m_MovingThreshold)
        {
            if(num_overlapped_particles == 0)
            {
                converged = true;
                break;
            }

            if(num_overlapped_particles > 0)
            {
                UInt32 num_removed_particles = remove_overlapped_particles(clusterCenters);
                m_Logger.printLog("Removed particles: " +
                                  NumberHelpers::formatWithCommas(num_removed_particles) +
                                  " =======================================");
                m_Logger.newLine();

                if(num_removed_particles == 0)
                {
                    // if cannot remove any particles, exit...
                    break;
                }
            }
        }
    }     // end iterations


    min_cluster_distance = check_min_distance(clusterCenters, num_overlapped_particles);
    m_Logger.printLog("Finished relaxation. Min distance: " +
                      NumberHelpers::formatToScientific(min_cluster_distance) +
                      ". Min distance/particle_radius = " +
                      NumberHelpers::formatWithCommas(min_cluster_distance / m_ParticleRadius));
    m_Logger.printLog("Max moving distance in last iteration: " +
                      NumberHelpers::formatWithCommas(max_moved_percentage) +
                      "% of particle radius");
    m_Logger.printLog("Number of overlapped particles: " +
                      NumberHelpers::formatWithCommas(num_overlapped_particles));

    if(converged)
    {
        m_Logger.printLog("Relaxation converged.");
    }
    else
    {
        m_Logger.printLog("Relaxation did NOT converge.");
    }

    m_Logger.newLine();
    __BNN_ASSERT(min_cluster_distance > m_OverlapThreshold * m_ParticleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::computeWeightedLloydClusters(const std::vector<ScalarType>& weights,
                                                                           std::vector<VectorType>&       samples,
                                                                           std::vector<VectorType>&       clusterCenters,
                                                                           int                            minIterations = 10,
                                                                           int                            maxIterations = 100,
                                                                           bool                           bUseCandidateCenters = false)
{
    __BNN_ASSERT(samples.size() == weights.size());

    Timer      timer;
    ScalarType total_time;

    Vec_VecUInt             samples_per_cluster;
    std::vector<VectorType> cluster_backup;
    std::vector<ScalarType> move_distance;
    size_t                  num_clusters         = clusterCenters.size();
    ScalarType              min_cluster_distance = BIG_NUMBER;
    ScalarType              max_moved_percentage = BIG_NUMBER;
    UInt32                  num_overlapped_particles;

    bool converged = false;

    ////////////////////////////////////////////////////////////////////////////////
    cluster_backup.resize(clusterCenters.size());
    move_distance.resize(clusterCenters.size());

    for(int iter = 0; iter < maxIterations; ++iter)
    {
        m_Logger.printLog("Relaxation iteration: " + std::to_string(iter));

        timer.tick();
        collectSampleToCluster(clusterCenters, samples, samples_per_cluster);
        __BNN_ASSERT(samples_per_cluster.size() == num_clusters);
        total_time = timer.tock();
        m_Logger.printLog(timer.getRunTime("Collect samples to clusters total: "));

        ////////////////////////////////////////////////////////////////////////////////
        std::copy(clusterCenters.begin(), clusterCenters.end(), cluster_backup.begin());

        timer.tick();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, num_clusters),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              for(size_t cluster_id = r.begin(); cluster_id != r.end(); ++cluster_id)
                              {
                                  __BNN_ASSERT(samples_per_cluster[cluster_id].size() > 0);
                                  VectorType& center = clusterCenters[cluster_id];

                                  if(bUseCandidateCenters)
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
                          });     // end parallel for

        total_time += timer.tock();
        m_Logger.printLog(timer.getRunTime("Compute new cluster positions: "));

        VectorMaxElement<ScalarType> md2(move_distance);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, move_distance.size()), md2);
        max_moved_percentage = sqrt(md2.result) / m_ParticleRadius * 100.0;
        m_Logger.printLog("Max moving distance in this iteration: " +
                          NumberHelpers::formatWithCommas(max_moved_percentage) +
                          "% of particle radius");

        if(iter % m_NumCheckIterations == 0)
        {
            min_cluster_distance = check_min_distance(clusterCenters, num_overlapped_particles);
            m_Logger.newLine();
            m_Logger.printLog("Min distance: " + NumberHelpers::formatToScientific(
                                  min_cluster_distance) +
                              ". Min distance/particle_radius = " +
                              NumberHelpers::formatWithCommas(min_cluster_distance / m_ParticleRadius));
            m_Logger.printLog("Number of overlapped particles: " +
                              NumberHelpers::formatWithCommas(num_overlapped_particles));
            m_Logger.newLine();
        }

        if((iter + 1) % 10 == 0 && max_moved_percentage > 10)
        {
            sort_sample(samples, samples_per_cluster);
        }

        m_Logger.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(
                              total_time) + "ms");
        m_Logger.printLog("Memory usage: " + NumberHelpers::formatWithCommas(
                              getCurrentRSS() / 1048576.0) +
                          " MB(s). Peak: " + NumberHelpers::formatWithCommas(getPeakRSS() / 1048576.0) + " MB(s).");
        m_Logger.newLine();

        if(max_moved_percentage < m_MovingThreshold && iter > minIterations)
        {
            converged = true;
            break;
        }
    }     // end iterations

    min_cluster_distance = check_min_distance(clusterCenters, num_overlapped_particles);
    m_Logger.printLog("Finished relaxation. Min distance: " +
                      NumberHelpers::formatWithCommas(min_cluster_distance) +
                      ". Min distance/particle_radius = " +
                      NumberHelpers::formatWithCommas(min_cluster_distance / m_ParticleRadius));
    m_Logger.printLog("Max moving distance in last iteration: " +
                      NumberHelpers::formatWithCommas(max_moved_percentage) +
                      "% of particle radius");
    m_Logger.printLog("Number of overlapped particles: " +
                      NumberHelpers::formatWithCommas(num_overlapped_particles));

    if(converged)
    {
        m_Logger.printLog("Relaxation converged.");
    }
    else
    {
        m_Logger.printLog("Relaxation did NOT converge.");
    }

    m_Logger.newLine();
    __BNN_ASSERT(min_cluster_distance > m_OverlapThreshold * m_ParticleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///	Clusters a set of samples by assigning each sample to its closest cluster.
///	\c clusterCenter contains the centers of the cluster.
///	On return, \c samplesPerCluster will contain for each cluster the indices of the samples in the cluster.
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::collectSampleToCluster(const std::vector<VectorType>& clusterCenters, const std::vector<VectorType>& samples, Vec_VecUInt& samplesPerCluster)
{
    static Timer timer;
    size_t       num_clusters = clusterCenters.size();
    size_t       num_samples  = samples.size();

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    collect_clusters_to_cells(clusterCenters);
    timer.tock();
    m_Logger.printLogIndent(timer.getRunTime("Collect clusters to cells: "));

    m_ClosestCluster.resize(num_samples);
    samplesPerCluster.resize(num_clusters);
    __NOODLE_CLEAR_VECVEC(samplesPerCluster);


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
                              ScalarType closest_cluster_d2 = BIG_NUMBER;
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

                                          for(const UInt32 cluster_id : m_CellParticles(cellId))
                                          {
                                              const Vec3& cp = clusterCenters[cluster_id];
                                              const ScalarType d2 = glm::length2(sp - cp);

                                              if(d2 < closest_cluster_d2)
                                              {
                                                  has_close_cluster = true;
                                                  closest_cluster_d2 = d2;
                                                  closest_cluster_id = cluster_id;
                                              }
                                          }
                                      }
                                  }
                              }     // end loop over neighbor cells

                              __BNN_ASSERT(has_close_cluster);
                              m_ClosestCluster[sample_id] = closest_cluster_id;
                          }
                      });     // end parallel for
    timer.tock();
    m_Logger.printLogIndent(timer.getRunTime("Find nearest cluster for each sample: "));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();

    for(size_t sample_id = 0; sample_id < num_samples; ++sample_id)
    {
        samplesPerCluster[m_ClosestCluster[sample_id]].push_back(sample_id);
    }

    timer.tock();
    m_Logger.printLogIndent(timer.getRunTime("Write nearest cluster to sample array: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
////////////////////////////////////////////////////////////////////////////////
// static functions

template<class VectorType, class ScalarType>
static size_t compute_median(const std::vector<VectorType>& samples, const Vec_UInt& subsetIndices)
{
    std::vector<ScalarType> dist2(subsetIndices.size(), 0);

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        const VectorType& si = samples[subsetIndices[i]];

        for(size_t j = 0; j < i; ++j)
        {
            const VectorType& sj = samples[subsetIndices[j]];
            const ScalarType  d2 = glm::length2(si - sj);
            dist2[i] += d2;
            dist2[j] += d2;
        }
    }

    ScalarType distMin = dist2[0];
    size_t     idxMin  = 0;

    for(size_t i = 1; i < dist2.size(); ++i)
    {
        if(dist2[i] < distMin)
        {
            distMin = dist2[i];
            idxMin  = i;
        }
    }

    return idxMin;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
static void LloydRelaxation<VectorType, ScalarType>::computeMean(const std::vector<VectorType>& samples,
                                                                 const Vec_UInt&                subsetIndices,
                                                                 VectorType&                    mean)
{
    mean = VectorType(0);

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        mean += samples[subsetIndices[i]];
    }

    mean /= (ScalarType)subsetIndices.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
static size_t compute_weighted_median(const std::vector<VectorType>& samples,
                                      const std::vector<ScalarType>& weights,
                                      const Vec_UInt&                subsetIndices)
{
    __BNN_ASSERT(samples.size() == weights.size());

    std::vector<ScalarType> dist2(subsetIndices.size(), 0);

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        const VectorType& si = samples[subsetIndices[i]];
        const ScalarType  wi = weights[subsetIndices[i]];

        for(size_t j = 0; j < i; ++j)
        {
            const VectorType& sj = samples[subsetIndices[j]];
            const ScalarType  wj = weights[subsetIndices[j]];
            ScalarType        d2 = glm::length2(si - sj);

            dist2[i] += d2 * wj;
            dist2[j] += d2 * wi;
        }
    }

    ScalarType distMin = dist2[0];
    size_t     idxMin  = 0;

    for(size_t i = 1; i < dist2.size(); ++i)
    {
        if(dist2[i] < distMin)
        {
            distMin = dist2[i];
            idxMin  = i;
        }
    }

    return idxMin;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
static void LloydRelaxation<VectorType, ScalarType>::compute_weighted_mean(const std::vector<VectorType>& samples,
                                                                           const std::vector<ScalarType>& weights,
                                                                           const Vec_UInt&                subsetIndices,
                                                                           Vec3&                          mean)
{
    mean = VectorType::Zero();
    __BNN_ASSERT(samples.size() == weights.size());

    ScalarType sumW = 0;

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        const VectorType& sp = samples[subsetIndices[i]];
        const ScalarType  w  = weights[subsetIndices[i]];
        mean += sp * w;
        sumW += w;
    }

    __BNN_ASSERT(sumW != 0);

    mean = mean / sumW;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
ScalarType check_vector_distance(const std::vector<VectorType>& clusterCenters)
{
    std::vector<ScalarType> dist2(clusterCenters.size(), 0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const VectorType& ppos = clusterCenters[p];
                              const Vec3i& pcellId = get_small_cellId(ppos);
                              ScalarType min_d2 = BIG_NUMBER;

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

                                          for(const UInt32 cluster_id : m_CellParticles(cellId))
                                          {
                                              if(p == cluster_id)
                                              {
                                                  continue;
                                              }

                                              const Vec3& qpos = clusterCenters[cluster_id];
                                              const ScalarType d2 = glm::length2(ppos - qpos);

                                              if(d2 < min_d2)
                                              {
                                                  min_d2 = d2;
                                              }
                                          }
                                      }
                                  }     // end loop over neighbor cells
                              }

                              dist2[p] = min_d2;
                          }
                      });

    VectorMinElement<ScalarType> md2(dist2);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

    return sqrt(md2.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
ScalarType check_min_distance(const std::vector<VectorType>& clusterCenters,
                              UInt32&                        num_overlapped_particles)
{
    std::vector<ScalarType> dist2(clusterCenters.size(), 0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const VectorType& ppos = clusterCenters[p];
                              const Vec3i& pcellId = get_small_cellId(ppos);
                              ScalarType min_d2 = BIG_NUMBER;

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

                                          for(const UInt32 cluster_id : m_CellParticles(cellId))
                                          {
                                              if(p == cluster_id)
                                              {
                                                  continue;
                                              }

                                              const Vec3& qpos = clusterCenters[cluster_id];
                                              const ScalarType d2 = glm::length2(ppos - qpos);

                                              if(d2 < min_d2)
                                              {
                                                  min_d2 = d2;
                                              }
                                          }
                                      }
                                  }     // end loop over neighbor cells
                              }

                              dist2[p] = min_d2;
                          }
                      });

    num_overlapped_particles = 0;
    const ScalarType threshold = MathHelpers::sqr(m_OverlapThreshold * m_ParticleRadius);

    for(size_t p = 0; p < dist2.size(); ++p)
    {
        if(dist2[p] < threshold)
        {
            ++num_overlapped_particles;
        }
    }

    VectorMinElement<ScalarType> md2(dist2);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

    return sqrt(md2.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
UInt32 remove_overlapped_particles(std::vector<VectorType>& clusterCenters)
{
    Vec_Char check_remove;
    check_remove.assign(clusterCenters.size(), 0);
    const ScalarType threshold = MathHelpers::sqr(m_RemovingThreshold * m_ParticleRadius);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const VectorType& ppos = clusterCenters[p];
                              const Vec3i& pcellId = get_small_cellId(ppos);
                              ScalarType min_d2 = BIG_NUMBER;

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

                                          for(const UInt32 q : m_CellParticles(cellId))
                                          {
                                              if(p <= q)
                                              {
                                                  continue;
                                              }

                                              const Vec3& qpos = clusterCenters[q];
                                              const ScalarType d2 = glm::length2(ppos - qpos);

                                              if(d2 < min_d2)
                                              {
                                                  min_d2 = d2;
                                              }
                                          }
                                      }
                                  }
                              }     // end loop over neighbor cells


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
            clusterCenters.erase(clusterCenters.begin() + i);
        }
    }

    return num_removed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::sort_sample(std::vector<VectorType>& samples, const Vec_VecUInt& samples_per_cluster)
{
    static Timer timer;
    size_t       num_samples = samples.size();

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    static std::vector<VectorType> tmp_samples;
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

    __BNN_ASSERT(index == num_samples);

    std::copy(tmp_samples.begin(), tmp_samples.end(), samples.begin());
    tmp_samples.clear();

    timer.tock();
    m_Logger.printLog(timer.getRunTime("Sort samples: ") + " =======================================");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void LloydRelaxation<VectorType, ScalarType>::collect_clusters_to_cells(const std::vector<VectorType>& clusterCenters)
{
    __NOODLE_CLEAR_VECVEC(m_CellParticles);

    // cannot run in parallel....
    for(UInt32 p = 0; p < clusterCenters.size(); ++p)
    {
        const Vec3&  pos    = clusterCenters[p];
        const Vec3i& cellId = get_small_cellId(pos);

        m_CellParticles(cellId).push_back(p);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
