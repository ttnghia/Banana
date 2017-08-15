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

#include "SDFGrid.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::setSDFObjectType(SDFObjectTypes SDFObjType)
{
    m_SDFObjectType          = SDFObjType;
    m_SDFGenerationFutureObj = std::async(std::launch::async, [&] { generateParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::setResolution(int resolution)
{
    m_Resolution             = resolution;
    m_SDFGenerationFutureObj = std::async(std::launch::async, [&] { generateParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::generateParticles()
{
    switch(m_SDFObjectType)
    {
        case Box:
            m_SDFObject = std::make_unique<GeometryObjects::BoxObject<float> >();
            break;
        case Sphere:
            m_SDFObject = std::make_unique<GeometryObjects::SphereObject<float> >();
            break;
    }

    Q_ASSERT(m_SDFObject != nullptr);

    Q_ASSERT(m_ParticleData != nullptr);

    const float        step           = 2.0f / float(m_Resolution);
    const float        particleRadius = 0.5 * step;
    const unsigned int numParticles   = m_Resolution * m_Resolution * m_Resolution;

    m_ParticleData->setNumParticles(numParticles);
    m_ParticleData->setParticleRadius(particleRadius);

    unsigned char*                   dataPtr = (m_ParticleData->getArray("Position")->data());
    static std::vector<Vec3<float> > negativeParticles;
    static std::vector<Vec3<float> > positiveParticles;
    negativeParticles.reserve(numParticles);
    positiveParticles.reserve(numParticles);

    negativeParticles.resize(0);
    positiveParticles.resize(0);

    const Vec3<float> corner(-1.0f + particleRadius);
    unsigned int      numNegative = 0;
    unsigned int      numPositive = 0;

    for(int i = 0; i < m_Resolution; ++i)
    {
        for(int j = 0; j < m_Resolution; ++j)
        {
            for(int k = 0; k < m_Resolution; ++k)
            {
                Vec3<float> ppos = corner + 2.0f * Vec3<float>(static_cast<float>(i) / static_cast<float>(m_Resolution),
                                                               static_cast<float>(j) / static_cast<float>(m_Resolution),
                                                               static_cast<float>(k) / static_cast<float>(m_Resolution));

                if(m_SDFObject->isInside(ppos))
                {
                    ++numNegative;
                    negativeParticles.push_back(ppos);
                }
                else
                {
                    ++numPositive;
                    positiveParticles.push_back(ppos);
                }
            }
        }
    }

    std::memcpy(dataPtr,                                   negativeParticles.data(), numNegative * sizeof(float) * 3);
    std::memcpy(&dataPtr[numNegative * sizeof(float) * 3], positiveParticles.data(), numPositive * sizeof(float) * 3);
    m_ParticleData->setUInt("NumNegative", numNegative);
    m_ParticleData->setUInt("NumPositive", numPositive);

    emit dataReady();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
