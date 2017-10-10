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
void ParticleObject<N, RealType >::generateParticles(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities, RealType particleRadius, bool bUseCache /*= true*/)
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
void ParticleObject<N, RealType >::generatePositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    // Firstly, generate a signed distance field
    Grid<VecX<N, RealType>::length(), RealType> grid;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleObject<N, RealType >::relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius)
{
    bool   bRelax      = false;
    String relaxMethod = String("SPH");
    JSONHelpers::readBool(m_jParams, bRelax, "RelaxPosition");
    JSONHelpers::readValue(m_jParams, relaxMethod, "RelaxMethod");

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
void ParticleObject<N, RealType >::generateVelocities(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities)
{
    VecX<N, RealType> initVelocity = VecX<N, RealType>(0);
    JSONHelpers::readVector(m_jParams, initVelocity, "InitialVelocity");
    velocities.resize(positions.size(), initVelocity);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
