//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
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
#include <vector>
#include <string>
#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSystemData
{
public:
    ParticleSystemData() :
        m_NumParticles(0),
        m_NumActiveParticles(0),
        m_MaxNumParticles(0)
    {}

    ~ParticleSystemData()
    {
        clearData();
    }

    void clearData()
    {
        for(auto it = m_DataArrays.begin(); it != m_DataArrays.end(); ++it)
        {
            it->second.clear();
        }

        m_DataArrays.clear();

        m_NumParticles       = 0;
        m_NumActiveParticles = 0;
        m_MaxNumParticles    = 0;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    unsigned int getNumParticles()
    {
        return m_NumParticles;
    }
    unsigned int getNumActiveParticles()
    {
        return m_NumActiveParticles;
    }
    unsigned int getMaxNumParticles()
    {
        return m_MaxNumParticles;
    }

    bool hasData(std::string dataName)
    {
        return (m_DataArrays.find(dataName) != m_DataArrays.end());
    }

    std::vector<unsigned char>& getParticleData(std::string dataName)
    {
        if(!hasData(dataName))
        {
            m_DataArrays[dataName] = std::vector<unsigned char>(0);
        }

        return m_DataArrays[dataName];
    }

private:
    unsigned int m_NumParticles;
    unsigned int m_NumActiveParticles;
    unsigned int m_MaxNumParticles;

    std::map<std::string, std::vector<unsigned char> > m_DataArrays;
};
