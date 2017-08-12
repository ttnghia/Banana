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

#pragma once

#include <Banana/TypeNames.h>
#include <Banana/Macros.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/Data/DataIO.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <CompactNSearch/CompactNSearch.h>

#include <ParticleTools/BoundaryObjects/BoundaryObjectInterface.h>

#include <memory>
#include <map>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct FrameParameters
{
    RealType     frameDuration = RealType(1.0 / 30.0);
    unsigned int startFrame    = 1;
    unsigned int finalFrame    = 1;
    unsigned int finishedFrame = 0;
    unsigned int nThreads      = 0;

    ////////////////////////////////////////////////////////////////////////////////
    bool         bSaveParticleData = true;
    bool         bSaveMemoryState  = true;
    unsigned int framePerState     = 1;
    std::string  dataPath          = std::string("");
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleSolver
{
public:
    ParticleSolver()          = default;
    virtual ~ParticleSolver() = default;

    std::shared_ptr<FrameParameters<RealType> > getFrameParams() const noexcept { return m_FrameParams; }
    void setFrameParams(std::shared_ptr<FrameParameters<RealType> > frameParams) { m_FrameParams = frameParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady()        = 0;
    virtual void advanceFrame()     = 0;
    virtual void saveParticleData() = 0;
    virtual void saveMemoryState()  = 0;

    virtual std::string         getSolverName()   = 0;
    virtual unsigned int        getNumParticles() = 0;
    virtual Vec_Vec3<RealType>& getPositions()    = 0;
    virtual Vec_Vec3<RealType>& getVelocity()     = 0;

    void setupLogger(bool bLog2Std, bool bLog2File, const std::string& dataPath = std::string(""));

protected:
    Logger m_Logger;

    std::shared_ptr<FrameParameters<RealType> >     m_FrameParams = std::make_shared<FrameParameters<RealType> >();
    std::map<std::string, std::shared_ptr<DataIO> > m_ParticleDataIO;
    std::map<std::string, std::shared_ptr<DataIO> > m_MemoryStateIO;

    NeighborhoodSearch<RealType>               m_NSearch;
    std::unique_ptr<BoundaryObject<RealType> > m_BoundaryObject;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::setupLogger(bool bLog2Std, bool bLog2File, const std::string& dataPath /*= std::string("")*/)
{
    m_Logger.enableStdOut(bLog2Std);
    m_Logger.enableLogFile(bLog2File);

    // TODO
    if(bLog2File)
    {
        __BNN_ASSERT(!dataPath.empty());
        m_Logger.setDataPath(dataPath);
        m_Logger.setSourceName(getSolverName());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana