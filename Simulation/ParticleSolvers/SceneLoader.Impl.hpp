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
    for(auto& jObj : jParams)
    {
        // read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj["GeometryType"], geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<BoundaryObjType> obj;
        SimulationObjectFactory::createBoundaryObject(geometryType, obj);
        __BNN_ASSERT(obj->getGeometry() != nullptr);
        boundaryObjs.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj["MeshFile"], obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj["ParticleFile"], obj->particleFile(), "ParticleFile");
        JSONHelpers::readBool(jObj["IsDynamic"], obj->isDynamic(), "IsDynamic");

        // read object transformation
        VecX<BoundaryObjType::objDimension(), Real> translation;
        VecX<BoundaryObjType::objDimension(), Real> rotationAngles;
        VecX<BoundaryObjType::objDimension(), Real> scale;

        if(JSONHelpers::readVector(jObj["Translation"], translation, "Translation"))
            obj->getGeometry()->translate(translation);

        if(JSONHelpers::readVector(jObj["RotationAngles"], rotationAngles, "RotationAngles"))
            obj->getGeometry()->rotate(rotationAngles);

        if(JSONHelpers::readVector(jObj["Scale"], scale, "Scale"))
            obj->getGeometry()->scale(scale);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ParticleObjType>
void loadParticleObjects(const nlohmann::json& jParams, Vector<SharedPtr<ParticleObjType> >& particleObjs)
{
    for(auto& jObj : jParams)
    {
        // read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj["GeometryType"], geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<ParticleObjType> obj;
        SimulationObjectFactory::createParticleObject(geometryType, obj);
        __BNN_ASSERT(obj->getGeometry() != nullptr);
        particleObjs.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj["MeshFile"], obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj["ParticleFile"], obj->particleFile(), "ParticleFile");

        // read object transformation
        VecX<ParticleObjType::objDimension(), Real> translation;
        VecX<ParticleObjType::objDimension(), Real> rotationAngles;
        VecX<ParticleObjType::objDimension(), Real> scale;

        if(JSONHelpers::readVector(jObj["Translation"], translation, "Translation"))
            obj->getGeometry()->translate(translation);

        if(JSONHelpers::readVector(jObj["RotationAngles"], rotationAngles, "RotationAngles"))
            obj->getGeometry()->rotate(rotationAngles);

        if(JSONHelpers::readVector(jObj["Scale"], scale, "Scale"))
            obj->getGeometry()->scale(scale);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ParticleEmitterType>
void loadParticleEmitters(const nlohmann::json& jParams, Vector<SharedPtr<ParticleEmitterType> >& particleEmitters)
{
    for(auto& jObj : jParams)
    {
        // read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj["GeometryType"], geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<ParticleEmitterType> obj;
        SimulationObjectFactory::createParticleEmitter(geometryType, obj);
        __BNN_ASSERT(obj->getGeometry() != nullptr);
        particleEmitters.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj["MeshFile"], obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj["ParticleFile"], obj->particleFile(), "ParticleFile");

        // read object transformation
        VecX<ParticleEmitterType::objDimension(), Real> translation;
        VecX<ParticleEmitterType::objDimension(), Real> rotationAngles;
        VecX<ParticleEmitterType::objDimension(), Real> scale;

        if(JSONHelpers::readVector(jObj["Translation"], translation, "Translation"))
            obj->getGeometry()->translate(translation);

        if(JSONHelpers::readVector(jObj["RotationAngles"], rotationAngles, "RotationAngles"))
            obj->getGeometry()->rotate(rotationAngles);

        if(JSONHelpers::readVector(jObj["Scale"], scale, "Scale"))
            obj->getGeometry()->scale(scale);
    }
}

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
