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
template<class Real>
class MeshLoader
{
public:
    MeshLoader();
    MeshLoader(const std::string& meshFile);

    bool  loadMesh(const std::string& meshFile);
    Vec3r getMeshCenter() const;
    const Vec3r& getAABBMin() const { return m_AABBMin; }
    const Vec3r& getAABBMax() const { return m_AABBMax; }

    Vec3r getCameraPosition(Vec3r camDirection, Real fov = 45);
    Real  getCameraDistance(Real fov);

    const Vec_UInt& getFaces() const { assert(m_isMeshReady); return m_Faces; }
    const Vec_Real& getVertexNormal() const { assert(m_isMeshReady); return m_VertexNormals; }
    const Vec_Real& getVertexColor() const { assert(m_isMeshReady); return m_VertexColors; }
    const Vec_Real& getVTexCoord2D() const { assert(m_isMeshReady); return m_VertexTexCoord2D; }
    const Vec_Real& getVTexCoord3D() const { assert(m_isMeshReady); return m_VertexTexCoord3D; }
    const Vec_Real& getVertices() const { assert(m_isMeshReady); return m_Vertices; }
    const Vec_Real& getFaceVertices() const { assert(m_isMeshReady); return m_FaceVertices; }

private:
    void checkFileType(const std::string& meshFile);
    void clearData();

    bool loadObj(const std::string& meshFile);
    bool loadPly(const std::string& meshFile);

    void computeVertexNormal(Real N[3], Real v0[3], Real v1[3], Real v2[3]);

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
    std::string  m_LoadingErrorStr;

    Vec_UInt m_Faces;
    Vec_Real m_Vertices;
    Vec_Real m_FaceVertices;
    Vec_Real m_VertexNormals;
    Vec_Real m_VertexColors;
    Vec_Real m_VertexTexCoord2D;
    Vec_Real m_VertexTexCoord3D;
    Vec3r    m_AABBMin;
    Vec3r    m_AABBMax;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Geometry/MeshLoader.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana