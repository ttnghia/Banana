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

    bool         loadMesh(const String& meshFile);
    Vec3f        getMeshCenter() const;
    const Vec3f& getAABBMin() const { return m_AABBMin; }
    const Vec3f& getAABBMax() const { return m_AABBMax; }

    Vec3f getCameraPosition(Vec3f camDirection, float fov = 45);
    float getCameraDistance(float fov);

    const Vec_Float& getVertices() const { assert(m_isMeshReady); return m_Vertices; }
    const Vec_UInt&  getFaces() const { assert(m_isMeshReady); return m_Faces; }
    const Vec_Float& getFaceVertices() const { assert(m_isMeshReady); return m_FaceVertices; }
    const Vec_Float& getFaceVertexNormals() const { assert(m_isMeshReady); return m_FaceVertexNormals; }
    const Vec_Float& getFaceVertexColors() const { assert(m_isMeshReady); return m_FaceVertexColors; }
    const Vec_Float& getFaceVTexCoord2D() const { assert(m_isMeshReady); return m_FaceVertexTexCoord2D; }

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
    Vec_Float m_FaceVertices;
    Vec_Float m_FaceVertexNormals;
    Vec_Float m_FaceVertexColors;
    Vec_Float m_FaceVertexTexCoord2D;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana