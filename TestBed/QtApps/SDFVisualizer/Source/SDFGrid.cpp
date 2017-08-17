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

#include "SDFGrid.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::setSDFObjectType(SDFObjectTypes SDFObjType)
{
    m_SDFObjectType          = SDFObjType;
    m_SDFGenerationFutureObj = std::async(std::launch::async, [&] { generateParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::setResolution(int resolution)
{
    m_Resolution             = resolution;
    m_SDFGenerationFutureObj = std::async(std::launch::async, [&] { generateParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SDFGrid::generateParticles()
{
    switch(m_SDFObjectType)
    {
        case Box:
            m_SDFObject = std::make_shared<GeometryObjects::BoxObject<float> >();
            break;
        case Sphere:
            m_SDFObject = std::make_shared<GeometryObjects::SphereObject<float> >();
            break;
        case Torus:
            m_SDFObject = std::make_shared<GeometryObjects::TorusObject<float> >();
            break;
        case Torus28:
            m_SDFObject = std::make_shared<GeometryObjects::Torus28Object<float> >();
            break;
        case Torus2Inf:
            m_SDFObject = std::make_shared<GeometryObjects::Torus2InfObject<float> >();
            break;
        case Torus88:
            m_SDFObject = std::make_shared<GeometryObjects::Torus88Object<float> >();
            break;
        case TorusInfInf:
            m_SDFObject = std::make_shared<GeometryObjects::TorusInfInfObject<float> >();
            break;
        case Cylinder:
            m_SDFObject = std::make_shared<GeometryObjects::CylinderObject<float> >();
            break;
        case Cone:
            m_SDFObject = std::make_shared<GeometryObjects::ConeObject<float> >();
            break;
        case Plane:
            m_SDFObject = std::make_shared<GeometryObjects::PlaneObject<float> >();
            break;
        case TriangularPrism:
            m_SDFObject = std::make_shared<GeometryObjects::TriangularPrismObject<float> >();
            break;
        case HexagonalPrism:
            m_SDFObject = std::make_shared<GeometryObjects::HexagonalPrismObject<float> >();
            break;
        case Capsule:
            m_SDFObject = std::make_shared<GeometryObjects::CapsuleObject<float> >();
            break;
        case Ellipsoid:
            m_SDFObject = std::make_shared<GeometryObjects::EllipsoidObject<float> >();
            break;
        case BoxSphereUnion:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->addObject(std::make_shared<GeometryObjects::SphereObject<float> >(), GeometryObjects::CSGOperations::Union);
            break;
        }
        case BoxSubtractSphere:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->addObject(std::make_shared<GeometryObjects::SphereObject<float> >(), GeometryObjects::CSGOperations::Subtraction);
            break;
        }
        case BoxSphereIntersection:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->addObject(std::make_shared<GeometryObjects::SphereObject<float> >(), GeometryObjects::CSGOperations::Intersection);
            break;
        }
        case BoxSphereBlendExp:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->addObject(std::make_shared<GeometryObjects::SphereObject<float> >(), GeometryObjects::CSGOperations::BlendExp);
            break;
        }
        case BoxSphereBlendPoly:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->addObject(std::make_shared<GeometryObjects::SphereObject<float> >(), GeometryObjects::CSGOperations::BlendPoly);
            break;
        }
        case TorusTwist:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::TorusObject<float> >());
            csgObj->setDeformOp(GeometryObjects::DomainDeformation::Twist);
            break;
        }
        case BoxBend:
        {
            m_SDFObject = std::make_shared<GeometryObjects::CSGObject<float> >();
            std::shared_ptr<GeometryObjects::CSGObject<float> > csgObj = std::dynamic_pointer_cast<GeometryObjects::CSGObject<float> >(m_SDFObject);
            csgObj->addObject(std::make_shared<GeometryObjects::BoxObject<float> >());
            csgObj->setDeformOp(GeometryObjects::DomainDeformation::CheapBend);
            break;
        }
        case TriMeshObj:
        {
            m_SDFObject = std::make_shared<GeometryObjects::TriMeshObject<float> >();
            std::shared_ptr<GeometryObjects::TriMeshObject<float> > meshObj = std::dynamic_pointer_cast<GeometryObjects::TriMeshObject<float> >(m_SDFObject);
//            meshObj->meshFile() = "D:/Programming/Banana/Assets/PLY/bunny.ply";
            meshObj->meshFile() = "D:/Programming/Noodle/Data/Mesh/Bunny.obj";
//            meshObj->meshFile() = "D:/Programming/Banana/Assets/PLY/icosahedron_ascii.ply";
            meshObj->step() = float(1.0 / 128.0);
            meshObj->makeSDF();
            break;
        }
    }

    std::shared_ptr<GeometryObjects::TriMeshObject<float> > meshObj = std::dynamic_pointer_cast<GeometryObjects::TriMeshObject<float> >(m_SDFObject);

    if(meshObj != nullptr)
    {
        auto sdf = meshObj->getSDF();
//        for(float x : sdf.vec_data())
//            qDebug() << x;
    }

    Q_ASSERT(m_ParticleData != nullptr);

    const float        step           = 2.0f / float(m_Resolution);
    const float        particleRadius = 0.5 * step;
    const unsigned int numParticles   = m_Resolution * m_Resolution * m_Resolution;

    m_ParticleData->setNumParticles(numParticles);
    m_ParticleData->setParticleRadius(particleRadius);

    unsigned char* dataPtr       = (m_ParticleData->getArray("Position")->data());
    unsigned char* colorScalePtr = (m_ParticleData->getArray("ColorScale")->data());

    static std::vector<Vec3<float> > negativeParticles;
    static std::vector<Vec3<float> > positiveParticles;
    static std::vector<float>        negativeColorScale;
    static std::vector<float>        positiveColorScale;

    negativeParticles.reserve(numParticles);
    positiveParticles.reserve(numParticles);
    negativeColorScale.reserve(numParticles);
    positiveColorScale.reserve(numParticles);

    const Vec3<float> corner(-1.0f + particleRadius);
    unsigned int      numNegative            = 0;
    unsigned int      numPositive            = 0;
    float             maxPositiveDistance    = 0;
    float             maxAbsNegativeDistance = 0;

    for(int i = 0; i < m_Resolution; ++i)
    {
        for(int j = 0; j < m_Resolution; ++j)
        {
            for(int k = 0; k < m_Resolution; ++k)
            {
                Vec3<float> ppos = corner + 2.0f * Vec3<float>(static_cast<float>(i) / static_cast<float>(m_Resolution),
                                                               static_cast<float>(j) / static_cast<float>(m_Resolution),
                                                               static_cast<float>(k) / static_cast<float>(m_Resolution));

                float distance = m_SDFObject->signedDistance(ppos);
                if(distance < 0)
                {
                    ++numNegative;
                    negativeParticles.push_back(ppos);

                    negativeColorScale.push_back(distance);
                    maxAbsNegativeDistance = (maxAbsNegativeDistance < abs(distance)) ? abs(distance) : maxAbsNegativeDistance;
                }
                else
                {
                    ++numPositive;
                    positiveParticles.push_back(ppos);

                    positiveColorScale.push_back(distance);
                    maxPositiveDistance = (maxPositiveDistance < distance) ? distance : maxPositiveDistance;
                }
            }
        }
    }

    ParallelFuncs::parallel_for<size_t>(0, negativeColorScale.size(),
                                        [&](size_t i)
                                        {
                                            negativeColorScale[i] = 1.0f - sqrt(-negativeColorScale[i] / maxAbsNegativeDistance);
                                        });
    ParallelFuncs::parallel_for<size_t>(0, positiveColorScale.size(),
                                        [&](size_t i)
                                        {
                                            positiveColorScale[i] = 1.0f - sqrt(positiveColorScale[i] / maxPositiveDistance);
                                        });

    std::memcpy(dataPtr,                                      negativeParticles.data(),  numNegative * sizeof(float) * 3);
    std::memcpy(&dataPtr[numNegative * sizeof(float) * 3],    positiveParticles.data(),  numPositive * sizeof(float) * 3);

    std::memcpy(colorScalePtr,                                negativeColorScale.data(), numNegative * sizeof(float));
    std::memcpy(&colorScalePtr[numNegative * sizeof(float) ], positiveColorScale.data(), numPositive * sizeof(float));

    m_ParticleData->setUInt("NumNegative", numNegative);
    m_ParticleData->setUInt("NumPositive", numPositive);
    emit dataReady();

    ////////////////////////////////////////////////////////////////////////////////
    negativeParticles.resize(0);
    positiveParticles.resize(0);

    negativeColorScale.resize(0);
    positiveColorScale.resize(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
