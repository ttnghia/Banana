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
        m_ParamMap.clear();
    }

    void set_param(std::string key, std::string value)
    {
        m_ParamMap[key] = value;
    }

    bool has_param(const std::string& key)
    {
        return (m_ParamMap.find(key) != m_ParamMap.end());
    }

    bool get_bool(const std::string& key)
    {
        return (m_ParamMap[key] == "true" ||
                m_ParamMap[key] == "True" ||
                m_ParamMap[key] == "TRUE");
    }

    int get_int(const std::string& key)
    {
        return std::stoi(m_ParamMap[key]);
    }

    template<class T>
    T get_real(const std::string& key)
    {
        return (T)std::stod(m_ParamMap[key]);
    }

    size_t get_size_t(const std::string& key)
    {
        return (size_t)std::stoul(m_ParamMap[key]);
    }

private:
    std::map<std::string, std::string> m_ParamMap;
};
