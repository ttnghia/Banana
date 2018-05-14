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

#pragma once

#include <Banana/Setup.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

#ifdef __BANANA_WINDOWS__
#  include <windows.h>
#  include <filesystem>
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::FileHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void createFolder(const char* folderName)
{
#ifdef __BANANA_WINDOWS__
    //CreateDirectoryA(folderName, NULL);
    std::experimental::filesystem::create_directories(folderName);
#else
    char buff[512];
    __BNN_SPRINT(buff, "mkdir -p %s", folderName);
    system(buff);
#endif
}

inline void createFolder(const String& folderName)
{
    createFolder(folderName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool fileExisted(const char* fileName)
{
#ifdef __BANANA_WINDOWS__
    return std::experimental::filesystem::exists(fileName);
#else
    if(FILE* file = fopen(fileName, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
#endif
}

inline bool fileExisted(const String& fileName)
{
    return fileExisted(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline size_t getFileSize(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open()) {
        return 0;
    }

    size_t fileSize = (size_t)file.tellg();
    file.close();

    return fileSize;
}

inline size_t getFileSize(const String& fileName)
{
    return getFileSize(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec_String getFolderSizeInfo(const char* folderName, int level = 0)
{
#ifdef __BANANA_WINDOWS__
    __BNN_UNUSED(level);
    namespace fs = std::experimental::filesystem;
    size_t     totalSize = 0;
    Vec_String strs;
    strs.emplace_back(String("Data size: "));

    for(auto& p : fs::directory_iterator(folderName)) {
        if(fs::is_directory(p)) {
            size_t folderSize = 0;
            for(auto& f : fs::directory_iterator(p)) {
                if(fs::is_regular_file(f)) {
                    folderSize += fs::file_size(f);
                }
            }
            totalSize += folderSize;
            strs.emplace_back(String("....") + fs::path(p).filename().string() + String(": ") + std::to_string(folderSize / 1048576) + String(" (MB) - ") +
                              std::to_string(std::distance(fs::directory_iterator(p), fs::directory_iterator{})) + String(" files"));
        }
    }

    strs[0] += std::to_string(totalSize / 1048576) + String(" (MB)");

    return strs;
#else
    const int maxBuffer = 256;
    char      buffer[maxBuffer];
    char      command[maxBuffer];

    if(level == 0) {
        __BNN_SPRINT(command, "du -h -d0 %s | cut -f1", folderName);
    } else {
        __BNN_SPRINT(command, "du -h -d%d %s", level, folderName);
    }

    FILE* stream = popen(command, "r");

    String output("Data: \n");

    if(!stream) {
        output = String("Error");
    } else {
        while(!feof(stream)) {
            if(fgets(buffer, maxBuffer, stream) != nullptr) {
                output.append(buffer);
            }
        }

        pclose(stream);
    }

    return { output };
#endif
}

inline Vec_String getFolderSizeInfo(const String& folderName, int level = 0)
{
    return getFolderSizeInfo(folderName.c_str(), level);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String getFullFilePath(const String& topFolder, const String& dataSubFolder, const String& fileName, const String& fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder.c_str(), fileName.c_str(), fileID, fileExtension.c_str());
    return String(buff);
}

inline String getFullFilePath(const char* topFolder, const char* dataSubFolder, const char* fileName, const char* fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder, dataSubFolder, fileName, fileID, fileExtension);

    return String(buff);
}

inline String getFullFilePath(const String& topFolder, const char* dataSubFolder, const char* fileName, const char* fileExtension, int fileID)
{
    char buff[512];
    __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder, fileName, fileID, fileExtension);

    return String(buff);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String getFileExtension(const String& filePath)
{
    return filePath.substr(filePath.find_last_of(".") + 1);
}

inline String getFileName(const String& filePath)
{
    String tmp = filePath;
    std::replace(tmp.begin(), tmp.end(), '\\', '/');
    return tmp.substr(tmp.find_last_of("/") + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void copyFile(const char* srcFile, const char* dstFile)
{
    const int bufferSize = 2048;
    char      buff[bufferSize];

    FILE* src = nullptr;
    FILE* dst = nullptr;
#ifdef __BANANA_WINDOWS__
    fopen_s(&src, srcFile, "r");
    fopen_s(&dst, dstFile, "w");
#else
    dst = fopen(dstFile, "w");
    src = fopen(srcFile, "r");
#endif
    if(src == nullptr || dst == nullptr) {
        if(src != nullptr) { fclose(src); }
        if(dst != nullptr) { fclose(dst); }
        return;
    }

    size_t size;
    while((size = fread(buff, 1, bufferSize, src)) > 0) {
        fwrite(buff, 1, size, dst);
    }

    fclose(src);
    fclose(dst);
}

inline void copyFile(const String& srcFile, const String& dstFile)
{
    copyFile(srcFile.c_str(), dstFile.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool writeFile(const String& str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open()) {
        return false;
    }

    file << str;
    file.close();
    return true;
}

inline bool writeFile(const String& str, const String& fileName)
{
    return writeFile(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool writeFile(const Vector<String>& vecStr, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    if(!file.is_open()) {
        return false;
    }

    for(auto& str : vecStr) {
        file << str << std::endl;
    }

    file.close();
    return true;
}

inline bool writeFile(const Vector<String>& vecStr, const String& fileName)
{
    return writeFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool writeFile(const void* dataBuffer, size_t dataSize, const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    if(!file.is_open()) {
        return false;
    }

    file.write((char*)dataBuffer, dataSize);
    file.close();
    return true;
}

inline bool writeFile(const void* dataBuffer, size_t dataSize, const String& fileName)
{
    return writeFile(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::future<void> writeFileAsync(const void* dataBuffer, size_t dataSize, const char* fileName)
{
    std::future<void> futureObj = std::async(std::launch::async, [&]
                                             {
                                                 std::ofstream file(fileName, std::ios::binary | std::ios::out);
                                                 __BNN_REQUIRE_MSG(file.is_open(), "Could not open file for writing.");

                                                 file.write((char*)dataBuffer, dataSize);
                                                 file.close();
                                             });

    return futureObj;
}

inline std::future<void> writeFileAsync(const void* dataBuffer, size_t dataSize, const String& fileName)
{
    return writeFileAsync(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool appendToFile(const String& str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    if(!file.is_open()) {
        return false;
    }

    file << str;
    file.close();
    return true;
}

inline bool appendToFile(const String& str, const String& fileName)
{
    return appendToFile(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool appendToFile(const Vector<String>& vecStr, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    if(!file.is_open()) {
        return false;
    }

    for(auto& str : vecStr) {
        file << str << std::endl;
    }
    file.close();
    return true;
}

inline bool appendToFile(const Vector<String>& vecStr, const String& fileName)
{
    return appendToFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(unsigned char*& dataBuffer, size_t bufferSize, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if(!file.is_open()) {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    if(bufferSize < fileSize) {
        dataBuffer = reinterpret_cast<unsigned char*>(realloc(dataBuffer, fileSize));
    }

    file.seekg(0, std::ios::beg);
    file.read((char*)dataBuffer, fileSize);
    file.close();
    return true;
}

inline bool readFile(unsigned char*& dataBuffer, size_t bufferSize, const String& fileName)
{
    return readFile(dataBuffer, bufferSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(Vector<unsigned char>& dataBuffer, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if(!file.is_open()) {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    dataBuffer.resize(fileSize);

    file.seekg(0, std::ios::beg);
    file.read((char*)dataBuffer.data(), fileSize);
    file.close();
    return true;
}

inline bool readFile(Vector<unsigned char>& dataBuffer, const String& fileName)
{
    return readFile(dataBuffer, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool readFile(Vector<String>& vecStr, const char* fileName)
{
    std::ifstream file(fileName, std::ios::in);
    if(!file.is_open()) {
        return false;
    }

    vecStr.resize(0);
    String line;

    while(std::getline(file, line)) {
        vecStr.push_back(line);
    }

    file.close();
    return true;
}

inline bool readFile(Vector<String>& vecStr, const String& fileName)
{
    return readFile(vecStr, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// template funcs
template<class T>
inline bool writeBinaryFile(const Vector<T>& dvec, const char* fileName)
{
    return writeFile((unsigned char*)dvec.data(), dvec.size() * sizeof(T), fileName);
}

template<class T>
inline bool writeBinaryFile(const Vector<T>& dvec, const String& fileName)
{
    return writeBinaryFile(dvec, fileName.c_str());
}

template<class T>
inline std::future<void> writeBinaryFileAsync(const Vector<T>& dvec, const char* fileName)
{
    return writeFileAsync((unsigned char*)dvec.data(), dvec.size() * sizeof(T), fileName);
}

template<class T>
inline std::future<void> writeBinaryFileAsync(const Vector<T>& dvec, const String& fileName)
{
    return writeBinaryFileAsync(dvec, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::FileHelpers
