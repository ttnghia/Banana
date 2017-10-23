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
#include <Banana/Animation/Animation.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Array/Array.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/MeshLoader.h>

#include <glm/gtx/euler_angles.hpp>

#include <Banana/Geometry/OBJLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Base class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class GeometryObject
{
public:
    GeometryObject() = default;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }

    virtual String   name() = 0;
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const = 0;

    Vec2<RealType> gradSignedDistance(const Vec2<RealType>& ppos, RealType dxy = RealType(1e-4));
    Vec3<RealType> gradSignedDistance(const Vec3<RealType>& ppos, RealType dxyz = RealType(1e-4));
    bool           isInside(const VecX<N, RealType>& ppos) { return signedDistance(ppos) < 0; }

    void setTranslation(const VecX<N, RealType>& translation);
    void setRotation(const VecX<N + 1, RealType>& rotation);
    void setUniformScale(const RealType scaleVal);
    void resetTransformation();

    auto  getAABBMin() const { return transform(VecX<N, RealType>(0)) - VecX<N, RealType>(m_UniformScale) * sqrt(glm::compAdd(VecX<N, RealType>(1.0))); }
    auto  getAABBMax() const { return transform(VecX<N, RealType>(0)) + VecX<N, RealType>(m_UniformScale) * sqrt(glm::compAdd(VecX<N, RealType>(1.0))); }
    auto& getAnimation() { return m_Animation; }

    VecX<N, RealType> transform(const VecX<N, RealType>& ppos) const;
    VecX<N, RealType> invTransform(const VecX<N, RealType>& ppos) const;

    virtual void updateTransformation(UInt frame = 0, RealType fraction = RealType(0));
protected:


    ////////////////////////////////////////////////////////////////////////////////
    bool     m_bTransformed = false;
    RealType m_UniformScale = RealType(1.0);
    RealType m_InvScale     = RealType(1.0);

    MatXxX<N + 1, RealType> m_TransformationMatrix    = MatXxX<N + 1, RealType>(1.0);
    MatXxX<N + 1, RealType> m_InvTransformationMatrix = MatXxX<N + 1, RealType>(1.0);
    Animation<N, RealType>  m_Animation;
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
    virtual String   name() override { return String("BoxObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    auto boxMin() const { return transform(m_BoxMin); }
    auto boxMax() const { return transform(m_BoxMax); }

    void        setOriginalBox(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) { m_BoxMin = bMin; m_BoxMax = bMax; }
    const auto& originalBoxMin() const { return m_BoxMin; }
    const auto& originalBoxMax() const { return m_BoxMax; }

    void addKeyFrame(UInt frame, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax);
    void setPeriodic(bool bPeriodic, UInt startFrame = 0) { m_bPeriodic = bPeriodic; m_StartFrame = startFrame; }

    void         makeReadyAnimation();
    virtual void updateTransformation(UInt frame = 0, RealType fraction = RealType(0)) override;

protected:
    VecX<N, RealType> m_BoxMin = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_BoxMax = VecX<N, RealType>(1.0);

    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    struct BoxKeyFrame
    {
        BoxKeyFrame() = default;
        BoxKeyFrame(UInt frame_, const VecX<N, RealType>& bMin_, const VecX<N, RealType>& bMax_) { frame = frame_; bMin = bMin_; bMax = bMax_; }
        UInt              frame = 0;
        VecX<N, RealType> bMin  = VecX<N, RealType>(-1.0);
        VecX<N, RealType> bMax  = VecX<N, RealType>(1.0);
    };

    Vector<BoxKeyFrame>   m_KeyFrames;
    CubicSpline<RealType> m_BoxMinSpline[N];
    CubicSpline<RealType> m_BoxMaxSpline[N];
    UInt                  m_StartFrame      = 0;
    UInt                  m_MaxFrame        = 0;
    bool                  m_bPeriodic       = false;
    bool                  m_bAnimationReady = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SphereObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { if(N == 2) { return String("CircleObject"); } else { return String("SphereObject"); } }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusObject;

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class TorusObject<2, RealType> : public GeometryObject<2, RealType>
{
public:
    virtual String   name() override { return String("TorusObject"); }
    virtual RealType signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusRatio(RealType ratio) { __BNN_ASSERT(ratio > 0); m_OuterRadius = RealType(1.0) - m_InnerRadius; m_InnerRadius = m_OuterRadius / ratio; }

protected:
    const RealType m_OuterRadius = RealType(0.75);
    RealType       m_InnerRadius = RealType(0.25);
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class TorusObject<3, RealType> : public GeometryObject<3, RealType>
{
public:
    virtual String   name() override { return String("TorusObject"); }
    virtual RealType signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusRatio(RealType ratio) { __BNN_ASSERT(ratio > 0); m_OuterRadius = RealType(1.0) - m_InnerRadius; m_InnerRadius = m_OuterRadius / ratio; }

protected:
    const RealType m_OuterRadius = RealType(0.75);
    RealType       m_InnerRadius = RealType(0.25);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus28Object : public TorusObject<N, RealType>
{
public:
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus2InfObject : public TorusObject<N, RealType>
{
public:
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus88Object;

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class Torus88Object<2, RealType> : public TorusObject<2, RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside = true) const override;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class Torus88Object<3, RealType> : public TorusObject<3, RealType>
{
public:
    virtual RealType signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusInfInfObject;

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class TorusInfInfObject<2, RealType> : public TorusObject<2, RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside = true) const override;
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class TorusInfInfObject<3, RealType> : public TorusObject<3, RealType>
{
public:
    virtual RealType signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CylinderObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("CylinderObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusScale(RealType radius) { m_Radius = radius; }
protected:
    RealType m_Radius = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ConeObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("ConeObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusScale(RealType radius) { m_Radius = radius; }
protected:
    RealType m_Radius = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class PlaneObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("PlaneObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setNormal(const VecX<N, RealType>& normal) { m_Normal = normal; }
    void setOffset(RealType offset) { m_Offset = offset; }
protected:
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
    RealType          m_Offset = RealType(0);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangleObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("TriangleObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    template<class IndexType> void setVertex(IndexType idx, const VecX<N, RealType>& vertex) { m_Vertices[idx] = vertex; }
protected:
    VecX<N, RealType> m_Vertices[3];
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("HexagonObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangularPrismObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("TriangularPrismObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setWidthHeightRatio(RealType ratio) { return m_Width = ratio; }
protected:
    RealType m_Width = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonalPrismObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("HexagonalPrismObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void             setWidthHeightRatio(RealType ratio) { return m_Width = ratio; }

protected:
    RealType m_Width = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CapsuleObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("CapsuleObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusScale(RealType radius) { m_Radius = radius; }
protected:
    RealType m_Radius = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class EllipsoidObject : public GeometryObject<N, RealType>
{
public:
    virtual String   name() override { return String("SphereObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setRadiusScale(const VecX<N, RealType>& scale) { m_RadiusScale = scale; }
protected:
    VecX<N, RealType> m_RadiusScale = VecX<N, RealType>(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriMeshObject;

////////////////////////////////////////////////////////////////////////////////
// this is an empty class, to get rid of compiler error
template<class RealType>
class TriMeshObject<2, RealType> : public GeometryObject<2, RealType>
{
public:
    virtual String   name() override { return String("TriMeshObject-Unimplemented"); }
    virtual RealType signedDistance(const Vec2<RealType>&, bool bNegativeInside = true) const override { __BNN_UNUSED(bNegativeInside); return RealType(0); }

    auto& meshFile() { return m_TriMeshFile; }
    auto& sdfStep() { return m_Step; }
    void  computeSDF() {}

protected:
    String   m_TriMeshFile = String("");
    RealType m_Step        = RealType(1.0 / 256.0);
};

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
class TriMeshObject<3, RealType> : public GeometryObject<3, RealType>
{
public:
    virtual String   name() override { return String("TriMeshObject"); }
    virtual RealType signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside = true) const override;

    auto& meshFile() { return m_TriMeshFile; }
    auto& sdfStep() { return m_Step; }
    void  computeSDF();

protected:

    bool   m_bSDFGenerated = false;
    String m_TriMeshFile   = String("");

    RealType           m_Step      = RealType(1.0 / 256.0);
    RealType           m_Expanding = RealType(0.1);
    Array<3, RealType> m_SDFData;
    Grid<3, RealType>  m_Grid3D;
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

    virtual String   name() override { return String("CSGObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void addObject(const CSGData& obj) { m_Objects.push_back(obj); }
    void addObject(const SharedPtr<GeometryObject>& obj, CSGOperations op = Union) { addObject({ obj, op }); }
    void setDeformOp(DomainDeformation deformOp) { m_DeformOp = deformOp; }

protected:
    VecX<N, RealType> domainDeform(const VecX<N, RealType>& ppos) const;
    VecX<N, RealType> twist(const VecX<N, RealType>& ppos) const;
    VecX<N, RealType> cheapBend(const VecX<N, RealType>& ppos) const;

    ////////////////////////////////////////////////////////////////////////////////
    Vector<CSGData>   m_Objects;
    DomainDeformation m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Geometry/GeometryObjects.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana