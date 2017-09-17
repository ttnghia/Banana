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

#include <Banana/Geometry/GeometryObjectFactory.h>

#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjectFactory
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<GeometryObject2D::GeometryObject> createGeometry2D(const String& geometryType)
{
    if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX")
        return std::make_shared<GeometryObject2D::BoxObject>();

    if(geometryType == "Circle" || geometryType == "circle" || geometryType == "CIRCLE")
        return std::make_shared<GeometryObject2D::CircleObject>();

    if(geometryType == "Torus" || geometryType == "torus" || geometryType == "TORUS")
        return std::make_shared<GeometryObject2D::TorusObject>();

    if(geometryType == "Torus28" || geometryType == "torus28" || geometryType == "TORUS28")
        return std::make_shared<GeometryObject2D::Torus28Object>();

    if(geometryType == "Torus2Inf" || geometryType == "torus2inf" || geometryType == "TORUS2INF")
        return std::make_shared<GeometryObject2D::Torus2InfObject>();

    if(geometryType == "Torus88" || geometryType == "torus88" || geometryType == "TORUS88")
        return std::make_shared<GeometryObject2D::Torus88Object>();

    if(geometryType == "TorusInfInf" || geometryType == "torusinfinf" || geometryType == "TORUSINFINF")
        return std::make_shared<GeometryObject2D::TorusInfInfObject>();

    if(geometryType == "Triangle" || geometryType == "triangle" || geometryType == "TRIANGLE")
        return std::make_shared<GeometryObject2D::TriangleObject>();

    if(geometryType == "Plane" || geometryType == "plane" || geometryType == "PLANE")
        return std::make_shared<GeometryObject2D::PlaneObject>();

    if(geometryType == "Hexagon" || geometryType == "hexagon" || geometryType == "HEXAGON")
        return std::make_shared<GeometryObject2D::HexagonObject>();

    if(geometryType == "Capsule" || geometryType == "capsule" || geometryType == "CAPSULE")
        return std::make_shared<GeometryObject2D::CapsuleObject>();

    if(geometryType == "Ellipsoid" || geometryType == "ellipsoid" || geometryType == "ELLIPSOID")
        return std::make_shared<GeometryObject2D::EllipsoidObject>();

    if(geometryType == "CSGObject" || geometryType == "csgobject" || geometryType == "CSGOBJECT")
        return std::make_shared<GeometryObject2D::CSGObject>();

    return nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<GeometryObject3D::GeometryObject> createGeometry3D(const String& geometryType)
{
    if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX")
        return std::make_shared<GeometryObject3D::BoxObject>();

    if(geometryType == "Sphere" || geometryType == "sphere" || geometryType == "SPHERE")
        return std::make_shared<GeometryObject3D::SphereObject>();

    if(geometryType == "Torus" || geometryType == "torus" || geometryType == "TORUS")
        return std::make_shared<GeometryObject3D::TorusObject>();

    if(geometryType == "Torus28" || geometryType == "torus28" || geometryType == "TORUS28")
        return std::make_shared<GeometryObject3D::Torus28Object>();

    if(geometryType == "Torus2Inf" || geometryType == "torus2inf" || geometryType == "TORUS2INF")
        return std::make_shared<GeometryObject3D::Torus2InfObject>();

    if(geometryType == "Torus88" || geometryType == "torus88" || geometryType == "TORUS88")
        return std::make_shared<GeometryObject3D::Torus88Object>();

    if(geometryType == "TorusInfInf" || geometryType == "torusinfinf" || geometryType == "TORUSINFINF")
        return std::make_shared<GeometryObject3D::TorusInfInfObject>();

    if(geometryType == "Cylinder" || geometryType == "cylinder" || geometryType == "CYLINDER")
        return std::make_shared<GeometryObject3D::CylinderObject>();

    if(geometryType == "Cone" || geometryType == "cone" || geometryType == "CONE")
        return std::make_shared<GeometryObject3D::ConeObject>();

    if(geometryType == "Plane" || geometryType == "plane" || geometryType == "PLANE")
        return std::make_shared<GeometryObject3D::PlaneObject>();

    if(geometryType == "TriangularPrism" || geometryType == "triangularprism" || geometryType == "TRIANGULARPRISM")
        return std::make_shared<GeometryObject3D::TriangularPrismObject>();

    if(geometryType == "HexagonalPrism" || geometryType == "hexagonalprism" || geometryType == "HEXAGONALPRISM")
        return std::make_shared<GeometryObject3D::HexagonalPrismObject>();

    if(geometryType == "Capsule" || geometryType == "capsule" || geometryType == "CAPSULE")
        return std::make_shared<GeometryObject3D::CapsuleObject>();

    if(geometryType == "Ellipsoid" || geometryType == "ellipsoid" || geometryType == "ELLIPSOID")
        return std::make_shared<GeometryObject3D::EllipsoidObject>();

    if(geometryType == "TriMesh" || geometryType == "trimesh" || geometryType == "TRIMESH")
        return std::make_shared<GeometryObject3D::TriMeshObject>();

    if(geometryType == "CSGObject" || geometryType == "csgobject" || geometryType == "CSGOBJECT")
        return std::make_shared<GeometryObject3D::CSGObject>();

    return nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<GeometryObject2D::GeometryObject> combineGeometryObjects2D(const Vector<SharedPtr<GeometryObject2D::GeometryObject> >& geometryObjs)
{
    if(geometryObjs.size() == 0)
        return nullptr;
    if(geometryObjs.size() == 1)
        return geometryObjs.front();

    SharedPtr<GeometryObject2D::CSGObject> csgObj = std::make_shared<GeometryObject2D::CSGObject>();

    for(auto& obj : geometryObjs)
        csgObj->addObject(obj);

    return csgObj;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<GeometryObject3D::GeometryObject> combineGeometryObjects3D(const Vector<SharedPtr<GeometryObject3D::GeometryObject> >& geometryObjs)
{
    if(geometryObjs.size() == 0)
        return nullptr;
    if(geometryObjs.size() == 1)
        return geometryObjs.front();

    SharedPtr<GeometryObject3D::CSGObject> csgObj = std::make_shared<GeometryObject3D::CSGObject>();

    for(auto& obj : geometryObjs)
        csgObj->addObject(obj);

    return csgObj;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjectFactory

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana