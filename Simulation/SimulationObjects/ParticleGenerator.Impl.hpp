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
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::setEmitterParams(const VecX<N, RealType>& v0, UInt maxNParticles /*= std::numeric_limits<UInt>::max()*/,
                                                       UInt maxFrame /*= std::numeric_limits<UInt>::max()*/, RealType minDistance /*= 2.0*/,
                                                       bool bFullShapeObj /*= false*/)
{
    m_v0            = v0;
    m_MaxNParticles = maxNParticles;
    m_MaxFrame      = maxFrame;
    m_MinDistance   = minDistance;
    m_bFullShapeObj = bFullShapeObj;
    m_bParamsReady  = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::emitParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
{
    if(bEmissionFinished()) {
        return 0;
    }
    __BNN_ASSERT(m_bEmitterReady);
    collectNeighborParticles();
    return m_bFullShapeObj ? addFullShapeParticles(positions, velocities) : addParticles(positions, velocities);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addFullShapeParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
{
    volatile bool bEmptyRegion = true;
    ParallelFuncs::parallel_for(m_EmittingParticles.size(),
                                [&](size_t i)
                                {
                                    const VecX<N, RealType>& ppos = m_EmittingParticles[i];
                                    if(NumberHelpers::isInside(ppos, m_BMin, m_BMax)) {
                                        bEmptyRegion = false;
                                    }
                                });

    if(bEmptyRegion) {
        positions.reserve(positions.size() + m_EmittingParticles.size());
        positions.insert(positions.end(), m_EmittingParticles.begin(), m_EmittingParticles.end());
        velocities.insert(velocities.end() + m_EmittingParticles.size(), m_v0);
    }

    return static_cast<UInt>(m_EmittingParticles.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
{
    __BNN_TODO
    return 0;
}

template<Int N, class RealType>
void ParticleGenerator<N, RealType >::generateParticles(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities, RealType particleRadius, bool bUseCache /*= true*/)
{
    if(bUseCache && !m_ParticleFile.empty() && FileHelpers::fileExisted(m_ParticleFile)) {
        positions.resize(0);

        // todo
        //ParticleHelpers::loadBinaryAndDecompress(m_ParticleFile, positions, particleRadius);
        generateVelocities(positions, velocities);

        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    generatePositions(positions, particleRadius);
    relaxPositions(positions, particleRadius);
    generateVelocities(positions, velocities);
    ////////////////////////////////////////////////////////////////////////////////

    // todo
    //if(bUseCache && !m_ParticleFile.empty())
    //    ParticleHelpers::compressAndSaveBinary(m_ParticleFile, positions, particleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::generatePositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    // Firstly, generate a signed distance field
    Grid<VecX<N, RealType>::length(), RealType> grid;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius)
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
//    Vector<VecX<N, RealType> > denseSamples;
//    RealType                       denseSampleRatio = 0.1;
//    JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

//    generatePositions(denseSamples, particleRadius * denseSampleRatio);
//    LloydRelaxation::relaxPositions(denseSamples, positions);
//}
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::generateVelocities(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities)
{
    __BNN_UNUSED(positions);
    __BNN_UNUSED(velocities);

    //VecX<N, RealType> initVelocity = VecX<N, RealType>(0);
    //JSONHelpers::readVector(m_jParams, initVelocity, "InitialVelocity");
    //velocities.resize(positions.size(), initVelocity);
}