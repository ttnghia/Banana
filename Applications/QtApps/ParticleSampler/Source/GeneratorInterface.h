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
#include <SimulationObjects/BoundaryObject.h>
#include <ParticleTools/SPHBasedRelaxation.h>
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
    void loadScene(const String& sceneFile);
    void setRelaxationParameters(const ParticleTools::SPHRelaxationParameters<float>& params);
    void doFrameRelaxation(UInt frame);
    void finalizeRelaxation(UInt frame);
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
    Vector<SharedPtr<SimulationObjects::BoundaryObject<2, float>>>    m_BoundaryObjs2D;
    Vector<SharedPtr<SimulationObjects::BoundaryObject<3, float>>>    m_BoundaryObjs3D;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<2, float>>> m_Generators2D;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<3, float>>> m_Generators3D;
    UniquePtr<ParticleTools::SPHBasedRelaxation<2, float>>            m_Relax2D;
    UniquePtr<ParticleTools::SPHBasedRelaxation<3, float>>            m_Relax3D;
    SharedPtr<Logger>                                                 m_Logger = nullptr;

    struct ParticleData
    {
        float particleRadius;
        char* positions = nullptr;
        Vec3f domainBMin;
        Vec3f domainBMax;

        UInt16     nParticles = 0;
        UInt16     nObjects   = 0;
        Vec_UInt16 objectIndex;

        Vector<Vec2<float>> positions2D;
        Vector<Vec3<float>> positions3D;
    } m_ParticleData;

    ParticleTools::SPHRelaxationParameters<float> m_RelaxParams;
};
