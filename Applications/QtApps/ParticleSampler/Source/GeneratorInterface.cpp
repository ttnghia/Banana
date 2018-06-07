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

#include "GeneratorInterface.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleGeneratorInterface::loadScene(const String& sceneFile)
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
    __BNN_REQUIRE(jParams.find("SimulationParameters") != jParams.end());
    __BNN_REQUIRE(jParams.find("ParticleGenerators") != jParams.end());

    String solverName;
    JSONHelpers::readValue(jParams["GlobalParameters"], solverName, "Solver");
    __BNN_REQUIRE((solverName.find_first_of('2') != String::npos) ^ (solverName.find_first_of('3') != String::npos));
    m_Dimension = (solverName.find_first_of('2') != String::npos) ? 2 : 3;
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////
    // read particle radius
    if(!JSONHelpers::readValue(jParams["SimulationParameters"], m_ParticleData.particleRadius, "ParticleRadius")) {
        float cellSize;
        float ratioCellSizeRadius;
        __BNN_REQUIRE(JSONHelpers::readValue(jParams["SimulationParameters"], cellSize, "CellSize"));
        __BNN_REQUIRE(JSONHelpers::readValue(jParams["SimulationParameters"], ratioCellSizeRadius, "RatioCellSizePRadius"));
        m_ParticleData.particleRadius = cellSize / ratioCellSizeRadius;
        __BNN_REQUIRE(m_ParticleData.particleRadius > 0);
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // read domain box
    {
        JParams jBoxParams = jParams["SimulationParameters"]["SimulationDomainBox"];
        jBoxParams["GeometryType"] = String("Box");

        if(m_Dimension == 2) {
            Vec2f bMin, bMax;
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMin, "BoxMin"));
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMax, "BoxMax"));
            memcpy(&m_ParticleData.domainBMin[0], &bMin[0], sizeof(float) * m_Dimension);
            memcpy(&m_ParticleData.domainBMax[0], &bMax[0], sizeof(float) * m_Dimension);
        } else {
            Vec3f bMin, bMax;
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMin, "BoxMin"));
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMax, "BoxMax"));
            memcpy(&m_ParticleData.domainBMin[0], &bMin[0], sizeof(float) * m_Dimension);
            memcpy(&m_ParticleData.domainBMax[0], &bMax[0], sizeof(float) * m_Dimension);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////;
    // read boundary objects
    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        for(auto& jObj : jParams["AdditionalBoundaryObjects"]) {
            String geometryType;
            __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
            __BNN_REQUIRE(!geometryType.empty());

            if(m_Dimension == 2) {
                SharedPtr<SimulationObjects::BoundaryObject<2, float>> obj = nullptr;
                if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
                    obj = std::make_shared<SimulationObjects::BoxBoundary<2, float>>(jObj);
                } else {
                    obj = std::make_shared<SimulationObjects::BoundaryObject<2, float>>(jObj);
                }
                __BNN_REQUIRE(obj != nullptr);
                m_BoundaryObjs2D.push_back(obj);
            } else {
                SharedPtr<SimulationObjects::BoundaryObject<3, float>> obj = nullptr;
                if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
                    obj = std::make_shared<SimulationObjects::BoxBoundary<3, float>>(jObj);
                } else {
                    obj = std::make_shared<SimulationObjects::BoundaryObject<3, float>>(jObj);
                }
                __BNN_REQUIRE(obj != nullptr);
                m_BoundaryObjs3D.push_back(obj);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // combine static boundaries
        if(m_Dimension == 2) {
            if(m_BoundaryObjs2D.size() > 1) {
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<2, float>>(JParams(), true);
                auto csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<2, float>>(csgBoundary->geometry());
                __BNN_REQUIRE(csgObj != nullptr);

                for(auto& obj : m_BoundaryObjs2D) {
                    csgObj->addObject(obj->geometry(), GeometryObjects::CSGOperations::Union);
                }

                m_BoundaryObjs2D.resize(0);
                m_BoundaryObjs2D.push_back(csgBoundary);
            }
        } else {
            if(m_BoundaryObjs3D.size() > 1) {
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<3, float>>(JParams(), true);
                auto csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<3, float>>(csgBoundary->geometry());
                __BNN_REQUIRE(csgObj != nullptr);

                for(auto& obj : m_BoundaryObjs3D) {
                    csgObj->addObject(obj->geometry(), GeometryObjects::CSGOperations::Union);
                }

                m_BoundaryObjs3D.resize(0);
                m_BoundaryObjs3D.push_back(csgBoundary);
            }
        }
    } // end additional boundary
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
        generator->buildObject(m_ParticleData.particleRadius, m_BoundaryObjs2D);
        UInt nGen = generator->generateParticles(m_ParticleData.positions2D);
        if(nGen > 0) {
            auto& generatedPositions = generator->generatedPositions();
            m_ParticleData.positions2D.insert(m_ParticleData.positions2D.end(), generatedPositions.begin(), generatedPositions.end());
            m_ParticleData.objectIndex.insert(m_ParticleData.objectIndex.end(), generatedPositions.size(), m_ParticleData.nObjects);
            ++m_ParticleData.nObjects;
            m_Logger->printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID());
        }
    }
    for(auto& generator : m_Generators3D) {
        generator->buildObject(m_ParticleData.particleRadius, m_BoundaryObjs3D);
        UInt nGen = generator->generateParticles(m_ParticleData.positions3D);
        if(nGen > 0) {
            auto& generatedPositions = generator->generatedPositions();
            m_ParticleData.positions3D.insert(m_ParticleData.positions3D.end(), generatedPositions.begin(), generatedPositions.end());
            m_ParticleData.objectIndex.insert(m_ParticleData.objectIndex.end(), generatedPositions.size(), m_ParticleData.nObjects);
            ++m_ParticleData.nObjects;
            m_Logger->printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID());
        }
    }

    __BNN_REQUIRE((m_ParticleData.positions2D.size() > 0) ^ (m_ParticleData.positions3D.size() > 0));
    m_ParticleData.nParticles += static_cast<UInt>(m_ParticleData.positions2D.size());
    m_ParticleData.nParticles += static_cast<UInt>(m_ParticleData.positions3D.size());
    m_ParticleData.positions   = (m_ParticleData.positions2D.size() > 0) ?
                                 reinterpret_cast<char*>(m_ParticleData.positions2D.data()) : reinterpret_cast<char*>(m_ParticleData.positions3D.data());
    ////////////////////////////////////////////////////////////////////////////////
    m_Relax2D.reset();
    m_Relax3D.reset();
    if(m_Dimension == 2) {
        m_Relax2D = std::make_unique<ParticleTools::SPHBasedRelaxation<2, float>>(m_BoundaryObjs2D);
    } else {
        m_Relax3D = std::make_unique<ParticleTools::SPHBasedRelaxation<3, float>>(m_BoundaryObjs3D);
    }
}

void ParticleGeneratorInterface::updateRelaxParameters()
{
    if(m_Dimension == 2) {
        m_Relax2D->makeReady(m_ParticleData.positions2D.data(), m_ParticleData.nParticles);
    } else {
        m_Relax3D->makeReady(m_ParticleData.positions3D.data(), m_ParticleData.nParticles);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleGeneratorInterface::doFrameRelaxation(UInt frame)
{
    if(m_Dimension == 2) {
        m_Relax2D->iterate(m_ParticleData.positions2D.data(), m_ParticleData.nParticles, frame);
        if(frame > 1 && (frame % getRelaxParams()->checkFrequency) == 0) {
            m_Relax2D->computeMinDistanceRatio();
            if(m_Relax2D->getMinDistanceRatio() > getRelaxParams()->overlapThreshold) {
                m_Relax2D->logger().printLogPadding("Relaxation finished successfully.");
                m_Relax2D->logger().printMemoryUsage();
                m_Relax2D->logger().newLine();
                return true;
            }
        }
        m_Relax2D->logger().printMemoryUsage();
        m_Relax2D->logger().newLine();
    } else {
        m_Relax3D->iterate(m_ParticleData.positions3D.data(), m_ParticleData.nParticles, frame);
        if(frame > 1 && (frame % getRelaxParams()->checkFrequency) == 0) {
            m_Relax3D->computeMinDistanceRatio();
            if(m_Relax3D->getMinDistanceRatio() > getRelaxParams()->overlapThreshold) {
                m_Relax3D->logger().printLogPadding("Relaxation finished successfully.");
                m_Relax3D->logger().printMemoryUsage();
                m_Relax3D->logger().newLine();
                return true;
            }
        }
        m_Relax3D->logger().printMemoryUsage();
        m_Relax3D->logger().newLine();
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleGeneratorInterface::reportFailed(UInt frame)
{
    if(((frame - 1) % getRelaxParams()->checkFrequency) == 0) {
        if(m_Dimension == 2) {
            m_Relax2D->logger().printLogPadding("Relaxation failed after reaching maxIters = " + Formatters::toString(getRelaxParams()->maxIters));
            m_Relax2D->logger().newLine();
        } else {
            m_Relax3D->logger().printLogPadding("Relaxation failed after reaching maxIters = " + Formatters::toString(getRelaxParams()->maxIters));
            m_Relax3D->logger().newLine();
        }
    } else {
        if(m_Dimension == 2) {
            m_Relax2D->logger().printLogPadding("Relaxation failed after reaching maxIters = " + Formatters::toString(getRelaxParams()->maxIters));
            m_Relax2D->logger().newLine();
        } else {
            m_Relax3D->logger().printLogPadding("Relaxation failed after reaching maxIters = " + Formatters::toString(getRelaxParams()->maxIters));
            m_Relax3D->logger().newLine();
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const SharedPtr<ParticleTools::SPHRelaxationParameters<float>>& ParticleGeneratorInterface::getRelaxParams()
{
    return m_Dimension == 2 ? m_Relax2D->relaxParams() : m_Relax3D->relaxParams();
}
