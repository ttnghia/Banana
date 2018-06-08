//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <ParticleTools/ParticleSerialization.h>
#include <SimulationObjects/ParticleGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool ParticleGenerator<N, RealType>::isActive(UInt currentFrame)
{
    if(m_ActiveFrames.size() > 0 &&
       m_ActiveFrames.find(currentFrame) == m_ActiveFrames.end()) {
        return false;
    } else {
        return (currentFrame >= m_StartFrame &&
                currentFrame <= m_MaxFrame &&
                m_NGeneratedParticles < m_MaxNParticles);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::parseParameters(const JParams& jParams)
{
    JSONHelpers::readVector(jParams, v0(), "InitialVelocity");
    JSONHelpers::readValue(jParams, minDistanceRatio(),   "MinParticleDistanceRatio");
    JSONHelpers::readValue(jParams, jitter(),             "JitterRatio");
    JSONHelpers::readValue(jParams, startFrame(),         "StartFrame");
    JSONHelpers::readValue(jParams, maxFrame(),           "MaxFrame");
    JSONHelpers::readValue(jParams, maxNParticles(),      "MaxNParticles");
    JSONHelpers::readValue(jParams, activeFrames(),       "ActiveFrames");
    JSONHelpers::readValue(jParams, maxSamplingIters(),   "MaxSamplingIters");
    JSONHelpers::readValue(jParams, constraintObjectID(), "ConstraintObjectID");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects)
{
    if(this->m_bObjReady) {
        return;
    }
    m_ParticleRadius = particleRadius;
    m_MinDistanceSqr = m_MinDistanceRatio * particleRadius * particleRadius;
    m_Jitter        *= particleRadius;

    ////////////////////////////////////////////////////////////////////////////////
    // load particles from cache, if existed
    if(this->m_bUseCache && !this->m_ParticleFile.empty() && FileHelpers::fileExisted(this->m_ParticleFile)) {
        ParticleSerialization::loadParticle(this->m_MeshFile, m_ObjParticles, m_ParticleRadius);
        this->m_bObjReady = true;
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    RealType spacing = RealType(2.0) * m_ParticleRadius;
    auto     boxMin  = this->m_GeometryObj->getAABBMin();
    auto     boxMax  = this->m_GeometryObj->getAABBMax();
    auto     pGrid   = NumberHelpers::createGrid<UInt>(boxMin, boxMax, spacing);
    m_Grid.setGrid(boxMin, boxMax, RealType(4.0) * m_ParticleRadius);
    m_ParticleIdxInCell.resize(m_Grid.getNCells());
    m_Lock.resize(m_Grid.getNCells());

    NumberHelpers::scan(pGrid,
                        [&](const auto& idx)
                        {
                            VecN ppos = boxMin + VecX<N, RealType>(idx) * spacing;
                            for(auto& bdObj : boundaryObjects) {
                                if(bdObj->signedDistance(ppos) < 0) {
                                    return;
                                }
                            }

                            if(this->m_GeometryObj->signedDistance(ppos) < 0) {
                                m_ObjParticles.push_back(ppos);
                            }
                        });

    __BNN_REQUIRE(m_ObjParticles.size() > 0)

    ////////////////////////////////////////////////////////////////////////////////
    __BNN_TODO;
    //generatePositions(positions, particleRadius);
    //relaxPositions(positions, particleRadius);
    //generateVelocities(positions, velocities);

    this->m_bObjReady = true;

    ////////////////////////////////////////////////////////////////////////////////
    // save particles from cache, if needed
    if(this->m_bUseCache && !this->m_ParticleFile.empty()) {
        ParticleSerialization::saveParticle(this->m_MeshFile, m_ObjParticles, m_ParticleRadius);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::relaxPositions(Vector<VecN>& positions, RealType particleRadius)
{
    __BNN_UNUSED(positions);
    __BNN_UNUSED(particleRadius);

    bool   bRelax      = false;
    String relaxMethod = String("SPH");
    //JSONHelpers::readBool(m_jParams, bRelax, "RelaxPosition");
    //JSONHelpers::readValue(m_jParams, relaxMethod, "RelaxMethod");

    if(bRelax) {
        //if(relaxMethod == "SPH" || relaxMethod == "SPHBased")
        //    SPHBasedRelaxation::relaxPositions(positions, particleRadius);
        //else
        //{
        //    Vector<VecN > denseSamples;
        //    RealType                       denseSampleRatio = 0.1;
        //    JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

        //    generatePositions(denseSamples, particleRadius * denseSampleRatio);
        //    LloydRelaxation::relaxPositions(denseSamples, positions);
        //}
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::collectNeighborParticles(const Vec_VecN& positions)
{
    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p)
                            {
                                auto cellIdx = m_Grid.getCellIdx<Int>(positions[p]);
                                if(m_Grid.isValidCell(cellIdx)) {
                                    m_Lock(cellIdx).lock();
                                    m_ParticleIdxInCell(cellIdx).push_back(p);
                                    m_Lock(cellIdx).unlock();
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class ParticleGenerator<2, Real>;
template class ParticleGenerator<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
