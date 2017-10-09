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

#include <ParticleTools/ParticleSerialization.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <catch.hpp>
#include <random>

using namespace Banana;

#define NUM_TEST  10
#define DATA_SIZE 1000
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void runTest(Int testID)
{
    auto logger = Logger::create("Tester_" + std::to_string(testID));

    ////////////////////////////////////////////////////////////////////////////////
    // create test data
    std::random_device                     rd;
    std::mt19937                           gen(rd());
    std::uniform_real_distribution<float>  disf(0, 10.0f);
    std::uniform_real_distribution<double> disd(0, 10.0);
    std::uniform_int_distribution<UInt>    disi(0, 10000);

    Real  dataFloat = disf(gen);
    Vec3f dataVec3f(disf(gen), disf(gen), disf(gen));

    Vec_Vec3f   dataVecVec3f1(DATA_SIZE);
    Vec_Vec3f   dataVecVec3f2(DATA_SIZE);
    Vec_Vec3d   dataVecVec3d1(DATA_SIZE);
    Vec_Vec3d   dataVecVec3d2(DATA_SIZE);
    Vec_Float   dataVecFloat(DATA_SIZE);
    Vec_VecUInt dataVecVecUInt(DATA_SIZE);

    for(size_t i = 0; i < DATA_SIZE; ++i) {
        dataVecVec3f1[i] = Vec3f(disf(gen));
        dataVecVec3f2[i] = Vec3f(disf(gen));
        dataVecVec3d1[i] = Vec3d(disd(gen));
        dataVecVec3d2[i] = Vec3d(disd(gen));
        dataVecFloat[i]  = disf(gen);

        dataVecVecUInt[i].reserve(DATA_SIZE);
        for(size_t j = 0; j < DATA_SIZE; ++j) {
            dataVecVecUInt[i].push_back(disi(gen));
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // write
    ParticleSerialization particleWriter("./Scratch", "Frames", "frame", "TestData", logger);
    particleWriter.addFixedAtribute("radius", ParticleSerialization::TypeReal, ParticleSerialization::Size32b);
    particleWriter.addFixedAtribute("origin", ParticleSerialization::TypeReal, ParticleSerialization::Size32b, 3);

    particleWriter.addParticleAtribute("position", ParticleSerialization::TypeReal, ParticleSerialization::Size32b, 3);
    particleWriter.addParticleAtribute("velocity", ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size32b, 3);
    particleWriter.addParticleAtribute("position_state", ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size64b, 3);
    particleWriter.addParticleAtribute("velocity_state", ParticleSerialization::TypeReal, ParticleSerialization::Size64b, 3);
    particleWriter.addParticleAtribute("density", ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size32b);
    particleWriter.addParticleAtribute("connection", ParticleSerialization::TypeVector, ParticleSerialization::Size32b);


    particleWriter.setNParticles(DATA_SIZE);
    particleWriter.setFixedAttribute("radius", dataFloat);
    particleWriter.setFixedAttribute("origin", dataVec3f);

    particleWriter.setParticleAttribute("position", dataVecVec3f1);
    particleWriter.setParticleAttribute("velocity", dataVecVec3f2);
    particleWriter.setParticleAttribute("position_state", dataVecVec3d1);
    particleWriter.setParticleAttribute("velocity_state", dataVecVec3d2);
    particleWriter.setParticleAttribute("density", dataVecFloat);
    particleWriter.setParticleAttribute("connection", dataVecVecUInt);

    particleWriter.flush(testID);
    particleWriter.waitForBuffers();

    ////////////////////////////////////////////////////////////////////////////////
    // read
    ParticleSerialization particleReader("./Scratch", "Frames", "frame", "TestData", logger);
    __BNN_ASSERT(particleReader.read(testID));
    logger->printLog("Read NParticles: " + NumberHelpers::formatWithCommas(particleReader.getNParticles()));

    for(auto& kv : particleReader.getFixedAttributes())
        logger->printLog("Fixed Attr: " + kv.first);
    for(auto& kv : particleReader.getParticleAttributes())
        logger->printLog("Particle Attr: " + kv.first);

    Real  readDataFloat = Huge;
    Vec3f readDataVec3f(Huge);

    Vec_Vec3f   readDataVecVec3f1;
    Vec_Vec3f   readDataVecVec3f2;
    Vec_Vec3d   readDataVecVec3d1;
    Vec_Vec3d   readDataVecVec3d2;
    Vec_Float   readDataVecFloat;
    Vec_VecUInt readDataVecVecUInt;
    float       errorf;
    double      errord;
    UInt        errorui;

    {
        __BNN_ASSERT(particleReader.getFixedAttribute("radius", readDataFloat));
        logger->printLog("Read float, err = " + NumberHelpers::formatToScientific(fabs(readDataFloat - dataFloat)));
    }

    {
        __BNN_ASSERT(particleReader.getFixedAttribute("origin", readDataVec3f));
        logger->printLog("Read Vec3f, err = " + NumberHelpers::formatToScientific(glm::length(readDataVec3f - dataVec3f)));
    }

    {
        __BNN_ASSERT(particleReader.getParticleAttribute("position", readDataVecVec3f1));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i)
            errorf = MathHelpers::max(errorf, glm::length(readDataVecVec3f1[i] - dataVecVec3f1[i]));
        logger->printLog("Read VecVec3f1 (uncompressed real), max err = " + NumberHelpers::formatToScientific(errorf));
    }

    {
        __BNN_ASSERT(particleReader.getParticleAttribute("velocity", readDataVecVec3f2));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i)
            errorf = MathHelpers::max(errorf, glm::length(readDataVecVec3f2[i] - dataVecVec3f2[i]));
        logger->printLog("Read VecVec3f2 (compressed real), max err = " + NumberHelpers::formatToScientific(errorf));
    }

    {
        __BNN_ASSERT(particleReader.getParticleAttribute("position_state", readDataVecVec3d1));
        errord = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i)
            errord = MathHelpers::max(errord, glm::length(readDataVecVec3d1[i] - dataVecVec3d1[i]));
        logger->printLog("Read VecVec3d1 (compressed real), max err = " + NumberHelpers::formatToScientific(errord));
    }
    {
        __BNN_ASSERT(particleReader.getParticleAttribute("velocity_state", readDataVecVec3d2));
        errord = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i)
            errord = MathHelpers::max(errord, glm::length(readDataVecVec3d2[i] - dataVecVec3d2[i]));
        logger->printLog("Read VecVec3d2 (uncompressed real), max err = " + NumberHelpers::formatToScientific(errord));
    }

    {
        __BNN_ASSERT(particleReader.getParticleAttribute("density", readDataVecFloat));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i)
            errorf = MathHelpers::max(errorf, glm::length(readDataVecFloat[i] - dataVecFloat[i]));
        logger->printLog("Read VecFloat (compressed real), max err = " + NumberHelpers::formatToScientific(errorf));
    }
    {
        __BNN_ASSERT(particleReader.getParticleAttribute("connection", readDataVecVecUInt));
        errorui = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            __BNN_ASSERT(dataVecVecUInt[i].size() == readDataVecVecUInt[i].size());
            for(size_t j = 0; j < DATA_SIZE; ++j)
                errorui = MathHelpers::max(errorui, dataVecVecUInt[i][j] - readDataVecVecUInt[i][j]);
        }
        logger->printLog("Read VecVecUInt, max err = " + NumberHelpers::formatToScientific(errorui));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test ReadWrite", "[Test ReadWrite]")
{
    Logger::initialize();

    for(Int i = 0; i < NUM_TEST; ++i)
        runTest(i);
}
