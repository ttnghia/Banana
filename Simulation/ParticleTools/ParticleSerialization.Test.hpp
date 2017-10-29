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

#include <ParticleTools/ParticleSerialization.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <catch.hpp>
#include <random>

using namespace Banana;

#define NUM_TEST         1000
#define DATA_SIZE        1000
#define ERROR_THRESTHOLD 1e-3
#define FLOAT_TEST_RANGE 1.0
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void runTest(Int testID)
{
    auto logger = Logger::create("Tester_" + std::to_string(testID));
    logger->printTextBox("Test " + std::to_string(testID));

    ////////////////////////////////////////////////////////////////////////////////
    // create test data
    std::random_device                     rd;
    std::mt19937                           gen(rd());
    std::uniform_real_distribution<float>  disf1(0, FLOAT_TEST_RANGE);
    std::uniform_real_distribution<float>  disf2(100, 100 + FLOAT_TEST_RANGE);
    std::uniform_real_distribution<float>  disf3(200, 200 + FLOAT_TEST_RANGE);
    std::uniform_real_distribution<double> disd1(300, 300 + FLOAT_TEST_RANGE);
    std::uniform_real_distribution<double> disd2(400, 400 + FLOAT_TEST_RANGE);
    std::uniform_int_distribution<UInt>    disi(0, 10000);
    std::uniform_int_distribution<UInt>    disb(0, 10);

    Real  dataFloat = disf1(gen);
    Vec3f dataVec3f(disf1(gen), disf1(gen), disf1(gen));

    Vec_Vec3f   dataVecVec3f1(DATA_SIZE);
    Vec_Vec3f   dataVecVec3f2(DATA_SIZE);
    Vec_Vec3d   dataVecVec3d1(DATA_SIZE);
    Vec_Vec3d   dataVecVec3d2(DATA_SIZE);
    Vec_Float   dataVecFloat(DATA_SIZE);
    Vec_VecUInt dataVecVecUInt(DATA_SIZE);

    for(size_t i = 0; i < DATA_SIZE; ++i) {
        dataVecVec3f1[i] = Vec3f(disf1(gen));
        dataVecVec3f2[i] = Vec3f(disf2(gen));
        dataVecVec3d1[i] = Vec3d(disd1(gen));
        dataVecVec3d2[i] = Vec3d(disd2(gen));
        dataVecFloat[i]  = disf3(gen);

        dataVecVecUInt[i].reserve(1000);
        for(size_t j = 0; j < 1000; ++j) {
            dataVecVecUInt[i].push_back(disi(gen));
        }
    }

    Vector<String> attrNames(8);
    Vector<String> readAttrNames;
    bool           bReadAll = (disb(gen) < 5);
    logger->printWarning(String("ReadAll = ") + (bReadAll ? String("Yes") : String("No")));

    bool bReadAttrs[8];
    for(Int i = 0; i < 8; ++i) {
        bReadAttrs[i] = true;
        attrNames[i]  = String("Attribute_" + std::to_string(disi(gen)) + "_" + std::to_string(i));
    }

    if(!bReadAll) {
        for(Int i = 0; i < 8; ++i) {
            if(disb(gen) < 5) {
                readAttrNames.push_back(attrNames[i]);
                bReadAttrs[i] = true;
            } else {
                bReadAttrs[i] = false;
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // write
    ParticleSerialization particleWriter("./Scratch", "Frames", "frame", logger);
    particleWriter.addFixedAttribute(attrNames[0], ParticleSerialization::TypeReal, ParticleSerialization::Size32b);
    particleWriter.addFixedAttribute(attrNames[1], ParticleSerialization::TypeReal, ParticleSerialization::Size32b, 3);

    particleWriter.addParticleAttribute(attrNames[2], ParticleSerialization::TypeReal,           ParticleSerialization::Size32b, 3);
    particleWriter.addParticleAttribute(attrNames[3], ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size32b, 3);
    particleWriter.addParticleAttribute(attrNames[4], ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size64b, 3);
    particleWriter.addParticleAttribute(attrNames[5], ParticleSerialization::TypeReal,           ParticleSerialization::Size64b, 3);
    particleWriter.addParticleAttribute(attrNames[6], ParticleSerialization::TypeCompressedReal, ParticleSerialization::Size32b);
    particleWriter.addParticleAttribute(attrNames[7], ParticleSerialization::TypeVectorUInt,     ParticleSerialization::Size32b);


    particleWriter.setNParticles(DATA_SIZE);
    particleWriter.setFixedAttribute(attrNames[0], dataFloat);
    particleWriter.setFixedAttribute(attrNames[1], dataVec3f);

    particleWriter.setParticleAttribute(attrNames[2], dataVecVec3f1);
    particleWriter.setParticleAttribute(attrNames[3], dataVecVec3f2);
    particleWriter.setParticleAttribute(attrNames[4], dataVecVec3d1);
    particleWriter.setParticleAttribute(attrNames[5], dataVecVec3d2);
    particleWriter.setParticleAttribute(attrNames[6], dataVecFloat);
    particleWriter.setParticleAttribute(attrNames[7], dataVecVecUInt);

    particleWriter.flushAsync(testID);
    particleWriter.waitForBuffers();

    ////////////////////////////////////////////////////////////////////////////////
    // read
    ParticleSerialization particleReader("./Scratch", "Frames", "frame", logger);
    bool                  bRead = false;
    while(!bRead) {
        bRead = (bReadAll ? particleReader.read(testID) : particleReader.read(testID, readAttrNames));
    }
    logger->printLog("Read NParticles: " + NumberHelpers::formatWithCommas(particleReader.getNParticles()));

    for(auto& kv : particleReader.getFixedAttributes()) {
        logger->printLog("Fixed Attr: " + kv.first);
    }
    for(auto& kv : particleReader.getParticleAttributes()) {
        logger->printLog("Particle Attr: " + kv.first);
    }

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

    if(bReadAttrs[0]) {
        REQUIRE(particleReader.getFixedAttribute(attrNames[0], readDataFloat));
        errorf = fabs(readDataFloat - dataFloat);
        logger->printLog("Read float, err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < Tiny);

        REQUIRE(particleReader.getFixedAttribute(attrNames[0], &readDataFloat));
        errorf = fabs(readDataFloat - dataFloat);
        logger->printLog("Read float using pointer, err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < Tiny);
    }

    if(bReadAttrs[1]) {
        REQUIRE(particleReader.getFixedAttribute(attrNames[1], readDataVec3f));
        errorf = glm::length(readDataVec3f - dataVec3f);
        logger->printLog("Read Vec3f, err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < Tiny);

        REQUIRE(particleReader.getFixedAttribute(attrNames[1], (float*)glm::value_ptr(readDataVec3f)));
        errorf = glm::length(readDataVec3f - dataVec3f);
        logger->printLog("Read Vec3f, err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < Tiny);
    }

    if(bReadAttrs[2]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[2], readDataVecVec3f1));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            errorf = MathHelpers::max(errorf, glm::length(readDataVecVec3f1[i] - dataVecVec3f1[i]));
        }
        logger->printLog("Read VecVec3f1 (uncompressed real), max err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < Tiny);
    }

    if(bReadAttrs[3]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[3], readDataVecVec3f2));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            errorf = MathHelpers::max(errorf, glm::length(readDataVecVec3f2[i] - dataVecVec3f2[i]));
        }
        logger->printLog("Read VecVec3f2 (compressed real), max err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < ERROR_THRESTHOLD);
    }

    if(bReadAttrs[4]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[4], readDataVecVec3d1));
        errord = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            errord = MathHelpers::max(errord, glm::length(readDataVecVec3d1[i] - dataVecVec3d1[i]));
        }
        logger->printLog("Read VecVec3d1 (compressed real), max err = " + NumberHelpers::formatToScientific(errord));
        REQUIRE(errord < ERROR_THRESTHOLD);
    }

    if(bReadAttrs[5]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[5], readDataVecVec3d2));
        errord = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            errord = MathHelpers::max(errord, glm::length(readDataVecVec3d2[i] - dataVecVec3d2[i]));
        }
        logger->printLog("Read VecVec3d2 (uncompressed real), max err = " + NumberHelpers::formatToScientific(errord));
        REQUIRE(errord < Tiny);
    }

    if(bReadAttrs[6]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[6], readDataVecFloat));
        errorf = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            errorf = MathHelpers::max(errorf, glm::length(readDataVecFloat[i] - dataVecFloat[i]));
        }
        logger->printLog("Read VecFloat (compressed real), max err = " + NumberHelpers::formatToScientific(errorf));
        REQUIRE(errorf < ERROR_THRESTHOLD);
    }

    if(bReadAttrs[7]) {
        REQUIRE(particleReader.getParticleAttribute(attrNames[7], readDataVecVecUInt));
        errorui = 0;
        for(size_t i = 0; i < DATA_SIZE; ++i) {
            REQUIRE(dataVecVecUInt[i].size() == readDataVecVecUInt[i].size());
            for(size_t j = 0; j < 1000; ++j) {
                errorui = MathHelpers::max(errorui, dataVecVecUInt[i][j] - readDataVecVecUInt[i][j]);
            }
        }
        logger->printLog("Read VecVecUInt, max err = " + NumberHelpers::formatToScientific(errorui));
        REQUIRE(errorui == 0);
    }

    logger->newLine();
    logger->newLine();
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TEST_CASE("Test ReadWrite", "[Test ReadWrite]")
{
    Logger::initialize();

    for(Int i = 0; i < NUM_TEST; ++i) {
        runTest(i);
    }
}
