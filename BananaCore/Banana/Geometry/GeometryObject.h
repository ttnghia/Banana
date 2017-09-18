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

#include <Banana/Setup.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Array/Array.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/MeshLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class GeometryObject
{
public:
    GeometryObject() = default;

    __BNN_INLINE virtual String   name()                                                                     = 0;
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) = 0;

    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }
    Vec2<RealType>        gradientSignedDistance(const Vec2<RealType>& ppos, RealType dxy = RealType(1e-4));
    Vec3<RealType>        gradientSignedDistance(const Vec3<RealType>& ppos, RealType dxyz = RealType(1e-4));

    __BNN_INLINE bool isInside(const VecX<N, RealType>& ppos) { return signedDistance(ppos) < 0; }
    __BNN_INLINE bool isInAABBBox(const VecX<N, RealType>& ppos) { return NumberHelpers::isInside(ppos, m_AABBBoxMin, m_AABBBoxMax); }

    __BNN_INLINE const VecX<N, RealType>& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    __BNN_INLINE const VecX<N, RealType>& aabbBoxMax() const noexcept { return m_AABBBoxMax; }
    __BNN_INLINE const VecX<N, RealType>& objCenter() const noexcept { return m_ObjCenter; }

    __BNN_INLINE void translate(const VecX<N, RealType>& translation) { m_Translation = translation; m_bTranslated = true; update(); }
    __BNN_INLINE void rotate(const VecX<N, RealType>& angles) { m_Rotation = angles; m_bRotated = true; update(); }
    __BNN_INLINE void scale(const VecX<N, RealType>& scaleVal) { m_Scale = scaleVal; m_bScaled = true; update(); }

protected:
    virtual void update();

    ////////////////////////////////////////////////////////////////////////////////
    VecX<N, RealType> m_AABBBoxMin = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_AABBBoxMax = VecX<N, RealType>(1.0);
    VecX<N, RealType> m_ObjCenter  = VecX<N, RealType>(0);

    VecX<N, RealType> m_Translation = VecX<N, RealType>(0);
    VecX<N, RealType> m_Rotation    = VecX<N, RealType>(0.0);
    VecX<N, RealType> m_Scale       = VecX<N, RealType>(1.0);

    VecX<N, RealType> m_InvTranslation = VecX<N, RealType>(0);
    VecX<N, RealType> m_InvRotation    = VecX<N, RealType>(0.0);
    VecX<N, RealType> m_InvScale       = VecX<N, RealType>(1.0);

    bool m_bTranslated = false;
    bool m_bRotated    = false;
    bool m_bScaled     = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoxObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "BoxObject"; };
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE void setBox(const VecX<N, RealType>& boxMin, const VecX<N, RealType>& boxMax) { m_AABBBoxMin = boxMin; m_AABBBoxMax = boxMax; }
    __BNN_INLINE void setBMin(const VecX<N, RealType>& boxMin) { m_AABBBoxMin = boxMin; }
    __BNN_INLINE void setBMax(const VecX<N, RealType>& boxMax) { m_AABBBoxMax = boxMax; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SphereObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "CircleObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE const VecX<N, RealType>& center() const noexcept { return m_Center; }
    __BNN_INLINE RealType                 radius() const noexcept { return m_Radius; }
    __BNN_INLINE void                     setSphere(const Vec3r& center, RealType radius);

protected:
    VecX<N, RealType> m_Center = VecX<N, RealType>(0);
    RealType          m_Radius = RealType(1.0);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "TorusObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE VecX<N, RealType>& center() { return m_Center; }
    __BNN_INLINE RealType&          innerRadius() { return m_InnerRadius; }
    __BNN_INLINE RealType&          outerRadius() { return m_OuterRadius; }

protected:
    VecX<N, RealType> m_Center      = VecX<N, RealType>(0);
    RealType          m_InnerRadius = 0;
    RealType          m_OuterRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus28Object : public TorusObject<N, RealType>
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus2InfObject : public TorusObject<N, RealType>
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus88Object : public TorusObject<N, RealType>
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusInfInfObject : public TorusObject<N, RealType>
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CylinderObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "CylinderObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE RealType& radius() { return m_Radius; }
    __BNN_INLINE RealType& height() { return m_Height; }

protected:
    RealType m_Radius = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ConeObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "ConeObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE VecX<N, RealType>& surfaceNormal() { return m_Normal; }

protected:
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class PlaneObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "PlaneObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE VecX<N, RealType>& normal() { return m_Normal; }
    __BNN_INLINE RealType&          offset() { return m_Offset; }

protected:
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
    RealType          m_Offset = RealType(0);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangleObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "TriangleObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    template<class IndexType> VecX<N, RealType>& vertex(IndexType idx) { return m_Vertices[idx]; }

protected:
    VecX<N, RealType> m_Vertices[3];
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "HexagonObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    VecX<N, RealType>& center() { return m_Center; }
    RealType&          radius() { return m_Radius; }

protected:
    VecX<N, RealType> m_Center = VecX<N, RealType>(0);
    RealType          m_Radius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangularPrismObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "TriangularPrismObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE RealType& width() { return m_Width; }
    __BNN_INLINE RealType& height() { return m_Height; }

protected:
    RealType m_Width  = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonalPrismObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "HexagonalPrismObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE RealType& width() { return m_Width; }
    __BNN_INLINE RealType& height() { return m_Height; }

protected:
    RealType m_Width  = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CapsuleObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "SphereObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class EllipsoidObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "SphereObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriMeshObject : public GeometryObject<N, RealType>
{
public:
    __BNN_INLINE virtual String   name() override { return "TriMeshObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE String&                   meshFile() { return m_TriMeshFile; }
    __BNN_INLINE const Array<N, RealType>& getSDF() const noexcept { return m_SDFData; }

protected:
    __BNN_INLINE void makeSDF();

    bool   m_bSDFReady   = false;
    String m_TriMeshFile = String("");

    RealType           m_Step      = RealType(1.0 / 256.0);
    RealType           m_Expanding = RealType(0.1);
    Array<N, RealType> m_SDFData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum CSGOperations
{
    Overwrite,
    Union,
    Subtraction,
    Intersection,
    BlendExp,
    BlendPoly
};

enum DomainDeformation
{
    None,
    Twist,
    CheapBend
};

template<Int N, class RealType>
class CSGObject : public GeometryObject<N, RealType>
{
public:
    struct CSGData
    {
        SharedPtr<GeometryObject<N, RealType> > obj = nullptr;
        CSGOperations                           op  = Union;
    };

    __BNN_INLINE virtual String   name() override { return "CSGObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override;

    __BNN_INLINE void addObject(const CSGData& obj) { m_Objects.push_back(obj); updateAABB(obj); }
    __BNN_INLINE void addObject(const SharedPtr<GeometryObject>& obj, CSGOperations op = Union) { addObject({ obj, op }); }
    __BNN_INLINE void setDeformOp(DomainDeformation deformOp) { m_DeformOp = deformOp; }

protected:
    __BNN_INLINE void              updateAABB(const CSGData& obj);
    __BNN_INLINE VecX<N, RealType> domainDeform(const VecX<N, RealType>& ppos);
    __BNN_INLINE VecX<N, RealType> twist(const VecX<N, RealType>& ppos);
    __BNN_INLINE VecX<N, RealType> cheapBend(const VecX<N, RealType>& ppos);

    ////////////////////////////////////////////////////////////////////////////////
    Vector<CSGData>   m_Objects;
    DomainDeformation m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Geometry/GeometryObject.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana