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

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <stdio.h>
#include <Banana/Setup.h>

class OBJLoader
{
public:
    // Triangular Mesh Face
    struct Face
    {
        int vertices[3]; // vertex indices
    };

    OBJLoader() : vertices(nullptr), faces(nullptr), vertexNormals(nullptr), faceNormals(nullptr),
        vertexTextures(nullptr), faceTextures(nullptr), numVertices(0), numFaces(0),
        numVertexNormals(0),
        numVertexTextures(0), boundMin(0, 0, 0), boundMax(0, 0, 0)
    {}

    virtual ~OBJLoader()
    {
        clear();
    }

protected:
    Vec3f* vertices;          //< vertices
    Face*  faces;             //< faces

    Vec3f* vertexNormals;     //< vertex normal
    Face*  faceNormals;       //< normal faces

    Vec3f* vertexTextures;    //< texture vertices
    Face*  faceTextures;      //< texture faces

    int numVertices;          //< number of vertices
    int numFaces;             //< number of faces
    int numVertexNormals;     //< number of vertex normals
    int numVertexTextures;    //< number of texture vertices

    Vec3f boundMin, boundMax; //< bounding box

public:

    Vec3f& getVertex(int i)
    {
        return vertices[i];     //< returns the i^th vertex
    }

    const Vec3f& getVertex(int i) const
    {
        return vertices[i];     //< returns the i^th vertex
    }

    Face& getFace(int i)
    {
        return faces[i];     //< returns the i^th face
    }

    const Face& getFace(int i) const
    {
        return faces[i];     //< returns the i^th face
    }

    Vec3f& vertexNormal(int i)
    {
        return vertexNormals[i];     //< returns the i^th vertex normal
    }

    const Vec3f& vertexNormal(int i) const
    {
        return vertexNormals[i];     //< returns the i^th vertex normal
    }

    Face& faceNormal(int i)
    {
        return faceNormals[i];     //< returns the i^th normal face
    }

    const Face& faceNormal(int i) const
    {
        return faceNormals[i];     //< returns the i^th normal face
    }

    Vec3f& vertexTexture(int i)
    {
        return vertexTextures[i];     //< returns the i^th vertex texture
    }

    const Vec3f& vertexTexture(int i) const
    {
        return vertexTextures[i];     //< returns the i^th vertex texture
    }

    Face& faceTexture(int i)
    {
        return faceTextures[i];     //< returns the i^th texture face
    }

    const Face& faceTexture(int i) const
    {
        return faceTextures[i];     //< returns the i^th texture face
    }

    int getNumVertices() const
    {
        return numVertices;     //< returns the number of vertices
    }

    int getNumFaces() const
    {
        return numFaces;     //< returns the number of faces
    }

    int getNumVertexNormals() const
    {
        return numVertexNormals;     //< returns the number of vertex normals
    }

    int getNumVertexTextures() const
    {
        return numVertexTextures;     //< returns the number of texture vertices
    }

    bool hasNormals() const
    {
        return getNumVertexNormals() > 0;     //< returns true if the mesh has vertex normals
    }

    bool hasTextureVertices() const
    {
        return getNumVertexTextures() > 0;     //< returns true if the mesh has texture vertices
    }

    void clear()
    {
        setNumVertex(0);
        setNumFaces(0);
        setNumNormals(0);
        setNumTexVerts(0);
    }

    void setNumVertex(int n)
    {
        allocate(n, vertices, numVertices);
    }

    void setNumFaces(int n)
    {
        if(allocate(n, faces, numFaces)) {
            if(faceNormals) {
                allocate(n, faceNormals);
            }

            if(faceTextures) {
                allocate(n, faceTextures);
            }
        }
    }

    void setNumNormals(int n)
    {
        allocate(n, vertexNormals, numVertexNormals);

        if(!faceNormals) {
            allocate(numFaces, faceNormals);
        }
    }

    void setNumTexVerts(int n)
    {
        allocate(n, vertexTextures, numVertexTextures);

        if(!faceTextures) {
            allocate(numFaces, faceTextures);
        }
    }

    //@name Get Property Methods
    bool isBoundBoxReady() const
    {
        return boundMin[0] != 0 && boundMin[1] != 0 && boundMin[2] != 0 && boundMax[0] != 0
               && boundMax[1] != 0 && boundMax[2] != 0;
    }

    Vec3f getBoundMin()
    {
        if(!isBoundBoxReady()) {
            computeBoundingBox();
        }

        return boundMin;     //< Returns the minimum values of the bounding box
    }

    Vec3f getBoundMax()
    {
        if(!isBoundBoxReady()) {
            computeBoundingBox();
        }

        return boundMax;     //< Returns the maximum values of the bounding box
    }

    Vec3f getPoint(int faceID, const Vec3f& bc) const
    {
        return interpolate(faceID, vertices, faces,
                           bc);     //< Returns the point on the given face with the given barycentric coordinates (bc).
    }

    Vec3f getNormal(int faceID, const Vec3f& bc) const
    {
        return interpolate(faceID, vertexNormals, faceNormals,
                           bc);     //< Returns the the surface normal on the given face at the given barycentric coordinates (bc). The returned vector is not normalized.
    }

    Vec3f getTexCoord(int faceID, const Vec3f& bc) const
    {
        return interpolate(faceID, vertexTextures, faceTextures,
                           bc);     //< Returns the texture coordinate on the given face at the given barycentric coordinates (bc).
    }

    //@name Compute Methods
    void computeBoundingBox();                   //< Computes the bounding box
    void computeNormals(bool clockwise = false); //< Computes and stores vertex normals

    //@name Load and Save methods
    bool loadFromFileObj(const char*
                         filename);     //< Loads the mesh from an OBJ file. Automatically converts all faces to triangles.

private:
    template<class T> void allocate(int n, T*& t)
    {
        if(t) {
            delete[] t;
        }

        if(n > 0) {
            t = new T[n];
        } else {
            t = nullptr;
        }
    }

    template<class T> bool allocate(int n, T*& t, int& nt)
    {
        if(n == nt) {
            return false;
        }

        nt = n;
        allocate(n, t);
        return true;
    }

    static Vec3f interpolate(int i, const Vec3f* v, const Face* f, const Vec3f& bc)
    {
        return v[f[i].vertices[0]] * bc[0] + v[f[i].vertices[1]] * bc[1] +
               v[f[i].vertices[2]] * bc[2];
    }

    static int readLine(FILE* fp, int size, char* buffer);
    static void readVertex(const char* buffer, Vec3f& v)
    {
        static float x, y, z;
        //        sscanf( buffer + 2, "%f %f %f", &v[0], &v[1], &v[2] );
        __BNN_SSCAN(buffer + 2, "%f %f %f", &x, &y, &z);
        v[0] = (Real)x;
        v[1] = (Real)y;
        v[2] = (Real)z;
    }
};

#endif // OBJLOADER_H