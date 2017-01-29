﻿//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <string>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

#include <BananaAppCore/Macros.h>
#include <BananaAppCore/TypeNames.h>
#include <BananaAppCore/FileHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataBuffer
{
public:
    DataBuffer(size_t bufferSize = 0);

    ~DataBuffer();

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
    void set_data(const DataBuffer& dataBuffer);
    void set_data(const unsigned char* arrData, size_t dataSize);

    template<class T>
    void set_to_float(T data)
    {
        clearBuffer();
        push_back(static_cast<float>(data));
    }

    template<class T>
    void set_to_double(T data)
    {
        clearBuffer();
        push_back(static_cast<double>(data));
    }

    template<class T>
    void set_data(T data)
    {
        clearBuffer();
        push_back(data);
    }

    // this is similar to the one above, but using reference parameter
    template<class T>
    void set_data(const std::vector<T>& vData)
    {
        clearBuffer();
        push_back(vData);
    }

    template<class T>
    void set_to_float_array(const std::vector<T>& vData)
    {
        clearBuffer();
        push_back_to_float_array(vData);
    }

    template<class T>
    void set_to_double_array(const std::vector<T>& vData)
    {
        clearBuffer();
        push_back_to_double_array(vData);
    }


    //////////////////////////////////////////////////////////////////////
    /// append data
    //////////////////////////////////////////////////////////////////////

    void push_back(const DataBuffer& dataBuffer);
    void push_back(const unsigned char* arrData, size_t dataSize);

    template<class T>
    void push_back(T value)
    {
        size_t dataSize = sizeof(T);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T Tdata = value;
        memcpy((void*) & (m_Buffer.data()[endOffset]), (void*)&Tdata, dataSize);
    }

    template<class T>
    void push_back(const std::vector<T>& vData)
    {
        // any vector data begins with the number of vector elements
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(T);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        memcpy((void*) & (m_Buffer.data()[endOffset]), (void*)vData.data(), dataSize);
    }

    template<class T>
    void push_back(const Vec_Vec2<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(T) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T* buff_ptr = (T*) &(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec  = vData[i];
            buff_ptr[i * 2]     = vec[0];
            buff_ptr[i * 2 + 1] = vec[1];
        }
    }

    template<class T>
    void push_back(const Vec_Vec3<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(T) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        T* buff_ptr = (T*) &(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec  = vData[i];
            buff_ptr[i * 3]     = vec[0];
            buff_ptr[i * 3 + 1] = vec[1];
            buff_ptr[i * 3 + 2] = vec[2];
        }
    }

    template<class T>
    void push_back(const std::vector<std::vector<T> >& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const std::vector<T>& vec = vData[i];
            push_back(vec);
        }
    }

    template<class T>
    void push_back_to_float(T value)
    {
        size_t dataSize = sizeof(float);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float fldata = static_cast<float>(value);
        memcpy((void*) & (m_Buffer.data()[endOffset]), (void*)&fldata, dataSize);

    }

    template<class T>
    void push_back_to_float_array(const std::vector<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(float);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*) &(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            buff_ptr[i] = static_cast<float>(vData[i]);
        }

    }

    template<class T>
    void push_back_to_float_array(const Vec_Vec2<T> & vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);;

        size_t dataSize = vData.size() * sizeof(float) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*) & (m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec  = vData[i];
            buff_ptr[i * 2]     = static_cast<float>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<float>(vec[1]);
        }
    }

    template<class T>
    void push_back_to_float_array(const Vec_Vec3<T> & vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);;

        size_t dataSize = vData.size() * sizeof(float) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        float* buff_ptr = (float*) & (m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec  = vData[i];
            buff_ptr[i * 2]     = static_cast<float>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<float>(vec[1]);
            buff_ptr[i * 2 + 2] = static_cast<float>(vec[2]);
        }
    }

    template<class T>
    void push_back_to_double(T value)
    {
        size_t dataSize = sizeof(double);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double dbdata = (double)value;
        memcpy((void*) &(m_Buffer.data()[endOffset]), (void*)&dbdata, dataSize);
    }

    template<class T>
    void push_back_to_double_array(const std::vector<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(double);
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*) & (m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            buff_ptr[i] = static_cast<double>(vData[i]);
        }
    }

    template<class T>
    void push_back_to_double_array(const Vec_Vec2<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(double) * 2;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*) &(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec2<T>& vec  = vData[i];
            buff_ptr[i * 2]     = static_cast<double>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<double>(vec[1]);
        }
    }

    template<class T>
    void push_back_to_double_array(const Vec_Vec3<T>& vData)
    {
        const UInt32 numElements = (UInt32)vData.size();
        push_back(numElements);

        size_t dataSize = vData.size() * sizeof(double) * 3;
        size_t endOffset = m_Buffer.size();
        resize(endOffset + dataSize);

        double* buff_ptr = (double*) &(m_Buffer.data()[endOffset]);

        for(size_t i = 0; i < vData.size(); ++i)
        {
            const Vec3<T>& vec  = vData[i];
            buff_ptr[i * 2]     = static_cast<double>(vec[0]);
            buff_ptr[i * 2 + 1] = static_cast<double>(vec[1]);
            buff_ptr[i * 2 + 2] = static_cast<double>(vec[2]);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    size_t get_data(T& value, size_t startOffset = 0)
    {
        size_t dataSize = (size_t)sizeof(T);
        assert(startOffset + dataSize <= m_Buffer.size());

        memcpy(&value, &m_Buffer.data()[startOffset], dataSize);

        return dataSize;
    }

    template<class T>
    size_t get_data(std::vector<T>& vData, size_t startOffset = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize = (size_t)sizeof(UInt32);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        UInt32 numElements;
        memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        vData.resize(numElements);

        segmentSize = (size_t)(sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        memcpy(vData.data(), &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t get_data(std::vector<std::vector<T> >& vData, size_t startOffset = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize = (size_t)sizeof(UInt32);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        UInt32 numElements;
        memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        vData.resize(numElements);

        for(UInt32 i = 0; i < numElements; ++i)
        {
            segmentStart += get_data(vData[i], segmentStart);
        }

        return (segmentStart - startOffset);
    }

    size_t get_data(unsigned char* arrData, size_t dataSize, size_t startOffset = 0)
    {
        size_t readSize = (std::min)(dataSize, m_Buffer.size() - startOffset);

        memcpy(arrData, &m_Buffer.data()[startOffset], readSize);

        return readSize;
    }

    template<class T>
    size_t get_data(Vec_Vec2<T>& vData, size_t startOffset = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize = (size_t)sizeof(UInt32);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        UInt32 numElements;
        memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        vData.resize(numElements);

        segmentSize = (size_t)(2 * sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        T* buff_ptr = (T*)&m_Buffer.data()[segmentStart];

        for(size_t i = 0; i < numElements; ++i)
        {
            vData[i] = Vec2<T>(buff_ptr[i * 2],
                               buff_ptr[i * 2 + 1]);
        }

        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

    template<class T>
    size_t get_data(Vec_Vec3<T>& vData, size_t startOffset = 0)
    {
        size_t segmentStart = startOffset;
        size_t segmentSize = (size_t)sizeof(UInt32);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        UInt32 numElements;
        memcpy(&numElements, &m_Buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        vData.resize(numElements);

        segmentSize = (size_t)(3 * sizeof(T) * numElements);
        assert(segmentStart + segmentSize <= m_Buffer.size());

        T* buff_ptr = (T*)&m_Buffer.data()[segmentStart];

        for(size_t i = 0; i < numElements; ++i)
        {
            vData[i] = Vec3<T>(buff_ptr[i * 3],
                               buff_ptr[i * 3 + 1],
                               buff_ptr[i * 3 + 2]);
        }

        segmentStart += segmentSize;

        return (segmentStart - startOffset);
    }

private:
    size_t                     m_BufferSize;
    std::vector<unsigned char> m_Buffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataConverter
{
public:
    DataConverter()
    {}

    ~DataConverter()
    {
        dataBuffer.clearBuffer();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_float_array(const std::vector<T>& vData)
    {
        dataBuffer.set_to_float_array(vData);

        return dataBuffer.data();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_float_array(const Vec_Vec2<T>& vData)
    {
        dataBuffer.set_to_float_array(vData);

        return dataBuffer.data();
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_float_array(const Vec_Vec3<T>& vData)
    {
        dataBuffer.set_to_float_array(vData);

        return dataBuffer.data();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_double_array(const std::vector<T>& vData)
    {
        dataBuffer.set_to_double_array(vData);

        return dataBuffer.data();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_double_array(const Vec_Vec2<T>& vData)
    {
        dataBuffer.set_to_double_array(vData);

        return dataBuffer.data();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T>
    unsigned char* to_double_array(const Vec_Vec3<T>& vData)
    {
        dataBuffer.set_to_double_array(vData);

        return dataBuffer.data();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    static std::string byte_to_hex_encoder(const unsigned char* arrBytes, size_t arrSize)
    {
        std::stringstream ss;

        for(size_t i = 0; i < arrSize; ++i)
        {
            ss << std::hex << std::uppercase << std::setw(2) << (int)arrBytes[i];
        }

        return ss.str();
    }

private:
    DataBuffer dataBuffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataIO
{
public:
    DataIO(std::string dataRootFolder, std::string dataFolder,
           std::string fileName, std::string fileExtension);
    virtual ~DataIO();

    int find_latest_file_index(int maxIndex);
    bool existed_file_index(int fileID);
    void reset_buffer();
    void flush_buffer(int fileID);
    void flush_buffer_async(int fileID);
    bool load_file_index(int fileID);
    std::string get_file_name(int fileID);

    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void append_to_buffer(T value)
    {
        m_FileBuffer.push_back(value);
    }

    template<class T>
    void get_data(T& value, size_t startOffset = 0)
    {
        return m_FileBuffer.get_data(value, startOffset);
    }

private:
    void create_output_folders();

    ////////////////////////////////////////////////////////////////////////////////
    bool               m_isOutputFolderCreated;
    std::string        m_DataRootFolder;
    std::string        m_DataFolder;
    std::string        m_FileName;
    std::string        m_FileExtension;
    DataBuffer         m_FileBuffer;
    std::future<void>  m_WriteFutureObj;
};
