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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Geometry/GeometryObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjectFactory
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
SharedPtr<GeometryObjects::GeometryObject<N, RealType> > createGeometry(const String& geometryType)
{
    if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
        return std::make_shared<GeometryObjects::BoxObject<N, RealType> >();
    }

    if(geometryType == "Sphere" || geometryType == "sphere" || geometryType == "SPHERE") {
        return std::make_shared<GeometryObjects::SphereObject<N, RealType> >();
    }

    if(geometryType == "Torus" || geometryType == "torus" || geometryType == "TORUS") {
        return std::make_shared<GeometryObjects::TorusObject<N, RealType> >();
    }

    if(geometryType == "Torus28" || geometryType == "torus28" || geometryType == "TORUS28") {
        return std::make_shared<GeometryObjects::Torus28Object<N, RealType> >();
    }

    if(geometryType == "Torus2Inf" || geometryType == "torus2inf" || geometryType == "TORUS2INF") {
        return std::make_shared<GeometryObjects::Torus2InfObject<N, RealType> >();
    }

    if(geometryType == "Torus88" || geometryType == "torus88" || geometryType == "TORUS88") {
        return std::make_shared<GeometryObjects::Torus88Object<N, RealType> >();
    }

    if(geometryType == "TorusInfInf" || geometryType == "torusinfinf" || geometryType == "TORUSINFINF") {
        return std::make_shared<GeometryObjects::TorusInfInfObject<N, RealType> >();
    }

    if(geometryType == "Cylinder" || geometryType == "cylinder" || geometryType == "CYLINDER") {
        return std::make_shared<GeometryObjects::CylinderObject<N, RealType> >();
    }

    if(geometryType == "Cone" || geometryType == "cone" || geometryType == "CONE") {
        return std::make_shared<GeometryObjects::ConeObject<N, RealType> >();
    }

    if(geometryType == "Plane" || geometryType == "plane" || geometryType == "PLANE") {
        return std::make_shared<GeometryObjects::PlaneObject<N, RealType> >();
    }

    if(geometryType == "Triangle" || geometryType == "triangle" || geometryType == "TRIANGLE") {
        return std::make_shared<GeometryObjects::TriangleObject<N, RealType> >();
    }

    if(geometryType == "Hexagon" || geometryType == "hexagon" || geometryType == "HEXAGON") {
        return std::make_shared<GeometryObjects::HexagonObject<N, RealType> >();
    }

    if(geometryType == "TriangularPrism" || geometryType == "triangularprism" || geometryType == "TRIANGULARPRISM") {
        return std::make_shared<GeometryObjects::TriangularPrismObject<N, RealType> >();
    }

    if(geometryType == "HexagonalPrism" || geometryType == "hexagonalprism" || geometryType == "HEXAGONALPRISM") {
        return std::make_shared<GeometryObjects::HexagonalPrismObject<N, RealType> >();
    }

    if(geometryType == "Capsule" || geometryType == "capsule" || geometryType == "CAPSULE") {
        return std::make_shared<GeometryObjects::CapsuleObject<N, RealType> >();
    }

    if(geometryType == "Ellipsoid" || geometryType == "ellipsoid" || geometryType == "ELLIPSOID") {
        return std::make_shared<GeometryObjects::EllipsoidObject<N, RealType> >();
    }

    if(geometryType == "TriMesh" || geometryType == "trimesh" || geometryType == "TRIMESH") {
        return std::make_shared<GeometryObjects::TriMeshObject<N, RealType> >();
    }

    if(geometryType == "CSGObject" || geometryType == "csgobject" || geometryType == "CSGOBJECT") {
        return std::make_shared<GeometryObjects::CSGObject<N, RealType> >();
    }

    return nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void combineGeometryObjects(const Vector<SharedPtr<GeometryObjects::GeometryObject<N, RealType> > >& geometryObjs,
                            SharedPtr<GeometryObjects::GeometryObject<N, RealType> >& unifiedGeometry)
{
    if(geometryObjs.size() == 0) {
        return nullptr;
    }
    if(geometryObjs.size() == 1) {
        return geometryObjs.front();
    }

    SharedPtr<GeometryObjects::CSGObject<N, RealType> > csgObj = std::make_shared<GeometryObjects::CSGObject<N, RealType> >();

    for(auto& obj : geometryObjs) {
        csgObj->addObject(obj);
    }

    return csgObj;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjectFactory

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana