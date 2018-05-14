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

#include "OBJLoader.h"
//#include <Olicado/math/vec_utils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OBJLoader::computeBoundingBox()
{
    boundMin = vertices[0];
    boundMax = vertices[0];

    for(int i = 1; i < numVertices; ++i) {
        if(boundMin[0] > vertices[i][0]) {
            boundMin[0] = vertices[i][0];
        }

        if(boundMin[1] > vertices[i][1]) {
            boundMin[1] = vertices[i][1];
        }

        if(boundMin[2] > vertices[i][2]) {
            boundMin[2] = vertices[i][2];
        }

        if(boundMax[0] < vertices[i][0]) {
            boundMax[0] = vertices[i][0];
        }

        if(boundMax[1] < vertices[i][1]) {
            boundMax[1] = vertices[i][1];
        }

        if(boundMax[2] < vertices[i][2]) {
            boundMax[2] = vertices[i][2];
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OBJLoader::computeNormals(bool clockwise)
{
    setNumNormals(numVertices);

    for(int i = 0; i < numVertexNormals; ++i) {
        vertexNormals[i][0] = 0;
        vertexNormals[i][1] = 0;
        vertexNormals[i][2] = 0;
    }

    for(int i = 0; i < numFaces; ++i) {
        // face normal (not normalized)
        Vec3f N = glm::cross(vertices[faces[i].vertices[1]] - vertices[faces[i].vertices[0]],
                             vertices[faces[i].vertices[2]] - vertices[faces[i].vertices[0]]);

        if(clockwise) {
            N = -N;
        }

        vertexNormals[faces[i].vertices[0]] += N;
        vertexNormals[faces[i].vertices[1]] += N;
        vertexNormals[faces[i].vertices[2]] += N;
        faceNormals[i]                       = faces[i];
    }

    for(int i = 0; i < numVertexNormals; ++i) {
        vertexNormals[i] = glm::normalize(vertexNormals[i]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OBJLoader::loadFromFileObj(const char* filename)
{
    FILE* fp = 0;
#ifdef __BANANA_WINDOWS__
    fopen_s(&fp, filename, "r");
#else
    fp = fopen(filename, "r");
#endif

    if(!fp) {
        printf("Could not open obj file: %s\n", filename);
        exit(-1);
    }

    clear();

    int numVerts = 0, numTVerts = 0, numNormals = 0, numFaces = 0;

    const int bufsize = 1024;
    char      buffer[bufsize];

    while(int rb = readLine(fp, bufsize, buffer)) {
        switch(buffer[0]) {
            case 'v':
                switch(buffer[1]) {
                    case ' ':
                    case '\t':
                        numVerts++;
                        break;

                    case 't':
                        numTVerts++;
                        break;

                    case 'n':
                        numNormals++;
                        break;
                }

                break;

            case 'f':
            {
                int  nFaceVerts = 0; // count face vertices
                bool inspace    = true;

                for(int i = 2; i < rb - 1; ++i) {
                    if(buffer[i] == ' ' || buffer[i] == '\t') {
                        inspace = true;
                    } else if(inspace) {
                        nFaceVerts++;
                        inspace = false;
                    }
                }

                if(nFaceVerts > 2) {
                    numFaces += nFaceVerts - 2;    // non-triangle faces will be multiple triangle faces
                }
            }
            break;
        }

        if(feof(fp)) {
            break;
        }
    }

    if(numFaces == 0) {
        return true;    // No faces found
    }

    setNumVertex(numVerts);
    setNumFaces(numFaces);
    setNumNormals(numNormals);
    setNumTexVerts(numTVerts);

    int readVerts   = 0;
    int readTVerts  = 0;
    int readNormals = 0;
    int readFaces   = 0;

    rewind(fp);

    while(int rb = readLine(fp, bufsize, buffer)) {
        switch(buffer[0]) {
            case 'v':
                switch(buffer[1]) {
                    case ' ':
                    case '\t':
                        readVertex(buffer, vertices[readVerts++]);
                        break;

                    case 't':
                        readVertex(buffer, vertexTextures[readTVerts++]);
                        break;

                    case 'n':
                        readVertex(buffer, vertexNormals[readNormals++]);
                        break;
                }

                break;

            case 'f':
            {
                int  facevert = -1;
                bool inspace  = true;
                int  type     = 0;
                int  index    = 0;

                for(int i = 2; i < rb - 1; ++i) {
                    if(buffer[i] == ' ' || buffer[i] == '\t') {
                        inspace = true;
                    } else {
                        if(inspace) {
                            inspace = false;
                            type    = 0;
                            index   = 0;

                            switch(facevert) {
                                case -1:
                                    // initialize face
                                    faces[readFaces].vertices[0] = faces[readFaces].vertices[1] = faces[readFaces].vertices[2] = 0;

                                    if(faceTextures) {
                                        faceTextures[readFaces].vertices[0] = faceTextures[readFaces].vertices[1] = faceTextures[readFaces].vertices[2] = 0;
                                    }

                                    if(faceNormals) {
                                        faceNormals[readFaces].vertices[0] = faceNormals[readFaces].vertices[1] = faceNormals[readFaces].vertices[2] = 0;
                                    }

                                case 0:
                                case 1:
                                    facevert++;
                                    break;

                                case 2:
                                    // copy the first two vertices from the previous face
                                    readFaces++;
                                    faces[readFaces].vertices[0] = faces[readFaces - 1].vertices[0];
                                    faces[readFaces].vertices[1] = faces[readFaces - 1].vertices[2];

                                    if(faceTextures) {
                                        faceTextures[readFaces].vertices[0] = faceTextures[readFaces - 1].vertices[0];
                                        faceTextures[readFaces].vertices[1] = faceTextures[readFaces - 1].vertices[2];
                                    }

                                    if(faceNormals) {
                                        faceNormals[readFaces].vertices[0] = faceNormals[readFaces - 1].vertices[0];
                                        faceNormals[readFaces].vertices[1] = faceNormals[readFaces - 1].vertices[2];
                                    }

                                    break;
                            }
                        }

                        if(buffer[i] == '/') {
                            type++;
                            index = 0;
                        }

                        if(buffer[i] >= '0' && buffer[i] <= '9') {
                            index = index * 10 + (buffer[i] - '0');

                            switch(type) {
                                case 0:
                                    faces[readFaces].vertices[facevert] = index - 1;
                                    break;

                                case 1:
                                    if(faceTextures) {
                                        faceTextures[readFaces].vertices[facevert] = index - 1;
                                    }

                                    break;

                                case 2:
                                    if(faceNormals) {
                                        faceNormals[readFaces].vertices[facevert] = index - 1;
                                    }

                                    break;
                            }
                        }
                    }
                }

                readFaces++;
            }
            break;
        }

        if(feof(fp)) {
            break;
        }
    }

    fclose(fp);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline int OBJLoader::readLine(FILE* fp, int size, char* buffer)
{
    int i;

    for(i = 0; i < size; ++i) {
        buffer[i] = fgetc(fp);

        if(feof(fp) || buffer[i] == '\n' || buffer[i] == '\r') {
            buffer[i] = '\0';
            return i + 1;
        }
    }

    return i;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
