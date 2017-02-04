﻿//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <random>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
inline T generate_random_int(T start, T end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(start, end);

    return dis(gen);
}

template<class T>
inline T generate_random_real(T start, T end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dis(start, end);

    return dis(gen);
}

template<class T>
inline T lerp(T a, T b, T t)
{
    return static_cast<T>(a + t * (b - a));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
        m_MaxNumParticles(0),
        m_ParticleRadius(0)
    {}

    ~ParticleSystemData()
    {
        clearData();
    }

    void clearData()
    {
        for(auto it = m_ArrayData.begin(); it != m_ArrayData.end(); ++it)
        {
            it->second->clear();
        }

        m_ArrayData.clear();

        m_NumParticles    = 0;
        m_MaxNumParticles = 0;
    }

    void resize(unsigned int numParticles)
    {
        m_NumParticles = numParticles;
        if(m_MaxNumParticles < numParticles)
        {
            m_MaxNumParticles = numParticles;
        }

        for(auto it = m_ArrayData.begin(); it != m_ArrayData.end(); ++it)
        {
            const std::string& arrName = it->first;
            size_t arraySize = m_NumParticles * m_ArrayElementSize[arrName];
            it->second->resize(arraySize);
        }
    }

    void reserve(unsigned int maxNumParticles)
    {
        m_MaxNumParticles = maxNumParticles;

        for(auto it = m_ArrayData.begin(); it != m_ArrayData.end(); ++it)
        {
            const std::string& arrName = it->first;
            size_t arraySize = m_MaxNumParticles * m_ArrayElementSize[arrName];
            it->second->reserve(arraySize);
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

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T, int N>
    void addArray(std::string arrName, bool iniZero = false)
    {
        size_t arraySize    = m_NumParticles * sizeof(T) * N;
        size_t maxArraySize = m_MaxNumParticles * sizeof(T) * N;

        if(!hasArray(arrName))
        {
            m_ArrayElementSize[arrName] = sizeof(T) * N;
            m_ArrayData[arrName]        = new std::vector<unsigned char>;
            m_ArrayData[arrName]->reserve(maxArraySize);
            m_ArrayData[arrName]->resize(arraySize);
        }
        else
        {
            m_ArrayData[arrName]->reserve(maxArraySize);
            m_ArrayData[arrName]->resize(arraySize);
        }

        if(iniZero)
        {
            T* dataPtr = reinterpret_cast<T*>(getArray(arrName)->data());

            for(unsigned int i = 0; i < m_MaxNumParticles * N; ++i)
            {
                dataPtr[i] = T(0);
            }
        }
    }

    void setNumParticles(unsigned int numParticles)
    {
        resize(numParticles);
    }

    template<class T>
    void setParticleRadius(T particleRadius)
    {
        m_ParticleRadius = static_cast<double>(particleRadius);
    }

    void setUInt(std::string dataName, unsigned int value)
    {
        ScalarValue sValue;
        sValue.uintValue = value;
        m_ScalarData[dataName] = sValue;
    }

    void setDouble(std::string dataName, double value)
    {
        ScalarValue sValue;
        sValue.doubleValue = value;
        m_ScalarData[dataName] = sValue;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

    double getDouble(std::string dataName)
    {
        return hasScalar(dataName) ? m_ScalarData[dataName].doubleValue : 0;
    }

    std::vector<unsigned char>* getArray(std::string arrName)
    {
        assert(hasArray(arrName));
        return m_ArrayData[arrName];
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    template<class T, int N>
    void generateRandomIntData(std::string dataName,
                               T minVal = 0,
                               T maxVal = std::numeric_limits<T>::max())
    {
        addArray<T, N>(dataName);

        T* dataPtr = reinterpret_cast<T*>(getArray(dataName)->data());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dis(minVal, maxVal);

        for(unsigned int i = 0; i < m_NumParticles * N; ++i)
        {
            dataPtr[i] = dis(gen);
        }
    }

    template<class T, int N>
    void generateRandomRealData(std::string dataName,
                                T minVal = 0,
                                T maxVal = std::numeric_limits<T>::max())
    {
        addArray<T, N>(dataName);

        T* dataPtr = reinterpret_cast<T*>(getArray(dataName)->data());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> dis(minVal, maxVal);

        for(unsigned int i = 0; i < m_NumParticles * N; ++i)
        {
            dataPtr[i] = dis(gen);
        }
    }

    template<class T, class S, int N>
    void generateRampData(std::string dataName,
                          std::vector<S> rangeVals)
    {
        addArray<T, N>(dataName);

        T* dataPtr = reinterpret_cast<T*>(getArray(dataName)->data());
        T segmentSize = static_cast<T>(m_NumParticles) / static_cast<T>(rangeVals.size() - 1);

        for(unsigned int i = 0; i < m_NumParticles; ++i)
        {
            int segment = static_cast<int>(floor(static_cast<float>(i)) / segmentSize);
            assert(segment < rangeVals.size() - 1);

            T t = static_cast<T>(i - segmentSize * segment) / segmentSize;
            S startVal = rangeVals[segment];
            S endVal = rangeVals[segment + 1];

            for(int j = 0; j < N; ++j)
            {
                dataPtr[N * i + j] = lerp(startVal[j], endVal[j], t);
            }
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
private:
    double       m_ParticleRadius;
    unsigned int m_NumParticles;
    unsigned int m_MaxNumParticles;

    std::map<std::string, ScalarValue>                  m_ScalarData;
    std::map<std::string, std::vector<unsigned char>* > m_ArrayData;
    std::map<std::string, size_t>                       m_ArrayElementSize;
};
