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

#include <BananaCore/TypeNames.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
class MeshLoader
{
public:
    MeshLoader();
    MeshLoader(const std::string& meshFile);

    bool    loadMesh(const std::string& meshFile);
    Vec3<T> getMeshCenter();

    Vec3<T> getCameraPosition(Vec3<T> camDirection, T fov = 45);
    T       getCameraDistance(T fov);

    std::vector<T>& getVertices() { assert(m_isMeshReady); return m_Vertices; }
    std::vector<T>& getVertexNormal(){ assert(m_isMeshReady); return m_VertexNormals; }
    std::vector<T>& getVertexColor() { assert(m_isMeshReady); return m_VertexColors; }
    std::vector<T>& getVTexCoord2D(){ assert(m_isMeshReady); return m_VertexTexCoord2D; }
    std::vector<T>& getVTexCoord3D(){ assert(m_isMeshReady); return m_VertexTexCoord3D; }

private:
    void checkFileType(const std::string& meshFile);
    void clearData();

    bool loadObj(const std::string& meshFile);
    bool loadPly(const std::string& meshFile);

    void computeVertexNormal(T N[3], T v0[3], T v1[3], T v2[3]);

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

    std::vector<T> m_Vertices;
    std::vector<T> m_VertexNormals;
    std::vector<T> m_VertexColors;
    std::vector<T> m_VertexTexCoord2D;
    std::vector<T> m_VertexTexCoord3D;
    Vec3<T>        m_BBoxMin;
    Vec3<T>        m_BBoxMax;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <BananaCore/Geometry/MeshLoader_Impl.hpp>
