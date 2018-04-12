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

#include <Banana/Geometry/MeshLoader.h>
#include <Banana/Utils/MathHelpers.h>

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
            __BNN_DENIED_SWITCH_DEFAULT_VALUE;
    }

    m_isMeshReady = result;
    computeFaceVertexData();

    ////////////////////////////////////////////////////////////////////////////////

    return result;
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

    m_AABBMin = Vec3f(1e10);
    m_AABBMax = Vec3f(-1e10);

    m_Faces.resize(0);

    m_Vertices.resize(0);
    m_Normals.resize(0);
    m_FaceVertices.resize(0);
    m_FaceVertexNormals.resize(0);
    m_FaceVertexColors.resize(0);
    m_FaceVertexTexCoord2D.resize(0);

    ////////////////////////////////////////////////////////////////////////////////
    m_LoadingErrorStr.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MeshLoader::loadObj(const String& meshFile)
{
    std::vector<tinyobj::shape_t>    obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;
    tinyobj::attrib_t                attrib;

    bool result = tinyobj::LoadObj(&attrib, &obj_shapes, &obj_materials, &m_LoadingErrorStr, meshFile.c_str(), NULL, true);
    m_Vertices   = attrib.vertices;
    m_Normals    = attrib.normals;
    m_TexCoord2D = attrib.texcoords;

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

            int v0 = idx0.vertex_index;
            int v1 = idx1.vertex_index;
            int v2 = idx2.vertex_index;
            assert(v0 >= 0);
            assert(v1 >= 0);
            assert(v2 >= 0);

            m_Faces.push_back(static_cast<UInt>(v0));
            m_Faces.push_back(static_cast<UInt>(v1));
            m_Faces.push_back(static_cast<UInt>(v2));

            Vec_Vec3f v(3, Vec3f(0));
            for(int k = 0; k < 3; ++k) {
                v[0][k] = m_Vertices[3 * v0 + k];
                v[1][k] = m_Vertices[3 * v1 + k];
                v[2][k] = m_Vertices[3 * v2 + k];

                m_AABBMin[k] = MathHelpers::min(v[0][k], m_AABBMin[k]);
                m_AABBMin[k] = MathHelpers::min(v[1][k], m_AABBMin[k]);
                m_AABBMin[k] = MathHelpers::min(v[2][k], m_AABBMin[k]);

                m_AABBMax[k] = MathHelpers::max(v[0][k], m_AABBMax[k]);
                m_AABBMax[k] = MathHelpers::max(v[1][k], m_AABBMax[k]);
                m_AABBMax[k] = MathHelpers::max(v[2][k], m_AABBMax[k]);
            }

            for(int k = 0; k < 3; ++k) {
                m_FaceVertices.push_back(v[k][0]);
                m_FaceVertices.push_back(v[k][1]);
                m_FaceVertices.push_back(v[k][2]);
            }

            if(attrib.normals.size() > 0) {
                Vec_Vec3f n(3, Vec3f(0));
                int       n0 = idx0.normal_index;
                int       n1 = idx1.normal_index;
                int       n2 = idx2.normal_index;
                assert(n0 >= 0);
                assert(n1 >= 0);
                assert(n2 >= 0);

                for(int k = 0; k < 3; ++k) {
                    n[0][k] = attrib.normals[3 * n0 + k];
                    n[1][k] = attrib.normals[3 * n1 + k];
                    n[2][k] = attrib.normals[3 * n2 + k];
                }

                for(int k = 0; k < 3; ++k) {
                    m_FaceVertexNormals.push_back(n[k][0]);
                    m_FaceVertexNormals.push_back(n[k][1]);
                    m_FaceVertexNormals.push_back(n[k][2]);
                }
            }

            if(attrib.texcoords.size() > 0) {
                Vec_Vec2f tex(3, Vec2f(0));
                int       t0 = idx0.texcoord_index;
                int       t1 = idx1.texcoord_index;
                int       t2 = idx2.texcoord_index;
                assert(t0 >= 0);
                assert(t1 >= 0);
                assert(t2 >= 0);

                for(int k = 0; k < 2; ++k) {
                    tex[0][k] = attrib.texcoords[2 * t0 + k];
                    tex[1][k] = attrib.texcoords[2 * t1 + k];
                    tex[2][k] = attrib.texcoords[2 * t1 + k];
                }

                for(int k = 0; k < 3; ++k) {
                    m_FaceVertexTexCoord2D.push_back(tex[k][0]);
                    m_FaceVertexTexCoord2D.push_back(tex[k][1]);
                }
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
    try {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(meshFile, std::ios::binary);
        if(ss.fail()) { return false; }

        // Parse the ASCII header fields
        tinyply::PlyFile file;
        file.parse_header(ss);

        Int hasNormals   = 0;
        Int hasTexCoords = 0;
        for(auto e : file.get_elements()) {
            //std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
            for(auto p : e.properties) {
                //std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
                if(p.name == "nx" || p.name == "ny" || p.name == "nz") { ++hasNormals; }
                if(p.name == "texcoord") { ++hasTexCoords; }
            }
        }
        __BNN_REQUIRE(hasNormals == 0 || hasNormals == 3);

        // Define containers to hold the extracted data. The type must match
        // the property type given in the header. Tinyply will interally allocate the
        // the appropriate amount of memory.

        std::shared_ptr<tinyply::PlyData> vertices, normals, faces, texcoords;

        try {
            vertices = file.request_properties_from_element("vertex", { "x", "y", "z" });
        } catch(const std::exception& e) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        try {
            faces = file.request_properties_from_element("face", { "vertex_indices" });
        } catch(const std::exception& e) {
            std::cerr << "tinyply exception: " << e.what() << std::endl;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // optional
        if(hasNormals) {
            try {
                normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" });
            } catch(const std::exception& e) {
                std::cerr << "tinyply exception: " << e.what() << std::endl;
            }
        }
        if(hasTexCoords) {
            try {
                texcoords = file.request_properties_from_element("face", { "texcoord" });
            } catch(const std::exception& e) {
                std::cerr << "tinyply exception: " << e.what() << std::endl;
            }
        }

        // Now populate the vectors...
        file.read(ss);
        __BNN_REQUIRE(vertices->t == tinyply::Type::FLOAT32);
        if(normals != nullptr) {
            __BNN_REQUIRE(normals->count == 0 || normals->count == vertices->count);
            m_Normals.resize(normals->count * 3);
            std::memcpy(m_Normals.data(), normals->buffer.get(), normals->buffer.size_bytes());
        }
        if(texcoords != nullptr) {
            m_TexCoord2D.resize(texcoords->count * 6);
            std::memcpy(m_TexCoord2D.data(), texcoords->buffer.get(), texcoords->buffer.size_bytes());
        }

        m_Vertices.resize(vertices->count * 3);
        m_Faces.resize(faces->count * 3);
        std::memcpy(m_Vertices.data(), vertices->buffer.get(), vertices->buffer.size_bytes());

        __BNN_REQUIRE(faces->buffer.size_bytes() / sizeof(UInt) == m_Faces.size());
        if(faces->t == tinyply::Type::UINT32) {
            std::memcpy(m_Faces.data(), faces->buffer.get(), faces->buffer.size_bytes());
        } else if(faces->t == tinyply::Type::INT32) {
            Int* ptr = reinterpret_cast<Int*>(faces->buffer.get());
            for(size_t i = 0; i < faces->count * 3; ++i) {
                m_Faces[i] = static_cast<UInt>(ptr[i]);
            }
        } else {
            __BNN_CALLED_TO_WRONG_PLACE
        }

        ////////////////////////////////////////////////////////////////////////////////
        // => convert data
        for(size_t f = 0; f < getNFaces(); f++) {
            ++m_NumTriangles;

            UInt v0 = m_Faces[3 * f + 0];
            UInt v1 = m_Faces[3 * f + 1];
            UInt v2 = m_Faces[3 * f + 2];

            Vec_Vec3f v(3, Vec3f(0));
            for(int k = 0; k < 3; ++k) {
                v[0][k] = m_Vertices[3 * v0 + k];
                v[1][k] = m_Vertices[3 * v1 + k];
                v[2][k] = m_Vertices[3 * v2 + k];

                m_AABBMin[k] = MathHelpers::min(v[0][k], m_AABBMin[k]);
                m_AABBMin[k] = MathHelpers::min(v[1][k], m_AABBMin[k]);
                m_AABBMin[k] = MathHelpers::min(v[2][k], m_AABBMin[k]);

                m_AABBMax[k] = MathHelpers::max(v[0][k], m_AABBMax[k]);
                m_AABBMax[k] = MathHelpers::max(v[1][k], m_AABBMax[k]);
                m_AABBMax[k] = MathHelpers::max(v[2][k], m_AABBMax[k]);
            }

            for(int k = 0; k < 3; ++k) {
                m_FaceVertices.push_back(v[k][0]);
                m_FaceVertices.push_back(v[k][1]);
                m_FaceVertices.push_back(v[k][2]);
            }

            if(m_Normals.size() > 0) {
                Vec_Vec3f n(3, Vec3f(0));
                for(int k = 0; k < 3; ++k) {
                    n[0][k] = m_Normals[3 * v0 + k];
                    n[1][k] = m_Normals[3 * v1 + k];
                    n[2][k] = m_Normals[3 * v2 + k];
                }

                for(int k = 0; k < 3; ++k) {
                    m_FaceVertexNormals.push_back(n[k][0]);
                    m_FaceVertexNormals.push_back(n[k][1]);
                    m_FaceVertexNormals.push_back(n[k][2]);
                }
            }

            if(m_TexCoord2D.size() > 0) {
                Vec_Vec2f tex(3, Vec2f(0));
                for(int k = 0; k < 2; ++k) {
                    tex[0][k] = m_TexCoord2D[2 * v0 + k];
                    tex[1][k] = m_TexCoord2D[2 * v1 + k];
                    tex[2][k] = m_TexCoord2D[2 * v1 + k];
                }

                for(int k = 0; k < 3; ++k) {
                    m_FaceVertexTexCoord2D.push_back(tex[k][0]);
                    m_FaceVertexTexCoord2D.push_back(tex[k][1]);
                }
            }
        }         // end process current shape
    } catch(const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshLoader::computeFaceVertexData()
{
    if(m_FaceVertexNormals.size() != m_FaceVertices.size()) {
        m_FaceVertexNormals.assign(m_FaceVertices.size(), 0);
        m_FaceVertexColors.assign(m_FaceVertices.size(), 0);
        m_Normals.assign(m_Vertices.size(), 0);

        for(size_t f = 0, f_end = getNFaces(); f < f_end; ++f) {
            // Get index of vertices for the current face
            UInt v0 = m_Faces[3 * f];
            UInt v1 = m_Faces[3 * f + 1];
            UInt v2 = m_Faces[3 * f + 2];

            Vec_Vec3f v(3, Vec3f(0));
            for(Int k = 0; k < 3; ++k) {
                v[0][k] = m_Vertices[3 * v0 + k];
                v[1][k] = m_Vertices[3 * v1 + k];
                v[2][k] = m_Vertices[3 * v2 + k];
            }

            Vec3f faceNormal = glm::normalize(glm::cross(v[1] - v[0], v[2] - v[0]));

            for(Int k = 0; k < 3; ++k) {
                m_Normals[v0 * 3 + k] += faceNormal[k];
                m_Normals[v1 * 3 + k] += faceNormal[k];
                m_Normals[v2 * 3 + k] += faceNormal[k];
            }
        }

        for(size_t f = 0, f_end = getNFaces(); f < f_end; ++f) {
            UInt v0 = m_Faces[3 * f];
            UInt v1 = m_Faces[3 * f + 1];
            UInt v2 = m_Faces[3 * f + 2];

            Vec_Vec3f fNormals(3, Vec3f(0));
            for(Int k = 0; k < 3; ++k) {
                fNormals[0][k] = m_Normals[3 * v0 + k];
                fNormals[1][k] = m_Normals[3 * v1 + k];
                fNormals[2][k] = m_Normals[3 * v2 + k];
            }

            for(Int k = 0; k < 3; ++k) {
                for(Int l = 0; l < 3; ++l) {
                    m_FaceVertexNormals[9 * f + 3 * k + l] = fNormals[k][l];
                    m_FaceVertexColors[9 * f + 3 * k + l]  = fNormals[k][l];
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana