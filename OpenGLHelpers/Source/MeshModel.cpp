//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 10/25/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//------------------------------------------------------------------------------------------

#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


#include <OpenGLHelpers/MeshModel.h>

//------------------------------------------------------------------------------------------
bool MeshModel::load_mesh(std::string mesh_file)
{
    check_filetype(mesh_file);

    ////////////////////////////////////////////////////////////////////////////////
    // => clear data
    clear_data();

    ////////////////////////////////////////////////////////////////////////////////
    // => load mesh
    bool result = false;

    switch(meshFileType)
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
    mesh_ready = result;

    return result;
}

//------------------------------------------------------------------------------------------
void MeshModel::set_default_camera(Camera& camera, float fov)
{
    glm::vec3 currentDir = camera.getCameraDirection();
    camera.setDefaultCamera(currentDir * get_camera_distance(fov * 0.75) +
                            get_mesh_center(),
                            get_mesh_center(), glm::vec3(0, 1, 0));
}

//------------------------------------------------------------------------------------------
glm::vec3 MeshModel::get_mesh_center()
{
    return 0.5f * (bmin + bmax);
}

//------------------------------------------------------------------------------------------
float MeshModel::get_camera_distance(float fov)
{
    float half_length = (bmax.y - bmin.y) * 0.5f;

    return (half_length / std::tan(fov * 0.5f * M_PI / 180.0));
}

//------------------------------------------------------------------------------------------
std::vector<float>& MeshModel::get_vertices()
{
    assert(mesh_ready);

    return vertices;
}

//------------------------------------------------------------------------------------------
std::vector<float>& MeshModel::get_vnormals()
{
    assert(mesh_ready);

    return vnormals;
}

//------------------------------------------------------------------------------------------
std::vector<float>& MeshModel::get_vcolors()
{
    assert(mesh_ready);

    return vcolors;
}

//------------------------------------------------------------------------------------------
std::vector<float>& MeshModel::get_texcoord_2d()
{
    assert(mesh_ready);

    return texcoord_2d;
}

//------------------------------------------------------------------------------------------
std::vector<float>& MeshModel::get_texcoord_3d()
{
    assert(mesh_ready);

    return texcoord_3d;
}

//------------------------------------------------------------------------------------------
void MeshModel::check_filetype(std::string mesh_file)
{
    const std::string extension = mesh_file.substr(mesh_file.rfind('.') + 1);
    meshFileType = MeshFileType::UnsupportedFileType;

    if(extension == "obj" || extension == "OBJ" || extension == "Obj")
    {
        meshFileType = MeshFileType::OBJFile;
    }

    if(extension == "ply" || extension == "PLY" || extension == "Ply")
    {
        meshFileType = MeshFileType::PLYFile;
    }

    assert(meshFileType != MeshFileType::UnsupportedFileType);
}

//------------------------------------------------------------------------------------------
void MeshModel::clear_data()
{
    num_triangles = 0;

    bmin = glm::vec3(1e100, 1e100, 1e100);
    bmax = glm::vec3(-1e100, -1e100, -1e100);

    vertices.clear();
    vnormals.clear();
    vcolors.clear();
    texcoord_2d.clear();
    texcoord_3d.clear();

    ////////////////////////////////////////////////////////////////////////////////
    loading_error.clear();
}

//------------------------------------------------------------------------------------------
bool MeshModel::load_obj(std::string mesh_file)
{
    // if mesh is obj
    std::vector<tinyobj::shape_t> obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;
    tinyobj::attrib_t attrib;

    bool result = tinyobj::LoadObj(&attrib, &obj_shapes, &obj_materials,
                                   &loading_error, mesh_file.c_str(), NULL);


    if(!loading_error.empty())
    {
        std::cerr << "tinyobj: " << loading_error << std::endl;
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
            ++num_triangles;

            tinyobj::index_t idx0 = obj_shapes[s].mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = obj_shapes[s].mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = obj_shapes[s].mesh.indices[3 * f + 2];

            float v[3][3];

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

                bmin[k] = std::min(v[0][k], bmin[k]);
                bmin[k] = std::min(v[1][k], bmin[k]);
                bmin[k] = std::min(v[2][k], bmin[k]);

                bmax[k] = std::max(v[0][k], bmax[k]);
                bmax[k] = std::max(v[1][k], bmax[k]);
                bmax[k] = std::max(v[2][k], bmax[k]);
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
                compute_normal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
            }

            for(int k = 0; k < 3; k++)
            {
                vertices.push_back(v[k][0]);
                vertices.push_back(v[k][1]);
                vertices.push_back(v[k][2]);

                vnormals.push_back(n[k][0]);
                vnormals.push_back(n[k][1]);
                vnormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = {n[k][0], n[k][1], n[k][2]};
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0.0f)
                {
                    float len = sqrtf(len2);

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                vcolors.push_back(c[0] * 0.5 + 0.5);
                vcolors.push_back(c[1] * 0.5 + 0.5);
                vcolors.push_back(c[2] * 0.5 + 0.5);
            }
        } // end process current shape
    }

    ////////////////////////////////////////////////////////////////////////////////
    return result;
}

//------------------------------------------------------------------------------------------
bool MeshModel::load_ply(std::string mesh_file)
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
        std::vector<float> verts;
        std::vector<float> norms;
        std::vector<uint8_t> colors;

        std::vector<uint32_t> faces;
        std::vector<float> uvCoords;
        //        std::vector<uint8_t> faceColors;

        uint32_t vertexCount, normalCount, colorCount, faceCount,
            faceTexcoordCount/*, faceColorCount*/;
        vertexCount = normalCount = colorCount = faceCount =
            faceTexcoordCount /*= faceColorCount */ = 0;

        // The count returns the number of instances of the property group. The vectors
        // above will be resized into a multiple of the property group size as
        // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
        vertexCount = file.request_properties_from_element("vertex", {"x", "y", "z"}, verts);
        normalCount = file.request_properties_from_element("vertex", {"nx", "ny", "nz"}, norms);
        colorCount = file.request_properties_from_element("vertex", {"red", "green", "blue", "alpha"},
                                                          colors);

        // For properties that are list types, it is possibly to specify the expected count (ideal if a
        // consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
        // defers allocation of memory until the first instance of the property has been found
        // as implemented in file.read(ss)
        faceCount = file.request_properties_from_element("face", {"vertex_indices"}, faces, 3);
        faceTexcoordCount = file.request_properties_from_element("face", {"texcoord"}, uvCoords,
                                                                 6);
        //        faceColorCount = file.request_properties_from_element("face", {"red", "green", "blue", "alpha"}, faceColors);

                // Now populate the vectors...
        file.read(ss);

        ////////////////////////////////////////////////////////////////////////////////
        // => convert data
        for(size_t f = 0; f < faces.size() / 3; f++)
        {
            ++num_triangles;

            uint32_t f0 = faces[3 * f + 0];
            uint32_t f1 = faces[3 * f + 1];
            uint32_t f2 = faces[3 * f + 2];

            float v[3][3];

            for(int k = 0; k < 3; k++)
            {

                v[0][k] = verts[3 * f0 + k];
                v[1][k] = verts[3 * f1 + k];
                v[2][k] = verts[3 * f2 + k];

                bmin[k] = std::min(v[0][k], bmin[k]);
                bmin[k] = std::min(v[1][k], bmin[k]);
                bmin[k] = std::min(v[2][k], bmin[k]);

                bmax[k] = std::max(v[0][k], bmax[k]);
                bmax[k] = std::max(v[1][k], bmax[k]);
                bmax[k] = std::max(v[2][k], bmax[k]);
            }

            float n[3][3];

            if(norms.size() > 0)
            {
                uint32_t f0 = norms[3 * f + 0];
                uint32_t f1 = norms[3 * f + 1];
                uint32_t f2 = norms[3 * f + 2];

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
                compute_normal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
            }

            for(int k = 0; k < 3; k++)
            {
                vertices.push_back(v[k][0]);
                vertices.push_back(v[k][1]);
                vertices.push_back(v[k][2]);

                vnormals.push_back(n[k][0]);
                vnormals.push_back(n[k][1]);
                vnormals.push_back(n[k][2]);

                // Use normal as color.
                float c[3] = {n[k][0], n[k][1], n[k][2]};
                float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];

                if(len2 > 0.0f)
                {
                    float len = sqrtf(len2);

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                vcolors.push_back(c[0] * 0.5 + 0.5);
                vcolors.push_back(c[1] * 0.5 + 0.5);
                vcolors.push_back(c[2] * 0.5 + 0.5);
            }
        } // end process current shape
    }
    catch(const std::exception& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return true;
}

//------------------------------------------------------------------------------------------
void MeshModel::compute_normal(float N[3], float v0[3], float v1[3], float v2[3])
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
