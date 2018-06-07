//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <SimulationObjects/SimulationObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline SimulationObject<N, RealType>::SimulationObject(const JParams& jParams, bool bCSGObj) :
    m_jParams(jParams), m_NameID(String(String("Object_") + std::to_string(NumberHelpers::generateRandomInt<Int>())))
{
    if(bCSGObj) {
        m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>("CSGObject");
    } else {
        String geometryType;
        __BNN_REQUIRE(JSONHelpers::readValue(jParams, geometryType, "GeometryType"));
        m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>(geometryType);
    }
    __BNN_REQUIRE(m_GeometryObj != nullptr);
    parseParameters(jParams);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationObject<N, RealType>::parseParameters(const JParams& jParams)
{
    if(jParams.is_null()) {
        return;
    }
    JSONHelpers::readValue(jParams, nameID(),       "UniqueName");
    ////////////////////////////////////////////////////////////////////////////////
    JSONHelpers::readValue(jParams, meshFile(),     "MeshFile");
    JSONHelpers::readValue(jParams, particleFile(), "ParticleFile");
    JSONHelpers::readBool(jParams, useCache(),     "UseCache");
    JSONHelpers::readBool(jParams, fullShapeObj(), "FullShapeObj");

    VecX<N, Real>     translation;
    VecX<N, Real>     rotationEulerAngles;
    VecX<N + 1, Real> rotationAxisAngle;
    Real              scale;

    if(JSONHelpers::readVector(jParams, translation, "Translation")) {
        geometry()->setTranslation(translation);
    }

    if(JSONHelpers::readVector(jParams, rotationEulerAngles, "RotationEulerAngles") ||
       JSONHelpers::readVector(jParams, rotationEulerAngles, "RotationEulerAngle")) {
        geometry()->setRotation(MathHelpers::EulerToAxisAngle(rotationEulerAngles, false));
    } else if(JSONHelpers::readVector(jParams, rotationAxisAngle, "RotationAxisAngle")) {
        rotationAxisAngle[N] = glm::radians(rotationAxisAngle[N]);
        geometry()->setRotation(rotationAxisAngle);
    }

    if(JSONHelpers::readValue(jParams, scale, "Scale")) {
        geometry()->setUniformScale(scale);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    if(jParams.find("Animation") != jParams.end()) {
        isDynamic() = true;
        auto  jAnimation = jParams["Animation"];
        auto& aniObj     = geometry()->getAnimation();

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
            if(JSONHelpers::readVector(jKeyFrame, rotationEulerAngles, "RotationEulerAngles") ||
               JSONHelpers::readVector(jKeyFrame, rotationEulerAngles, "RotationEulerAngle")) {
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
    auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(geometry());
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
    auto meshObj = std::dynamic_pointer_cast<GeometryObjects::TriMeshObject<N, RealType>>(geometry());
    if(meshObj != nullptr) {
        __BNN_REQUIRE(JSONHelpers::readValue(jParams, meshObj->meshFile(), "MeshFile"));
        JSONHelpers::readValue(jParams, meshObj->sdfStep(), "SDFStep");
        meshObj->computeSDF();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class SimulationObject<2, Real>;
template class SimulationObject<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
