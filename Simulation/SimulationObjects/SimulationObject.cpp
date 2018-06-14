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
#include <Banana/Utils/FileHelpers.h>
#include <ParticleTools/ParticleHelpers.h>
#include <SimulationObjects/SimulationObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline SimulationObject<N, RealType>::SimulationObject(const JParams& jParams, bool bCSGObj) :
    m_jParams(jParams), m_NameID(String(String("Object_") + std::to_string(NumberHelpers::iRand<Int>::rnd())))
{
    if(bCSGObj) {
        m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>("CSGObject", jParams);
    } else {
        String geometryType;
        __BNN_REQUIRE(JSONHelpers::readValue(jParams, geometryType, "GeometryType"));
        m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>(geometryType, jParams);
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
    JSONHelpers::readValue(jParams, nameID(),   "UniqueName");
    JSONHelpers::readValue(jParams, meshFile(), "MeshFile");
    ////////////////////////////////////////////////////////////////////////////////

    JSONHelpers::readBool(jParams, fullShapeObj(), "FullShapeObj");
    JSONHelpers::readBool(jParams, useFileCache(), "UseFileCache");
    JSONHelpers::readValue(jParams, particleInputFile(),  "ParticleInputFile");
    JSONHelpers::readValue(jParams, particleOutputFile(), "ParticleOutputFile");
    String pFileType = "BNN";
    if(JSONHelpers::readValue(jParams, pFileType, "ParticleFileType")) {
        if(pFileType == "OBJ" || pFileType == "obj") {
            particleFileType() = ParticleFileType::OBJ;
        } else if(pFileType == "BGEO" || pFileType == "bgeo") {
            particleFileType() = ParticleFileType::BGEO;
        } else if(pFileType == "BNN" || pFileType == "bnn") {
            particleFileType() = ParticleFileType::BNN;
        } else if(pFileType == "BINARY" || pFileType == "binary") {
            particleFileType() = ParticleFileType::BINARY;
        } else {
            __BNN_DIE_UNKNOWN_ERROR
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    if(jParams.find("Animation") != jParams.end()) {
        isDynamic() = true;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SimulationObject<N, RealType>::loadParticlesFromFile()
{
    if(m_bUseFileCache && !m_ParticleInputFile.empty() && FileHelpers::fileExisted(m_ParticleInputFile)) {
        bool bResult = false;
        switch(m_ParticleFileType) {
            case ParticleFileType::OBJ:
                bResult = ParticleHelpers::loadParticlesFromObj(m_ParticleInputFile, m_ObjParticles);
                break;
            case ParticleFileType::BGEO:
                bResult = ParticleHelpers::loadParticlesFromBGEO(m_ParticleInputFile, m_ObjParticles, m_ParticleRadius);
                break;
            case ParticleFileType::BNN:
                bResult = ParticleHelpers::loadParticlesFromBNN(m_ParticleInputFile, m_ObjParticles, m_ParticleRadius);
                break;
            case ParticleFileType::BINARY:
                bResult = ParticleHelpers::loadParticlesFromBinary(m_ParticleInputFile, m_ObjParticles, m_ParticleRadius);
                break;
        }
        if(bResult) {
            m_bObjReady = true;
            return true;
        }
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationObject<N, RealType>::saveParticlesToFile()
{
    if(m_bUseFileCache && !m_ParticleOutputFile.empty()) {
        switch(m_ParticleFileType) {
            case ParticleFileType::OBJ:
                ParticleHelpers::saveParticlesToObj(m_ParticleOutputFile, m_ObjParticles);
                break;
            case ParticleFileType::BGEO:
                ParticleHelpers::saveParticlesToBGEO(m_ParticleOutputFile, m_ObjParticles, m_ParticleRadius);
                break;
            case ParticleFileType::BNN:
                ParticleHelpers::saveParticlesToBNN(m_ParticleOutputFile, m_ObjParticles, m_ParticleRadius);
                break;
            case ParticleFileType::BINARY:
                ParticleHelpers::saveParticlesToBinary(m_ParticleOutputFile, m_ObjParticles, m_ParticleRadius);
                break;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class SimulationObject<2, Real>;
template class SimulationObject<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
