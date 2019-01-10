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
namespace Banana::SimulationObjects {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool ParticleGenerator<N, RealType>::isActive(UInt currentFrame) {
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
void ParticleGenerator<N, RealType>::parseParameters(const JParams& jParams) {
    JSONHelpers::readVector(jParams, v0(), "InitialVelocity");
    JSONHelpers::readValue(jParams, samplingRatio(),      "SamplingRatio"); // recommend: 0.85-0.9 for 2D, 0.8-0.85 for 3D
    JSONHelpers::readValue(jParams, jitterRatio(),        "JitterRatio");
    JSONHelpers::readValue(jParams, startFrame(),         "StartFrame");
    JSONHelpers::readValue(jParams, maxFrame(),           "MaxFrame");
    JSONHelpers::readValue(jParams, maxNParticles(),      "MaxNParticles");
    JSONHelpers::readValue(jParams, activeFrames(),       "ActiveFrames");
    JSONHelpers::readValue(jParams, maxSamplingIters(),   "MaxSamplingIters");
    JSONHelpers::readValue(jParams, constraintObjectID(), "ConstraintObjectID");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects) {
    if(this->m_bObjReady) {
        return;
    }
    this->m_ParticleRadius = particleRadius;
    m_Spacing    = particleRadius * RealType(2.0) * samplingRatio();
    m_SpacingSqr = m_Spacing * m_Spacing;

    ////////////////////////////////////////////////////////////////////////////////
    // load particles from cache, if existed
    if(this->loadParticlesFromFile()) {
        this->m_bObjReady = true;
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////

    auto jitter = m_JitterRatio * this->m_ParticleRadius;
    auto boxMin = this->m_GeometryObj->getAABBMin();
    auto boxMax = this->m_GeometryObj->getAABBMax();

    auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(this->m_GeometryObj);
    if(box != nullptr) {
        boxMin = box->boxMin() - VecX<N, RealType>(RealType(4.0) * this->m_ParticleRadius);
        boxMax = box->boxMax() + VecX<N, RealType>(RealType(4.0) * this->m_ParticleRadius);
    }

    auto pGrid = NumberHelpers::createGrid<UInt>(boxMin, boxMax, m_Spacing);
    m_Grid.setGrid(boxMin - RealType(4.0) * this->m_ParticleRadius, boxMax + RealType(4.0) * this->m_ParticleRadius, RealType(4.0) * this->m_ParticleRadius);

    printf("min: %f, %f, %f,   max: %f, %f, %f\n", boxMin[0], boxMin[1], boxMin[2],
           boxMax[0], boxMax[1], boxMax[2]
           );

    m_ParticleIdxInCell.resize(m_Grid.getNCells());
    m_Lock.resize(m_Grid.getNCells());
    this->m_ObjParticles.reserve(glm::compMul(pGrid));
    ////////////////////////////////////////////////////////////////////////////////
    ParallelObjects::SpinLock lock;
    Scheduler::parallel_for(pGrid,
                            [&](auto... idx) {
                                VecN ppos = boxMin + VecX<N, RealType>(idx...) * m_Spacing;
                                for(auto& bdObj : boundaryObjects) {
                                    if(bdObj->signedDistance(ppos) < 0) {
                                        return;
                                    }
                                }
                                auto geoPhi = this->m_GeometryObj->signedDistance(ppos);
                                if(geoPhi < 0) {
                                    if(geoPhi < -jitter) {
                                        ppos += jitter * glm::normalize(NumberHelpers::fRand11<RealType>::template vrnd<VecN>());
                                    }
                                    lock.lock();
                                    this->m_ObjParticles.push_back(ppos);
                                    lock.unlock();
                                }
                            });
    ////////////////////////////////////////////////////////////////////////////////
    m_Relaxer = std::make_shared<ParticleTools::SPHBasedRelaxation<N, RealType>>(this->m_NameID, this->m_ObjParticles, this->m_GeometryObj, boundaryObjects);
    __BNN_TODO;
    //generatePositions(positions, particleRadius);
    //relaxPositions(positions, particleRadius);
    //generateVelocities(positions, velocities);

    this->m_bObjReady = true;
    ////////////////////////////////////////////////////////////////////////////////
    // save particles to file, if needed
    this->saveParticlesToFile();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType>::relaxPositions(Vector<VecN>& positions, RealType particleRadius) {
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
void ParticleGenerator<N, RealType>::collectNeighborParticles(const Vec_VecN& positions) {
    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }
    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p) {
                                auto cellIdx = m_Grid.template getCellIdx<Int>(positions[p]);
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
} // end namespace Banana::SimulationObjects
