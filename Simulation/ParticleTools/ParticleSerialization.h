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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Banana Particle Data format =====>
////////////////////////////////////////////////////////////////////////////////
//
// "BananaParticleData"
// "FixedAtributes:" <Num. of fixed attributes>
// "    <FixedAtributeName1>:" <DataType> <DataSize> <Count>
//      ...
//
// "ParticleAtributes:" <Num. of particles attributes>
// "    <ParticleAttribute1>:" <DataType> <DataSize> <Count>
//      ...
//
// "NParticles:" <Num. of particles>
// "EndHeader"
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
        TypeInteger,
        TypeReal,
        TypeCompressedReal,
        TypeVector
    };

    enum DataSize
    {
        Size8,
        Size16,
        Size32,
        Size64
    };

private:
    struct Attribute
    {
        DataBuffer buffer;
        String     name;
        DataType   type;
        DataSize   size;
        Int        count;
        bool       bReady;

        Attribute(const String& name_, DataType type_, DataSize size_, Int count_) : name(name_), type(type_), size(size_), count(count_), bReady(false) {}
        String typeName();
        size_t typeSize();
    };

public:
    ParticleSerialization(const String& dataRootFolder,
                          const String& dataFolder,
                          const String& fileName,
                          const String& dataName) :
        m_DataIO(dataRootFolder, dataFolder, fileName, String("bnn"), dataName)
    {}

    ////////////////////////////////////////////////////////////////////////////////
    void setLogger(const SharedPtr<Logger>& logger) { m_Logger = logger; }
    void setNParticles(UInt nParticles) { m_nParticles = nParticles; }
    void addFixedAtribute(const String& attrName, DataType type, DataSize size, Int count = 1) { m_FixedAttributes.emplace_back(Attribute(attrName, type, size, count)); }
    void addParticleAtribute(const String& attrName, DataType type, DataSize size, Int count = 1) { m_ParticleAttributes.emplace_back(Attribute(attrName, type, size, count)); }

    ////////////////////////////////////////////////////////////////////////////////
    template<class T> void        setFixedAtribute(const String& attrName, T value);
    template<class T> void        setFixedAtribute(const String& attrName, const Vector<T>& values);
    template<Int N, class T> void setFixedAtribute(const String& attrName, const VecX<N, T>& value);

    ////////////////////////////////////////////////////////////////////////////////
    template<class T> void        setParticleAtribute(const String& attrName, const Vector<T>& values);
    template<class T> void        setParticleAtribute(const String& attrName, const Vector<Vector<T> >& values);
    template<Int N, class T> void setParticleAtribute(const String& attrName, const Vector<VecX<N, T> >& values);

    ////////////////////////////////////////////////////////////////////////////////
    void clear();
    void flush(int fileID);

private:
    void waitForBuffers() { if(m_WriteFutureObj.valid()) m_WriteFutureObj.wait(); }
    void writeHeader(std::ofstream& of);

    UInt              m_nParticles;
    Vector<Attribute> m_FixedAttributes;
    Vector<Attribute> m_ParticleAttributes;

    SharedPtr<Logger> m_Logger;
    DataIO            m_DataIO;
    std::future<void> m_WriteFutureObj;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/ParticleSerialization.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
