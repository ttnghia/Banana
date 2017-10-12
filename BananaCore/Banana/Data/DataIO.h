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
    DataBuffer(size_t bufferSize = 0) { if(bufferSize > 0) { reserve(bufferSize); } }

    size_t size() const { return m_Buffer.size(); }
    void   resize(size_t bufferSize) { m_Buffer.resize(bufferSize); }
    void   reserve(size_t bufferSize) { m_Buffer.reserve(bufferSize); }
    void   clearBuffer() { m_Buffer.resize(0); }

    unsigned char*       data(size_t offset = 0) { return &m_Buffer.data()[offset]; }
    const unsigned char* data(size_t offset = 0) const { return &m_Buffer.data()[offset]; }

    Vector<UChar>&       buffer() { return m_Buffer; }
    const Vector<UChar>& buffer() const { return m_Buffer; }

    //////////////////////////////////////////////////////////////////////
    // set data (= clear + append)
    void setData(const DataBuffer& dataBuffer) { clearBuffer(); append(dataBuffer); }
    void setData(const void* dataPtr, size_t dataSize) { clearBuffer(); append(dataPtr, dataSize); }

    template<class T> void setData(T data) { clearBuffer(); append<T>(data); }
    template<class T> void setData(const Vector<T>& dvec, bool bWriteVectorSize = true) { clearBuffer(); append<T>(dvec, bWriteVectorSize); }
    template<class T> void setData(const Vector<Vector<T> >& dvec, bool bWriteVectorSize = true) { clearBuffer(); append<T>(dvec, bWriteVectorSize); }

    //////////////////////////////////////////////////////////////////////
    // append data
    void append(const DataBuffer& dataBuffer) { append((const void*)dataBuffer.data(), dataBuffer.size()); }
    void append(const void* dataPtr, size_t dataSize);

    template<class T> void        append(T value) { append((const void*)&value, sizeof(T)); }
    template<class T> void        append(const Vector<T>& dvec, bool bWriteVectorSize = true);
    template<class T> void        append(const Vector<Vector<T> >& dvec, bool bWriteVectorSize = true);
    template<Int N, class T> void append(const Vector<VecX<N, T> >& dvec, bool bWriteVectorSize = true);

    ////////////////////////////////////////////////////////////////////////////////
    // get data
    size_t getData(void* dataPtr, size_t dataSize, size_t startOffset = 0);

    template<class T> size_t        getData(T& value, size_t startOffset = 0) { return getData((void*)&value, sizeof(T), startOffset); }
    template<class T> size_t        getData(Vector<T>& dvec, size_t startOffset = 0, UInt vSize = 0);
    template<class T> size_t        getData(Vector<Vector<T> >& dvec, size_t startOffset = 0, UInt vSize = 0);
    template<Int N, class T> size_t getData(Vector<VecX<N, T> >& dvec, size_t startOffset = 0, UInt vSize = 0);

private:
    Vector<UChar> m_Buffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DataIO class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataIO
{
public:
    DataIO(const String& dataRootFolder,
           const String& dataFolder,
           const String& fileName,
           const String& fileExtension,
           const String& dataName) :
        m_DataFolder(dataRootFolder),
        m_DataSubFolder(dataFolder),
        m_FileName(fileName),
        m_FileExtension(fileExtension),
        m_DataName(dataName),
        m_bOutputFolderCreated(false) {}

    virtual ~DataIO() { if(m_WriteFutureObj.valid()) { m_WriteFutureObj.wait(); } }

    ////////////////////////////////////////////////////////////////////////////////
    void createOutputFolders();
    Int  getLatestFileIndex(Int maxIndex = 10000);
    bool existedFileIndex(Int fileID) { return FileHelpers::fileExisted(getFilePath(fileID)); }
    bool loadFileIndex(Int fileID) { return FileHelpers::readFile(m_DataBuffer.buffer(), getFilePath(fileID)); }

    ////////////////////////////////////////////////////////////////////////////////
    void waitForBuffer() { if(m_WriteFutureObj.valid()) { m_WriteFutureObj.wait(); } }
    void clearBuffer() { waitForBuffer();  m_DataBuffer.clearBuffer(); }
    void flushBuffer(Int fileID);
    void flushBufferAsync(Int fileID);

    ////////////////////////////////////////////////////////////////////////////////
    String            getFilePath(Int fileID);
    const DataBuffer& getBuffer() const { return m_DataBuffer; }
    DataBuffer&       getBuffer() { return m_DataBuffer; }

private:
    bool              m_bOutputFolderCreated;
    String            m_DataFolder;
    String            m_DataSubFolder;
    String            m_FileName;
    String            m_FileExtension;
    String            m_DataName;
    DataBuffer        m_DataBuffer;
    std::future<void> m_WriteFutureObj;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Data/DataIO.Impl.hpp>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana