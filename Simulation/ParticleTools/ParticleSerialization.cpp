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

#include <ParticleTools/ParticleSerialization.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
String ParticleSerialization::Attribute::typeName()
{
    switch(type) {
        case TypeInt:
            return String("Int");
        case TypeUInt:
            return String("UInt");
        case TypeReal:
            return String("Real");
        case TypeCompressedReal:
            return String("CompressedReal");
        case TypeVectorInt:
            return String("VectorInt");
        case TypeVectorUInt:
            return String("VectorUInt");
        case TypeVectorFloat:
            return String("VectorFloat");
        default:
            __BNN_DIE_UNKNOWN_ERROR
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t ParticleSerialization::Attribute::typeSize()
{
    return static_cast<size_t>(size);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::clearData()
{
    for(auto& attr : m_FixedAttributes) {
        attr.second->buffer.clearBuffer();
        attr.second->bReady = false;
    }
    for(auto& attr : m_ParticleAttributes) {
        attr.second->buffer.clearBuffer();
        attr.second->bReady = false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::flush(Int fileID)
{
    __BNN_ASSERT(m_DataIO != nullptr);
    m_DataIO->createOutputFolders();
    const String fileName = m_DataIO->getFilePath(fileID);
    flush(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::flush(const String& fileName)
{
    waitForBuffers();
    m_WriteFutureObj = std::async(std::launch::async, [&, fileName]()
                                  {
                                      std::ofstream opf(fileName, std::ios::binary | std::ios::out);
                                      if(!opf.is_open()) {
                                          if(m_Logger != nullptr)
                                              m_Logger->printError("Cannot write file: " + fileName);
                                          return;
                                      }

                                      ////////////////////////////////////////////////////////////////////////////////
                                      if(m_Logger != nullptr)
                                          m_Logger->printLog("Saving particle file: " + fileName);

                                      writeHeader(opf);
                                      for(auto& attr : m_FixedAttributes) {
                                          __BNN_ASSERT(attr.second->bReady);
                                          opf.write((char*)attr.second->buffer.data(), attr.second->buffer.size());
                                      }
                                      for(auto& attr : m_ParticleAttributes) {
                                          __BNN_ASSERT(attr.second->bReady);
                                          opf.write((char*)attr.second->buffer.data(), attr.second->buffer.size());
                                      }
                                      opf.close();
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::writeHeader(std::ofstream& opf)
{
    const std::locale& fixLoc = std::locale("C");
    opf.imbue(fixLoc);

    opf << "BananaParticleData\n";
    for(auto& attr : m_FixedAttributes)
        opf << "FixedAttribute " << attr.second->name << " " << attr.second->typeName() << " " << attr.second->typeSize() << " " << attr.second->count << " " << attr.second->buffer.size() << "\n";

    for(auto& attr : m_ParticleAttributes)
        opf << "ParticleAttribute " << attr.second->name << " " << attr.second->typeName() << " " << attr.second->typeSize() << " " << attr.second->count << " " << attr.second->buffer.size() << "\n";

    opf << "NParticles " << m_nParticles << "\n";
    opf << "EndHeader.\n";
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::read(Int fileID, const Vector<String>& readAttributes /*= {}*/)
{
    __BNN_ASSERT(m_DataIO != nullptr);
    const String fileName = m_DataIO->getFilePath(fileID);
    return read(fileName, readAttributes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::read(const String& fileName, const Vector<String>& readAttributes /*= {}*/)
{
    std::ifstream ipf(fileName, std::ios::binary | std::ios::in);
    if(!ipf.is_open()) {
        if(m_Logger != nullptr)
            m_Logger->printError("Cannot read file: " + fileName);
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(readAttributes.size() > 0) {
        // set all to false
        for(auto& kv : m_bReadAttributeMap)
            kv.second = false;

        // only given attributes will be true
        for(auto& attrName : readAttributes)
            m_bReadAttributeMap[attrName] = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(!readHeader(ipf))
        return false;

    size_t cursor = ipf.tellg();
    for(auto& attr : m_FixedAttributes) {
        if(m_bReadAttributeMap[attr.second->name]) {
            bool success = readAttribute(attr.second, ipf, cursor);
            if(!success) return false;
            cursor = ipf.tellg();
        }
        else
            cursor += m_ReadAttributeDataSizeMap[attr.second->name];
    }

    for(auto& attr : m_ParticleAttributes) {
        if(m_bReadAttributeMap[attr.second->name]) {
            bool success = readAttribute(attr.second, ipf, cursor);
            if(!success) return false;
            cursor = ipf.tellg();
        }
        else{
            cursor += m_ReadAttributeDataSizeMap[attr.second->name];
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    ipf.close();
    if(m_Logger != nullptr)
        m_Logger->printLog("Read particle file: " + fileName);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::readHeader(std::ifstream& ipf)
{
    std::string line;
    bool        gotMagic = false;

    auto getType = [&](const String& typeName) -> DataType
                   {
                       if(typeName == "Int")
                           return TypeInt;
                       if(typeName == "UInt")
                           return TypeUInt;
                       if(typeName == "Real")
                           return TypeReal;
                       if(typeName == "CompressedReal")
                           return TypeCompressedReal;
                       if(typeName == "VectorInt")
                           return TypeVectorInt;
                       if(typeName == "VectorUInt")
                           return TypeVectorUInt;
                       if(typeName == "VectorFloat")
                           return TypeVectorFloat;
                       return TypeInt;
                   };

    while(std::getline(ipf, line)) {
        std::istringstream ls(line);
        std::string        token;
        ls >> token;

        if(token == "BananaParticleData")
            gotMagic = true;
        else if(token == "EndHeader.")
            break;
        else if(token == "NParticles")
            ls >> m_nParticles;
        else if(token == "FixedAttribute" || token == "ParticleAttribute") {
            String attrName, typeName;
            Int    typeSize, count;
            size_t dataSize;
            ls >> attrName >> typeName;
            ls >> typeSize >> count;
            ls >> dataSize;
            m_ReadAttributeDataSizeMap[attrName] = dataSize;
            m_bReadAttributeMap[attrName]        = true;

            if(token == "FixedAttribute")
                m_FixedAttributes[attrName] = std::make_shared<Attribute>(attrName, getType(typeName), static_cast<ElementSize>(typeSize), count);
            else
                m_ParticleAttributes[attrName] = std::make_shared<Attribute>(attrName, getType(typeName), static_cast<ElementSize>(typeSize), count);
        }
        else return false;
    }

    return gotMagic;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::readAttribute(SharedPtr<Attribute>& attr, std::ifstream& ipf, size_t cursor)
{
    ipf.seekg(cursor);
    size_t dataSize = m_ReadAttributeDataSizeMap[attr->name];
    attr->buffer.resize(dataSize);
    ipf.read((char*)attr->buffer.data(), dataSize);

    return ipf.good();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
