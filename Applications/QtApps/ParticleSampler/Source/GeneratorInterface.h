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
    ////////////////////////////////////////////////////////////////////////////////
    void saveParticles(const String& fileName, bool bDouble = false);

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::createGenerator(const String& sceneFile)
{
    ////////////////////////////////////////////////////////////////////////////////;;
    // load parameters
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open()) {
        QMessageBox::critical(nullptr, QString("Error"), QString("Cannot open scene file '%1'!").arg(QString::fromStdString(sceneFile)));
        return;
    }
    auto jParams = JParams::parse(inputFile);
    __BNN_REQUIRE(jParams.find("GlobalParameters") != jParams.end());
    __BNN_REQUIRE(jParams.find("ParticleGenerators") != jParams.end());

    String solverName;
    JSONHelpers::readValue(jParams["GlobalParameters"], solverName, "Solver");
    __BNN_REQUIRE(solverName.find_first_of('2') > 0 || solverName.find_first_of('3') > 0);
    m_Dimension = (solverName.find_first_of('2') > 0) ? 2 : 3;
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////;
    // create generators
    m_Generators2D.resize(0);
    m_Generators3D.resize(0);
    if(m_Dimension == 2) {
        for(auto& jObj : jParams["ParticleGenerators"]) {
            m_Generators2D.emplace_back(std::make_shared<SimulationObjects::ParticleGenerator<2, float>>(jObj));
        }
    } else {
        for(auto& jObj : jParams["ParticleGenerators"]) {
            m_Generators3D.emplace_back(std::make_shared<SimulationObjects::ParticleGenerator<3, float>>(jObj));
        }
    }
    __BNN_REQUIRE((m_Generators2D.size() > 0) ^ (m_Generators3D.size() > 0));
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////
    // add particles without relaxation
    m_ParticleData.positions2D.resize(0);
    m_ParticleData.positions3D.resize(0);
    m_ParticleData.objectIndex.resize(0);
    m_ParticleData.nParticles = 0;
    m_ParticleData.nObjects   = 0;
    for(auto& generator : m_Generators2D) {
        generator->buildObject(m_ParticleData.particleRadius);
        ////////////////////////////////////////////////////////////////////////////////
        UInt nGen = generator->generateParticles(m_ParticleData.positions2D);
        if(nGen > 0) {
            auto& generatedPositions = generator->generatedPositions();
            m_ParticleData.positions2D.insert(m_ParticleData.positions2D.end(), generatedPositions.begin(), generatedPositions.end());
            m_ParticleData.objectIndex.insert(m_ParticleData.objectIndex.end(), generatedPositions.size(), m_ParticleData.nObjects);
            ++m_ParticleData.nObjects;
            m_Logger->printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by generator: ") + generator->nameID());
        }
    }
    for(auto& generator : m_Generators3D) {
        generator->buildObject(m_ParticleData.particleRadius);
        ////////////////////////////////////////////////////////////////////////////////
        UInt nGen = generator->generateParticles(m_ParticleData.positions3D);
        if(nGen > 0) {
            auto& generatedPositions = generator->generatedPositions();
            m_ParticleData.positions2D.insert(m_ParticleData.positions2D.end(), generatedPositions.begin(), generatedPositions.end());
            m_ParticleData.objectIndex.insert(m_ParticleData.objectIndex.end(), generatedPositions.size(), m_ParticleData.nObjects);
            ++m_ParticleData.nObjects;
            m_Logger->printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by generator: ") + generator->nameID());
        }
    }

    __BNN_REQUIRE((m_ParticleData.positions2D.size() > 0) ^ (m_ParticleData.positions3D.size() > 0));
    m_ParticleData.nParticles += static_cast<UInt>(m_ParticleData.positions2D.size());
    m_ParticleData.nParticles += static_cast<UInt>(m_ParticleData.positions3D.size());
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::doFrameRelaxation(UInt frame)
{
    //    m_Generators2D != nullptr ?
    //    m_Generators2D->doFrameRelaxation(frame) :
    //    m_Generators3D->doFrameRelaxation(frame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::finalizeRelaxation()
{
    //    m_Generators2D != nullptr ?
    //    m_Generators2D->finalizeRelaxation() :
    //    m_Generators3D->finalizeRelaxation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::saveParticles(const String& fileName, bool bDouble /*= false*/)
{
    //
}