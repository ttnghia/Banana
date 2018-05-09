//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Utils/Logger.h>
#include <Banana/Utils/NumberHelpers.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <ParticleTools/ParticleSerialization.h>
#include <QMessageBox>
#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleGeneratorInterface
{
public:
    ParticleGeneratorInterface()
    {
        Logger::initialize();
        m_Logger = Logger::createLogger("Generator");
    }

    ~ParticleGeneratorInterface() { Logger::shutdown(); }
    ////////////////////////////////////////////////////////////////////////////////
    void createGenerator(const String& sceneFile);
    void doFrameRelaxation(UInt frame);
    void finalizeRelaxation();
    ////////////////////////////////////////////////////////////////////////////////
    Int    getDimension()  const { return m_Dimension; }
    float* getBMin() { return &m_ParticleData.domainBMin[0]; }
    float* getBMax() { return &m_ParticleData.domainBMax[0]; }
    ////////////////////////////////////////////////////////////////////////////////
    char* getParticlePositions() { return m_ParticleData.positions; }
    char* getObjectIndex() { return reinterpret_cast<char*>(m_ParticleData.objectIndex.data()); }
    UInt  getNObjects() const { return static_cast<UInt>(m_ParticleData.nObjects); }
    UInt  getNParticles()  const { return m_ParticleData.nParticles; }
    float getParticleRadius() const { return m_ParticleData.particleRadius; }

private:
    Int                                                               m_Dimension = 2;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<2, float>>> m_Generators2D;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<3, float>>> m_Generators3D;
    SharedPtr<Logger>                                                 m_Logger = nullptr;

    struct ParticleData
    {
        float particleRadius;
        float domainBMin[3];
        float domainBMax[3];
        char* positions = nullptr;

        UInt16              nParticles = 0;
        UInt16              nObjects   = 0;
        Vec_UInt16          objectIndex;
        Vector<Vec2<float>> positions2D;
        Vector<Vec3<float>> positions3D;
    } m_ParticleData;
};
