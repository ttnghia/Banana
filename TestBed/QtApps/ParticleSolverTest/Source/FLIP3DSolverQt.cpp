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

#include "FLIP3DSolverQt.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolverQt::doSimulationFrame(UInt frame)
{
    m_Logger->newLine();
    m_Logger->printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
    m_Logger->newLine();

    ////////////////////////////////////////////////////////////////////////////////
    static String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(m_GlobalParams->frameDuration) +
                           String("(s) (~") + std::to_string(static_cast<int>(round(Real(1.0) / m_GlobalParams->frameDuration))) + String(" fps). Run time: ");
    static Timer frameTimer;
    m_Logger->printRunTime(strMsg.c_str(), frameTimer,
                           [&]()
                           {
                               advanceScene();
                               advanceFrame();
                               sortParticles();
                           });

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printMemoryUsage();
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolverQt::endSimulation()
{
    m_Logger->newLine();
    m_Logger->printAligned("Simulation finished", '+');
    m_Logger->printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams->finalFrame - m_GlobalParams->startFrame + 1));
    m_Logger->printLog("Data path: " + m_GlobalParams->dataPath);
    m_Logger->newLine();
    Logger::shutdown();
}
