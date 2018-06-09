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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This class need to be a template, as it can be used in simulation(double) and rendering(float)
class MeshLoader
{
public:
    MeshLoader() { clearData(); }
    MeshLoader(String meshFile) { loadMesh(meshFile); }

    bool loadMesh(const String& meshFile);
    bool isMeshReady() const { return m_isMeshReady; }
    void scaleToBox();

    auto  getMeshCenter() const { assert(m_isMeshReady); return float(0.5) * (m_AABBMin + m_AABBMax); }
    auto  getNTriangles() const { return m_NumTriangles; }
    const auto& getAABBMin() const { assert(m_isMeshReady); return m_AABBMin; }
    const auto& getAABBMax() const { assert(m_isMeshReady); return m_AABBMax; }

    Vec3f getCameraPosition(Vec3f camDirection, float fov = 45);
    float getCameraDistance(float fov);

    const auto& getVertices() const { assert(m_isMeshReady); return m_Vertices; }
    const auto& getNormals() const { assert(m_isMeshReady); return m_Normals; }
    const auto& getTexCoord2D() const { assert(m_isMeshReady); return m_TexCoord2D; }

    const auto& getFaces() const { assert(m_isMeshReady); return m_Faces; }
    const auto& getFaceVertices() const { assert(m_isMeshReady); return m_FaceVertices; }
    const auto& getFaceVertexNormals() const { assert(m_isMeshReady); return m_FaceVertexNormals; }
    const auto& getFaceVertexColors() const { assert(m_isMeshReady); return m_FaceVertexColors; }
    const auto& getFaceVTexCoord2D() const { assert(m_isMeshReady); return m_FaceVertexTexCoord2D; }

    auto getNFaces() const noexcept { assert(m_isMeshReady); return (m_Faces.size() / 3); }
    auto getNVertices() const noexcept { assert(m_isMeshReady); return (m_Vertices.size() / 3); }
    auto getNFaceVertices() const noexcept { assert(m_isMeshReady); return (m_FaceVertices.size() / 3); }

    void swapXY() { swapCoordinates(0, 1); }
    void swapYZ() { swapCoordinates(1, 2); }
    void swapXZ() { swapCoordinates(0, 2); }
private:
    enum class MeshFileType
    {
        OBJFile,
        PLYFile,
        UnsupportedFileType
    };
    MeshFileType getMeshType(const String& meshFile);
    bool         loadObj(const String& meshFile);
    bool         loadPly(const String& meshFile);

    void clearData();
    void swapCoordinates(int k1, int k2);
    void computeFaceVertexData();
    ////////////////////////////////////////////////////////////////////////////////

    UInt m_NumTriangles = 0;
    bool m_isMeshReady  = false;

    Vec3f m_AABBMin;
    Vec3f m_AABBMax;

    Vec_UInt  m_Faces;
    Vec_Float m_Vertices;
    Vec_Float m_Normals;
    Vec_Float m_TexCoord2D;
    Vec_Float m_FaceVertices;
    Vec_Float m_FaceVertexNormals;
    Vec_Float m_FaceVertexColors;
    Vec_Float m_FaceVertexTexCoord2D;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana