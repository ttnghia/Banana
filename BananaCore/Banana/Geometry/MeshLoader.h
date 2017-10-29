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

#include <fstream>

#include <tiny_obj_loader.h>
#include <tinyply.h>

#include <Banana/Setup.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This class need to be a template, as it can be used in simulation(double) and rendering(float)
class MeshLoader
{
public:
    MeshLoader() : m_isMeshReady(false) { clearData(); }
    MeshLoader(const String& meshFile) : m_isMeshReady(false) { loadMesh(meshFile); }

    bool        loadMesh(const String& meshFile);
    Vec3f       getMeshCenter() const { assert(m_isMeshReady); return float(0.5) * (m_AABBMin + m_AABBMax); }
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

    size_t getNFaces() const noexcept { assert(m_isMeshReady); return (m_Faces.size() / 3); }
    size_t getNVertices() const noexcept { assert(m_isMeshReady); return (m_Vertices.size() / 3); }
    size_t getNFaceVertices() const noexcept { assert(m_isMeshReady); return (m_FaceVertices.size() / 3); }

private:
    void checkFileType(const String& meshFile);
    void clearData();

    bool loadObj(const String& meshFile);
    bool loadPly(const String& meshFile);

    void computeFaceVertexData();

    ////////////////////////////////////////////////////////////////////////////////
    enum class MeshFileType
    {
        OBJFile,
        PLYFile,
        UnsupportedFileType
    };

    unsigned int m_NumTriangles;
    bool         m_isMeshReady;

    MeshFileType m_MeshFileType;
    String       m_LoadingErrorStr;

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