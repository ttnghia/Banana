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
    void createBuffer(GLenum bufferType, size_t bufferSize, const GLvoid * buffData = nullptr,
                      GLenum bufferUsage = GL_STATIC_DRAW)
    {
        m_BufferType      = bufferType;
        m_BufferUsage     = bufferUsage;
        m_isBufferCreated = true;

        ////////////////////////////////////////////////////////////////////////////////
        glCall(glGenBuffers(1, &m_BufferID));
        glCall(glBindBuffer(m_BufferType, m_BufferID));
        glCall(glBufferData(m_BufferType, bufferSize, buffData, m_BufferUsage));
        glCall(glBindBuffer(m_BufferType, 0));

        ////////////////////////////////////////////////////////////////////////////////
        // create a binding point index if this is a uniform buffer
        if(m_BufferType == GL_UNIFORM_BUFFER)
        {
            m_BindingPoint = ++s_TotalBindingPoints;
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void uploadData(const GLvoid* data, size_t offset, size_t dataSize)
    {
        glCall(glBindBuffer(m_BufferType, m_BufferID));
        glCall(glBufferSubData(m_BufferType, offset, dataSize, data));
        glCall(glBindBuffer(m_BufferType, 0));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void bind()
    {
        assert(m_isBufferCreated);

        if(m_BufferType == GL_UNIFORM_BUFFER)
        {
            glCall(glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_BufferID));
        }
        else
        {
            glCall(glBindBuffer(m_BufferID));
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void release()
    {
        glCall(glBindBuffer(0));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    GLuint getBufferID()
    {
        return m_BufferID;
    }

    GLuint getBindingPoint()
    {
        assert(m_BufferType == GL_UNIFORM_BUFFER);
        return m_BindingPoint;
    }

    bool isCreated()
    {
        return m_isBufferCreated;
    }
private:
    bool   m_isBufferCreated;
    GLuint m_BufferID;
    GLenum m_BufferType;
    GLenum m_BufferUsage;

    GLuint m_BindingPoint;
    static GLuint s_TotalBindingPoints;
};

GLuint OpenGLBuffer::s_TotalBindingPoints = 0;
