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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Utils/FileHelpers.h>

#include <string>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <sstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DataBuffer class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataBuffer
{
public:
    DataBuffer(size_t bufferSize = 0)
    {
        if(bufferSize > 0)
            reserve(bufferSize);
    }

    ~DataBuffer()
    {
        clearBuffer();
    }

    size_t size() const
    {
        return static_cast<size_t>(m_Buffer.size());
    }

    void resize(size_t bufferSize)
    {
        m_Buffer.resize(bufferSize);
    }

    void reserve(size_t bufferSize)
    {
        m_Buffer.reserve(bufferSize);
    }

    const unsigned char* data() const
    {
        return m_Buffer.data();
    }

    std::vector<unsigned char>& buffer()
    {
        return m_Buffer;
    }

    void clearBuffer()
    {
        m_Buffer.resize(0);
    }

    //////////////////////////////////////////////////////////////////////
    /// replace data in the buffer with new data
    //////////////////////////////////////////////////////////////////////
    void setData(const DataBuffer& dataBuffer)
    {
        clearBuffer();
        append(dataBuffer);
    }

    void setData(const unsigned char* arrData, size_t dataSize)
    {
        clearBuffer();
        append(arrData, dataSize);
    }

    template<class T>
    void setFloat(T data)
    {
        clearBuffer();
        append<float>(static_cast<float>(data));
    }

    template<class T>
    void setDouble(T data)
    {
        clearBuffer();
        append<double>(static_cast<double>(data));
    }

    template<class T>
    void setData(T data)
    {
        clearBuffer();
        append<T>(data);
    }

    // need this, it is similar to the one above, but using reference parameter
    template<class T>
    void setData(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        clearBuffer();
        append<T>(vData, bWriteVectorSize);
    }

    template<class T>
    void setFloatArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        clearBuffer();
        appendFloatArray<T>(vData, bWriteVectorSize);
    }

    template<class T>
    void setDoubleArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        clearBuffer();
        appendDoubleArray<T>(vData, bWriteVectorSize);
    }

    //////////////////////////////////////////////////////////////////////
    /// append data
    //////////////////////////////////////////////////////////////////////
    void append(const DataBuffer& dataBuffer)
    {
        append((const unsigned char*)dataBuffer.data(), dataBuffer.size());
    }

    void append(const unsigned char* arrData, size_t dataSize)
    {
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        memcpy((void*)&(m_Buffer.data()[endOffset]), arrData, dataSize);
    }

    template<class T>
    void append(T value)
    {
        size_t dataSize  = sizeof(T);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T Tdata = value;
        memcpy((void*)&(m_Buffer.data()[endOffset]), (void*)&Tdata, dataSize);
    }

    template<class T>
    void append(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        // any vector data begins with the number of vector elements
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(T);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        memcpy((void*)&(m_Buffer.data()[endOffset]), (void*)vData.data(), dataSize);
    }

    template<class T>
    void append(const Vec_Vec2<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(T) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T* buff_ptr = (T*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec = vData[i];
            buff_ptr[i * 2]     = vec[0];
            buff_ptr[i * 2 + 1] = vec[1];
        }
    }

    template<class T>
    void append(const Vec_Vec3<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(T) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T* buff_ptr = (T*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec = vData[i];
            buff_ptr[i * 3]     = vec[0];
            buff_ptr[i * 3 + 1] = vec[1];
            buff_ptr[i * 3 + 2] = vec[2];
        }
    }

    template<class T>
    void append(const std::vector<std::vector<T> >& vData)
    {
        const UInt numElements = (UInt)vData.size();
        append(numElements);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const std::vector<T>& vec = vData[i];
            append(vec);
        }
    }

    template<class T>
    void appendFloat(T value)
    {
        size_t dataSize  = sizeof(float);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float fldata = static_cast<float>(value);
        memcpy((void*)&(m_Buffer.data()[endOffset]), (void*)&fldata, dataSize);
    }

    template<class T>
    void appendFloatArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(float);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            buff_ptr[i] = static_cast<float>(vData[i]);
        }
    }

    template<class T>
    void appendFloatArray(const Vec_Vec2<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(float) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec = vData[i];
            buff_ptr[i * 2]     = static_cast<float>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<float>(vec[1]);
        }
    }

    template<class T>
    void appendFloatArray(const Vec_Vec3<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(float) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec = vData[i];
            buff_ptr[i * 3]     = static_cast<float>(vec[0]);
            buff_ptr[i * 3 + 1] = static_cast<float>(vec[1]);
            buff_ptr[i * 3 + 2] = static_cast<float>(vec[2]);
        }
    }

    template<class T>
    void appendDouble(T value)
    {
        size_t dataSize  = sizeof(double);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double dbdata = static_cast<double>(value);
        memcpy((void*)&(m_Buffer.data()[endOffset]), (void*)&dbdata, dataSize);
    }

    template<class T>
    void appendDoubleArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(double);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            buff_ptr[i] = static_cast<double>(vData[i]);
        }
    }

    template<class T>
    void appendDoubleArray(const Vec_Vec2<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(double) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec = vData[i];
            buff_ptr[i * 2]     = static_cast<double>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<double>(vec[1]);
        }
    }

    template<class T>
    void appendDoubleArray(const Vec_Vec3<T>& vData, bool bWriteVectorSize = true)
    {
        if(bWriteVectorSize)
        {
            const UInt numElements = (UInt)vData.size();
            append(numElements);
        }

        size_t dataSize  = vData.size() * sizeof(double) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*)&(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec = vData[i];
            buff_ptr[i * 3]     = static_cast<double>(vec[0]);
            buff_ptr[i * 3 + 1] = static_cast<double>(vec[1]);
            buff_ptr[i * 3 + 2] = static_cast<double>(vec[2]);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// get data
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    size_t getData(T& value, size_t startOffset = 0)
    {
        size_t dataSize = (size_t)sizeof(T);
        assert(startOffset + dataSize <= m_Buffer.size());

        memcpy(&value, &m_Buffer.data()[startOffset], dataSize);

        return dataSize;
    }

    template<class T>
    size_t getData(std::vector<T>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        segmentSize = (size_t)(sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        memcpy(vData.data(), &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getData(Vec_Vec2<T>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        segmentSize = (size_t)(2 * sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        T* buff_ptr = (T*)&m_Buffer.data()[segmentStart];

        for(size_t i = 0; i < numElements; ++i)
        {
            vData[i] = Vec2<T>(buff_ptr[i * 2], buff_ptr[i * 2 + 1]);
        }

        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getData(Vec_Vec3<T>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        segmentSize = (size_t)(3 * sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        T* buff_ptr = (T*)&m_Buffer.data()[segmentStart];

        for(size_t i = 0; i < numElements; ++i)
        {
            vData[i] = Vec3<T>(buff_ptr[i * 3], buff_ptr[i * 3 + 1], buff_ptr[i * 3 + 2]);
        }

        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

    template<class T, int N>
    size_t getFloatArray(std::vector<float>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements * N);

        T value;
        segmentSize = (size_t)(sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)(&value), &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = static_cast<float>(value);
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getFloatArray(Vec_Vec2<float>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        T values[2];
        segmentSize = (size_t)(2 * sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)values, &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = Vec2<float>(static_cast<float>(values[0]), static_cast<float>(values[1]));
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getFloatArray(Vec_Vec3<float>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        T values[3];
        segmentSize = (size_t)(3 * sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)values, &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = Vec3<float>(static_cast<float>(values[0]), static_cast<float>(values[1]), static_cast<float>(values[2]));
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T, int N>
    size_t getDoubleArray(std::vector<double>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements * N);

        T value;
        segmentSize = (size_t)(sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)(&value), &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = static_cast<double>(value);
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getDoubleArray(Vec_Vec2<double>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        T values[2];
        segmentSize = (size_t)(2 * sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)values, &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = Vec2<double>(static_cast<double>(values[0]), static_cast<double>(values[1]));
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getDoubleArray(Vec_Vec3<T>& vData, size_t startOffset = 0, UInt vSize = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = 0;
        UInt   numElements  = vSize;

        if(numElements == 0)
        {
            segmentSize = (size_t)sizeof(UInt);
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
            segmentStart += segmentSize;
        }

        vData.resize(numElements);

        T values[3];
        segmentSize = (size_t)(3 * sizeof(T));

        for(size_t i = 0; i < vData.size(); ++i)
        {
            assert(segmentStart + segmentSize <= m_Buffer.size());

            memcpy((void*)values, &(m_Buffer.data()[segmentStart]), segmentSize);
            vData[i]      = Vec3<double>(static_cast<double>(values[0]), static_cast<double>(values[1]), static_cast<double>(values[2]));
            segmentStart += segmentSize;
        }

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t getData(std::vector<std::vector<T> >& vData, size_t startOffset = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize  = (size_t)sizeof(UInt);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        UInt numElements;
        memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        vData.resize(numElements);

        for(UInt i = 0; i < numElements; ++i)
        {
            segmentStart += getData(vData[i], segmentStart);
        }

        return (segmentStart - startOffset);
    }

    size_t getData(unsigned char* arrData, size_t dataSize, size_t startOffset = 0)
    {
        assert(startOffset + dataSize <= m_Buffer.size());
        memcpy(arrData, &m_Buffer.data()[startOffset], dataSize);

        return dataSize;
    }

private:
    std::vector<unsigned char> m_Buffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DataConverter class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataConverter
{
public:
    DataConverter() = default;

    ~DataConverter()
    {
        m_DataBuffer.clearBuffer();
    }

    template<class T>
    const unsigned char* toFloatArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setFloatArray(vData, bWriteVectorSize);

        return m_DataBuffer.data();
    }

    template<class T>
    const unsigned char* toFloatArray(const Vec_Vec2<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setFloatArray(vData, , bWriteVectorSize);

        return m_DataBuffer.data();
    }

    template<class T>
    const unsigned char* toFloatArray(const Vec_Vec3<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setFloatArray(vData, bWriteVectorSize);

        return m_DataBuffer.data();
    }

    template<class T>
    const unsigned char* toDoubleArray(const std::vector<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setDoubleArray(vData, bWriteVectorSize);

        return m_DataBuffer.data();
    }

    template<class T>
    const unsigned char* toDoubleArray(const Vec_Vec2<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setDoubleArray(vData, bWriteVectorSize);

        return m_DataBuffer.data();
    }

    template<class T>
    const unsigned char* toDoubleArray(const Vec_Vec3<T>& vData, bool bWriteVectorSize = true)
    {
        m_DataBuffer.setDoubleArray(vData, bWriteVectorSize);

        return m_DataBuffer.data();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    static std::string byteToHex(const T* arrBytes, size_t arrSize)
    {
        std::stringstream ss;

        for(size_t i = 0; i < arrSize; ++i)
        {
            ss << std::hex << std::uppercase << std::setw(2) << static_cast<int>(arrBytes[i]);
        }

        return ss.str();
    }

    template<class T>
    static std::string byteToHex(const std::vector<T>& vecBytes)
    {
        return byteToHex(vecBytes.data(), vecBytes.size());
    }

private:
    DataBuffer m_DataBuffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DataIO class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataIO
{
public:
    DataIO(const std::string& dataRootFolder,
           const std::string& dataFolder,
           const std::string& fileName,
           const std::string& fileExtension,
           const std::string& dataName) :
        m_DataFolder(dataRootFolder),
        m_DataSubFolder(dataFolder),
        m_FileName(fileName),
        m_FileExtension(fileExtension),
        m_DataName(dataName),
        m_bOutputFolderCreated(false) {}

    virtual ~DataIO()
    {
        if(m_WriteFutureObj.valid())
            m_WriteFutureObj.wait();
    }

    ////////////////////////////////////////////////////////////////////////////////
    int getLatestFileIndex(int maxIndex = 10000)
    {
        int latestIndex = -1;

        for(int index = maxIndex; index > 0; --index)
        {
            if(!existedFileIndex(index))
            {
                latestIndex = index;
                break;
            }
        }

        return latestIndex;
    }

    bool existedFileIndex(int fileID)
    {
        return FileHelpers::fileExisted(getFilePath(fileID));
    }

    void clearBuffer()
    {
        if(m_WriteFutureObj.valid())
        {
            m_WriteFutureObj.wait();
        }

        m_DataBuffer.clearBuffer();
    }

    void flushBuffer(int fileID)
    {
        if(!m_bOutputFolderCreated)
        {
            createOutputFolders();
        }

        FileHelpers::writeFile(m_DataBuffer.data(), m_DataBuffer.size(), getFilePath(fileID));
    }

    void flushBufferAsync(int fileID)
    {
        m_WriteFutureObj = std::async(std::launch::async, [&]()
                                      {
                                          if(!m_bOutputFolderCreated)
                                          {
                                              createOutputFolders();
                                          }

                                          FileHelpers::writeFile(m_DataBuffer.data(), m_DataBuffer.size(), getFilePath(fileID));
                                      });
    }

    bool loadFileIndex(int fileID)
    {
        return FileHelpers::readFile(m_DataBuffer.buffer(), getFilePath(fileID));
    }

    std::string getFilePath(int fileID)
    {
        char filePath[1024];
        __BNN_SPRINT(filePath, "%s/%s/%s.%04d.%s", m_DataFolder.c_str(), m_DataSubFolder.c_str(), m_FileName.c_str(), fileID, m_FileExtension.c_str());
        return std::string(filePath);
    }

    const DataBuffer& getBuffer() const
    {
        return m_DataBuffer;
    }

    DataBuffer& getBuffer()
    {
        return m_DataBuffer;
    }

    std::string& dataFolder()
    {
        return m_DataFolder;
    }

    std::string& dataSubFolder()
    {
        return m_DataSubFolder;
    }

    std::string& fileName()
    {
        return m_FileName;
    }

    std::string& fileExtension()
    {
        return m_FileExtension;
    }

    std::string& dataName()
    {
        return m_DataName;
    }

private:
    void createOutputFolders()
    {
        char outputFolder[512];
        __BNN_SPRINT(outputFolder, "%s/%s", m_DataFolder.c_str(), m_DataSubFolder.c_str());
        FileHelpers::createFolder(outputFolder);
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool              m_bOutputFolderCreated;
    std::string       m_DataFolder;
    std::string       m_DataSubFolder;
    std::string       m_FileName;
    std::string       m_FileExtension;
    std::string       m_DataName;
    DataBuffer        m_DataBuffer;
    std::future<void> m_WriteFutureObj;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana