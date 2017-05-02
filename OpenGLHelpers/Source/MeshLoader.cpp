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

#include <OpenGLHelpers/MeshLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MeshLoader::MeshLoader() : m_isMeshReady(false)
{
    clearData();
}

MeshLoader::MeshLoader(std::string mesh_file) :
    m_isMeshReady(false)
{
    loadMesh(mesh_file);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::loadMesh(std::string mesh_file)
{
    checkFileType(mesh_file);

    ////////////////////////////////////////////////////////////////////////////////
    // => clear data
    clearData();

    ////////////////////////////////////////////////////////////////////////////////
    // => load mesh
    bool result = false;

    switch(m_MeshFileType)
    {
        case MeshFileType::OBJFile:
            result = load_obj(mesh_file);
            break;

        case MeshFileType::PLYFile:
            result = load_ply(mesh_file);
            break;

        default:
            assert(false);
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_isMeshReady = result;

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::setDefaultCamera(Camera& camera, float fov)
{
    glm::vec3 currentDir = camera.getCameraDirection();
    camera.setDefaultCamera(currentDir * getCameraDistance(fov * 0.75f) +
        getMeshCenter(),
        getMeshCenter(), glm::vec3(0, 1, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float MeshLoader::getCameraDistance(float fov)
{
    float half_length = (m_BBoxMax.y - m_BBoxMin.y) * 0.5f;

    return (half_length / std::tan(fov * 0.5f * M_PI / 180.0f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<float>& MeshLoader::getVertices()
{
    assert(m_isMeshReady);

    return m_Vertices;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<float>& MeshLoader::getVertexNormal()
{
    assert(m_isMeshReady);

    return m_VertexNormals;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<float>& MeshLoader::getVertexColor()
{
    assert(m_isMeshReady);

    return m_VertexColors;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<float>& MeshLoader::getVTexCoord2D()
{
    assert(m_isMeshReady);

    return m_VertexTexCoord2D;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<float>& MeshLoader::getVTexCoord3D()
{
    assert(m_isMeshReady);

    return m_VertexTexCoord3D;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::checkFileType(std::string mesh_file)
{
    const std::string extension = mesh_file.substr(mesh_file.rfind('.') + 1);
    m_MeshFileType = MeshFileType::UnsupportedFileType;

    if(extension == "obj" || extension == "OBJ" || extension == "Obj")
    {
        m_MeshFileType = MeshFileType::OBJFile;
    }

    if(extension == "ply" || extension == "PLY" || extension == "Ply")
    {
        m_MeshFileType = MeshFileType::PLYFile;
    }

    assert(m_MeshFileType != MeshFileType::UnsupportedFileType);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::clearData()
{
    m_NumTriangles = 0;

    m_BBoxMin = glm::vec3(1e10, 1e10, 1e10);
    m_BBoxMax = glm::vec3(-1e10, -1e10, -1e10);

    m_Vertices.clear();
    m_VertexNormals.clear();
    m_VertexColors.clear();
    m_VertexTexCoord2D.clear();
    m_VertexTexCoord3D.clear();

    ////////////////////////////////////////////////////////////////////////////////
    m_LoadingErrorStr.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::load_obj(std::string mesh_file)
{
    // if mesh is obj
    std::vector<tinyobj::shape_t>    obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;
    tinyobj::attrib_t                attrib;


    bool result = tinyobj::LoadObj(&attrib, &obj_shapes, &obj_materials,
        &m_LoadingErrorStr, mesh_file.c_str(), NULL);

    if(!m_LoadingErrorStr.empty())
    {
        std::cerr << "tinyobj: " << m_LoadingErrorStr << std::endl;
    }

    if(!result)
    {
        std::cerr << "Failed to load " << mesh_file << std::endl;
        return false;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // => convert data
    for(size_t s = 0; s < obj_shapes.size(); s++)
    {
        for(size_t f = 0; f < obj_shapes[s].mesh.indices.size() / 3; f++)
        {
            ++m_NumTriangles;

            tinyobj::index_t idx0 = obj_shapes[s].mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = obj_shapes[s].mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = obj_shapes[s].mesh.indices[3 * f + 2];

            float            v[3][3];

            for(int k = 0; k < 3; k++)
            {
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = attrib.vertices[3 * f0 + k];
                v[1][k] = attrib.vertices[3 * f1 + k];
                v[2][k] = attrib.vertices[3 * f2 + k];

                m_BBoxMin[k] = std::min(v[0][k], m_BBoxMin[k]);
                m_BBoxMin[k] = std::min(v[1][k], m_BBoxMin[k]);
                m_BBoxMin[k] = std::min(v[2][k], m_BBoxMin[k]);

                m_BBoxMax[k] = std::max(v[0][k], m_BBoxMax[k]);
                m_BBoxMax[k] = std::max(v[1][k], m_BBoxMax[k]);
                m_BBoxMax[k] = std::max(v[2][k], m_BBoxMax[k]);
            }

            float n[3][3];

            if(attrib.normals.size() > 0)
            {
                int f0 = idx0.normal_index;
                int f1 = idx1.normal_index;
                int f2 = idx2.normal_index;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                for(int k = 0; k < 3; k++)
                {
                    n[0][k] = attrib.normals[3 * f0 + k];
                    n[1][k] = attrib.normals[3 * f1 + k];
                    n[2][k] = attrib.normals[3 * f2 + k];
                }
            }
            else
            {
                // compute geometric normal
                computeVertexNormal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
            }

            for(int k = 0; k < 3; k++)
            {
                m_Vertices.push_back(v[k][0]);
                m_Vertices.push_back(v[k][1]);
                m_Vertices.push_back(v[k][2]);

                m_VertexNormals.push_back(n[k][0]);
                m_VertexNormals.push_back(n[k][1]);
                m_VertexNormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = { n[k][0], n[k][1], n[k][2] };
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0.0f)
                {
                    float len = sqrtf(len2);

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                m_VertexColors.push_back(c[0] * 0.5f + 0.5f);
                m_VertexColors.push_back(c[1] * 0.5f + 0.5f);
                m_VertexColors.push_back(c[2] * 0.5f + 0.5f);
            }
        } // end process current shape
    }

    ////////////////////////////////////////////////////////////////////////////////
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::load_ply(std::string mesh_file)
{
    // Tinyply can and will throw exceptions at you!
    try
    {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(mesh_file, std::ios::binary);

        // Parse the ASCII header fields
        tinyply::PlyFile file(ss);

        // Define containers to hold the extracted data. The type must match
        // the property type given in the header. Tinyply will interally allocate the
        // the appropriate amount of memory.
        std::vector<float>    verts;
        std::vector<float>    norms;
        std::vector<uint8_t>  colors;

        std::vector<uint32_t> faces;
        std::vector<float>    uvCoords;
        //        std::vector<uint8_t> faceColors;

        size_t vertexCount       = 0;
        size_t normalCount       = 0;
        size_t colorCount        = 0;
        size_t faceCount         = 0;
        size_t faceTexcoordCount = 0;

        // The count returns the number of instances of the property group. The vectors
        // above will be resized into a multiple of the property group size as
        // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
        vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
        normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
        colorCount  = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" },
            colors);

        // For properties that are list types, it is possibly to specify the expected count (ideal if a
        // consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
        // defers allocation of memory until the first instance of the property has been found
        // as implemented in file.read(ss)
        faceCount         = file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
        faceTexcoordCount = file.request_properties_from_element("face", { "texcoord" }, uvCoords,
            6);
        //        faceColorCount = file.request_properties_from_element("face", {"red", "green", "blue", "alpha"}, faceColors);

        // Now populate the vectors...
        file.read(ss);

        ////////////////////////////////////////////////////////////////////////////////
        // => convert data
        for(size_t f = 0; f < faces.size() / 3; f++)
        {
            ++m_NumTriangles;

            uint32_t f0 = faces[3 * f + 0];
            uint32_t f1 = faces[3 * f + 1];
            uint32_t f2 = faces[3 * f + 2];

            float    v[3][3];

            for(int k = 0; k < 3; k++)
            {
                v[0][k] = verts[3 * f0 + k];
                v[1][k] = verts[3 * f1 + k];
                v[2][k] = verts[3 * f2 + k];

                m_BBoxMin[k] = std::min(v[0][k], m_BBoxMin[k]);
                m_BBoxMin[k] = std::min(v[1][k], m_BBoxMin[k]);
                m_BBoxMin[k] = std::min(v[2][k], m_BBoxMin[k]);

                m_BBoxMax[k] = std::max(v[0][k], m_BBoxMax[k]);
                m_BBoxMax[k] = std::max(v[1][k], m_BBoxMax[k]);
                m_BBoxMax[k] = std::max(v[2][k], m_BBoxMax[k]);
            }

            float n[3][3];

            if(norms.size() > 0)
            {
                for(int k = 0; k < 3; k++)
                {
                    n[0][k] = norms[3 * f0 + k];
                    n[1][k] = norms[3 * f1 + k];
                    n[2][k] = norms[3 * f2 + k];
                }
            }
            else
            {
                // compute geometric normal
                computeVertexNormal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
            }

            for(int k = 0; k < 3; k++)
            {
                m_Vertices.push_back(v[k][0]);
                m_Vertices.push_back(v[k][1]);
                m_Vertices.push_back(v[k][2]);

                m_VertexNormals.push_back(n[k][0]);
                m_VertexNormals.push_back(n[k][1]);
                m_VertexNormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = { n[k][0], n[k][1], n[k][2] };
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0.0f)
                {
                    float len = sqrtf(len2);

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                m_VertexColors.push_back(c[0] * 0.5f + 0.5f);
                m_VertexColors.push_back(c[1] * 0.5f + 0.5f);
                m_VertexColors.push_back(c[2] * 0.5f + 0.5f);
            }
        } // end process current shape
    }
    catch(const std::exception& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::computeVertexNormal(float N[3], float v0[3], float v1[3], float v2[3])
{
    float v10[3];
    v10[0] = v1[0] - v0[0];
    v10[1] = v1[1] - v0[1];
    v10[2] = v1[2] - v0[2];

    float v20[3];
    v20[0] = v2[0] - v0[0];
    v20[1] = v2[1] - v0[1];
    v20[2] = v2[2] - v0[2];

    N[0] = v20[1] * v10[2] - v20[2] * v10[1];
    N[1] = v20[2] * v10[0] - v20[0] * v10[2];
    N[2] = v20[0] * v10[1] - v20[1] * v10[0];

    float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];

    if(len2 > 0.0f)
    {
        float len = sqrtf(len2);

        N[0] /= len;
        N[1] /= len;
        N[2] /= len;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 MeshLoader::getMeshCenter()
{
    return 0.5f * (m_BBoxMin + m_BBoxMax);
}
