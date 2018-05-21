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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Data/DataBuffer.h>
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Int DataIO::getLatestFileIndex(Int maxIndex)
{
    for(Int index = maxIndex; index >= 1; --index) {
        if(existedFileIndex(index)) {
            return index;
        }
    }

    return -1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void DataIO::flushBuffer(Int fileID)
{
    createOutputFolders();
    FileHelpers::writeFile(m_DataBuffer.data(), m_DataBuffer.size(), getFilePath(fileID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void DataIO::flushBufferAsync(Int fileID)
{
    m_WriteFutureObj = std::async(std::launch::async, [&, fileID]()
                                  {
                                      createOutputFolders();
                                      FileHelpers::writeFile(m_DataBuffer.data(), m_DataBuffer.size(), getFilePath(fileID));
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String DataIO::getFilePath(Int fileID)
{
    char filePath[1024];
    __BNN_SPRINT(filePath, "%s/%s/%s.%04d.%s", m_DataFolder.c_str(), m_DataSubFolder.c_str(), m_FileName.c_str(), fileID, m_FileExtension.c_str());
    return String(filePath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void DataIO::createOutputFolders()
{
    if(m_bOutputFolderCreated) {
        return;
    }
    char outputFolder[512];
    __BNN_SPRINT(outputFolder, "%s/%s", m_DataFolder.c_str(), m_DataSubFolder.c_str());
    FileHelpers::createFolder(outputFolder);
    m_bOutputFolderCreated = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana