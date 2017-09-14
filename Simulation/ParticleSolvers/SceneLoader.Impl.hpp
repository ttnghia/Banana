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
template<class BoundaryObjType>
void loadBoundaryObjects(const nlohmann::json& jParams, Vector<SharedPtr<BoundaryObjType> >& boundaryObjs)
{
    for(auto& jBoundaryObj : jParams)
    {
        // read geometry type of the object
        {
            String geometryType = String("");
            __BNN_ASSERT(JSONHelpers::readValue(jBoundaryObj["GeometryType"], geometryType));
            __BNN_ASSERT(!geometryType.empty());
        }

        // create the object
        SharedPtr<BoundaryObjType> bdObject = nullptr;
        {
            if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX")
                bdObject = std::make_shared<BoxBoundary2D>();

            if(bdObject == nullptr)
                continue;
            boundaryObjs.push_back(bdObject);
        }

        // read mesh/cache/dynamic
        {
            JSONHelpers::readValue(jBoundaryObj["MeshFile"], bdObject->meshFile());
            JSONHelpers::readValue(jBoundaryObj["ParticleFile"], bdObject->particleFile());
            JSONHelpers::readBool(jBoundaryObj["IsDynamic"], bdObject->isDynamic());
        }

        // read object transformation
        {
            MatXxX<BoundaryObjType::objDimension() + 1, Real> transformation(1.0);
            VecX<BoundaryObjType::objDimension(), Real>       translation;
            VecX<BoundaryObjType::objDimension(), Real>       rotationAngles;
            VecX<BoundaryObjType::objDimension(), Real>       scale;

            if(JSONHelpers::readVector(jBoundaryObj["Translation"], translation))
                transformation *= glm::translate(translation);

            if(JSONHelpers::readVector(jBoundaryObj["RotationAngles"], rotationAngles))
                transformation *= glm::eulerAngleYXZ(rotationAngles.y, rotationAngles.x, rotationAngles.z);

            if(JSONHelpers::readVector(jBoundaryObj["Scale"], scale))
                transformation *= glm::scale(scale);

            bdObject->setTransformation(transformation);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ParticleObjType>
void loadParticleObjects(const nlohmann::json& jParams, Vector<SharedPtr<ParticleObjType> >& particleObjs)
{
    for(auto& jParticleObj : jParams)
    {
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jParticleObj["GeometryType"], geometryType));
        __BNN_ASSERT(!geometryType.empty());

        SharedPtr<ParticleObjType> bdObject = ParticleObjType::create(geometryType);
        boundaryObjs.push_back(bdObject);

        const bool bMesh            = JSONHelpers::readValue(jParticleObj["MeshFile"], bdObject->meshFile());
        const bool bCachedParticles = JSONHelpers::readValue(jParticleObj["ParticleFile"], bdObject->particleFile());

        MatXxX<ParticleObjType::objDimension() + 1, Real> transformation(1.0);
        VecX<ParticleObjType::objDimension(), Real>       translation;
        VecX<ParticleObjType::objDimension(), Real>       rotationAngles;
        VecX<ParticleObjType::objDimension(), Real>       scale;

        if(JSONHelpers::readVector(jParticleObj["Translation"], translation))
            transformation *= glm::translate(translation);

        if(JSONHelpers::readVector(jParticleObj["RotationAngles"], rotationAngles))
            transformation *= glm::eulerAngleYXZ(rotationAngles.y, rotationAngles.x, rotationAngles.z);

        if(JSONHelpers::readVector(jParticleObj["Scale"], scale))
            transformation *= glm::scale(scale);

        bdObject->setTransformation(transformation);

        JSONHelpers::readBool(jParticleObj["IsDynamic"], bdObject->isDynamic());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ParticleEmitterType>
void loadParticleEmitters(const nlohmann::json& jParams, Vector<SharedPtr<ParticleEmitterType> >& particleEmitters)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class BoundaryObjType, class ParticleObjType, class ParticleEmitterType>
void loadSceneObjects(const nlohmann::json&                    jParams,
                      Vector<SharedPtr<BoundaryObjType> >&     boundaryObjs,
                      Vector<SharedPtr<ParticleObjType> >&     particleObjs,
                      Vector<SharedPtr<ParticleEmitterType> >& particleEmitters)
{
    __BNN_ASSERT(jParams.find("BoundaryObjects") != jParams.end());
    __BNN_ASSERT(jParams.find("ParticleObjects") != jParams.end());

    loadBoundaryObjects(jParams["BoundaryObjects"], boundaryObjs);
    loadParticleObjects(jParams["ParticleObjects"], particleObjs);

    if((jParams.find("ParticleEmitters") != jParams.end()))
        loadParticleEmitters(jParams["ParticleEmitters"], particleEmitters);
}
