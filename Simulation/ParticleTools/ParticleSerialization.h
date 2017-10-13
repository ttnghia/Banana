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
        TypeInt,
        TypeUInt,
        TypeReal,
        TypeCompressedReal,
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
        Int         count;
        bool        bReady;

        Attribute(const String& name_, DataType type_, ElementSize size_, Int count_ = 1) :
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
    void addFixedAtribute(const String& attrName, DataType type, Int count = 1)
    {
        addFixedAtribute(attrName, type, static_cast<ParticleSerialization::ElementSize>(sizeof(T)), count);
    }

    template<class T>
    void addParticleAtribute(const String& attrName, DataType type, Int count = 1)
    {
        addParticleAtribute(attrName, type, static_cast<ParticleSerialization::ElementSize>(sizeof(T)), count);
    }

    void addFixedAtribute(const String& attrName, DataType type, ElementSize size, Int count = 1)
    {
        __BNN_ASSERT(type == TypeInt || type == TypeReal);
        m_FixedAttributes[attrName] = std::make_shared<Attribute>(attrName, type, size, count);
    }

    void addParticleAtribute(const String& attrName, DataType type, ElementSize size, Int count = 1)
    {
        m_ParticleAttributes[attrName] = std::make_shared<Attribute>(attrName, type, size, count);
    }

    void setNParticles(UInt nParticles) { m_nParticles = nParticles; }

    template<class T> void        setFixedAttribute(const String& attrName, T value);
    template<class T> void        setFixedAttribute(const String& attrName, T* values);
    template<class T> void        setFixedAttribute(const String& attrName, const Vector<T>& values);
    template<Int N, class T> void setFixedAttribute(const String& attrName, const VecX<N, T>& value);
    template<Int N, class T> void setFixedAttribute(const String& attrName, const MatXxX<N, T>& value);

    template<class T> void        setParticleAttribute(const String& attrName, const Vector<T>& values);
    template<class T> void        setParticleAttribute(const String& attrName, const Vector<Vector<T> >& values);
    template<Int N, class T> void setParticleAttribute(const String& attrName, const Vector<VecX<N, T> >& values);
    template<Int N, class T> void setParticleAttribute(const String& attrName, const Vector<MatXxX<N, T> >& values);

    void clearData();
    void flush(Int fileID);
    void flush(const String& fileName);
    void waitForBuffers() { if(m_WriteFutureObj.valid()) { m_WriteFutureObj.wait(); } }

    ////////////////////////////////////////////////////////////////////////////////
    // functions for reading data
    const Map<String, SharedPtr<Attribute> >& getFixedAttributes() { return m_FixedAttributes; }
    const Map<String, SharedPtr<Attribute> >& getParticleAttributes() { return m_ParticleAttributes; }

    Int    getLatestFileIndex(Int maxIndex) const { return m_DataIO->getLatestFileIndex(maxIndex); }
    bool   read(Int fileID, const Vector<String>& readAttributes = {});
    bool   read(const String& fileName, const Vector<String>& readAttributes = {});
    size_t getBytesRead() const { return m_ByteRead; }
    UInt   getNParticles() const { return m_nParticles; }

    template<class T> bool        getFixedAttribute(const String& attrName, T& value);
    template<class T> bool        getFixedAttribute(const String& attrName, T* value);
    template<class T> bool        getFixedAttribute(const String& attrName, Vector<T>& values);
    template<Int N, class T> bool getFixedAttribute(const String& attrName, VecX<N, T>& value);
    template<Int N, class T> bool getFixedAttribute(const String& attrName, MatXxX<N, T>& value);

    template<class T> bool        getParticleAttribute(const String& attrName, T* values);
    template<class T> bool        getParticleAttribute(const String& attrName, Vector<T>& values);
    template<class T> bool        getParticleAttribute(const String& attrName, Vector<Vector<T> >& values);
    template<Int N, class T> bool getParticleAttribute(const String& attrName, Vector<VecX<N, T> >& values);
    template<Int N, class T> bool getParticleAttribute(const String& attrName, Vector<MatXxX<N, T> >& values);

private:
    void buildAttrNameList();
    void writeHeader(std::ofstream& opf);
    bool readHeader(std::ifstream& ipf);
    bool readAttribute(SharedPtr<Attribute>& attr, std::ifstream& ipf, size_t cursor);

    UInt                               m_nParticles;
    Map<String, SharedPtr<Attribute> > m_FixedAttributes;
    Map<String, SharedPtr<Attribute> > m_ParticleAttributes;
    String                             m_AttributeNameList;

    Map<String, size_t> m_ReadAttributeDataSizeMap;
    Map<String, bool>   m_bReadAttributeMap;
    size_t              m_ByteRead;

    SharedPtr<Logger> m_Logger;
    SharedPtr<DataIO> m_DataIO;
    std::future<void> m_WriteFutureObj;

public:
    template<Int N, class T> static void saveParticle(const String& fileName, const Vector<VecX<N, T> >& positions, T particleRadius, bool bCompress = true);
    template<Int N, class T> static bool loadParticle(const String& fileName, Vector<VecX<N, T> >& positions, T particleRadius);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/ParticleSerialization.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
