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
void ParticleEmitter<N, RealType >::setEmitterParams(const VecX<N, RealType>& v0, UInt maxNParticles /*= std::numeric_limits<UInt>::max()*/,
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
UInt ParticleEmitter<N, RealType > ::emitParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
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
UInt ParticleEmitter<N, RealType > ::addFullShapeParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
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
UInt ParticleEmitter<N, RealType > ::addParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
{
    __BNN_TODO
    return 0;
}
