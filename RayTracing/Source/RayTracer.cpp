//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <RayTracing/RayTracer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::destroyOptiXContext()
{
    if(m_OptiXContext)
    {
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
void RayTracer::resizeViewport(int width, int height)
{
    resizeBuffer(getOptiXOutputBuffer(), width, height);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::saveFrame()
{
    sutil::writeBufferToFile(outputImage.c_str(), getOutputBuffer());







    //void sutil::writeBufferToFile(const char* filename, RTbuffer buffer)

    GLsizei width, height;
    RTsize  buffer_width, buffer_height;

    GLvoid* imageData;
    RT_CHECK_ERROR(rtBufferMap(buffer, &imageData));

    RT_CHECK_ERROR(rtBufferGetSize2D(buffer, &buffer_width, &buffer_height));
    width  = static_cast<GLsizei>(buffer_width);
    height = static_cast<GLsizei>(buffer_height);

    std::vector<unsigned char> pix(width* height * 3);

    RTformat buffer_format;
    RT_CHECK_ERROR(rtBufferGetFormat(buffer, &buffer_format));

    switch(buffer_format)
    {
        case RT_FORMAT_UNSIGNED_BYTE4:
            // Data is BGRA and upside down, so we need to swizzle to RGB
            for(int j = height - 1; j >= 0; --j)
            {
                unsigned char* dst = &pix[0] + (3 * width * (height - 1 - j));
                unsigned char* src = ((unsigned char*)imageData) + (4 * width * j);
                for(int i = 0; i < width; i++)
                {
                    *dst++ = *(src + 2);
                    *dst++ = *(src + 1);
                    *dst++ = *(src + 0);
                    src   += 4;
                }
            }
            break;

        case RT_FORMAT_FLOAT:
            // This buffer is upside down
            for(int j = height - 1; j >= 0; --j)
            {
                unsigned char* dst = &pix[0] + width * (height - 1 - j);
                float*         src = ((float*)imageData) + (3 * width * j);
                for(int i = 0; i < width; i++)
                {
                    int          P       = static_cast<int>((*src++) * 255.0f);
                    unsigned int Clamped = P < 0 ? 0 : P > 0xff ? 0xff : P;

                    // write the pixel to all 3 channels
                    *dst++ = static_cast<unsigned char>(Clamped);
                    *dst++ = static_cast<unsigned char>(Clamped);
                    *dst++ = static_cast<unsigned char>(Clamped);
                }
            }
            break;

        case RT_FORMAT_FLOAT3:
            // This buffer is upside down
            for(int j = height - 1; j >= 0; --j)
            {
                unsigned char* dst = &pix[0] + (3 * width * (height - 1 - j));
                float*         src = ((float*)imageData) + (3 * width * j);
                for(int i = 0; i < width; i++)
                {
                    for(int elem = 0; elem < 3; ++elem)
                    {
                        int          P       = static_cast<int>((*src++) * 255.0f);
                        unsigned int Clamped = P < 0 ? 0 : P > 0xff ? 0xff : P;
                        *dst++ = static_cast<unsigned char>(Clamped);
                    }
                }
            }
            break;

        case RT_FORMAT_FLOAT4:
            // This buffer is upside down
            for(int j = height - 1; j >= 0; --j)
            {
                unsigned char* dst = &pix[0] + (3 * width * (height - 1 - j));
                float*         src = ((float*)imageData) + (4 * width * j);
                for(int i = 0; i < width; i++)
                {
                    for(int elem = 0; elem < 3; ++elem)
                    {
                        int          P       = static_cast<int>((*src++) * 255.0f);
                        unsigned int Clamped = P < 0 ? 0 : P > 0xff ? 0xff : P;
                        *dst++ = static_cast<unsigned char>(Clamped);
                    }

                    // skip alpha
                    src++;
                }
            }
            break;

        default:
            fprintf(stderr, "Unrecognized buffer data type or format.\n");
            exit(2);
            break;
    }

    std::string suffix;
    std::string fn(filename);
    if(fn.length() > 4)
    {
        suffix = fn.substr(fn.length() - 4);
    }

    if(suffix == ".ppm")
    {
        SavePPM(&pix[0], filename, width, height, 3);
    }
    else if(suffix == ".png")
    {
        if(!stbi_write_png(filename, (int)width, (int)height, 3, &pix[0], /*row stride in bytes*/ width * 3 * sizeof(unsigned char)))
        {
            throw Exception(std::string("Failed to write image: ") + filename);
        }
    }
    else
    {
        throw Exception(std::string("Unrecognized output image file extension: ") + filename);
    }

    // Now unmap the buffer
    RT_CHECK_ERROR(rtBufferUnmap(buffer));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::setCamera(const optix::float3& eye, const optix::float3& u, const optix::float3& v, const optix::float3& w)
{
    m_OptiXContext["eye"]->setFloat(eye);
    m_OptiXContext["U"]->setFloat(u);
    m_OptiXContext["V"]->setFloat(v);
    m_OptiXContext["W"]->setFloat(w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RayTracer::resizeBuffer(optix::Buffer buffer, int width, int height)
{
    buffer->setSize(width, height);

    // Check if we have a GL interop display buffer
    const unsigned pboId = buffer->getGLBOId();
    if(pboId)
    {
        buffer->unregisterGLBuffer();
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, buffer->getElementSize() * width * height, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        buffer->registerGLBuffer();
    }
}
