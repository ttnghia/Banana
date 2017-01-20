//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <map>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OptionalParameter
{
public:
    OptionalParameter()
    {}
    ~OptionalParameter()
    {
        paramMap.clear();
    }

    void set_param(std::string key, std::string value)
    {
        paramMap[key] = value;
    }

    bool has_param(const std::string& key)
    {
        return (paramMap.find(key) != paramMap.end());
    }

    bool get_bool(const std::string& key)
    {
        return (paramMap[key] == "true" ||
                paramMap[key] == "True" ||
                paramMap[key] == "TRUE");
    }

    int get_int(const std::string& key)
    {
        return std::stoi(paramMap[key]);
    }

    template<class T>
    T get_real(const std::string& key)
    {
        return (T)std::stod(paramMap[key]);
    }

    size_t get_size_t(const std::string& key)
    {
        return (size_t)std::stoul(paramMap[key]);
    }

private:
    std::map<std::string, std::string> paramMap;
};
