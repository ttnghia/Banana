//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/19/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <array>

#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CubeObject : public MeshObject
{
public:
    CubeObject()
    {
        generateCube(
    }

    void generateCube()
    {
        clearData();

        GLfloat cube_vertices[] ={
            // front
            -1.0, -1.0,  1.0,
             1.0, -1.0,  1.0,
             1.0,  1.0,  1.0,
            -1.0,  1.0,  1.0,
            // top
            -1.0,  1.0,  1.0,
             1.0,  1.0,  1.0,
             1.0,  1.0, -1.0,
            -1.0,  1.0, -1.0,
            // back
             1.0, -1.0, -1.0,
            -1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
             1.0,  1.0, -1.0,
             // bottom
             -1.0, -1.0, -1.0,
              1.0, -1.0, -1.0,
              1.0, -1.0,  1.0,
             -1.0, -1.0,  1.0,
             // left
             -1.0, -1.0, -1.0,
             -1.0, -1.0,  1.0,
             -1.0,  1.0,  1.0,
             -1.0,  1.0, -1.0,
             // right
              1.0, -1.0,  1.0,
              1.0, -1.0, -1.0,
              1.0,  1.0, -1.0,
              1.0,  1.0,  1.0,
        };

        GLfloat cube_texcoords[2 * 4 * 6] ={
            // front
            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0,
        };
        for(int i = 1; i < 6; i++)
            memcpy(&cube_texcoords[i * 4 * 2], &cube_texcoords[0], 2 * 4 * sizeof(GLfloat));

        m_IndicesList ={
            // front
             0,  1,  2,
             2,  3,  0,
             // top
              4,  5,  6,
              6,  7,  4,
              // back
               8,  9, 10,
              10, 11,  8,
              // bottom
              12, 13, 14,
              14, 15, 12,
              // left
              16, 17, 18,
              18, 19, 16,
              // right
              20, 21, 22,
              22, 23, 20,
        };

        std::array<GLfloat, 3> vertex;
        std::array<GLfloat, 3> normal;
        std::array<GLfloat, 3> color;
        std::array<GLfloat, 2> tex;

        // Vertex data for face 0
        // v0
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);

        // v1
        vertex ={1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={1.0, 0, 1.0};
        tex ={1, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);

        // v2
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, 1.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v3
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, 1.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);


        // Vertex data for face 1
        // v4
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, -1.0f, 1.0f);
        m_VertexColors.push_back(1.0f, 0.0f, 1.0f);
        m_VertexNormals.push_back(1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 0.0f);

        // v5
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 0.0f);

        // v6
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v7
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);


        // Vertex data for face 2
        // v8
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, -1.0f);
        m_VertexTexCoords.push_back(0.0f, 0.0f);

        // v9
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, -1.0f);
        m_VertexTexCoords.push_back(1.0f, 0.0f);

        // v10
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, -1.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v11
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 0.0f, -1.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);

        // Vertex data for face 3
        // v12
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(-1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 0.0f);

        // v13
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, -1.0f, 1.0f);
        m_VertexColors.push_back(0.0f, 0.0f, 1.0f);
        m_VertexNormals.push_back(-1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 0.0f);

        // v14
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(-1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v15
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(-1.0f, 0.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);

        // Vertex data for face 4
        // v16
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, -1.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 0.0f);

        // v17
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, -1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 0.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, -1.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 0.0f);

        // v18
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, -1.0f, 1.0f);
        m_VertexColors.push_back(0.0f, 0.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, -1.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v19
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, -1.0f, 1.0f);
        m_VertexColors.push_back(1.0f, 0.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, -1.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);


        // Vertex data for face 5
        // v20
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, 1.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 0.0f);

        // v21
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, 1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 1.0f);
        m_VertexNormals.push_back(0.0f, 1.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 0.0f);

        // v22
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(-1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(0.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 1.0f, 0.0f);
        m_VertexTexCoords.push_back(0.0f, 1.0f);

        // v23
        vertex ={-1.0, -1.0, 1.0};
        normal ={0, 0, 1.0};
        color ={0, 0, 1.0};
        tex ={0, 0};
        for(int i=0; i < 3; ++i)
        {
            m_Vertices.push_back(vertex[i]);
            m_VertexNormals.push_back(normal[i]);
            m_VertexColors.push_back(color[i]);
        }
        m_VertexTexCoords.push_back(tex[0]);
        m_VertexTexCoords.push_back(tex[1]);
        m_Vertices.push_back(1.0f, 1.0f, -1.0f);
        m_VertexColors.push_back(1.0f, 1.0f, 0.0f);
        m_VertexNormals.push_back(0.0f, 1.0f, 0.0f);
        m_VertexTexCoords.push_back(1.0f, 1.0f);

        ////////////////////////////////////////////////////////////////////////////////
        m_isDataReady       = true;
        m_hasVertexNormal   = true;
        m_hasVertexTexCoord = true;
        m_hasVertexColor    = true;
    }
};
