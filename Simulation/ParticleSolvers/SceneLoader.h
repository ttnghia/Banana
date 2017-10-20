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


template<Int N, class RealType> void loadSimulationObject(const nlohmann::json& jParams, const SharedPtr<SimulationObject<N, RealType> >& obj);
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
void loadSimulationObject(const nlohmann::json& jParams, const SharedPtr<SimulationObject<N, RealType> >& obj)
{
    __BNN_ASSERT(obj != nullptr);

    JSONHelpers::readValue(jParams, obj->meshFile(), "MeshFile");
    JSONHelpers::readValue(jParams, obj->particleFile(), "ParticleFile");
    JSONHelpers::readValue(jParams, obj->SDFFile(), "SDFFile");
    JSONHelpers::readBool(jParams, obj->useCache(), "UseCache");
    JSONHelpers::readBool(jParams, obj->isDynamic(), "IsDynamic");
    JSONHelpers::readBool(jParams, obj->fullShapeObj(), "FullShapeObj");

    VecX<N, Real>     translation;
    VecX<N + 1, Real> rotation;
    Real              scale;

    if(JSONHelpers::readVector(jParams, translation, "Translation")) {
        obj->getGeometry()->setTranslation(translation);
    }

    if(JSONHelpers::readVector(jParams, rotation, "Rotation")) {
        obj->getGeometry()->setRotation(rotation);
    }

    if(JSONHelpers::readValue(jParams, scale, "Scale")) {
        obj->getGeometry()->setUniformScale(scale);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    if(jParams.find("Animation") != jParams.end()) {
        for(auto& jKeyFrame : jParams["Animation"]) {
            KeyFrame<N, RealType> keyFrame;
            __BNN_ASSERT(JSONHelpers::readValue(jKeyFrame, keyFrame.frame, "Frame"));
            JSONHelpers::readVector(jKeyFrame, keyFrame.translation, "Translation");
            JSONHelpers::readVector(jKeyFrame, keyFrame.rotation, "Rotation");
            JSONHelpers::readValue(jKeyFrame, keyFrame.uniformScale, "Scale");

            obj->getGeometry()->getAnimation().addKeyFrame(keyFrame);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // specialized for box object
    auto box = dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(obj->getGeometry());
    if(box != nullptr) {
        VecX<N, Real> bMin, bMax;
        if(JSONHelpers::readVector(jParams, bMin, "BoxMin") && JSONHelpers::readVector(jParams, bMax, "BoxMax")) {
            box->setOriginalBox(bMin, bMax);
        }

        if(jParams.find("Animation") != jParams.end()) {
            for(auto& jKeyFrame : jParams["Animation"]) {
                UInt frame;
                __BNN_ASSERT(JSONHelpers::readValue(jKeyFrame, frame, "Frame"));
                if(JSONHelpers::readVector(jKeyFrame, bMin, "BoxMin") && JSONHelpers::readVector(jKeyFrame, bMax, "BoxMax")) {
                    box->addKeyFrame(frame, bMin, bMax);
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadBoundaryObjects(const nlohmann::json& jParams, Vector<SharedPtr<BoundaryObject<N, RealType> > >& boundaryObjs)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        SharedPtr<BoundaryObject<N, RealType> > obj = nullptr;
        if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
            obj = std::make_shared<BoxBoundary<N, RealType> >();
        } else {
            obj = std::make_shared<BoundaryObject<N, RealType> >(geometryType);
        }
        boundaryObjs.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleGenerators(const nlohmann::json& jParams, Vector<SharedPtr<ParticleGenerator<N, RealType> > >& particleGenerators)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        auto obj = std::make_shared<ParticleGenerator<N, RealType> >(geometryType);
        particleGenerators.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));

        JSONHelpers::readVector(jObj, obj->v0(), "InitialVelocity");
        JSONHelpers::readValue(jObj, obj->minDistanceRatio(), "MinParticleDistanceRatio");
        JSONHelpers::readValue(jObj, obj->jitter(), "JitterRatio");
        JSONHelpers::readValue(jObj, obj->startFrame(), "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(), "MaxFrame");
        JSONHelpers::readValue(jObj, obj->maxNParticles(), "MaxNParticles");
        JSONHelpers::readValue(jObj, obj->maxSamplingIters(), "MaxSamplingIters");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleRemovers(const nlohmann::json& jParams, Vector<SharedPtr<ParticleRemover<N, RealType> > >& particleRemovers)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        auto obj = std::make_shared<ParticleRemover<N, RealType> >(geometryType);
        particleRemovers.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));

        JSONHelpers::readValue(jObj, obj->startFrame(), "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(), "MaxFrame");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SceneLoader

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana