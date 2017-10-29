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

#include <RayTracing/RayTracer.h>
#include <Banana/Utils/FileHelpers.h>

#include <sutil/sutil.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::createOptiXContext(int width, int height)
{
    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_Width       = width;
    m_Height      = height;
    ////////////////////////////////////////////////////////////////////////////////
    // Set up context
    m_OptiXContext = optix::Context::create();

    ////////////////////////////////////////////////////////////////////////////////
    // create output buffer
    const unsigned int elmtSize = 4;
    GLuint             vbo      = 0;

    glCall(glGenBuffers(1, &vbo));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, elmtSize * width * height, 0, GL_STREAM_DRAW));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    m_OutBuffer = m_OptiXContext->createBufferFromGLBO(RT_BUFFER_OUTPUT, vbo);
    m_OutBuffer->setFormat(RT_FORMAT_UNSIGNED_BYTE4);
    m_OutBuffer->setSize(width, height);

    m_OptiXContext["output_buffer"]->set(m_OutBuffer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::resizeViewport(int width, int height)
{
    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_Width       = width;
    m_Height      = height;
    resizeBuffer(getOptiXOutputBuffer(), width, height);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::destroyOptiXContext()
{
    if(m_OptiXContext) {
        m_OptiXContext->destroy();
        m_OptiXContext = 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
optix::Buffer RayTracer::getOptiXOutputBuffer()
{
    return m_OptiXContext["output_buffer"]->getBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint RayTracer::getOutputBufferOID() const
{
    return m_OutBuffer->getGLBOId();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::getOutputAsTexture(const std::shared_ptr<OpenGLTexture>& texture)
{
    // Check if we have a GL interop display buffer
    const unsigned int pboId = getOutputBufferOID();
    assert(pboId != 0);

    if(!texture->isCreated()) {
        texture->createTexture(GL_TEXTURE_2D);
        texture->setSimplestTexture();
    }

    // send PBO to texture
    texture->bind();
    glCall(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId));

    RTsize elmtSize = m_OutBuffer->getElementSize();
    if(elmtSize % 8 == 0) {
        glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 8));
    } else if(elmtSize % 4 == 0) {
        glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    } else if(elmtSize % 2 == 0) {
        glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 2));
    } else {
        glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    }

    switch(m_OutBuffer->getFormat()) {
        case RT_FORMAT_UNSIGNED_BYTE4:
            glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0));
            break;
        case RT_FORMAT_FLOAT4:
            glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, 0));
            break;
        case RT_FORMAT_FLOAT3:
            glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, 0));
            break;
        case RT_FORMAT_FLOAT:
            glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, m_Width, m_Height, 0, GL_LUMINANCE, GL_FLOAT, 0));
            break;
        default:
            __BNN_DIE("Unknown buffer format");
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    texture->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::getOutputAsByteArray(std::vector<unsigned char>& data)
{
    data.resize(m_Width * m_Height * 3);
    getOutputAsByteArray(data.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::getOutputAsByteArray(unsigned char* data)
{
    GLvoid* imageData;
    RT_CHECK_ERROR(rtBufferMap(m_OutBuffer->get(), &imageData));

    switch(m_OutBuffer->getFormat()) {
        case RT_FORMAT_UNSIGNED_BYTE4:
            // Data is BGRA and upside down, so we need to swizzle to RGB
            for(int j = m_Height - 1; j >= 0; --j) {
                unsigned char* dst = &data[0] + (3 * m_Width * (m_Height - 1 - j));
                unsigned char* src = ((unsigned char*)imageData) + (4 * m_Width * j);
                for(int i = 0; i < m_Width; i++) {
                    *dst++ = *(src + 2);
                    *dst++ = *(src + 1);
                    *dst++ = *(src + 0);
                    src   += 4;
                }
            }
            break;

        case RT_FORMAT_FLOAT:
            // This buffer is upside down
            for(int j = m_Height - 1; j >= 0; --j) {
                unsigned char* dst = &data[0] + m_Width * (m_Height - 1 - j);
                float*         src = ((float*)imageData) + (3 * m_Width * j);
                for(int i = 0; i < m_Width; i++) {
                    int          P          = static_cast<int>((*src++) * 255.0f);
                    unsigned int clampedVal = P < 0 ? 0 : P > 0xff ? 0xff : P;

                    // write the pixel to all 3 channels
                    *dst++ = static_cast<unsigned char>(clampedVal);
                    *dst++ = static_cast<unsigned char>(clampedVal);
                    *dst++ = static_cast<unsigned char>(clampedVal);
                }
            }
            break;

        case RT_FORMAT_FLOAT3:
            // This buffer is upside down
            for(int j = m_Height - 1; j >= 0; --j) {
                unsigned char* dst = &data[0] + (3 * m_Width * (m_Height - 1 - j));
                float*         src = ((float*)imageData) + (3 * m_Width * j);
                for(int i = 0; i < m_Width; i++) {
                    for(int elem = 0; elem < 3; ++elem) {
                        int          P          = static_cast<int>((*src++) * 255.0f);
                        unsigned int clampedVal = P < 0 ? 0 : P > 0xff ? 0xff : P;
                        *dst++ = static_cast<unsigned char>(clampedVal);
                    }
                }
            }
            break;

        case RT_FORMAT_FLOAT4:
            // This buffer is upside down
            for(int j = m_Height - 1; j >= 0; --j) {
                unsigned char* dst = &data[0] + (3 * m_Width * (m_Height - 1 - j));
                float*         src = ((float*)imageData) + (4 * m_Width * j);
                for(int i = 0; i < m_Width; i++) {
                    for(int elem = 0; elem < 3; ++elem) {
                        int          P          = static_cast<int>((*src++) * 255.0f);
                        unsigned int clampedVal = P < 0 ? 0 : P > 0xff ? 0xff : P;
                        *dst++ = static_cast<unsigned char>(clampedVal);
                    }

                    // skip alpha
                    src++;
                }
            }
            break;

        default:
            __BNN_DIE("Unknown buffer format");
    }

    // Now unmap the buffer
    RT_CHECK_ERROR(rtBufferUnmap(m_OutBuffer->get()));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::validateContext()
{
    m_OptiXContext->validate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool RayTracer::updateCamera()
{
    if(!m_Camera->isCameraChanged()) {
        return false;
    }

    static optix::float3 eye;
    static optix::float3 u;
    static optix::float3 v;
    static optix::float3 w;

    computeEyeUVW(eye, u, v, w);
    m_OptiXContext["eye"]->setFloat(eye);
    m_OptiXContext["U"]->setFloat(u);
    m_OptiXContext["V"]->setFloat(v);
    m_OptiXContext["W"]->setFloat(w);

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
optix::TextureSampler RayTracer::createTexSampler(const std::string& texFile, bool bUseCache /*= true*/)
{
    if(bUseCache) {
        optix::TextureSampler sampler = m_TexSamplers[texFile];
        if(sampler != 0) {
            return sampler;
        }
    }

    optix::TextureSampler sampler = m_OptiXContext->createTextureSampler();
    sampler->setWrapMode(0, RT_WRAP_REPEAT);
    sampler->setWrapMode(1, RT_WRAP_REPEAT);
    sampler->setWrapMode(2, RT_WRAP_REPEAT);
    sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    sampler->setMaxAnisotropy(1.0f);
    sampler->setMipLevelCount(1u);
    sampler->setArraySize(1u);


    QImage         img(texFile.c_str());
    optix::Buffer  buffer     = m_OptiXContext->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE4, img.width(), img.height());
    unsigned char* bufferData = static_cast<unsigned char*>(buffer->map());
    memcpy(bufferData, img.convertToFormat(QImage::Format_RGBA8888).bits(), 4 * img.width() * img.height());

    buffer->unmap();
    sampler->setBuffer(0u, 0u, buffer);
    sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);

    return sampler;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
optix::TextureSampler RayTracer::getDummyTexSampler(const glm::vec3& defaultColor /*= glm::vec3(1.0f)*/)
{
    static optix::TextureSampler sampler = m_OptiXContext->createTextureSampler();
    static bool                  created = false;
    if(created) {
        return sampler;
    }

    ////////////////////////////////////////////////////////////////////////////////
    sampler->setWrapMode(0, RT_WRAP_REPEAT);
    sampler->setWrapMode(1, RT_WRAP_REPEAT);
    sampler->setWrapMode(2, RT_WRAP_REPEAT);
    sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    sampler->setMaxAnisotropy(1.0f);
    sampler->setMipLevelCount(1u);
    sampler->setArraySize(1u);

    // Create buffer with single texel set to default_color
    optix::Buffer buffer      = m_OptiXContext->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, 1u, 1u);
    float*        buffer_data = static_cast<float*>(buffer->map());
    buffer_data[0] = defaultColor.x;
    buffer_data[1] = defaultColor.y;
    buffer_data[2] = defaultColor.z;
    buffer_data[3] = 1.0f;
    buffer->unmap();

    sampler->setBuffer(0u, 0u, buffer);
    sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);

    created = true;
    return sampler;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::resizeBuffer(optix::Buffer buffer, int width, int height)
{
    buffer->setSize(width, height);

    GLuint pboId = buffer->getGLBOId();
    if(pboId) {
        buffer->unregisterGLBuffer();
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, buffer->getElementSize() * width * height, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        buffer->registerGLBuffer();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::computeEyeUVW(optix::float3& eye_out, optix::float3& u_out, optix::float3& v_out, optix::float3& w_out)
{
    float            ulen, vlen, wlen;
    const glm::vec3& eye = m_Camera->getCameraPosition();
    glm::vec3        W   = m_Camera->getCameraFocus() - eye; // Do not normalize W -- it implies focal length

    wlen = glm::length(W);
    glm::vec3 U = glm::normalize(glm::cross(W, m_Camera->getCameraUpDirection()));
    glm::vec3 V = glm::normalize(glm::cross(U, W));

    vlen = wlen * tanf(0.5f * M_PIf * m_Camera->getFrustum().m_Fov / 180.0f);
    ulen = vlen * m_AspectRatio;
    V   *= vlen;
    U   *= ulen;

    ////////////////////////////////////////////////////////////////////////////////
    eye_out = optix::make_float3(eye[0], eye[1], eye[2]);
    u_out   = optix::make_float3(U[0], U[1], U[2]);
    v_out   = optix::make_float3(V[0], V[1], V[2]);
    w_out   = optix::make_float3(W[0], W[1], W[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana