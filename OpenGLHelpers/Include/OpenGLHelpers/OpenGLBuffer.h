//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <cstddef> 
#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
class OpenGLBuffer : public OpenGLFunctions
{
public:
    OpenGLBuffer() : m_isBufferCreated(false)
    {
        initializeOpenGLFunctions();
    }
#else
class OpenGLBuffer
{
public:
    OpenGLBuffer() : m_isBufferCreated(false)
    {}
#endif

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    ~OpenGLBuffer()
    {
        deleteBuffer();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void deleteBuffer()
    {
        if(!m_isBufferCreated)
            return;

        glCall(glDeleteBuffers(1, &m_BufferID));
        m_isBufferCreated = false;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void createBuffer(GLenum bufferType, size_t bufferSize = 0, GLenum bufferUsage = GL_STATIC_DRAW)
    {
        m_BufferType = bufferType;
        m_BufferUsage = bufferUsage;

        ////////////////////////////////////////////////////////////////////////////////
        glCall(glGenBuffers(1, &m_BufferID));
        m_isBufferCreated = true;

        if(bufferSize > 0)
        {
            glCall(glBindBuffer(m_BufferType, m_BufferID));
            glCall(glBufferData(m_BufferType, bufferSize, NULL, m_BufferUsage));
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void uploadData(GLvoid* data, size_t offset, size_t dataSize)
    {
        glCall(glBindBuffer(m_BufferType, m_BufferID));
        glCall(glBufferSubData(m_BufferType, offset, dataSize, data));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void bind()
    {
        if(m_isBufferCreated)
            glCall(glBindBuffer(m_BufferID));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void release()
    {
        if(m_isBufferCreated)
            glCall(glBindBuffer(0));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool m_isBufferCreated;
    GLuint m_BufferID;
    GLenum m_BufferType;
    GLenum m_BufferUsage;
};
