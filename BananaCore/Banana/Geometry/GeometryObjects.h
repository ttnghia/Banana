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

#include <Banana/Setup.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Array/Array.h>
#include <Banana/Animation/Animation.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::GeometryObjects {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Base class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class GeometryObject {
public:
    GeometryObject() = delete;
    GeometryObject(const JParams& jParams) : m_jParams(jParams) {}
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }

    virtual String   name() = 0;
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const = 0;

    VecX<N, RealType> gradSignedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true, RealType dx = RealType(1e-4)) const;
    bool           isInside(const VecX<N, RealType>& ppos, bool bNegativeInside = true) const { return signedDistance(ppos, bNegativeInside) < 0; }

    void setTranslation(const VecX<N, RealType>& translation);
    void setRotation(const VecX<N + 1, RealType>& rotation);
    void setUniformScale(const RealType scaleVal);
    void resetTransformation();

    auto getAABBMin() const { return transform(VecX<N, RealType>(0)) - VecX<N, RealType>(m_UniformScale) * std::sqrt(glm::compAdd(VecX<N, RealType>(1.0))); }
    auto getAABBMax() const { return transform(VecX<N, RealType>(0)) + VecX<N, RealType>(m_UniformScale) * std::sqrt(glm::compAdd(VecX<N, RealType>(1.0))); }
    auto& getAnimation() { return m_Animation; }
    const auto& getTransformationMatrix() const { return m_TransformationMatrix; }

    VecX<N, RealType> getVelocityAt(const VecX<N, RealType>& ppos) const;
    VecX<N, RealType> transform(const VecX<N, RealType>& ppos) const;
    VecX<N, RealType> invTransform(const VecX<N, RealType>& ppos) const;

    virtual bool updateTransformation(UInt frame = 0, RealType fraction = RealType(0), RealType frameDuration = RealType(1.0 / 30.0));

protected:
    virtual void parseParameters(const JParams& jParams);
    ////////////////////////////////////////////////////////////////////////////////
    JParams  m_jParams;
    bool     m_bTransformed = false;
    RealType m_UniformScale = RealType(1.0);
    //RealType m_InvScale     = RealType(1.0);

    RealType                m_LastTime    = 0_f;
    RealType                m_CurrentTime = 0_f;
    MatXxX<N + 1, RealType> m_LastTransformationMatrix = MatXxX<N + 1, RealType>(1.0);

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
class BoxObject : public GeometryObject<N, RealType> {
public:
    BoxObject() = delete;
    BoxObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }

    virtual String   name() override { return String("BoxObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    auto boxMin() const { return this->transform(m_BoxMin); }
    auto boxMax() const { return this->transform(m_BoxMax); }

    void        setOriginalBox(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) { m_BoxMin = bMin; m_BoxMax = bMax; }
    const auto& originalBoxMin() const { return m_BoxMin; }
    const auto& originalBoxMax() const { return m_BoxMax; }

    void addKeyFrame(UInt frame, const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax);
    void setPeriodic(bool bPeriodic, UInt startFrame = 0) { m_bPeriodic = bPeriodic; m_StartFrame = startFrame; }

    void         makeReadyAnimation();
    virtual bool updateTransformation(UInt frame = 0, RealType fraction = RealType(0), RealType frameDuration = RealType(1.0_f / 30.0_f)) override;

protected:
    virtual void parseParameters(const JParams& jParams) override;

    VecX<N, RealType> m_BoxMin = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_BoxMax = VecX<N, RealType>(1.0);
    ////////////////////////////////////////////////////////////////////////////////
    // animation data
    struct BoxKeyFrame {
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
class SphereObject : public GeometryObject<N, RealType> {
public:
    SphereObject() = delete;
    SphereObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { if(N == 2) { return String("CircleObject"); } else { return String("SphereObject"); } }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusObject : public GeometryObject<N, RealType> {
public:
    TorusObject() = delete;
    TorusObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("TorusObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setRingRadius(RealType ringRadius) { __BNN_REQUIRE(ringRadius > 0); m_RingRadius = ringRadius; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_OuterRadius = RealType(1);
    RealType m_RingRadius  = RealType(0.25);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus28Object : public TorusObject<N, RealType> {
public:
    Torus28Object() = delete;
    Torus28Object(const JParams& jParams) : TorusObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus2InfObject : public TorusObject<N, RealType> {
public:
    Torus2InfObject() = delete;
    Torus2InfObject(const JParams& jParams) : TorusObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Torus88Object : public TorusObject<N, RealType> {
public:
    Torus88Object() = delete;
    Torus88Object(const JParams& jParams) : TorusObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TorusInfInfObject : public TorusObject<N, RealType> {
public:
    TorusInfInfObject() = delete;
    TorusInfInfObject(const JParams& jParams) : TorusObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CylinderObject : public GeometryObject<N, RealType> {
public:
    CylinderObject() = delete;
    CylinderObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("CylinderObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setRadius(RealType radius) { m_Radius = radius; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_Radius = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ConeObject : public GeometryObject<N, RealType> {
public:
    ConeObject() = delete;
    ConeObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("ConeObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setBaseRadius(RealType radius) { m_Radius = radius; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_Radius = RealType(1);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class PlaneObject : public GeometryObject<N, RealType> {
public:
    PlaneObject() = delete;
    PlaneObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("PlaneObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void setNormal(const VecX<N, RealType>& normal) { m_Normal = normal; }
    void setOffset(RealType offset) { m_Offset = offset; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
    RealType          m_Offset = RealType(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangleObject : public GeometryObject<N, RealType> {
public:
    TriangleObject() = delete;
    TriangleObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("TriangleObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    template<class IndexType> void setVertex(IndexType idx, const VecX<N, RealType>& vertex) { m_Vertices[idx] = vertex; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    VecX<N, RealType> m_Vertices[3];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonObject : public GeometryObject<N, RealType> {
public:
    HexagonObject() = delete;
    HexagonObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("HexagonObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriangularPrismObject : public GeometryObject<N, RealType> {
public:
    TriangularPrismObject() = delete;
    TriangularPrismObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("TriangularPrismObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setWidth(RealType ratio) { m_Width = ratio; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_Width = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HexagonalPrismObject : public GeometryObject<N, RealType> {
public:
    HexagonalPrismObject() = delete;
    HexagonalPrismObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("HexagonalPrismObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void             setWidth(RealType ratio) { m_Width = ratio; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_Width = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CapsuleObject : public GeometryObject<N, RealType> {
public:
    CapsuleObject() = delete;
    CapsuleObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("CapsuleObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setRadius(RealType radius) { m_Radius = radius; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    RealType m_Radius = RealType(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class EllipsoidObject : public GeometryObject<N, RealType> {
public:
    EllipsoidObject() = delete;
    EllipsoidObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("SphereObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;
    void setRadiusRatio(const VecX<N, RealType>& scale) { m_RadiusRatio = scale; }
protected:
    virtual void parseParameters(const JParams& jParams) override;
    VecX<N, RealType> m_RadiusRatio = VecX<N, RealType>(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class TriMeshObject : public GeometryObject<N, RealType> {
public:
    TriMeshObject() = delete;
    TriMeshObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("TriMeshObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>&, bool bNegativeInside = true) const override;

    auto& meshFile() { return m_TriMeshFile; }
    auto& sdfStep() { return m_Step; }
    void computeSDF();
protected:
    virtual void parseParameters(const JParams& jParams) override;
    bool               m_bSDFGenerated = false;
    String             m_TriMeshFile   = String("");
    RealType           m_Step          = RealType(1.0 / 256.0);
    Array<N, RealType> m_SDFData;
    Grid<N, RealType>  m_Grid3D;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum CSGOperations {
    Overwrite,
    Union,
    Subtraction,
    Intersection,
    BlendExp,
    BlendPoly
};

enum DomainDeformation {
    None,
    Twist,
    CheapBend
};

template<Int N, class RealType>
class CSGObject : public GeometryObject<N, RealType> {
public:
    struct CSGData {
        SharedPtr<GeometryObject<N, RealType>> obj = nullptr;
        CSGOperations                          op  = Union;
    };
    CSGObject() = delete;
    CSGObject(const JParams& jParams) : GeometryObject<N, RealType>(jParams) { this->parseParameters(jParams); }
    virtual String   name() override { return String("CSGObject"); }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside = true) const override;

    void addObject(const CSGData& obj) { m_Objects.push_back(obj); }
    void addObject(const SharedPtr<GeometryObject<N, RealType>>& obj, CSGOperations op = Union) { addObject({ obj, op }); }
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
} // end namespace Banana::GeometryObjects
