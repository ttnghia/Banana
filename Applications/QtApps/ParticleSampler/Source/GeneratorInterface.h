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

#include <SimulationObjects/ParticleGenerator.h>
#include <QMessageBox>
#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleGeneratorInterface
{
public:
    void createGenerator(const String& sceneFile);
    void loadScene(const String& sceneFile);
    void doFrameSimulation(UInt frame);
    void finalizeSimulation();
    ////////////////////////////////////////////////////////////////////////////////
    Int    getDimension()  const;
    auto&  getGlobalParams();
    float* getBMin();
    float* getBMax();
    ////////////////////////////////////////////////////////////////////////////////
    char* getParticlePositions();
    char* getParticleVelocities();
    char* getObjectIndex();
    UInt  getNObjects() const;
    UInt  getNParticles()  const;
    float getParticleRadius() const;

private:
    SharedPtr<ParticleGenerator<2, Real>> m_Generator2D = nullptr;
    SharedPtr<ParticleGenerator<3, Real>> m_Generator3D = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::createGenerator(const String& sceneFile)
{
    m_Generator2D.reset();
    m_Generator3D.reset();
    m_Generator2D = ParticleSolverFactory2D::createSolverFromJSon(sceneFile);
    m_Generator3D = ParticleSolverFactory3D::createSolverFromJSon(sceneFile);
    if(m_Generator2D == nullptr && m_Generator3D == nullptr) {
        QMessageBox::critical(nullptr, QString("Error"), QString("The solver in scene file '%1' is not supported!").arg(QString::fromStdString(sceneFile)));
    }
    __BNN_REQUIRE(m_Generator2D == nullptr ^ m_Generator3D == nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::loadScene(const String& sceneFile)
{
    m_Generator2D != nullptr ?
    m_Generator2D->loadScene(sceneFile) :
    m_Generator3D->loadScene(sceneFile);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::doFrameSimulation(UInt frame)
{
    m_Generator2D != nullptr ?
    m_Generator2D->doFrameSimulation(frame) :
    m_Generator3D->doFrameSimulation(frame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleGeneratorInterface::finalizeSimulation()
{
    m_Generator2D != nullptr ?
    m_Generator2D->finalizeSimulation() :
    m_Generator3D->finalizeSimulation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Int ParticleGeneratorInterface::getDimension() const
{
    return m_Generator2D != nullptr ?
           m_Generator2D->dimension() :
           m_Generator3D->dimension();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline auto& ParticleGeneratorInterface::getGlobalParams()
{
    return m_Generator2D != nullptr ?
           m_Generator2D->globalParams() :
           m_Generator3D->globalParams();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float* ParticleGeneratorInterface::getBMin()
{
    return m_Generator2D != nullptr ?
           reinterpret_cast<float*>(&m_Generator2D->generalSolverParams().movingBMin) :
           reinterpret_cast<float*>(&m_Generator3D->generalSolverParams().movingBMin);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float* ParticleGeneratorInterface::getBMax()
{
    return m_Generator2D != nullptr ?
           reinterpret_cast<float*>(&m_Generator2D->generalSolverParams().movingBMax) :
           reinterpret_cast<float*>(&m_Generator3D->generalSolverParams().movingBMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleGeneratorInterface::getParticlePositions()
{
    return m_Generator2D != nullptr ?
           reinterpret_cast<char*>(m_Generator2D->generalParticleData().positions.data()) :
           reinterpret_cast<char*>(m_Generator3D->generalParticleData().positions.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleGeneratorInterface::getParticleVelocities()
{
    return m_Generator2D != nullptr ?
           reinterpret_cast<char*>(m_Generator2D->generalParticleData().velocities.data()) :
           reinterpret_cast<char*>(m_Generator3D->generalParticleData().velocities.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleGeneratorInterface::getObjectIndex()
{
    return reinterpret_cast<char*>(m_Generator2D != nullptr ?
                                   m_Generator2D->generalParticleData().objectIndex.data() :
                                   m_Generator3D->generalParticleData().objectIndex.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline UInt ParticleGeneratorInterface::getNObjects() const
{
    return m_Generator2D != nullptr ?
           m_Generator2D->generalParticleData().nObjects :
           m_Generator3D->generalParticleData().nObjects;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline UInt ParticleGeneratorInterface::getNParticles() const
{
    return m_Generator2D != nullptr ?
           m_Generator2D->generalParticleData().getNParticles() :
           m_Generator3D->generalParticleData().getNParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float ParticleGeneratorInterface::getParticleRadius() const
{
    return m_Generator2D != nullptr ?
           m_Generator2D->generalSolverParams().particleRadius :
           m_Generator3D->generalSolverParams().particleRadius;
}
