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

#include <Banana/Utils/JSONHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <SimulationObjects/BoundaryObject.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <SimulationObjects/ParticleRemover.h>

#include <json.hpp>
#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SceneLoader
{
using namespace SimulationObjects;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool loadDataPath(const String& sceneFile, String& dataPath);
void loadGlobalParams(const nlohmann::json& jParams, ParticleSolvers::GlobalParameters& globalParams);

template<Int N, class RealType> void loadBoundaryObjects(const nlohmann::json& jParams, Vector<SharedPtr<BoundaryObject<N, RealType> > >& boundaryObjs);
template<Int N, class RealType> void loadParticleGenerators(const nlohmann::json& jParams, Vector<SharedPtr<ParticleGenerator<N, RealType> > >& particleGenerators);
template<Int N, class RealType> void loadParticleRemovers(const nlohmann::json& jParams, Vector<SharedPtr<ParticleRemover<N, RealType> > >& particleRemovers);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementations
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool loadDataPath(const String& sceneFile, String& dataPath)
{
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open()) {
        return false;
    }

    nlohmann::json jParams = nlohmann::json::parse(inputFile);
    if(jParams.find("GlobalParameters") == jParams.end()) {
        return false;
    } else {
        return JSONHelpers::readValue(jParams, dataPath, "DataPath");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void loadGlobalParams(const nlohmann::json& jParams, ParticleSolvers::GlobalParameters& globalParams)
{
    JSONHelpers::readValue(jParams, globalParams.frameDuration, "FrameDuration");
    JSONHelpers::readValue(jParams, globalParams.finalFrame, "FinalFrame");
    JSONHelpers::readValue(jParams, globalParams.nThreads, "NThreads");

    JSONHelpers::readBool(jParams, globalParams.bApplyGravity, "ApplyGravity");
    JSONHelpers::readBool(jParams, globalParams.bEnableSortParticle, "EnableSortParticle");
    JSONHelpers::readValue(jParams, globalParams.sortFrequency, "SortFrequency");

    JSONHelpers::readBool(jParams, globalParams.bLoadMemoryState, "LoadMemoryState");
    JSONHelpers::readBool(jParams, globalParams.bSaveFrameData, "SaveFrameData");
    JSONHelpers::readBool(jParams, globalParams.bSaveMemoryState, "SaveMemoryState");
    JSONHelpers::readBool(jParams, globalParams.bPrintLog2File, "PrintLogToFile");
    JSONHelpers::readValue(jParams, globalParams.framePerState, "FramePerState");
    JSONHelpers::readValue(jParams, globalParams.dataPath, "DataPath");
    JSONHelpers::readVector(jParams, globalParams.optionalSavingData, "OptionalSavingData");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadBoundaryObjects(const nlohmann::json& jParams, Vector<SharedPtr<BoundaryObject<N, RealType> > >& boundaryObjs)
{
    for(auto& jObj : jParams) {
// read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<BoundaryObject<N, RealType> > obj = nullptr;
        if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
            obj = std::make_shared<BoxBoundary<N, RealType> >();
        } else {
            obj = std::make_shared<BoundaryObject<N, RealType> >(geometryType);
        }
        boundaryObjs.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj, obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj, obj->particleFile(), "ParticleFile");
        JSONHelpers::readValue(jObj, obj->SDFFile(), "SDFFile");
        JSONHelpers::readBool(jObj, obj->useCache(), "UseCache");
        JSONHelpers::readBool(jObj, obj->isDynamic(), "IsDynamic");

        // read object transformation
        VecX<N, Real> translation;
        VecX<N, Real> rotationAxis;
        Real          rotationAngle;
        Real          scale;

        if(JSONHelpers::readVector(jObj, translation, "Translation")) {
            obj->getGeometry()->setTranslation(translation);
        }

        if(JSONHelpers::readVector(jObj, rotationAxis, "RotationAxis") && JSONHelpers::readValue(jObj, rotationAngle, "RotationAngle")) {
            obj->getGeometry()->setRotation(rotationAxis, rotationAngle);
        }

        if(JSONHelpers::readValue(jObj, scale, "Scale")) {
            obj->getGeometry()->setUniformScale(scale);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleGenerators(const nlohmann::json& jParams, Vector<SharedPtr<ParticleGenerator<N, RealType> > >& particleGenerators)
{
    for(auto& jObj : jParams) {
// read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        auto obj = std::make_shared<ParticleGenerator<N, RealType> >(geometryType);
        particleGenerators.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj, obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj, obj->particleFile(), "ParticleFile");
        JSONHelpers::readValue(jObj, obj->SDFFile(), "SDFFile");
        JSONHelpers::readBool(jObj, obj->useCache(), "UseCache");
        JSONHelpers::readBool(jObj, obj->isDynamic(), "IsDynamic");

        JSONHelpers::readVector(jObj, obj->v0(), "InitialVelocity");
        JSONHelpers::readValue(jObj, obj->minDistanceRatio(), "MinParticleDistanceRatio");
        JSONHelpers::readValue(jObj, obj->jitter(), "JitterRatio");
        JSONHelpers::readValue(jObj, obj->startFrame(), "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(), "MaxFrame");
        JSONHelpers::readValue(jObj, obj->maxNParticles(), "MaxNParticles");
        JSONHelpers::readValue(jObj, obj->maxSamplingIters(), "MaxSamplingIters");
        JSONHelpers::readBool(jObj, obj->fullShapeObj(), "FullShapeObj");

        // read object transformation
        VecX<N, Real> translation;
        VecX<N, Real> rotationAxis;
        Real          rotationAngle;
        Real          scale;

        if(JSONHelpers::readVector(jObj, translation, "Translation")) {
            obj->getGeometry()->setTranslation(translation);
        }

        if(JSONHelpers::readVector(jObj, rotationAxis, "RotationAxis") && JSONHelpers::readValue(jObj, rotationAngle, "RotationAngle")) {
            obj->getGeometry()->setRotation(rotationAxis, rotationAngle);
        }

        if(JSONHelpers::readValue(jObj, scale, "Scale")) {
            obj->getGeometry()->setUniformScale(scale);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleRemovers(const nlohmann::json& jParams, Vector<SharedPtr<ParticleRemover<N, RealType> > >& particleRemovers)
{
    for(auto& jObj : jParams) {
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        auto obj = std::make_shared<ParticleRemover<N, RealType> >(geometryType);
        particleRemovers.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj, obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj, obj->particleFile(), "ParticleFile");
        JSONHelpers::readValue(jObj, obj->SDFFile(), "SDFFile");
        JSONHelpers::readBool(jObj, obj->useCache(), "UseCache");
        JSONHelpers::readBool(jObj, obj->isDynamic(), "IsDynamic");

        JSONHelpers::readValue(jObj, obj->startFrame(), "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(), "MaxFrame");
        JSONHelpers::readBool(jObj, obj->fullShapeObj(), "FullShapeObj");

        // read object transformation
        VecX<N, Real> translation;
        VecX<N, Real> rotationAxis;
        Real          rotationAngle;
        Real          scale;

        if(JSONHelpers::readVector(jObj, translation, "Translation")) {
            obj->getGeometry()->setTranslation(translation);
        }

        if(JSONHelpers::readVector(jObj, rotationAxis, "RotationAxis") && JSONHelpers::readValue(jObj, rotationAngle, "RotationAngle")) {
            obj->getGeometry()->setRotation(rotationAxis, rotationAngle);
        }

        if(JSONHelpers::readValue(jObj, scale, "Scale")) {
            obj->getGeometry()->setUniformScale(scale);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SceneLoader

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana