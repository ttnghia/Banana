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
#include <Banana/Data/DataIO.h>
#include <Banana/Utils/Logger.h>
#include <ParticleTools/ParticleHelpers.h>

#include <map>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Banana Particle Data format =====>
////////////////////////////////////////////////////////////////////////////////
//
// "BananaParticleData"
// "<FixedAtributeName1>:" <DataType> <ElementSize> <Count> <DataSize>
//      ...
//
// "<ParticleAttribute1>:" <DataType> <ElementSize> <Count> <DataSize>
//      ...
//
// "NParticles:" <Num. of particles>
// "EndHeader."
// <DataOfParticleAttribute1>
// <DataOfParticleAttribute2>
// ...
//
////////////////////////////////////////////////////////////////////////////////
// =====> Banana Particle Data format
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSerialization
{
public:
    enum DataType
    {
        TypeChar,
        TypeUInt16,
        TypeInt,
        TypeUInt,
        TypeReal,
        TypeCompressedReal,
        TypeVectorChar,
        TypeVectorInt,
        TypeVectorUInt,
        TypeVectorReal,
        TypeVectorCompressedReal
    };

    enum ElementSize
    {
        Size8b  = sizeof(char),
        Size16b = sizeof(UInt16),
        Size32b = sizeof(float),
        Size64b = sizeof(double)
    };

    struct Attribute
    {
        DataBuffer  buffer;
        String      name;
        DataType    type;
        ElementSize size;
        UInt        count;
        bool        bReady;

        Attribute(const String& name_, DataType type_, ElementSize size_, UInt count_ = 1) :
            name(name_), type(type_), size(size_), count(count_), bReady(false) {}
        String typeName();
        size_t typeSize();
    };

public:
    ParticleSerialization() = default;
    ParticleSerialization(const String&     dataRootFolder,
                          const String&     dataFolder,
                          const String&     fileName,
                          SharedPtr<Logger> logger = nullptr) :
        m_DataIO(std::make_shared<DataIO>(dataRootFolder, dataFolder, fileName, String("bnn"), String("BananaParticleData"))), m_Logger(logger)
    {}

    virtual ~ParticleSerialization() { waitForBuffers(); }

    void setDataPath(const String& dataRootFolder,
                     const String& dataFolder,
                     const String& fileName)
    {
        m_DataIO = std::make_shared<DataIO>(dataRootFolder, dataFolder, fileName, String("bnn"), String("BananaParticleData"));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // functions for writing data
    template<class T>
    void addFixedAttribute(const String& attrName, DataType type, UInt count = 1)
    {
        addFixedAttribute(attrName, type, static_cast<ParticleSerialization::ElementSize>(sizeof(T)), count);
    }

    template<class T>
    void addParticleAttribute(const String& attrName, DataType type, UInt count = 1)
    {
        addParticleAttribute(attrName, type, static_cast<ParticleSerialization::ElementSize>(sizeof(T)), count);
    }

    void addFixedAttribute(const String& attrName, DataType type, ElementSize size, UInt count = 1)
    {
        __BNN_REQUIRE(type == TypeChar || type == TypeInt || type == TypeUInt || type == TypeReal);
        m_FixedAttributes[attrName] = std::make_shared<Attribute>(attrName, type, size, count);
    }

    void addParticleAttribute(const String& attrName, DataType type, ElementSize size, UInt count = 1)
    {
        m_ParticleAttributes[attrName] = std::make_shared<Attribute>(attrName, type, size, count);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IndexType> void setNParticles(IndexType nParticles) { m_nParticles = static_cast<UInt>(nParticles); }

    template<class T> void        setFixedAttribute(const String& attrName, T value);
    template<class T> void        setFixedAttribute(const String& attrName, T* values);
    template<class T> void        setFixedAttribute(const String& attrName, const Vector<T>& values);
    template<Int N, class T> void setFixedAttribute(const String& attrName, const VecX<N, T>& value);
    template<Int N, class T> void setFixedAttribute(const String& attrName, const MatXxX<N, T>& value);

    template<class T> void        setParticleAttribute(const String& attrName, const Vector<T>& values);
    template<class T> void        setParticleAttribute(const String& attrName, const Vector<Vector<T>>& values);
    template<Int N, class T> void setParticleAttribute(const String& attrName, const Vector<VecX<N, T>>& values);
    template<Int N, class T> void setParticleAttribute(const String& attrName, const Vector<MatXxX<N, T>>& values);

    void clearData();
    void flushAsync(Int fileID);
    void flushAsync(const String& fileName);
    void waitForBuffers() { if(m_WriteFutureObj.valid()) { m_WriteFutureObj.wait(); } }

    ////////////////////////////////////////////////////////////////////////////////
    // functions for reading data
    auto&       getFixedAttributes() { return m_FixedAttributes; }
    const auto& getFixedAttributes() const { return m_FixedAttributes; }
    auto&       getParticleAttributes() { return m_ParticleAttributes; }
    const auto& getParticleAttributes() const { return m_ParticleAttributes; }

    Int    getLatestFileIndex(Int maxIndex) const { return m_DataIO->getLatestFileIndex(maxIndex); }
    String getFilePath(Int fileID) { return m_DataIO->getFilePath(fileID); }
    bool   read(Int fileID, const Vector<String>& readAttributes = {}, bool bStopIfFailed = true);
    bool   read(const String& fileName, const Vector<String>& readAttributes = {}, bool bStopIfFailed = true);
    bool   readHeader(Int fileID, const Vector<String>& readAttributes = {}, bool bStopIfFailed = true);
    bool   readHeader(const String& fileName, const Vector<String>& readAttributes = {}, bool bStopIfFailed = true);
    size_t getBytesRead() const { return m_ByteRead; }
    UInt   getNParticles() const { return m_nParticles; }

    bool hasFixedAttribute(const String& attrName) const { return m_FixedAttributes.find(attrName) != m_FixedAttributes.end(); }
    bool hasParticleAttribute(const String& attrName) const { return m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end(); }
    bool hasAttribute(const String& attrName) const { return hasFixedAttribute(attrName) || hasParticleAttribute(attrName); }

    template<class T> bool        getFixedAttribute(const String& attrName, T& value);
    template<class T> bool        getFixedAttribute(const String& attrName, T* value);
    template<class T> bool        getFixedAttribute(const String& attrName, Vector<T>& values);
    template<Int N, class T> bool getFixedAttribute(const String& attrName, VecX<N, T>& value);
    template<Int N, class T> bool getFixedAttribute(const String& attrName, MatXxX<N, T>& value);

    template<class T> bool        getParticleAttribute(const String& attrName, T* values);
    template<class T> bool        getParticleAttribute(const String& attrName, Vector<T>& values);
    template<class T> bool        getParticleAttribute(const String& attrName, Vector<Vector<T>>& values);
    template<Int N, class T> bool getParticleAttribute(const String& attrName, Vector<VecX<N, T>>& values);
    template<Int N, class T> bool getParticleAttribute(const String& attrName, Vector<MatXxX<N, T>>& values);

    template<class T> bool        getParticleAttributeCompressed(const String& attrName, Vec_UInt16& values, T& dMin, T& dMax);
    template<Int N, class T> bool getParticleAttributeCompressed(const String& attrName, Vec_UInt16& values, VecX<N, T>& dMin, VecX<N, T>& dMax);
    template<class T> bool        getParticleAttributeCompressed(const String& attrName, Vec_VecUInt16& values, Vector<T>& dMin, Vector<T>& dMax);

private:
    size_t computeBufferSize();
    void   buildAttrNameList();
    void   writeHeader(std::ofstream& opf);
    bool   readHeader(std::ifstream& ipf);
    bool   readAttribute(SharedPtr<Attribute>& attr, std::ifstream& ipf, size_t cursor);

    UInt                              m_nParticles;
    Map<String, SharedPtr<Attribute>> m_FixedAttributes;
    Map<String, SharedPtr<Attribute>> m_ParticleAttributes;
    String                            m_AttributeNameList;

    Map<String, size_t> m_ReadAttributeDataSizeMap;
    Map<String, bool>   m_bReadAttributeMap;
    size_t              m_ByteRead;

    SharedPtr<Logger> m_Logger;
    SharedPtr<DataIO> m_DataIO;
    std::future<void> m_WriteFutureObj;

public:
    template<Int N, class T> static void saveParticle(const String& fileName, const Vector<VecX<N, T>>& positions, T particleRadius, bool bCompress = true);
    template<Int N, class T> static bool loadParticle(const String& fileName, Vector<VecX<N, T>>& positions, T particleRadius);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/ParticleSerialization.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
