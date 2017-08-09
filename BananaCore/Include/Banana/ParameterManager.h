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

#include <unordered_map>
#include <string>
#include <cassert>

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
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_POINTER
    };

    union VariantData
    {
        VariantData(void) = default;
        VariantData(void* value) : data_pointer(value) {}
        VariantData(double value) : data_double(value) {}
        VariantData(float value) : data_float(value) {}
        VariantData(int value) : data_int(value) {}
        VariantData(unsigned int value) : data_uint(value) {}

        ////////////////////////////////////////////////////////////////////////////////
        void*        data_pointer;
        double       data_double;
        float        data_float;
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
    void setInt(const KeyType& key, int value)
    {
        m_VariantData[key] = { value, TYPE_INT };
    }

    void setUInt(const KeyType& key, unsigned int value)
    {
        m_VariantData[key] = { value, TYPE_UNSIGNED_INT };
    }

    void setFloat(const KeyType& key, float value)
    {
        m_VariantData[key] = { value, TYPE_FLOAT};
    }

    void setDouble(const KeyType& key, double value)
    {
        m_VariantData[key] = { value, TYPE_DOUBLE };
    }

    void setVoidPtr(const KeyType& key, void* value)
    {
        m_VariantData[key] = { value, TYPE_POINTER };
    }

    void setString(const KeyType& key, const std::string& value)
    {
        m_StringData[key] = value;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int getInt(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_INT);
        return m_VariantData[key].data.data_int;
    }

    unsigned int getUInt(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_UNSIGNED_INT);
        return m_VariantData[key].data.data_uint;
    }

    float getFloat(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_FLOAT);
        return m_VariantData[key].data.data_float;
    }

    double getDouble(const KeyType& key)
    {
        assert(hasVariantParam(key));
        assert(m_VariantData[key].typeId == TYPE_DOUBLE);
        return m_VariantData[key].data.data_double;
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