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

#include <Banana/Geometry/MeshLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::loadMesh(const String& meshFile)
{
    checkFileType(meshFile);

    ////////////////////////////////////////////////////////////////////////////////
    // => clear data
    clearData();

    ////////////////////////////////////////////////////////////////////////////////
    // => load mesh
    bool result = false;

    switch(m_MeshFileType) {
        case MeshFileType::OBJFile:
            result = loadObj(meshFile);
            break;

        case MeshFileType::PLYFile:
            result = loadPly(meshFile);
            break;

        default:
            assert(false);
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_isMeshReady = result;

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3f MeshLoader::getMeshCenter() const
{
    return float(0.5) * (m_AABBMin + m_AABBMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3f MeshLoader::getCameraPosition(Vec3f camDirection, float fov /*= 45*/)
{
    return camDirection * getCameraDistance(fov * float(0.75)) + getMeshCenter();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float MeshLoader::getCameraDistance(float fov)
{
    float halfLength = (m_AABBMax.y - m_AABBMin.y) * float(0.5);

    return (halfLength / std::tan(fov * float(0.5 * M_PI / 180.0)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::checkFileType(const String& meshFile)
{
    const String extension = meshFile.substr(meshFile.rfind('.') + 1);
    m_MeshFileType = MeshFileType::UnsupportedFileType;

    if(extension == "obj" || extension == "OBJ" || extension == "Obj") {
        m_MeshFileType = MeshFileType::OBJFile;
    }

    if(extension == "ply" || extension == "PLY" || extension == "Ply") {
        m_MeshFileType = MeshFileType::PLYFile;
    }

    assert(m_MeshFileType != MeshFileType::UnsupportedFileType);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::clearData()
{
    m_NumTriangles = 0;

    m_AABBMin = Vec3f(1e10, 1e10, 1e10);
    m_AABBMax = Vec3f(-1e10, -1e10, -1e10);

    m_Faces.resize(0);
    m_Vertices.resize(0);
    m_FaceVertices.resize(0);
    m_VertexNormals.resize(0);
    m_VertexColors.resize(0);
    m_VertexTexCoord2D.resize(0);
    m_VertexTexCoord3D.resize(0);

    ////////////////////////////////////////////////////////////////////////////////
    m_LoadingErrorStr.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::loadObj(const String& meshFile)
{
    std::vector<tinyobj::shape_t>    obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;
    tinyobj::attrib_t                attrib;


    bool result = tinyobj::LoadObj(&attrib, &obj_shapes, &obj_materials, &m_LoadingErrorStr, meshFile.c_str(), NULL);
    m_Vertices = attrib.vertices;

    if(!m_LoadingErrorStr.empty()) {
        std::cerr << "tinyobj: " << m_LoadingErrorStr << std::endl;
    }

    if(!result) {
        std::cerr << "Failed to load " << meshFile << std::endl;
        return false;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // => convert data
    for(size_t s = 0; s < obj_shapes.size(); s++) {
        for(size_t f = 0; f < obj_shapes[s].mesh.indices.size() / 3; ++f) {
            ++m_NumTriangles;

            tinyobj::index_t idx0 = obj_shapes[s].mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = obj_shapes[s].mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = obj_shapes[s].mesh.indices[3 * f + 2];

            Vec3f v[3];

            for(int k = 0; k < 3; k++) {
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;
                m_Faces.push_back(static_cast<unsigned int>(f0));
                m_Faces.push_back(static_cast<unsigned int>(f1));
                m_Faces.push_back(static_cast<unsigned int>(f2));

                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = attrib.vertices[3 * f0 + k];
                v[1][k] = attrib.vertices[3 * f1 + k];
                v[2][k] = attrib.vertices[3 * f2 + k];

                m_AABBMin[k] = std::min(v[0][k], m_AABBMin[k]);
                m_AABBMin[k] = std::min(v[1][k], m_AABBMin[k]);
                m_AABBMin[k] = std::min(v[2][k], m_AABBMin[k]);

                m_AABBMax[k] = std::max(v[0][k], m_AABBMax[k]);
                m_AABBMax[k] = std::max(v[1][k], m_AABBMax[k]);
                m_AABBMax[k] = std::max(v[2][k], m_AABBMax[k]);
            }

            Vec3f n[3];

            if(attrib.normals.size() > 0) {
                int f0 = idx0.normal_index;
                int f1 = idx1.normal_index;
                int f2 = idx2.normal_index;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                for(int k = 0; k < 3; k++) {
                    n[0][k] = attrib.normals[3 * f0 + k];
                    n[1][k] = attrib.normals[3 * f1 + k];
                    n[2][k] = attrib.normals[3 * f2 + k];
                }
            }
            else{
                // compute geometric normal
                n[0] = computeVertexNormal(v[0], v[1], v[2]);
                n[1] = n[0];
                n[2] = n[0];
            }

            for(int k = 0; k < 3; k++) {
                m_FaceVertices.push_back(v[k][0]);
                m_FaceVertices.push_back(v[k][1]);
                m_FaceVertices.push_back(v[k][2]);

                m_VertexNormals.push_back(n[k][0]);
                m_VertexNormals.push_back(n[k][1]);
                m_VertexNormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = { n[k][0], n[k][1], n[k][2] };
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0) {
                    float len = float(sqrt(len2));

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                m_VertexColors.push_back(c[0] * float(0.5) + float(0.5));
                m_VertexColors.push_back(c[1] * float(0.5) + float(0.5));
                m_VertexColors.push_back(c[2] * float(0.5) + float(0.5));
            }
        }         // end process current shape
    }

    ////////////////////////////////////////////////////////////////////////////////
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::loadPly(const String& meshFile)
{
    // Tinyply can and will throw exceptions at you!
    try
    {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(meshFile, std::ios::binary);

        // Parse the ASCII header fields
        tinyply::PlyFile file(ss);

        // Define containers to hold the extracted data. The type must match
        // the property type given in the header. Tinyply will interally allocate the
        // the appropriate amount of memory.
        std::vector<float>   norms;
        std::vector<uint8_t> colors;

        std::vector<float> uvCoords;
        //        std::vector<uint8_t> faceColors;

        size_t vertexCount       = 0;
        size_t normalCount       = 0;
        size_t colorCount        = 0;
        size_t faceCount         = 0;
        size_t faceTexcoordCount = 0;

        // The count returns the number of instances of the property group. The vectors
        // above will be resized into a multiple of the property group size as
        // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
        vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, m_Vertices);
        normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
        colorCount  = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

        //printf("vertexCount: %lu\n", vertexCount);

        // For properties that are list types, it is possibly to specify the expected count (ideal if a
        // consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
        // defers allocation of memory until the first instance of the property has been found
        // as implemented in file.read(ss)
        m_Faces.resize(0);
        faceCount         = file.request_properties_from_element("face", { "vertex_indices" }, m_Faces, 3);
        faceTexcoordCount = file.request_properties_from_element("face", { "texcoord" }, uvCoords, 6);
        //        faceColorCount = file.request_properties_from_element("face", {"red", "green", "blue", "alpha"}, faceColors);

        // Now populate the vectors...
        file.read(ss);

        ////////////////////////////////////////////////////////////////////////////////
        // => convert data
        for(size_t f = 0; f < m_Faces.size() / 3; f++) {
            ++m_NumTriangles;

            uint32_t f0 = m_Faces[3 * f + 0];
            uint32_t f1 = m_Faces[3 * f + 1];
            uint32_t f2 = m_Faces[3 * f + 2];

            Vec3f v[3];

            for(int k = 0; k < 3; k++) {
                v[0][k] = m_Vertices[3 * f0 + k];
                v[1][k] = m_Vertices[3 * f1 + k];
                v[2][k] = m_Vertices[3 * f2 + k];

                m_AABBMin[k] = std::min(v[0][k], m_AABBMin[k]);
                m_AABBMin[k] = std::min(v[1][k], m_AABBMin[k]);
                m_AABBMin[k] = std::min(v[2][k], m_AABBMin[k]);

                m_AABBMax[k] = std::max(v[0][k], m_AABBMax[k]);
                m_AABBMax[k] = std::max(v[1][k], m_AABBMax[k]);
                m_AABBMax[k] = std::max(v[2][k], m_AABBMax[k]);
            }

            Vec3f n[3];

            if(norms.size() > 0) {
                for(int k = 0; k < 3; k++) {
                    n[0][k] = norms[3 * f0 + k];
                    n[1][k] = norms[3 * f1 + k];
                    n[2][k] = norms[3 * f2 + k];
                }
            }
            else{
                // compute geometric normal
                n[0] = computeVertexNormal(v[0], v[1], v[2]);
                n[1] = n[0];
                n[2] = n[0];
            }

            for(int k = 0; k < 3; k++) {
                m_FaceVertices.push_back(v[k][0]);
                m_FaceVertices.push_back(v[k][1]);
                m_FaceVertices.push_back(v[k][2]);

                m_VertexNormals.push_back(n[k][0]);
                m_VertexNormals.push_back(n[k][1]);
                m_VertexNormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = { n[k][0], n[k][1], n[k][2] };
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0) {
                    float len = float(sqrt(len2));

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                m_VertexColors.push_back(c[0] * float(0.5) + float(0.5));
                m_VertexColors.push_back(c[1] * float(0.5) + float(0.5));
                m_VertexColors.push_back(c[2] * float(0.5) + float(0.5));
            }
        }         // end process current shape
    }
    catch(const std::exception& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana