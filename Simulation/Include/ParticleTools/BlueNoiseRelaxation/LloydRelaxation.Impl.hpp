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
template<class RealType>
LloydRelaxation<RealType>::LloydRelaxation(const Vec3<RealType>& domainBMin, const Vec3<RealType>& domainBMax, RealType particleRadius) :
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
template<class RealType>
void LloydRelaxation<RealType>::relaxParticles(std::vector<Vec3<RealType> >& denseParticles,
                                                 std::vector<Vec3<RealType> >& particles,
                                                 int                             minIterations /*= 10*/,
                                                 int                             maxIterations /*= 1000*/,
                                                 bool                            bUseCandidateCenters /*= false*/)
{
    __BNN_ASSERT(denseParticles.size() > 4 * particles.size());
    computeLloydClusters(denseParticles, particles, minIterations, maxIterations, bUseCandidateCenters);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void LloydRelaxation<RealType>::relaxParticlesWeighted(const std::vector<RealType>&  weights,
                                                         std::vector<Vec3<RealType> >& denseParticles,
                                                         std::vector<Vec3<RealType> >& particles,
                                                         int                             minIterations /*= 10*/,
                                                         int                             maxIterations /*= 1000*/,
                                                         bool                            bUseCandidateCenters /*= false*/)
{
    __BNN_ASSERT(denseParticles.size() > 4 * particles.size());
    computeWeightedLloydClusters(weights, denseParticles, particles, minIterations, maxIterations, bUseCandidateCenters);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void LloydRelaxation<RealType>::computeLloydClusters(std::vector<Vec3<RealType> >& samples,
                                                       std::vector<Vec3<RealType> >& clusterCenters,
                                                       int                             minIterations /*= 10*/,
                                                       int                             maxIterations /*= 1000*/,
                                                       bool                            bUseCandidateCenters /*= false*/)
{
    Vec_VecUInt                    samplesInCluster;
    std::vector<Vec3<RealType> > clusterBackup;
    std::vector<RealType>        movingDistance;

    RealType totalTime           = 0;
    UInt32     numOverlapped       = 0;
    bool       converged           = false;
    RealType minClusterDistance  = std::numeric_limits<RealType>::max();
    RealType maxMovingPercentage = std::numeric_limits<RealType>::max();

    ////////////////////////////////////////////////////////////////////////////////

    clusterBackup.resize(clusterCenters.size());
    movingDistance.resize(clusterCenters.size());

    for(int iter = 0; iter < maxIterations; ++iter)
    {
        m_Logger.printLog("Relaxation iteration: " + std::to_string(iter));
        size_t numClusters = clusterCenters.size();

        m_Timer.tick();
        collectSampleToCluster(clusterCenters, samples, samplesInCluster);
        __BNN_ASSERT(samplesInCluster.size() == numClusters);
        totalTime = m_Timer.tock();
        m_Logger.printLog(m_Timer.getRunTime("Collect samples to clusters total: "));

        ////////////////////////////////////////////////////////////////////////////////
        std::copy(clusterCenters.begin(), clusterCenters.end(), clusterBackup.begin());

        m_Timer.tick();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, numClusters),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              for(size_t clusterIdx = r.begin(); clusterIdx != r.end(); ++clusterIdx)
                              {
                                  __BNN_ASSERT(samplesInCluster[clusterIdx].size() > 0);

                                  Vec3<RealType>& center = clusterCenters[clusterIdx];

                                  if(bUseCandidateCenters)
                                  {
                                      size_t medianIdx = computeMedian(samples, samplesInCluster[clusterIdx]);
                                      center = samples[samplesInCluster[clusterIdx][medianIdx]];
                                  }
                                  else
                                  {
                                      computeMean(samples, samplesInCluster[clusterIdx], center);
                                  }

                                  movingDistance[clusterIdx] = glm::length2(center - clusterBackup[clusterIdx]);
                              }
                          });     // end parallel for

        totalTime += m_Timer.tock();
        m_Logger.printLog(m_Timer.getRunTime("Compute new cluster positions: "));

        ////////////////////////////////////////////////////////////////////////////////
        VectorMaxElement<RealType> md2(movingDistance);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, movingDistance.size()), md2);
        maxMovingPercentage = sqrt(md2.result) / m_ParticleRadius * 100.0;
        m_Logger.printLog("Max moving distance in this iteration: " + NumberHelpers::formatWithCommas(maxMovingPercentage) + "% of particle radius");

        ////////////////////////////////////////////////////////////////////////////////
        if((iter % m_NumCheckIterations) == 0)
        {
            minClusterDistance = computeMinDistance(clusterCenters, numOverlapped);
            m_Logger.newLine();
            m_Logger.printLog("Min distance: " + NumberHelpers::formatToScientific(minClusterDistance) +
                              ". Min distance/particle_radius = " + NumberHelpers::formatWithCommas(minClusterDistance / m_ParticleRadius));
            m_Logger.printLog("Number of overlapped particles: " + NumberHelpers::formatWithCommas(numOverlapped));
            m_Logger.newLine();
        }

        if((iter + 1) % 10 == 0 && maxMovingPercentage > 10)
        {
            sortSamples(samples, samplesInCluster);
        }

        m_Logger.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(totalTime) + "ms");
        m_Logger.printMemoryUsage();
        m_Logger.newLine();

        if(iter > minIterations && maxMovingPercentage < m_MovingThreshold)
        {
            if(numOverlapped == 0)
            {
                converged = true;
                break;
            }

            if(numOverlapped > 0)
            {
                UInt32 numRemoved = removeOverlappedParticles(clusterCenters);
                m_Logger.printLog("Removed particles: " + NumberHelpers::formatWithCommas(numRemoved) + " =======================================");
                m_Logger.newLine();

                if(numRemoved == 0)
                {
                    // if cannot remove any particles, exit...
                    break;
                }
            }
        }
    }     // end iterations


    minClusterDistance = computeMinDistance(clusterCenters, numOverlapped);
    m_Logger.printLog("Finished relaxation. Min distance: " + NumberHelpers::formatToScientific(minClusterDistance) +
                      ". Min distance/particle_radius = " + NumberHelpers::formatWithCommas(minClusterDistance / m_ParticleRadius));
    m_Logger.printLog("Max moving distance in last iteration: " + NumberHelpers::formatWithCommas(maxMovingPercentage) + "% of particle radius");
    m_Logger.printLog("Number of overlapped particles: " + NumberHelpers::formatWithCommas(numOverlapped));

    if(converged)
    {
        m_Logger.printLog("Relaxation converged.");
    }
    else
    {
        m_Logger.printLog("Relaxation did NOT converge.");
    }

    m_Logger.newLine();
    __BNN_ASSERT(minClusterDistance > m_OverlapThreshold * m_ParticleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void LloydRelaxation<RealType>::computeWeightedLloydClusters(const std::vector<RealType>&  weights,
                                                               std::vector<Vec3<RealType> >& samples,
                                                               std::vector<Vec3<RealType> >& clusterCenters,
                                                               int                             minIterations /*= 10*/,
                                                               int                             maxIterations /*= 100*/,
                                                               bool                            bUseCandidateCenters /*= false*/)
{
    __BNN_ASSERT(samples.size() == weights.size());


    Vec_VecUInt                    samplesInCluster;
    std::vector<Vec3<RealType> > clusterBackup;
    std::vector<RealType>        movingDistance;
    size_t                         numClusters         = clusterCenters.size();
    RealType                     minClusterDistance  = std::numeric_limits<RealType>::max();
    RealType                     maxMovingPercentage = std::numeric_limits<RealType>::max();

    UInt32     numOverlapped = 0;
    RealType totalTime     = 0;

    bool converged = false;

    ////////////////////////////////////////////////////////////////////////////////
    clusterBackup.resize(clusterCenters.size());
    movingDistance.resize(clusterCenters.size());

    for(int iter = 0; iter < maxIterations; ++iter)
    {
        m_Logger.printLog("Relaxation iteration: " + std::to_string(iter));

        m_Timer.tick();
        collectSampleToCluster(clusterCenters, samples, samplesInCluster);
        __BNN_ASSERT(samplesInCluster.size() == numClusters);
        totalTime = m_Timer.tock();
        m_Logger.printLog(m_Timer.getRunTime("Collect samples to clusters total: "));

        ////////////////////////////////////////////////////////////////////////////////
        std::copy(clusterCenters.begin(), clusterCenters.end(), clusterBackup.begin());

        m_Timer.tick();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, numClusters),
                          [&](tbb::blocked_range<size_t> r)
                          {
                              for(size_t clusterIdx = r.begin(); clusterIdx != r.end(); ++clusterIdx)
                              {
                                  __BNN_ASSERT(samplesInCluster[clusterIdx].size() > 0);
                                  Vec3<RealType>& center = clusterCenters[clusterIdx];

                                  if(bUseCandidateCenters)
                                  {
                                      int medianIdx = computeWeightedMedian(samples, weights, samplesInCluster[clusterIdx]);
                                      center = samples[samplesInCluster[clusterIdx][medianIdx]];
                                  }
                                  else
                                  {
                                      computeWeightedMean(samples, weights, samplesInCluster[clusterIdx], center);
                                  }

                                  movingDistance[clusterIdx] = glm::length2(center - clusterBackup[clusterIdx]);
                              }
                          });     // end parallel for

        totalTime += m_Timer.tock();
        m_Logger.printLog(m_Timer.getRunTime("Compute new cluster positions: "));

        VectorMaxElement<RealType> md2(movingDistance);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, movingDistance.size()), md2);
        maxMovingPercentage = sqrt(md2.result) / m_ParticleRadius * 100.0;
        m_Logger.printLog("Max moving distance in this iteration: " + NumberHelpers::formatWithCommas(maxMovingPercentage) + "% of particle radius");

        if(iter % m_NumCheckIterations == 0)
        {
            minClusterDistance = computeMinDistance(clusterCenters, numOverlapped);
            m_Logger.newLine();
            m_Logger.printLog("Min distance: " + NumberHelpers::formatToScientific(minClusterDistance) +
                              ". Min distance/particle_radius = " + NumberHelpers::formatWithCommas(minClusterDistance / m_ParticleRadius));
            m_Logger.printLog("Number of overlapped particles: " + NumberHelpers::formatWithCommas(numOverlapped));
            m_Logger.newLine();
        }

        if((iter + 1) % 10 == 0 && maxMovingPercentage > 10)
        {
            sortSamples(samples, samplesInCluster);
        }

        m_Logger.printLog("Total iteration time: " + NumberHelpers::formatWithCommas(totalTime) + "ms");
        m_Logger.printMemoryUsage();
        m_Logger.newLine();

        if(maxMovingPercentage < m_MovingThreshold && iter > minIterations)
        {
            converged = true;
            break;
        }
    }     // end iterations

    minClusterDistance = computeMinDistance(clusterCenters, numOverlapped);
    m_Logger.printLog("Finished relaxation. Min distance: " + NumberHelpers::formatWithCommas(minClusterDistance) +
                      ". Min distance/particle_radius = " + NumberHelpers::formatWithCommas(minClusterDistance / m_ParticleRadius));
    m_Logger.printLog("Max moving distance in last iteration: " + NumberHelpers::formatWithCommas(maxMovingPercentage) + "% of particle radius");
    m_Logger.printLog("Number of overlapped particles: " + NumberHelpers::formatWithCommas(numOverlapped));

    if(converged)
    {
        m_Logger.printLog("Relaxation converged.");
    }
    else
    {
        m_Logger.printLog("Relaxation did NOT converge.");
    }

    m_Logger.newLine();
    __BNN_ASSERT(minClusterDistance > m_OverlapThreshold * m_ParticleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///	Clusters a set of samples by assigning each sample to its closest cluster.
///	\c clusterCenter contains the centers of the cluster.
///	On return, \c samplesInCluster will contain for each cluster the indices of the samples in the cluster.
template<class RealType>
void LloydRelaxation<RealType>::collectSampleToCluster(const std::vector<Vec3<RealType> >& clusterCenters, const std::vector<Vec3<RealType> >& samples, Vec_VecUInt& samplesInCluster)
{
    size_t numClusters = clusterCenters.size();
    size_t numSamples  = samples.size();

    ////////////////////////////////////////////////////////////////////////////////
    m_Timer.tick();
    collectClustersToCells(clusterCenters);
    m_Timer.tock();
    m_Logger.printLogIndent(m_Timer.getRunTime("Collect clusters to cells: "));

    m_ClosestCluster.resize(numSamples);
    samplesInCluster.resize(numClusters);

    for(auto& vec : samplesInCluster)
        vec.resize(0);


    ////////////////////////////////////////////////////////////////////////////////
    m_Timer.tick();
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numSamples),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t sampleIdx = r.begin(); sampleIdx != r.end(); ++sampleIdx)
                          {
                              const Vec3<RealType>& sp = samples[sampleIdx];
                              const Vec3i cellIdx = m_Grid3D.getCellIdx(sp);

                              bool bHasNeighbor = false;
                              UInt32 closestClusterIdx = 0;
                              RealType closestClusterD2 = std::numeric_limits<RealType>::max();


                              for(Int32 lk : {-1, 0, 1})
                              {
                                  for(Int32 lj : {-1, 0, 1})
                                  {
                                      for(Int32 li : {-1, 0, 1})
                                      {
                                          const Vec3i cellId = cellIdx + Vec3i(li, lj, lk);
                                          if(!m_Grid3D.isValidCell(cellId)) continue;

                                          for(const UInt32 clusterIdx : m_CellParticles(cellId))
                                          {
                                              const Vec3<RealType>& cp = clusterCenters[clusterIdx];
                                              const RealType d2 = glm::length2(sp - cp);

                                              if(d2 < closestClusterD2)
                                              {
                                                  bHasNeighbor = true;
                                                  closestClusterD2 = d2;
                                                  closestClusterIdx = clusterIdx;
                                              }
                                          }
                                      }
                                  }
                              }     // end loop over neighbor cells

                              __BNN_ASSERT(bHasNeighbor);
                              m_ClosestCluster[sampleIdx] = closestClusterIdx;
                          }
                      });     // end parallel for
    m_Timer.tock();
    m_Logger.printLogIndent(m_Timer.getRunTime("Find nearest cluster for each sample: "));

    ////////////////////////////////////////////////////////////////////////////////
    m_Timer.tick();

    for(size_t sample_id = 0; sample_id < numSamples; ++sample_id)
    {
        samplesInCluster[m_ClosestCluster[sample_id]].push_back(sample_id);
    }

    m_Timer.tock();
    m_Logger.printLogIndent(m_Timer.getRunTime("Write nearest cluster to sample array: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
////////////////////////////////////////////////////////////////////////////////
// static functions

template<class RealType>
size_t LloydRelaxation<RealType>::computeMedian(const std::vector<Vec3<RealType> >& samples, const Vec_UInt& subsetIndices)
{
    std::vector<RealType> dist2(subsetIndices.size(), 0);

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        const Vec3<RealType>& si = samples[subsetIndices[i]];

        for(size_t j = 0; j < i; ++j)
        {
            const Vec3<RealType>& sj = samples[subsetIndices[j]];
            const RealType        d2 = glm::length2(si - sj);
            dist2[i] += d2;
            dist2[j] += d2;
        }
    }

    RealType distMin = dist2[0];
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
template<class RealType>
size_t LloydRelaxation<RealType>::computeWeightedMedian(const std::vector<Vec3<RealType> >& samples, const std::vector<RealType>& weights, const Vec_UInt& subsetIndices)
{
    __BNN_ASSERT(samples.size() == weights.size());

    std::vector<RealType> dist2(subsetIndices.size(), 0);

    for(size_t i = 0; i < subsetIndices.size(); ++i)
    {
        const Vec3<RealType>& si = samples[subsetIndices[i]];
        const RealType        wi = weights[subsetIndices[i]];

        for(size_t j = 0; j < i; ++j)
        {
            const Vec3<RealType>& sj = samples[subsetIndices[j]];
            const RealType        wj = weights[subsetIndices[j]];
            RealType              d2 = glm::length2(si - sj);

            dist2[i] += d2 * wj;
            dist2[j] += d2 * wi;
        }
    }

    RealType distMin = dist2[0];
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
template<class RealType>
void LloydRelaxation<RealType>::computeMean(const std::vector<Vec3<RealType> >& samples, const Vec_UInt& subsetIndices, Vec3<RealType>& mean)
{
    mean = Vec3<RealType>(0);

    for(auto index : subsetIndices)
    {
        mean += samples[index];
    }

    mean /= (RealType)subsetIndices.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void LloydRelaxation<RealType>::computeWeightedMean(const std::vector<Vec3<RealType> >& samples, const std::vector<RealType>& weights, const Vec_UInt& subsetIndices, Vec3<RealType>& mean)
{
    mean = Vec3<RealType>(0);
    __BNN_ASSERT(samples.size() == weights.size());

    RealType sumW = 0;

    for(auto index : subsetIndices)
    {
        const Vec3<RealType>& sp = samples[index];
        const RealType        w  = weights[index];
        mean += sp * w;
        sumW += w;
    }

    __BNN_ASSERT(sumW != 0);

    mean = mean / sumW;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType LloydRelaxation<RealType>::computeMinDistance(const std::vector<Vec3<RealType> >& clusterCenters)
{
    std::vector<RealType> dist2(clusterCenters.size(), 0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3<RealType>& ppos = clusterCenters[p];
                              const Vec3i& pcellId = m_Grid3D.getCellIdx(ppos);
                              RealType min_d2 = std::numeric_limits<RealType>::max();

                              for(Int32 lk = -1; lk <= 1; ++lk)
                              {
                                  for(Int32 lj = -1; lj <= 1; ++lj)
                                  {
                                      for(Int32 li = -1; li <= 1; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Grid3D.isValidCell(cellId)) continue;

                                          for(auto clusterIdx : m_CellParticles(cellId))
                                          {
                                              if(p == clusterIdx) continue;

                                              const auto& qpos = clusterCenters[clusterIdx];
                                              const RealType d2 = glm::length2(ppos - qpos);

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

    VectorMinElement<RealType> md2(dist2);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

    return sqrt(md2.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType LloydRelaxation<RealType>::computeMinDistance(const std::vector<Vec3<RealType> >& clusterCenters, UInt32& numOverlappedParticles)
{
    std::vector<RealType> dist2(clusterCenters.size(), 0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3<RealType>& ppos = clusterCenters[p];
                              const Vec3i& pcellId = m_Grid3D.getCellIdx(ppos);
                              RealType min_d2 = std::numeric_limits<RealType>::max();

                              for(Int32 lk = -1; lk <= 1; ++lk)
                              {
                                  for(Int32 lj = -1; lj <= 1; ++lj)
                                  {
                                      for(Int32 li = -1; li <= 1; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Grid3D.isValidCell(cellId)) continue;

                                          for(const UInt32 clusterIdx : m_CellParticles(cellId))
                                          {
                                              if(p == clusterIdx) continue;

                                              const Vec3<RealType>& qpos = clusterCenters[clusterIdx];
                                              const RealType d2 = glm::length2(ppos - qpos);

                                              if(d2 < min_d2) min_d2 = d2;
                                          }
                                      }
                                  }     // end loop over neighbor cells
                              }

                              dist2[p] = min_d2;
                          }
                      });

    numOverlappedParticles = 0;
    const RealType threshold = MathHelpers::sqr(m_OverlapThreshold * m_ParticleRadius);

    for(size_t p = 0; p < dist2.size(); ++p)
    {
        if(dist2[p] < threshold)
        {
            ++numOverlappedParticles;
        }
    }

    VectorMinElement<RealType> md2(dist2);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dist2.size()), md2);

    return sqrt(md2.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
UInt32 LloydRelaxation<RealType>::removeOverlappedParticles(std::vector<Vec3<RealType> >& clusterCenters)
{
    Vec_Char check_remove;
    check_remove.assign(clusterCenters.size(), 0);
    const RealType threshold = MathHelpers::sqr(m_RemovingThreshold * m_ParticleRadius);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, clusterCenters.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3<RealType>& ppos = clusterCenters[p];
                              const Vec3i& pcellId = m_Grid3D.getCellIdx(ppos);
                              RealType min_d2 = std::numeric_limits<RealType>::max();

                              for(Int32 lk = -1; lk <= 1; ++lk)
                              {
                                  for(Int32 lj = -1; lj <= 1; ++lj)
                                  {
                                      for(Int32 li = -1; li <= 1; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Grid3D.isValidCell(cellId))
                                          {
                                              continue;
                                          }

                                          for(const UInt32 q : m_CellParticles(cellId))
                                          {
                                              if(p <= q)
                                              {
                                                  continue;
                                              }

                                              const Vec3<RealType>& qpos = clusterCenters[q];
                                              const RealType d2 = glm::length2(ppos - qpos);

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
template<class RealType>
void LloydRelaxation<RealType>::sortSamples(std::vector<Vec3<RealType> >& samples, const Vec_VecUInt& samplesInCluster)
{
    static Timer m_Timer;
    size_t       numSamples = samples.size();

    ////////////////////////////////////////////////////////////////////////////////
    m_Timer.tick();
    static std::vector<Vec3<RealType> > tmp_samples;
    tmp_samples.resize(samples.size());

    size_t index = 0;

    for(size_t i = 0; i < samplesInCluster.size(); ++i)
    {
        size_t cluster_samples = samplesInCluster[i].size();

        for(size_t j = 0; j < cluster_samples; ++j)
        {
            tmp_samples[index] = samples[samplesInCluster[i][j]];
            ++index;
        }
    }

    __BNN_ASSERT(index == numSamples);

    std::copy(tmp_samples.begin(), tmp_samples.end(), samples.begin());
    tmp_samples.clear();

    m_Timer.tock();
    m_Logger.printLog(m_Timer.getRunTime("Sort samples: ") + " =======================================");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void LloydRelaxation<RealType>::collectClustersToCells(const std::vector<Vec3<RealType> >& clusterCenters)
{
    for(auto& cell : m_CellParticles)
        cell.resize(0);

    // cannot run in parallel....
    for(const auto& pos : clusterCenters)
    {
        const Vec3i& cellId = m_Grid3D.getCellIdx(pos);

        m_CellParticles(cellId).push_back(p);
    }
}
