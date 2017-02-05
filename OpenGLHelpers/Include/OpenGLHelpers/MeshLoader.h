//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 10/24/2016
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/OpenGLMacros.h>

#include <fstream>

#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#include <tinyply.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshLoader
{
    enum class MeshFileType
    {
        OBJFile,
        PLYFile,
        UnsupportedFileType
    };

public:
    MeshLoader();
    MeshLoader(std::string mesh_file);

    bool loadMesh(std::string mesh_file);
    glm::vec3 getMeshCenter();

    void setDefaultCamera(Camera& camera, float fov = 45);
    float getCameraDistance(float fov);

    std::vector<float>& getVertices();
    std::vector<float>& getVertexNormal();
    std::vector<float>& getVertexColor();
    std::vector<float>& getVTexCoord2D();
    std::vector<float>& getVTexCoord3D();

    ////////////////////////////////////////////////////////////////////////////////
    unsigned int m_NumTriangles;
    bool m_isMeshReady;

private:
    void checkFileType(std::string mesh_file);
    void clearData();

    bool load_obj(std::string mesh_file);
    bool load_ply(std::string mesh_file);

    void computeVertexNormal(float N[3], float v0[3], float v1[3], float v2[3]);

    ////////////////////////////////////////////////////////////////////////////////
    MeshFileType m_MeshFileType;
    std::string  m_LoadingErrorStr;

    std::vector<float> m_Vertices;
    std::vector<float> m_VertexNormals;
    std::vector<float> m_VertexColors;
    std::vector<float> m_VertexTexCoord2D;
    std::vector<float> m_VertexTexCoord3D;
    glm::vec3          m_BBoxMin;
    glm::vec3          m_BBoxMax;

};
