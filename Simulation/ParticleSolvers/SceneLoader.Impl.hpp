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
template<Int N, class RealType>
void loadBoundaryObjects(const nlohmann::json& jParams, Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > >& boundaryObjs)
{
    for(auto& jObj : jParams) {
        // read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > obj = SimulationObjectFactory::createBoundaryObject<N, RealType>(geometryType);
        __BNN_ASSERT(obj->getGeometry() != nullptr);
        boundaryObjs.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj, obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj, obj->particleFile(), "ParticleFile");
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
void loadParticleGenerators(const nlohmann::json& jParams, Vector<SharedPtr<SimulationObjects::ParticleGenerator<N, RealType> > >& particleGenerators)
{
    for(auto& jObj : jParams) {
        // read geometry type of the object
        String geometryType = String("");
        __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
        __BNN_ASSERT(!geometryType.empty());

        // create the object
        SharedPtr<SimulationObjects::ParticleGenerator<N, RealType> > obj = SimulationObjectFactory::createParticleGenerator<N, RealType>(geometryType);
        __BNN_ASSERT(obj->getGeometry() != nullptr);
        particleGenerators.push_back(obj);

        // read mesh/cache/dynamic
        JSONHelpers::readValue(jObj, obj->meshFile(), "MeshFile");
        JSONHelpers::readValue(jObj, obj->particleFile(), "ParticleFile");

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
