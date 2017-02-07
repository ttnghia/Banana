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

#include <BananaAppCore/Macros.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <filesystem>

#ifdef __Banana_Windows__
#include <windows.h>
#endif

namespace FileHelpers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void create_folder(const char* folderName)
{
#ifdef __Banana_Windows__
    CreateDirectory(folderName, NULL);
#else
    char buff[512];
    sprintf(buff, "mkdir -p %s", folderName);
    system(buff);
#endif
}

inline void create_folder(std::string folderName)
{
    create_folder(folderName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool file_existed(const char* fileName)
{
    FILE* file;
#ifdef __Banana_Windows__
    if(fopen_s(&file, fileName, "r"))
#else
    if(FILE* file = fopen(fileName, "r"))
#endif
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

inline bool file_existed(std::string fileName)
{
    return file_existed(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline size_t get_file_size(const char* fileName)
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

inline size_t get_file_size(std::string fileName)
{
    return get_file_size(fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::string get_folder_size(const char* folderName, int level = 0)
{
#ifdef __Banana_Windows__
    __BNN_CalledToUmimplementedFunc
#else
    const int max_buffer = 256;
    char buffer[max_buffer];
    char command[max_buffer];

    if(level == 0)
    {
        sprintf(command, "du -h -d0 %s | cut -f1", folderName);
    }
    else
    {
        sprintf(command, "du -h -d%d %s", level, folderName);
    }

    FILE* stream = _popen(command, "r");
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
            if(fgets(buffer, max_buffer, stream) != nullptr)
            {
                output.append(buffer);
            }
        }

        pclose(stream);
    }

    return output;
#endif
}

inline std::string get_folder_size(std::string folderName, int level = 0)
{
    return get_folder_size(folderName.c_str(), level);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Windows__
#   define sprintf sprintf_s
#endif
inline std::string get_file_name(std::string topFolder, std::string dataSubFolder,
                                 std::string fileName, std::string fileExtension,
                                 int fileID)
{
    char buff[512];
    sprintf(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder.c_str(),
            fileName.c_str(), fileID, fileExtension.c_str());
    return std::string(buff);
}

inline std::string get_file_name(const char* topFolder, const char* dataSubFolder,
                                 const char* fileName, const char* fileExtension,
                                 int fileID)
{
    char buff[512];
    sprintf(buff, "%s/%s/%s.%04d.%s", topFolder, dataSubFolder, fileName, fileID,
            fileExtension);

    return std::string(buff);
}

inline std::string get_file_name(std::string topFolder, const char* dataSubFolder,
                                 const char* fileName, const char* fileExtension,
                                 int fileID)
{
    char buff[512];
    sprintf(buff, "%s/%s/%s.%04d.%s", topFolder.c_str(), dataSubFolder, fileName, fileID,
            fileExtension);

    return std::string(buff);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writte_file(std::string str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    file << str;
    file.close();
}

inline void writte_file(std::string str, std::string fileName)
{
    writte_file(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writte_file(const std::vector<std::string>& vec_str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    for(size_t i = 0; i < vec_str.size(); ++i)
    {
        file << vec_str[i] << std::endl;
    }

    file.close();
}

inline void writte_file(const std::vector<std::string>& vec_str,
                        std::string fileName)
{
    writte_file(vec_str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void writte_file(const unsigned char* dataBuffer, size_t dataSize, const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    file.write((char*)dataBuffer, dataSize);
    file.close();
}

inline void writte_file(const unsigned char* dataBuffer, size_t dataSize, std::string fileName)
{
    writte_file(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::future<void> writte_file_async(const unsigned char* dataBuffer, size_t dataSize,
                                           const char* fileName)
{
    std::future<void> futureObj = std::async(std::launch::async, [&]()
    {
        std::ofstream file(fileName, std::ios::binary | std::ios::out);
        __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

        file.write((char*)dataBuffer, dataSize);
        file.close();
    });

    return futureObj;
}

inline std::future<void> writte_file_async(const unsigned char* dataBuffer, size_t dataSize,
                                           std::string fileName)
{
    return writte_file_async(dataBuffer, dataSize, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void append_file(std::string str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    file << str;
    file.close();
}

inline void append_file(std::string str, std::string fileName)
{
    append_file(str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void append_file(const std::vector<std::string>& vec_str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    for(size_t i = 0; i < vec_str.size(); ++i)
    {
        file << vec_str[i] << std::endl;
    }

    file.close();
}


inline void append_file(const std::vector<std::string>& vec_str, std::string fileName)
{
    append_file(vec_str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool read_file(unsigned char* dataBuffer, size_t bufferSize, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    if(bufferSize < fileSize)
    {
        delete[] dataBuffer;
        dataBuffer = new unsigned char[fileSize];
    }

    file.seekg(0, std::ios::beg);
    file.read((char*)dataBuffer, fileSize);
    file.close();

    return true;
}

inline bool read_file(unsigned char* dataBuffer, std::string fileName)
{
    return read_file(dataBuffer, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool read_file(std::vector<unsigned char>& dataBuffer, const char* fileName)
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

inline bool read_file(std::vector<unsigned char>& dataBuffer, std::string fileName)
{
    return read_file(dataBuffer, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline bool read_file(std::vector<std::string>& vec_str, const char* fileName)
{
    std::ifstream file(fileName, std::ios::in);

    if(!file.is_open())
    {
        return false;
    }

    vec_str.resize(0);
    std::string line;

    while(std::getline(file, line))
    {
        vec_str.push_back(line);
    }

    file.close();

    return true;
}

inline bool read_file(std::vector<std::string>& vec_str, std::string fileName)
{
    return read_file(vec_str, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// template funcs
template<class T>
inline void write_binary_file(const std::vector<T>& dvec, const char* fileName)
{
    writte_file((unsigned char*)dvec.data(), fileName);
}

template<class T>
inline void write_binary_file(const std::vector<T>& data, std::string fileName)
{
    write_binary_file(data, fileName.c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace FileHelpers