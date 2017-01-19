//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/18/2017
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
//------------------------------------------------------------------------------------------

#pragma once

#include <BananaAppCore/Macros.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <future>
#include <filesystem>

#ifdef __Banana_Windows__
#include <windows.h>
#endif

namespace FileHelpers
{
//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
inline bool file_existed(const char* fileName)
{
    if(FILE* file = fopen(fileName, "r"))
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

//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------
inline void writte_file(char* dataBuffer, size_t dataSize, std::string fileName)
{
    writte_file(dataBuffer, dataSize, fileName.c_str());
}

inline void writte_file(char* dataBuffer, size_t dataSize, const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    file.write(dataBuffer, dataSize);
    file.close();
}

//------------------------------------------------------------------------------------------
std::future<void> writte_file_async(char* dataBuffer, size_t dataSize,
                                    std::string fileName)
{
    return writte_file_async(dataBuffer, dataSize, fileName.c_str());
}

std::future<void> writte_file_async(char* dataBuffer, size_t dataSize,
                                    const char* fileName)
{
    std::future<void> futureObj = std::async(std::launch::async, [&]()
    {
        std::ofstream file(fileName, std::ios::binary | std::ios::out);
        __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

        file.write(dataBuffer, dataSize);
        file.close();
    });

    return futureObj;
}

//------------------------------------------------------------------------------------------
inline void append_file(std::string str, std::string fileName)
{
    append_file(str, fileName.c_str());
}

inline void append_file(std::string str, const char* fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ofstream::app);
    __BNN_AssertMsg(file.is_open(), "Could not open file for writing.");

    file << str;
    file.close();
}

//------------------------------------------------------------------------------------------
inline void append_file(const std::vector<std::string>& vec_str,
                        std::string fileName)
{
    append_file(vec_str, fileName.c_str());
}

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


//------------------------------------------------------------------------------------------
inline bool read_file(char* dataBuffer, std::string fileName)
{
    return read_file(dataBuffer, fileName.c_str());
}

inline bool read_file(char* dataBuffer, const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    //    check_file_existed(fileName);

    if(!file.is_open())
    {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();
    dataBuffer.resize(fileSize);

    file.seekg(0, std::ios::beg);
    file.read(dataBuffer.buffer.data(), fileSize);
    file.close();

    return true;
}

//------------------------------------------------------------------------------------------
inline bool read_file(std::vector<std::string>& vec_str, std::string fileName)
{
    return read_file(vec_str, fileName.c_str());
}

inline bool read_file(std::vector<std::string>& vec_str, const char* fileName)
{
    std::ifstream file(fileName, std::ios::in);
    //    check_file_existed(fileName);

    if(!file.is_open())
    {
        return false;
    }

    vec_str.clear();
    std::string line;

    while(std::getline(file, line))
    {
        vec_str.push_back(line);
    }

    file.close();

    return true;
}


/////////////////////////////////////////////////////////////////////////////////
// template funcs

template<class T>
inline void write_binary_file(const std::vector<T>& data, std::string fileName)
{
    write_binary_file(data, fileName.c_str());
}

template<class T>
inline void write_binary_file(const std::vector<T>& data, const char* fileName)
{
    DataBuffer buffer;
    buffer.clear();

    buffer.push_back_data(data);

    writte_file(buffer, fileName);
}
//------------------------------------------------------------------------------------------
} // end namespace FileHelpers