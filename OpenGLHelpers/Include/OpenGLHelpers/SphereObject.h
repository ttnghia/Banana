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

#include <OpenGLHelpers/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SphereObject : public MeshObject
{
public:
    SphereObject() : scale(1.0)
    {}

    {
    }

    SphereObject::~SphereObject()
    {
        clearData();
    }

    //------------------------------------------------------------------------------------------
    void UnitSphere::generateSphere(int _numStacks, int _numSlices)
    {
        numStacks = _numStacks;
        numSlices = _numSlices;

        clearData();
        QVector3D vertex;
        QVector2D tex;

        for(int j = 0; j <= _numStacks; ++j)
        {
            float theta = (float)j * M_PI / _numStacks;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for(int i = 0; i <= _numSlices; ++i)
            {
                float phi = (float)i * 2 * M_PI / _numSlices;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                vertex.setX(cosPhi * sinTheta);
                vertex.setY(cosTheta);
                vertex.setZ(sinPhi * sinTheta);
                verticesList.append(vertex);

                // normal at this point is the same value with coordinate
                normalsList.append(vertex);


                tex.setX(2.0*(1.0 - (float)i / (float)_numSlices));
                tex.setY(1.0 - (float)j / (float)_numStacks);
                texCoordList.append(tex);
            }
        }

        for(int j = 0; j < _numStacks; ++j)
        {
            for(int i = 0; i < _numSlices; ++i)
            {
                int first = (j * (_numSlices + 1)) + i;
                int second = first + _numSlices + 1;

                indicesList.append(first);
                indicesList.append(first + 1);
                indicesList.append(second);

                indicesList.append(second);
                indicesList.append(first + 1);
                indicesList.append(second + 1);
            }
        }

    }

    //------------------------------------------------------------------------------------------
    int UnitSphere::getNumVertices()
    {

        return verticesList.size();
    }

    //------------------------------------------------------------------------------------------
    int UnitSphere::getNumIndices()
    {
        return indicesList.size();
    }

    //------------------------------------------------------------------------------------------
    int UnitSphere::getVertexOffset()
    {
        return (sizeof(GLfloat) * getNumVertices() * 3);
    }

    //------------------------------------------------------------------------------------------
    int UnitSphere::getIndexOffset()
    {
        return (sizeof(GLushort) * getNumIndices());
    }

    //------------------------------------------------------------------------------------------
    int UnitSphere::getTexCoordOffset()
    {
        return (sizeof(GLfloat) * getNumVertices() * 2);
    }

    //------------------------------------------------------------------------------------------
    GLfloat* UnitSphere::getVertices()
    {
        if(!vertices)
        {
            vertices = new GLfloat[verticesList.size() * 3];

            for(int i = 0; i < verticesList.size(); ++i)
            {
                QVector3D vertex = verticesList.at(i);
                vertices[3 * i] = vertex.x();
                vertices[3 * i + 1] = vertex.y();
                vertices[3 * i + 2] = vertex.z();
                //            qDebug() << i << vertex;
            }

        }

        return vertices;
    }

    //------------------------------------------------------------------------------------------
    GLfloat* UnitSphere::getNormals()
    {
        if(!normals)
        {
            normals = new GLfloat[normalsList.size() * 3];

            for(int i = 0; i < normalsList.size(); ++i)
            {
                QVector3D normal = normalsList.at(i);
                normals[3 * i] = normal.x();
                normals[3 * i + 1] = normal.y();
                normals[3 * i + 2] = normal.z();
            }

        }

        return normals;
    }

    //------------------------------------------------------------------------------------------
    GLfloat* UnitSphere::getNegativeNormals()
    {
        if(!normals)
        {
            normals = new GLfloat[normalsList.size() * 3];
        }

        for(int i = 0; i < normalsList.size(); ++i)
        {
            QVector3D normal = normalsList.at(i);
            normals[3 * i] = -normal.x();
            normals[3 * i + 1] = -normal.y();
            normals[3 * i + 2] = -normal.z();
        }



        return normals;
    }

    //------------------------------------------------------------------------------------------
    GLfloat* UnitSphere::getTexureCoordinates()
    {
        if(!texCoord)
        {
            texCoord = new GLfloat[texCoordList.size() * 2];

            for(int i = 0; i < texCoordList.size(); ++i)
            {
                QVector2D tex = texCoordList.at(i);
                texCoord[2 * i] = tex.x();
                texCoord[2 * i + 1] = tex.y();
            }

        }

        return texCoord;
    }

    //------------------------------------------------------------------------------------------
    GLushort* UnitSphere::getIndices()
    {
        return (GLushort*)indicesList.data();
    }

    //------------------------------------------------------------------------------------------
    void UnitSphere::clearData()
    {
        verticesList.clear();
        normalsList.clear();
        texCoordList.clear();
        indicesList.clear();

        if(vertices)
        {
            delete[] vertices;
            vertices = NULL;
        }

        if(normals)
        {
            delete[] normals;
            normals = NULL;
        }

        if(texCoord)
        {
            delete[] texCoord;
            texCoord = NULL;
        }
    }


private:
    void clearData();


    float scale;
};