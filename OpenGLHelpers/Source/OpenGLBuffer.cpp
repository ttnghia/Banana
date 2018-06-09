//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <OpenGLHelpers/OpenGLBuffer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint OpenGLBuffer::s_TotalBindingPoints = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::deleteBuffer()
{
    if(!m_isBufferCreated) {
        return;
    }

    glCall(glDeleteBuffers(1, &m_BufferID));
    m_isBufferCreated = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::createBuffer(GLenum bufferType, size_t bufferSize, const GLvoid* buffData, GLenum bufferUsage)
{
    m_BufferType  = bufferType;
    m_BufferUsage = bufferUsage;
    m_BufferSize  = bufferSize;

    if(m_isBufferCreated) {
        resize(bufferSize);
        return;
    }

    m_isBufferCreated = true;

    ////////////////////////////////////////////////////////////////////////////////
    glCall(glGenBuffers(1, &m_BufferID));
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    glCall(glBufferData(m_BufferType, bufferSize, buffData, m_BufferUsage));
    glCall(glBindBuffer(m_BufferType, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // create a binding point index if this is a uniform buffer
    if(m_BufferType == GL_UNIFORM_BUFFER) {
        m_BindingPoint = ++s_TotalBindingPoints;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::resize(size_t bufferSize)
{
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    glCall(glBufferData(m_BufferType, bufferSize, nullptr, m_BufferUsage));
    glCall(glBindBuffer(m_BufferType, 0));

    m_BufferSize = bufferSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::uploadData(const GLvoid* data, size_t offset, size_t dataSize)
{
    assert(dataSize <= m_BufferSize);
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    glCall(glBufferSubData(m_BufferType, offset, dataSize, data));
    glCall(glBindBuffer(m_BufferType, 0));
}

void OpenGLBuffer::uploadData(const Vector<const GLvoid*>& data, size_t offset, const Vector<size_t>& dataSize)
{
    size_t currentOffset = offset;
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    for(size_t i = 0; i < data.size(); ++i) {
        assert(currentOffset + dataSize[i] <= m_BufferSize);
        glCall(glBufferSubData(m_BufferType, currentOffset, dataSize[i], data[i]));
        currentOffset += dataSize[i];
    }
    glCall(glBindBuffer(m_BufferType, 0));
}

void OpenGLBuffer::uploadDataAsync(const GLvoid* data, size_t offset, size_t dataSize)
{
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    glCall(glBufferData(m_BufferType, dataSize, nullptr, m_BufferUsage));
    glCall(glBufferSubData(m_BufferType, offset, dataSize, data));
    glCall(glBindBuffer(m_BufferType, 0));

    // buffer orphaning can also change buffer size
    m_BufferSize = dataSize;
}

void OpenGLBuffer::uploadDataAsync(const Vector<const GLvoid*>& data, size_t offset, const Vector<size_t>& dataSize)
{
    size_t totalDataSize = 0;
    for(size_t size: dataSize) {
        totalDataSize += size;
    }
    size_t currentOffset = offset;
    glCall(glBindBuffer(m_BufferType, m_BufferID));
    glCall(glBufferData(m_BufferType, totalDataSize, nullptr, m_BufferUsage));
    for(size_t i = 0; i < data.size(); ++i) {
        glCall(glBufferSubData(m_BufferType, currentOffset, dataSize[i], data[i]));
        currentOffset += dataSize[i];
    }
    glCall(glBindBuffer(m_BufferType, 0));

    // buffer orphaning can also change buffer size
    m_BufferSize = totalDataSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::bind()
{
    assert(m_isBufferCreated);
    glCall(glBindBuffer(m_BufferType, m_BufferID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::bindBufferBase()
{
    assert(m_isBufferCreated && m_BufferType == GL_UNIFORM_BUFFER);
    glCall(glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_BufferID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLBuffer::release()
{
    glCall(glBindBuffer(m_BufferType, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint OpenGLBuffer::getBufferID() const
{
    return m_BufferID;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint OpenGLBuffer::getBindingPoint() const
{
    assert(m_BufferType == GL_UNIFORM_BUFFER);
    return m_BindingPoint;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t OpenGLBuffer::getBufferSize() const
{
    return m_BufferSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLBuffer::isCreated() const
{
    return m_isBufferCreated;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana