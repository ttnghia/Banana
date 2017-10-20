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

#include <cstdio>
#include <cstdlib>
#include <exception>

#include <Banana/Data/DataIO.h>
#include <Banana/Geometry/MeshLoader.h>
#include <Banana/Geometry/ObjLoader.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/CommandLineParser.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/NumberHelpers.h>

#include <SurfaceReconstruction/AniKernelGenerator.h>

using namespace Banana;
SharedPtr<Logger> logger;
CommandLineParser paramParser;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void printUsageAndExit()
{
    printf("\nWelcome!\n");
    printf("Usage: %s variableName1=variableValue1 variableName2=variableValue2...\n\n", paramParser.getProgramName().c_str());
    printf("Required variable: dataPath, particleRadius\n");
    printf("Optional variable: kernelRatio, convertFluid, convertMesh, convertOldData, startFrame, endFrame\n\n");

    exit(EXIT_FAILURE);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convertFluid()
{
    const String dataFolder("FluidFrame");
    String       dataPath;
    float        particleRadius;
    float        kernelRatio = 8.0f;
    Int          startFrame  = 1;
    Int          endFrame    = 10000;

    if(!paramParser.getString("dataPath", dataPath)) { printUsageAndExit(); }
    if(!paramParser.getReal("particleRadius", particleRadius)) { printUsageAndExit(); }

    paramParser.getInt("startFrame", startFrame);
    paramParser.getInt("endFrame", endFrame);
    paramParser.getReal("kernelRatio", kernelRatio);

    ////////////////////////////////////////////////////////////////////////////////
    UniquePtr<DataIO> outputPos       = std::make_unique<DataIO>(dataPath, dataFolder, "frame", "pos", "FramePosition");
    UniquePtr<DataIO> outputAniKernel = std::make_unique<DataIO>(dataPath, dataFolder, "frame", "ani", "FrameAniKernel");

    MeshLoader loader;
    UInt32     numConverted = 0;

    for(Int frame = startFrame; frame <= endFrame; ++frame) {
        char inFile[512];
        __BNN_SPRINT(inFile, "%s/Fluid/frame.%d.obj", dataPath.c_str(), frame);

        if(!FileHelpers::fileExisted(inFile)) {
            __BNN_SPRINT(inFile, "%s/Fluid/frame.%d.ply", dataPath.c_str(), frame);
            if(!FileHelpers::fileExisted(inFile)) {
                break;
            }
        }

        loader.loadMesh(inFile);

        Vec3f*                     particles = reinterpret_cast<Vec3f*>((void*)loader.getVertices().data());
        AnisotropicKernelGenerator aniKernelGenerator(static_cast<UInt>(loader.getNVertices()), particles, particleRadius, kernelRatio);
        aniKernelGenerator.generateAniKernels();

        outputPos->clearBuffer();
        outputPos->getBuffer().append(static_cast<UInt>(loader.getNVertices()));
        outputPos->getBuffer().append(particleRadius);
        outputPos->getBuffer().append(aniKernelGenerator.kernelCenters(), false);
        outputPos->flushBufferAsync(frame);

        outputAniKernel->clearBuffer();
        outputAniKernel->getBuffer().append(static_cast<UInt>(loader.getNVertices()));
        outputAniKernel->getBuffer().append((const unsigned char*)aniKernelGenerator.kernelMatrices().data(), sizeof(Mat3x3f) * loader.getNVertices());
        outputAniKernel->flushBufferAsync(frame);

        logger->printLog("Saved file: " + outputPos->getFilePath(frame));
        logger->printLog("Saved file: " + outputAniKernel->getFilePath(frame));

        ////////////////////////////////////////////////////////////////////////////////
        ++numConverted;
    }

    logger->newLine();
    logger->printLog("Finished conversion of " + NumberHelpers::formatWithCommas(numConverted) + " fluid file(s)");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convertMesh()
{
    const String dataFolder("SolidFrame");
    String       dataPath;
    Int          nMeshes    = 1;
    Int          startFrame = 1;
    Int          endFrame   = 10000;

    if(!paramParser.getString("dataPath", dataPath)) { printUsageAndExit(); }

    paramParser.getInt("nMeshes", nMeshes);
    paramParser.getInt("startFrame", startFrame);
    paramParser.getInt("endFrame", endFrame);

    ////////////////////////////////////////////////////////////////////////////////
    UniquePtr<DataIO> outputPos = std::make_unique<DataIO>(dataPath, dataFolder, "frame", "pos", "FramePosition");

#if 1
    Vector<UniquePtr<MeshLoader> > loader;
    for(Int i = 0; i < nMeshes; ++i) {
        loader.emplace_back(std::make_unique<MeshLoader>());
    }

    UInt32 numConverted = 0;
    for(Int frame = startFrame; frame <= endFrame; ++frame) {
        bool bFrameSuccess = true;
        outputPos->clearBuffer();
        outputPos->getBuffer().append(static_cast<UInt>(nMeshes));

        for(Int meshIdx = 0; meshIdx < nMeshes; ++meshIdx) {
            char inFile[512];
            __BNN_SPRINT(inFile, "%s/Solid/mesh%d.%d.obj", dataPath.c_str(), meshIdx + 1, frame);
            //__BNN_SPRINT(inFile, "%s/Solid/frame.%d.obj", dataPath.c_str(), frame);

            if(!FileHelpers::fileExisted(inFile)) {
                __BNN_SPRINT(inFile, "%s/Solid/mesh%d.%d.ply", dataPath.c_str(), meshIdx + 1, frame);
                if(!FileHelpers::fileExisted(inFile)) {
                    bFrameSuccess = false;
                    break;
                }
            }

            loader[meshIdx]->loadMesh(inFile);
            outputPos->getBuffer().append(static_cast<UInt>(loader[meshIdx]->getNFaceVertices()));
            outputPos->getBuffer().append((const unsigned char*)loader[meshIdx]->getFaceVertices().data(), loader[meshIdx]->getNFaceVertices() * sizeof(Vec3f));
            outputPos->getBuffer().append((const unsigned char*)loader[meshIdx]->getFaceVertexNormals().data(), loader[meshIdx]->getNFaceVertices() * sizeof(Vec3f));
        }

        if(!bFrameSuccess) {
            break;
        }


        outputPos->flushBufferAsync(frame);
        logger->printLog("Saved file: " + outputPos->getFilePath(frame));

        ////////////////////////////////////////////////////////////////////////////////
        ++numConverted;
    }
#else
    Vector<UniquePtr<OBJLoader> > loader;
    for(Int i = 0; i < nMeshes; ++i) {
        loader.emplace_back(std::make_unique<OBJLoader>());
    }

    UInt32    numConverted = 0;
    Vec_Vec3f vertices;
    Vec_Vec3f normals;

    for(Int frame = startFrame; frame <= endFrame; ++frame) {
        bool bFrameSuccess = true;
        outputPos->clearBuffer();
        outputPos->getBuffer().append(static_cast<UInt>(nMeshes));

        for(Int meshIdx = 0; meshIdx < nMeshes; ++meshIdx) {
            char inFile[512];
            //__BNN_SPRINT(inFile, "%s/Solid/mesh%d.%d.obj", dataPath.c_str(), meshIdx + 1, frame);
            __BNN_SPRINT(inFile, "%s/Solid/frame.%d.obj", dataPath.c_str(), frame);

            if(!FileHelpers::fileExisted(inFile)) {
                bFrameSuccess = false;
                break;
            }

            loader[meshIdx]->loadFromFileObj(inFile);
            vertices.clear();

            if(loader[meshIdx]->getNumFaces() > 0) {
                vertices.reserve(loader[meshIdx]->getNumFaces() * 3);
                loader[meshIdx]->computeNormals();
                normals.clear();
                normals.reserve(loader[meshIdx]->getNumFaces() * 3);

                for(int i = 0; i < loader[meshIdx]->getNumFaces(); ++i) {
                    OBJLoader::Face face = loader[meshIdx]->getFace(i);

                    for(int j = 0; j < 3; ++j) {
                        int   v_index = face.vertices[j];
                        Vec3f v       = loader[meshIdx]->getVertex(v_index);
                        vertices.push_back(v);

                        Vec3f n = loader[meshIdx]->vertexNormal(v_index);
                        normals.push_back(n);
                    }
                }
            } else {
                vertices.reserve(loader[meshIdx]->getNumVertices());

                for(int i = 0; i < loader[meshIdx]->getNumVertices(); ++i) {
                    Vec3f v = loader[meshIdx]->getVertex(i);
                    vertices.push_back(v);
                }
            }

            outputPos->getBuffer().append(static_cast<UInt>(vertices.size()));
            outputPos->getBuffer().append((const unsigned char*)vertices.data(), vertices.size() * 3 * sizeof(float));
            outputPos->getBuffer().append((const unsigned char*)normals.data(), normals.size() * 3 * sizeof(float));
        }

        if(!bFrameSuccess) {
            break;
        }


        outputPos->flushBufferAsync(frame);
        logger->printLog("Saved file: " + outputPos->getFilePath(frame));

        ////////////////////////////////////////////////////////////////////////////////
        ++numConverted;
    }
#endif

    logger->newLine();
    logger->printLog("Finished conversion of " + NumberHelpers::formatWithCommas(numConverted) + " mesh file(s)");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main(int argc, char** argv)
{
    ////////////////////////////////////////////////////////////////////////////////
    // firstly, parse parameters
    Logger::initialize();
    logger = Logger::create("Converter");
    String bConvertFluid("");
    String bConvertMesh("");

    paramParser.init(argc, argv);
    paramParser.getString("convertFluid", bConvertFluid);
    paramParser.getString("convertMesh",  bConvertMesh);

    if(bConvertFluid.empty() && bConvertMesh.empty()) {
        printUsageAndExit();
    }

    if(bConvertFluid == "true") {
        convertFluid();
    }

    if(bConvertMesh == "true") {
        convertMesh();
    }

    ////////////////////////////////////////////////////////////////////////////////
    return EXIT_SUCCESS;
}
