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
#include <Banana/Logger.h>
#include <Banana/FileHelpers.h>
#include <Banana/MathHelpers.h>
#include <Banana/Timer.h>
#include <Banana/Data/DataIO.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Grid/Grid3D.h>

#include <tbb/tbb.h>

#include <memory>
#include <random>
#include <map>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters
{};

struct SimulationData
{};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct GlobalParameters : public SimulationParameters
{
    RealType     frameDuration = 1.0 / 30.0;
    unsigned int startFrame    = 1;
    unsigned int finalFrame    = 1;
    unsigned int nThreads      = 0;
};

struct DataParameters : public SimulationParameters
{
    bool         bSaveParticleData = true;
    bool         bSaveMemoryState  = true;
    unsigned int framePerState     = 1;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleSolver
{
public:
    ParticleSolver(const std::shared_ptr<GlobalParameters<RealType> >& timeParams, const std::shared_ptr<DataParameters>& dataParams) :
        m_TimeParams(timeParams), m_DataParams(dataParams) {}
    virtual ~ParticleSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName()    = 0;
    virtual void        advanceFrame()     = 0;
    virtual void        makeReady()        = 0;
    virtual void        saveParticleData() = 0;
    virtual void        saveMemoryState()  = 0;

    void setupLogger(bool bLog2Std, bool bLog2File, const std::string& logLocation = std::string(""))
    {
        m_Logger.enableStdOut(bLog2Std);
        m_Logger.enableLogFile(bLog2File);

        // TODO
        if(bLog2File)
        {
            __BNN_ASSERT(!logLocation.empty());
            m_Logger.setDataPath(logLocation);
        }
    }

protected:
    Logger m_Logger;

    std::shared_ptr<GlobalParameters<RealType> >    m_TimeParams;
    std::shared_ptr<DataParameters>                 m_DataParams;
    std::map<std::string, std::shared_ptr<DataIO> > m_ParticleDataIO;
    std::map<std::string, std::shared_ptr<DataIO> > m_MemoryStateIO;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana