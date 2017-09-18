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
template<class VectorType>
void ParticleObject::generateParticles(Vector<VectorType>& positions, Vector<VectorType>& velocities, Real particleRadius, bool bUseCache /*= true*/)
{
    if(bUseCache && !m_ParticleFile.empty() && FileHelpers::fileExisted(m_ParticleFile))
    {
        positions.resize(0);
        ParticleHelpers::loadBinaryAndDecompress(m_ParticleFile, positions, m_ParticleRadius);
        generateVelocities();

        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    generatePositions(positions, particleRadius);
    relaxPositions(positions, particleRadius);
    generateVelocities(positions, velocities);
    ////////////////////////////////////////////////////////////////////////////////

    if(bUseCache && !m_ParticleFile.empty())
        ParticleHelpers::compressAndSaveBinary(m_ParticleFile, positions, m_ParticleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void generatePositions(Vector<VectorType>& positions, Real particleRadius)
{
    //
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void relaxPositions(Vector<VectorType>& positions, Real particleRadius)
{
    bool   bRelax      = false;
    String relaxMethod = String("SPH");
    JSONHelpers::readBool(m_jParams, bRelax, "RelaxPosition");
    JSONHelpers::readValue(m_jParams, relaxMethod, "RelaxMethod");

    if(bRelax)
    {
        if(relaxMethod == "SPH" || relaxMethod == "SPHBased")
            LloydRelaxation::relaxParticles(positions, positions);
        else
        {
            Vector<VectorType> denseSamples;
            Real               denseSampleRatio = 0.1;
            JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

            generatePositions(denseSamples, particleRadius * denseSampleRatio);
            LloydRelaxation::relaxParticles(denseSamples, positions);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType>
void generateVelocities(Vector<VectorType>& positions, Vector<VectorType>& velocities)
{
    VectorType initVelocity = VectorType(0);
    JSONHelpers::readVector(m_jParams, initVelocity, "InitialVelocity");
    velocities.resize(positions.size(), initVelocity);
}