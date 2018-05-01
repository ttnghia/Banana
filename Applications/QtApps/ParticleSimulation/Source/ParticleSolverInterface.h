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

#include <ParticleSolvers/ParticleSolverFactory.h>
#include <QMessageBox>
#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolverInterface
{
public:
    void createSolver(const String& sceneFile);
    void loadScene(const String& sceneFile);
    void doFrameSimulation(UInt frame);
    void finalizeSimulation();
    ////////////////////////////////////////////////////////////////////////////////
    Int    getSolverDimension()  const;
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
    SharedPtr<ParticleSolver<2, Real>> m_Solver2D = nullptr;
    SharedPtr<ParticleSolver<3, Real>> m_Solver3D = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleSolverInterface::createSolver(const String& sceneFile)
{
    m_Solver2D.reset();
    m_Solver3D.reset();
    m_Solver2D = ParticleSolverFactory2D::createSolverFromJSon(sceneFile);
    m_Solver3D = ParticleSolverFactory3D::createSolverFromJSon(sceneFile);
    if(m_Solver2D == nullptr && m_Solver3D == nullptr) {
        QMessageBox::critical(nullptr, QString("Error"), QString("The solver in scene file '%1' is not supported!").arg(QString::fromStdString(sceneFile)));
    }
    __BNN_REQUIRE(m_Solver2D == nullptr ^ m_Solver3D == nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleSolverInterface::loadScene(const String& sceneFile)
{
    m_Solver2D != nullptr ?
    m_Solver2D->loadScene(sceneFile) :
    m_Solver3D->loadScene(sceneFile);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleSolverInterface::doFrameSimulation(UInt frame)
{
    m_Solver2D != nullptr ?
    m_Solver2D->doFrameSimulation(frame) :
    m_Solver3D->doFrameSimulation(frame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void ParticleSolverInterface::finalizeSimulation()
{
    m_Solver2D != nullptr ?
    m_Solver2D->finalizeSimulation() :
    m_Solver3D->finalizeSimulation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Int ParticleSolverInterface::getSolverDimension() const
{
    return m_Solver2D != nullptr ?
           m_Solver2D->dimension() :
           m_Solver3D->dimension();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline auto& ParticleSolverInterface::getGlobalParams()
{
    return m_Solver2D != nullptr ?
           m_Solver2D->globalParams() :
           m_Solver3D->globalParams();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float* ParticleSolverInterface::getBMin()
{
    return m_Solver2D != nullptr ?
           reinterpret_cast<float*>(&m_Solver2D->generalSolverParams().movingBMin) :
           reinterpret_cast<float*>(&m_Solver3D->generalSolverParams().movingBMin);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float* ParticleSolverInterface::getBMax()
{
    return m_Solver2D != nullptr ?
           reinterpret_cast<float*>(&m_Solver2D->generalSolverParams().movingBMax) :
           reinterpret_cast<float*>(&m_Solver3D->generalSolverParams().movingBMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleSolverInterface::getParticlePositions()
{
    return m_Solver2D != nullptr ?
           reinterpret_cast<char*>(m_Solver2D->generalParticleData().positions.data()) :
           reinterpret_cast<char*>(m_Solver3D->generalParticleData().positions.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleSolverInterface::getParticleVelocities()
{
    return m_Solver2D != nullptr ?
           reinterpret_cast<char*>(m_Solver2D->generalParticleData().velocities.data()) :
           reinterpret_cast<char*>(m_Solver3D->generalParticleData().velocities.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline char* ParticleSolverInterface::getObjectIndex()
{
    return reinterpret_cast<char*>(m_Solver2D != nullptr ?
                                   m_Solver2D->generalParticleData().objectIndex.data() :
                                   m_Solver3D->generalParticleData().objectIndex.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline UInt ParticleSolverInterface::getNObjects() const
{
    return m_Solver2D != nullptr ?
           m_Solver2D->generalParticleData().nObjects :
           m_Solver3D->generalParticleData().nObjects;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline UInt ParticleSolverInterface::getNParticles() const
{
    return m_Solver2D != nullptr ?
           m_Solver2D->generalParticleData().getNParticles() :
           m_Solver3D->generalParticleData().getNParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline float ParticleSolverInterface::getParticleRadius() const
{
    return m_Solver2D != nullptr ?
           m_Solver2D->generalSolverParams().particleRadius :
           m_Solver3D->generalSolverParams().particleRadius;
}
