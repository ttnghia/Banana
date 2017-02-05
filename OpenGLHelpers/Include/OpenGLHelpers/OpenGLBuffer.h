//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/20/2017
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

#include <cstddef>
#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLBuffer : public OpenGLCallable
{
public:
    OpenGLBuffer() : m_isBufferCreated(false), m_BufferSize(0)
    {}

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    ~OpenGLBuffer();

    void deleteBuffer();
    void createBuffer(GLenum bufferType, size_t bufferSize, const GLvoid * buffData = nullptr,
                      GLenum bufferUsage = GL_STATIC_DRAW);
    void resize(size_t bufferSize);
    void uploadData(const GLvoid* data, size_t offset, size_t dataSize);
    void uploadDataAsync(const GLvoid* data, size_t offset, size_t dataSize);
    void bind();
    void bindBufferBase();
    void release();

    GLuint getBufferID();
    GLuint getBindingPoint();
    size_t getBufferSize();
    bool isCreated();

private:
    bool   m_isBufferCreated;
    GLuint m_BufferID;
    GLenum m_BufferType;
    GLenum m_BufferUsage;

    size_t m_BufferSize;
    GLuint m_BindingPoint;
    static GLuint s_TotalBindingPoints;
};

