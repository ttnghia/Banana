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

#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <SimulationObjects/BoundaryObject.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <SimulationObjects/HairObjectGenerator.h>
#include <SimulationObjects/ParticleRemover.h>

#include <json.hpp>
#include <fstream>
#include <unordered_set>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SceneLoader
{
using namespace SimulationObjects;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool loadDataPath(const String& sceneFile, String& dataPath);
void loadGlobalParams(const JParams& jParams, ParticleSolvers::GlobalParameters& globalParams);

template<Int N, class RealType> void loadGeneralSolverParams(const JParams& jParams, ParticleSolvers::SimulationParameters<N, RealType>& solverParams);
template<Int N, class RealType> void loadSimulationObject(const JParams& jParams, const SharedPtr<SimulationObject<N, RealType> >& obj);
template<Int N, class RealType> void loadBoundaryObjects(const JParams& jParams, Vector<SharedPtr<BoundaryObject<N, RealType> > >& boundaryObjs);
template<Int N, class RealType> void loadParticleGenerators(const JParams& jParams, Vector<SharedPtr<ParticleGenerator<N, RealType> > >& particleGenerators);
template<Int N, class RealType> void loadParticleRemovers(const JParams& jParams, Vector<SharedPtr<ParticleRemover<N, RealType> > >& particleRemovers);

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

    JParams jParams = JParams::parse(inputFile);
    if(jParams.find("GlobalParameters") == jParams.end()) {
        return false;
    } else {
        return JSONHelpers::readValue(jParams, dataPath, "DataPath");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void loadGlobalParams(const JParams& jParams, ParticleSolvers::GlobalParameters& globalParams)
{
    JSONHelpers::readValue(jParams, globalParams.nThreads, "NThreads");

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    JSONHelpers::readValue(jParams, globalParams.frameDuration, "FrameDuration");
    JSONHelpers::readValue(jParams, globalParams.startFrame,    "StartFrame");
    JSONHelpers::readValue(jParams, globalParams.finalFrame,    "FinalFrame");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data IO parameters
    JSONHelpers::readValue(jParams, globalParams.dataPath,              "DataPath");
    JSONHelpers::readValue(jParams, globalParams.memoryStateDataFolder, "MemoryStateDataFolder");
    JSONHelpers::readValue(jParams, globalParams.frameDataFolder,       "FrameDataFolder");
    JSONHelpers::readBool(jParams, globalParams.bLoadMemoryState, "LoadMemoryState");
    JSONHelpers::readBool(jParams, globalParams.bSaveMemoryState, "SaveMemoryState");
    JSONHelpers::readBool(jParams, globalParams.bSaveFrameData,   "SaveFrameData");
    JSONHelpers::readBool(jParams, globalParams.bSaveSubstepData, "SaveSubstepData");
    JSONHelpers::readValue(jParams, globalParams.framePerState, "FramePerState");
    JSONHelpers::readVector(jParams, globalParams.SaveDataList, "OptionalSavingData");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // logging parameters
    String logLevel = String("Trace");
    JSONHelpers::readValue(jParams, logLevel, "LogLevel");
    if(logLevel == "Debug") {
        globalParams.logLevel = spdlog::level::debug;
    } else {
        globalParams.logLevel = spdlog::level::trace;
    }
    JSONHelpers::readBool(jParams, globalParams.bPrintLog2File, "PrintLogToFile");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // misc parameters
    JSONHelpers::readBool(jParams, globalParams.bApplyGravity,       "ApplyGravity");
    JSONHelpers::readBool(jParams, globalParams.bEnableSortParticle, "EnableSortParticle");
    JSONHelpers::readValue(jParams, globalParams.sortFrequency, "SortFrequency");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadGeneralSolverParams(const JParams& jParams, ParticleSolvers::SimulationParameters<N, RealType>& solverParams)
{
    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    JSONHelpers::readValue(jParams, solverParams.minTimestep, "MinTimestep");
    JSONHelpers::readValue(jParams, solverParams.maxTimestep, "MaxTimestep");
    JSONHelpers::readValue(jParams, solverParams.CFLFactor,   "CFLFactor");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    JSONHelpers::readValue(jParams, solverParams.particleRadius,       "ParticleRadius");
    JSONHelpers::readValue(jParams, solverParams.cellSize,             "CellSize");
    JSONHelpers::readValue(jParams, solverParams.ratioCellSizePRadius, "RatioCellSizePRadius");
    JSONHelpers::readValue(jParams, solverParams.nExpandCells,         "NExpandCells");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    JSONHelpers::readValue(jParams, solverParams.maxNParticles,  "MaxNParticles");
    JSONHelpers::readValue(jParams, solverParams.advectionSteps, "AdvectionSteps");
    JSONHelpers::readBool(jParams, solverParams.bCorrectPosition, "CorrectPosition");
    JSONHelpers::readValue(jParams, solverParams.repulsiveForceStiffness, "RepulsiveForceStiffness");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    JSONHelpers::readValue(jParams, solverParams.CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams.maxCGIteration,      "MaxCGIteration");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data only for PIC/FLIP blending, if applicable
    JSONHelpers::readValue(jParams, solverParams.PIC_FLIP_ratio, "PIC_FLIP_ratio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    JSONHelpers::readValue(jParams, solverParams.boundaryRestitution, "BoundaryRestitution");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
void loadSimulationObject(const JParams& jParams, const SharedPtr<SimulationObject<N, RealType> >& obj)
{
    __BNN_REQUIRE(obj != nullptr);
    __BNN_REQUIRE(JSONHelpers::readValue(jParams, obj->nameID(), "UniqueName"));

    ////////////////////////////////////////////////////////////////////////////////
    JSONHelpers::readValue(jParams, obj->meshFile(),     "MeshFile");
    JSONHelpers::readValue(jParams, obj->particleFile(), "ParticleFile");
    JSONHelpers::readBool(jParams, obj->useCache(),     "UseCache");
    JSONHelpers::readBool(jParams, obj->fullShapeObj(), "FullShapeObj");

    VecX<N, Real>     translation;
    VecX<N, Real>     rotationEulerAngles;
    VecX<N + 1, Real> rotationAxisAngle;
    Real              scale;

    if(JSONHelpers::readVector(jParams, translation, "Translation")) {
        obj->geometry()->setTranslation(translation);
    }

    if(JSONHelpers::readVector(jParams, rotationEulerAngles, "RotationEulerAngles") || JSONHelpers::readVector(jParams, rotationEulerAngles, "RotationEulerAngle")) {
        obj->geometry()->setRotation(MathHelpers::EulerToAxisAngle(rotationEulerAngles, false));
    } else if(JSONHelpers::readVector(jParams, rotationAxisAngle, "RotationAxisAngle")) {
        rotationAxisAngle[N] = glm::radians(rotationAxisAngle[N]);
        obj->geometry()->setRotation(rotationAxisAngle);
    }

    if(JSONHelpers::readValue(jParams, scale, "Scale")) {
        obj->geometry()->setUniformScale(scale);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    if(jParams.find("Animation") != jParams.end()) {
        obj->isDynamic() = true;
        auto  jAnimation = jParams["Animation"];
        auto& aniObj     = obj->geometry()->getAnimation();

        bool bCubicInterpolationTranslation = true;
        bool bCubicInterpolationRotation    = true;
        bool bCubicInterpolationScale       = true;
        bool bPeriodic                      = false;
        UInt startFrame                     = 0;

        JSONHelpers::readBool(jAnimation, bCubicInterpolationTranslation, "CubicInterpolationTranslation");
        JSONHelpers::readBool(jAnimation, bCubicInterpolationRotation,    "CubicInterpolationRotation");
        JSONHelpers::readBool(jAnimation, bCubicInterpolationScale,       "CubicInterpolationScale");
        if(JSONHelpers::readBool(jAnimation, bPeriodic, "Periodic")) {
            JSONHelpers::readValue(jAnimation, startFrame, "StartFrame");
            aniObj.setPeriodic(bPeriodic, startFrame);
        }
        __BNN_REQUIRE(jAnimation.find("KeyFrames") != jAnimation.end());
        for(auto& jKeyFrame : jAnimation["KeyFrames"]) {
            KeyFrame<N, RealType> keyFrame;
            __BNN_REQUIRE(JSONHelpers::readValue(jKeyFrame, keyFrame.frame, "Frame"));
            JSONHelpers::readVector(jKeyFrame, keyFrame.translation, "Translation");

            VecX<N, Real> rotationEulerAngles;
            if(JSONHelpers::readVector(jKeyFrame, rotationEulerAngles, "RotationEulerAngles") || JSONHelpers::readVector(jKeyFrame, rotationEulerAngles, "RotationEulerAngle")) {
                keyFrame.rotation = MathHelpers::EulerToAxisAngle(rotationEulerAngles, false, true);
            } else {
                JSONHelpers::readVector(jKeyFrame, keyFrame.rotation, "RotationAxisAngle");
                keyFrame.rotation = glm::radians(keyFrame.rotation);
            }

            JSONHelpers::readValue(jKeyFrame, keyFrame.uniformScale, "Scale");
            aniObj.addKeyFrame(keyFrame);
        }

        aniObj.makeReady(bCubicInterpolationTranslation, bCubicInterpolationRotation, bCubicInterpolationScale);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // specialized for box object
    auto box = dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(obj->geometry());
    if(box != nullptr) {
        VecX<N, Real> bMin, bMax;
        if(JSONHelpers::readVector(jParams, bMin, "BoxMin") && JSONHelpers::readVector(jParams, bMax, "BoxMax")) {
            box->setOriginalBox(bMin, bMax);
        }

        if(jParams.find("Animation") != jParams.end()) {
            auto jAnimation = jParams["Animation"];
            bool bPeriodic  = false;
            UInt startFrame = 0;

            if(JSONHelpers::readBool(jAnimation, bPeriodic, "Periodic")) {
                JSONHelpers::readValue(jAnimation, startFrame, "StartFrame");
                box->setPeriodic(bPeriodic, startFrame);
            }

            __BNN_REQUIRE(jAnimation.find("KeyFrames") != jAnimation.end());
            for(auto& jKeyFrame : jAnimation["KeyFrames"]) {
                UInt frame;
                __BNN_REQUIRE(JSONHelpers::readValue(jKeyFrame, frame, "Frame"));
                if(JSONHelpers::readVector(jKeyFrame, bMin, "BoxMin") && JSONHelpers::readVector(jKeyFrame, bMax, "BoxMax")) {
                    box->addKeyFrame(frame, bMin, bMax);
                }
            }

            box->makeReadyAnimation();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // specialized for trimesh object
    auto meshObj = dynamic_pointer_cast<GeometryObjects::TriMeshObject<N, RealType> >(obj->geometry());
    if(meshObj != nullptr) {
        __BNN_REQUIRE(JSONHelpers::readValue(jParams, meshObj->meshFile(), "MeshFile"));
        JSONHelpers::readValue(jParams, meshObj->sdfStep(), "SDFStep");
        meshObj->computeSDF();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadBoundaryObjects(const JParams& jParams, Vector<SharedPtr<BoundaryObject<N, RealType> > >& boundaryObjs)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_REQUIRE(!geometryType.empty());

        SharedPtr<BoundaryObject<N, RealType> > obj = nullptr;
        if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
            obj = std::make_shared<BoxBoundary<N, RealType> >(jObj);
        } else {
            obj = std::make_shared<BoundaryObject<N, RealType> >(jObj, geometryType);
        }
        boundaryObjs.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleGenerators(const JParams& jParams, Vector<SharedPtr<ParticleGenerator<N, RealType> > >& particleGenerators)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_REQUIRE(!geometryType.empty());

        auto obj = std::make_shared<ParticleGenerator<N, RealType> >(jObj, geometryType);
        particleGenerators.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));

        JSONHelpers::readVector(jObj, obj->v0(), "InitialVelocity");
        JSONHelpers::readValue(jObj, obj->minDistanceRatio(),   "MinParticleDistanceRatio");
        JSONHelpers::readValue(jObj, obj->jitter(),             "JitterRatio");
        JSONHelpers::readValue(jObj, obj->startFrame(),         "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(),           "MaxFrame");
        JSONHelpers::readValue(jObj, obj->maxNParticles(),      "MaxNParticles");
        JSONHelpers::readValue(jObj, obj->activeFrames(),       "ActiveFrames");
        JSONHelpers::readValue(jObj, obj->maxSamplingIters(),   "MaxSamplingIters");
        JSONHelpers::readValue(jObj, obj->constraintObjectID(), "ConstraintObjectID");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void loadParticleRemovers(const JParams& jParams, Vector<SharedPtr<ParticleRemover<N, RealType> > >& particleRemovers)
{
    for(auto& jObj : jParams) {
        String geometryType;
        __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_REQUIRE(!geometryType.empty());

        auto obj = std::make_shared<ParticleRemover<N, RealType> >(jObj, geometryType);
        particleRemovers.push_back(obj);
        loadSimulationObject(jObj, static_pointer_cast<SimulationObject<N, RealType> >(obj));

        JSONHelpers::readValue(jObj, obj->startFrame(), "StartFrame");
        JSONHelpers::readValue(jObj, obj->maxFrame(),   "MaxFrame");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SceneLoader

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana