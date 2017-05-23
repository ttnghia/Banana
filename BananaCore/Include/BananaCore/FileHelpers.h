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

#include <BananaCore/Macros.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

#ifdef __BANANA_WINDOWS__
#include <filesystem>
#include <windows.h>
#endif

namespace FileHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void createFolder(const char* folderName)
{
#ifdef __BANANA_WINDOWS__
    CreateDirectoryA(folderName, NULL);
#else
    char buff[512];
    __BNN_SPRINT(buff, "mkdir -p %s", folderName);
    system(buff);
#endif
}

inline void createFolder(const std::string& folderName)
{
    createFolder(folderName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool fileExisted(const char* fileName)
{
    FILE* file = nullptr;
#ifdef __BANANA_WINDOWS__
    fopen_s(&file, fileName, "r");
#else
    file = fopen(fileName, "r");
#endif
    if(file != nullptr)
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

inline bool fileExisted(const std::string& fileName)
{
    return fileExisted(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline size_t getFileSize(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        return 0;
    }

    size_t fileSize = (size_t)file.tellg();
    file.close();

    return fileSize;
}

inline size_t getFileSize(const std::string& fileName)
{
    return getFileSize(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::string getFolderSize(const char* folderName, int level = 0)
{
#ifdef __BANANA_WINDOWS__
    __BNN_UNIMPLEMENTED_FUNC
#else
    const int maxBuffer = 256;
    char buffer[maxBuffer];
    char command[maxBuffer];

    if(level == 0)
    {
        __BNN_SPRINT(command, "du -h -d0 %s | cut -f1", folderName);
    }
    else
    {
        __BNN_SPRINT(command, "du -h -d%d %s", level, folderName);
    }

    FILE* stream = popen(command, "r");

    std::string output;

    if(!stream)
    {
        output = "Error";
    }
    else
    {
        while(!feof(stream))
        {
            if(fgets(buffer, maxBuffer, stream) != nullptr)
            {
                output.append(buffer);
            }
        }

        pclose(stream);
    }

    return output;
#endif
}

inline std::string getFolderSize(const std::string& folderName, int level = 0)
{
    return getFolderSize(folderName.c_str(), level);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::string getFullFilePath(const std::string& topFolder, std::string dataSubFolder, std::string fileName, std::string fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder.c_str(), fileName.c_str(), fileID, fileExtension.c_str());
    return std::string(buff);
}

inline std::string getFullFilePath(const char* topFolder, const char* dataSubFolder, const char* fileName, const char* fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder, dataSubFolder, fileName, fileID, fileExtension);

    return std::string(buff);
}

inline std::string getFullFilePath(const std::string& topFolder, const char* dataSubFolder, const char* fileName, const char* fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder, fileName, fileID, fileExtension);

    return std::string(buff);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writeFile(const std::string& str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

    file << str;
    file.close();
}

inline void writeFile(const std::string& str, std::string fileName)
{
    writeFile(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writeFile(const std::vector<std::string>& vecStr, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

    for(auto& str : vecStr)
    {
        file << str << std::endl;
    }

    file.close();
}

inline void writeFile(const std::vector<std::string>& vecStr, std::string fileName)
{
    writeFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writeFile(const unsigned char* dataBuffer, size_t dataSize, const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

    file.write((char*)dataBuffer, dataSize);
    file.close();
}

inline void writeFile(const unsigned char* dataBuffer, size_t dataSize, std::string fileName)
{
    writeFile(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::future<void> writeFileAsync(const unsigned char* dataBuffer, size_t dataSize, const char* fileName)
{
    std::future<void> futureObj = std::async(std::launch::async, [&]
        {
            std::ofstream file(fileName, std::ios::binary | std::ios::out);
            __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

            file.write((char*)dataBuffer, dataSize);
            file.close();
        });

    return futureObj;
}

inline std::future<void> writeFileAsync(const unsigned char* dataBuffer, size_t dataSize, std::string fileName)
{
    return writeFileAsync(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void appendToFile(const std::string& str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

    file << str;
    file.close();
}

inline void appendToFile(const std::string& str, std::string fileName)
{
    appendToFile(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void appendToFile(const std::vector<std::string>& vecStr, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    __BNN_ASSERT_MSG(file.is_open(), "Could not open file for writing.");

    for(auto& str : vecStr)
    {
        file << str << std::endl;
    }

    file.close();
}


inline void appendToFile(const std::vector<std::string>& vecStr, std::string fileName)
{
    appendToFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(unsigned char* dataBuffer, size_t bufferSize, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    if(bufferSize < fileSize)
    {
        dataBuffer = reinterpret_cast<unsigned char*>(dataBuffer, fileSize);
    }

    file.seekg(0, std::ios::beg);
    file.read((char*)dataBuffer, fileSize);
    file.close();

    return true;
}

inline bool readFile(unsigned char* dataBuffer, size_t bufferSize, std::string fileName)
{
    return readFile(dataBuffer, bufferSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(std::vector<unsigned char>& dataBuffer, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    dataBuffer.resize(fileSize);

    file.seekg(0, std::ios::beg);
    file.read((char*)dataBuffer.data(), fileSize);
    file.close();

    return true;
}

inline bool readFile(std::vector<unsigned char>& dataBuffer, std::string fileName)
{
    return readFile(dataBuffer, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(std::vector<std::string>& vecStr, const char* fileName)
{
    std::ifstream file(fileName, std::ios::in);

    if(!file.is_open())
    {
        return false;
    }

    vecStr.resize(0);
    std::string line;

    while(std::getline(file, line))
    {
        vecStr.push_back(line);
    }

    file.close();

    return true;
}

inline bool readFile(std::vector<std::string>& vecStr, std::string fileName)
{
    return readFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// template funcs
template<class T>
inline void writeBinaryFile(const std::vector<T>& dvec, const char* fileName)
{
    writeFile((unsigned char*)dvec.data(), dvec.size() * sizeof(T), fileName);
}

template<class T>
inline void writeBinaryFile(const std::vector<T>& data, std::string fileName)
{
    writeBinaryFile(data, fileName.c_str());
}

template<class T>
inline std::future<void> writeBinaryFileAsync(const std::vector<T>& dvec, const char* fileName)
{
    return writeFileAsync((unsigned char*)dvec.data(), dvec.size() * sizeof(T), fileName);
}

template<class T>
inline std::future<void> writeBinaryFileAsync(const std::vector<T>& data, std::string fileName)
{
    return writeBinaryFileAsync(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace FileHelpers
