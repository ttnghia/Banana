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

#include "ParticleSolverQt.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSolverQt::doSimulationFrame(UInt frame)
{
    logger().newLine();
    logger().printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
    logger().newLine();

    ////////////////////////////////////////////////////////////////////////////////
    static String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(m_GlobalParams.frameDuration) +
                           String("(s) (~") + std::to_string(static_cast<int>(round(Real(1.0) / m_GlobalParams.frameDuration))) + String(" fps). Run time: ");
    static Timer frameTimer;
    logger().printRunTime(strMsg.c_str(), frameTimer,
                          [&]()
                          {
                              advanceFrame();
                              sortParticles();
                          });

    ////////////////////////////////////////////////////////////////////////////////
    logger().printMemoryUsage();
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSolverQt::endSimulation()
{
    logger().newLine();
    logger().printAligned("Simulation finished", '+');
    logger().printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams.finalFrame - m_GlobalParams.startFrame + 1));
    logger().printLog("Data path: " + m_GlobalParams.dataPath);
    auto strs = FileHelpers::getFolderSizeInfo(globalParams().dataPath, 1);
    for(auto& str: strs) {
        logger().printLog(str);
    }
    logger().newLine();
}
