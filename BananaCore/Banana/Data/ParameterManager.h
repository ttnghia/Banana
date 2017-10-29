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

#include <unordered_map>
#include <string>
#include <cassert>
#include <array>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class KeyType>
class ParameterManager
{
    enum TypeID
    {
        TYPE_INT = 0,
        TYPE_UNSIGNED_INT,
        TYPE_REAL,
        TYPE_VEC3,
        TYPE_VEC4,
        TYPE_POINTER
    };

    union VariantData
    {
        VariantData(void) = default;
        VariantData(void* value) : data_pointer(value) {}
        VariantData(double value) : data_real(value) {}
        VariantData(float value) : data_real(value) {}
        VariantData(int value) : data_int(value) {}
        VariantData(unsigned int value) : data_uint(value) {}

        ////////////////////////////////////////////////////////////////////////////////
        void*        data_pointer;
        double       data_vec[4];
        double       data_real;
        int          data_int;
        unsigned int data_uint;
    };

    struct VariantField
    {
        VariantData data;
        int         typeId;
    };

public:
    ParameterManager()  = default;
    ~ParameterManager() = default;

    void clearParams()
    {
        m_VariantData.clear();
        m_StringData.clear();
    }

    bool hasVariantParam(const KeyType& key)
    {
        return (m_VariantData.find(key) != m_VariantData.end());
    }

    bool hasStringParam(const KeyType& key)
    {
        return (m_StringData.find(key) != m_StringData.end());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void set(const KeyType& key, int value)
    {
        m_VariantData[key] = { value, TYPE_INT };
    }

    void set(const KeyType& key, unsigned int value)
    {
        m_VariantData[key] = { value, TYPE_UNSIGNED_INT };
    }

    void set(const KeyType& key, float value)
    {
        m_VariantData[key] = { static_cast<double>(value), TYPE_REAL };
    }

    void set(const KeyType& key, double value)
    {
        m_VariantData[key] = { value, TYPE_REAL };
    }

    void set(const KeyType& key, void* value)
    {
        m_VariantData[key] = { value, TYPE_POINTER };
    }

    template<class Real>
    void set(const KeyType& key, const Vec3<Real> value)
    {
        VariantField field;
        field.typeId           = TYPE_VEC3;
        field.data.data_vec[0] = static_cast<double>(value[0]);
        field.data.data_vec[1] = static_cast<double>(value[1]);
        field.data.data_vec[2] = static_cast<double>(value[2]);

        m_VariantData[key] = field;
    }

    template<class Real>
    void set(const KeyType& key, const Vec4<Real> value)
    {
        VariantField field;
        field.typeId           = TYPE_VEC4;
        field.data.data_vec[0] = value[0];
        field.data.data_vec[1] = value[1];
        field.data.data_vec[2] = value[2];
        field.data.data_vec[3] = value[3];

        m_VariantData[key] = field;
    }

    void set(const KeyType& key, const std::string& value)
    {
        m_StringData[key] = value;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int getInt(const KeyType& key)
    {
        assert(hasVariantParam(key) || hasStringParam(key));
        if(hasVariantParam(key)) {
            assert(m_VariantData[key].typeId == TYPE_INT);
            return m_VariantData[key].data.data_int;
        } else {
            return stoi(m_StringData[key]);
        }
    }

    unsigned int getUInt(const KeyType& key)
    {
        assert(hasVariantParam(key) || hasStringParam(key));
        if(hasVariantParam(key)) {
            assert(m_VariantData[key].typeId == TYPE_UNSIGNED_INT);
            return m_VariantData[key].data.data_uint;
        } else {
            return static_cast<unsigned int> stoi(m_StringData[key]);
        }
    }

    template<class Real>
    Real getReal(const KeyType& key)
    {
        assert(hasVariantParam(key) || hasStringParam(key));
        if(hasVariantParam(key)) {
            assert(m_VariantData[key].typeId == TYPE_REAL);
            return static_cast<Real>(m_VariantData[key].data.data_real);
        } else {
            return static_cast<Real>(stod(m_StringData[key]));
        }
    }

    template<class Real>
    Vec3<Real> getVec3(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_VEC3);
        Vec3<Real> v;
        v[0] = static_cast<Real>(m_VariantData[key].data.data_vec[0]);
        v[1] = static_cast<Real>(m_VariantData[key].data.data_vec[1]);
        v[2] = static_cast<Real>(m_VariantData[key].data.data_vec[2]);

        return v;
    }

    template<class Real>
    Vec4<Real> getVec4(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_VEC4);
        Vec3<Real> v;
        v[0] = static_cast<Real>(m_VariantData[key].data.data_vec[0]);
        v[1] = static_cast<Real>(m_VariantData[key].data.data_vec[1]);
        v[2] = static_cast<Real>(m_VariantData[key].data.data_vec[2]);
        v[3] = static_cast<Real>(m_VariantData[key].data.data_vec[3]);

        return v;
    }

    void* getVoidPtr(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_POINTER);
        return m_VariantData[key].data.data_pointer;
    }

    const std::string& getString(const KeyType& key)
    {
        assert(hasStringParam(key));
        return m_StringData[key];
    }

private:
    std::unordered_map<KeyType, VariantField> m_VariantData;
    std::unordered_map<KeyType, std::string>  m_StringData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana