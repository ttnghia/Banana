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
    enum class DataType
    {
        IntType,
        UIntType,
        FloatType,
        DoubleType
    };

    union ScalarValue
    {
        double doubleValue;
        unsigned int uintValue;
    };

    ParticleSystemData() :
        m_NumParticles(0),
        m_MaxNumParticles(0)
    {}

    ~ParticleSystemData()
    {
        clearData();
    }

    void clearData()
    {
        for(auto it = m_ArrayData.begin(); it != m_ArrayData.end(); ++it)
        {
            it->second.clear();
        }

        m_ArrayData.clear();

        m_NumParticles       = 0;
        m_MaxNumParticles    = 0;
    }

    void reserve(unsigned int maxNumParticles)
    {
        m_MaxNumParticles    = maxNumParticles;

        for(auto it = m_ArrayData.begin(); it != m_ArrayData.end(); ++it)
        {
            const std::string& arrName = it->first;
            size_t arraySize = m_MaxNumParticles * m_ArrayElementSize[arrName];
            it->second.resize(arraySize);
        }
    }

    bool hasScalar(std::string dataName)
    {
        return (m_ScalarData.find(dataName) != m_ScalarData.end());
    }

    bool hasArray(std::string dataName)
    {
        return (m_ArrayData.find(dataName) != m_ArrayData.end());
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T, int N>
    void addArray(std::string arrName)
    {
        size_t arraySize = m_MaxNumParticles * sizeof(T) * N;

        if(!hasArray(arrName))
        {
            m_ArrayElementSize[arrName] = sizeof(T) * N;
            m_ArrayData[arrName]        = std::vector<unsigned char>(arraySize);
        }
        else
        {
            m_ArrayData[arrName].resize(arraySize);
        }
    }

    void setNumParticles(unsigned int numParticles)
    {
        m_NumParticles = numParticles;
    }

    template<class T>
    void setParticleRadius(T particleRadius)
    {
        m_ParticleRadius = static_cast<double>(particleRadius);
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    unsigned int getNumParticles()
    {
        return m_NumParticles;
    }
    unsigned int getMaxNumParticles()
    {
        return m_MaxNumParticles;
    }

    template<class T>
    T getParticleRadius()
    {
        return static_cast<T>(m_ParticleRadius);
    }

    unsigned int getUInt(std::string dataName)
    {
        return hasScalar(dataName) ? m_ScalarData[dataName].uintValue : 0;
    }

    unsigned int getDouble(std::string dataName)
    {
        return hasScalar(dataName) ? m_ScalarData[dataName].doubleValue : 0;
    }

    std::vector<unsigned char>& getArray(std::string arrName)
    {
        assert(hasArray(arrName));
        return m_ArrayData[arrName];
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
private:
    double       m_ParticleRadius;
    unsigned int m_NumParticles;
    unsigned int m_MaxNumParticles;

    std::map<std::string, ScalarValue>                 m_ScalarData;
    std::map<std::string, std::vector<unsigned char> > m_ArrayData;
    std::map<std::string, size_t>                      m_ArrayElementSize;
};
