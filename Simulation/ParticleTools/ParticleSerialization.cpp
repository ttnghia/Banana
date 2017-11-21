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

#include <ParticleTools/ParticleSerialization.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
String ParticleSerialization::Attribute::typeName()
{
    switch(type) {
        case TypeInt:
            return String("int");
        case TypeUInt:
            return String("uint");
        case TypeReal:
            return String("real");
        case TypeCompressedReal:
            return String("compressed_real");
        case TypeVectorInt:
            return String("vector_int");
        case TypeVectorUInt:
            return String("vector_uint");
        case TypeVectorReal:
            return String("vector_real");
        case TypeVectorCompressedReal:
            return String("vector_compressed_real");
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
    m_nParticles = 0;

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
void ParticleSerialization::flushAsync(Int fileID)
{
    __BNN_ASSERT(m_DataIO != nullptr);
    m_DataIO->createOutputFolders();
    flushAsync(m_DataIO->getFilePath(fileID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::flushAsync(const String& fileName)
{
    __BNN_ASSERT(m_nParticles > 0 && m_FixedAttributes.size() > 0);
    if(m_Logger != nullptr) {
        buildAttrNameList();
        String str = String("Saving file: "); str += fileName;
        str += String(" ("); str += NumberHelpers::formatWithCommas(static_cast<double>(computeBufferSize()) / 1048576.0); str += String(" MBs)");
        m_Logger->printLog(str);
        str = String("File data: "); str += m_AttributeNameList;
        m_Logger->printLogIndent(str);
    }

    waitForBuffers();
    m_WriteFutureObj = std::async(std::launch::async, [&, fileName]()
                                  {
                                      std::ofstream opf(fileName, std::ios::binary | std::ios::out);
                                      if(!opf.is_open()) {
                                          if(m_Logger != nullptr) {
                                              m_Logger->printError("Cannot write file: " + fileName);
                                          }
                                          return;
                                      }

                                      ////////////////////////////////////////////////////////////////////////////////
                                      writeHeader(opf);
                                      for(auto& kv : m_FixedAttributes) {
                                          __BNN_ASSERT(kv.second->bReady);
                                          opf.write((char*)kv.second->buffer.data(), kv.second->buffer.size());
                                      }
                                      for(auto& kv : m_ParticleAttributes) {
                                          __BNN_ASSERT(kv.second->bReady);
                                          opf.write((char*)kv.second->buffer.data(), kv.second->buffer.size());
                                      }
                                      opf.close();
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t ParticleSerialization::computeBufferSize()
{
    size_t totalSize = 0;
    for(auto& kv : m_FixedAttributes) {
        totalSize += kv.second->buffer.size();
    }
    for(auto& kv : m_ParticleAttributes) {
        totalSize += kv.second->buffer.size();
    }

    return totalSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::buildAttrNameList()
{
    if(m_AttributeNameList.empty()) {
        for(auto& kv : m_FixedAttributes) {
            __BNN_ASSERT(kv.second->bReady);
            m_AttributeNameList += kv.first;
            m_AttributeNameList += String(", ");
        }
        for(auto& kv : m_ParticleAttributes) {
            __BNN_ASSERT(kv.second->bReady);
            m_AttributeNameList += kv.first;
            m_AttributeNameList += String(", ");
        }
        ////////////////////////////////////////////////////////////////////////////////
        m_AttributeNameList.erase(m_AttributeNameList.find_last_of(","), m_AttributeNameList.size()); // remove last ',' character
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::writeHeader(std::ofstream& opf)
{
    const std::locale& fixLoc = std::locale("C");
    opf.imbue(fixLoc);

    opf << "BananaParticleData\n";
    for(auto& kv : m_FixedAttributes) {
        if(kv.second->bReady) {
            opf << "FixedAttribute " << kv.second->name << " " << kv.second->typeName() << " " << kv.second->typeSize() << " " << kv.second->count << " " << kv.second->buffer.size() << "\n";
        }
    }

    for(auto& kv : m_ParticleAttributes) {
        if(kv.second->bReady) {
            opf << "ParticleAttribute " << kv.second->name << " " << kv.second->typeName() << " " << kv.second->typeSize() << " " << kv.second->count << " " << kv.second->buffer.size() << "\n";
        }
    }

    opf << "NParticles " << m_nParticles << "\n";
    opf << "EndHeader.\n";
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::read(Int fileID, const Vector<String>& readAttributes /*= {}*/, bool bStopIfFailed /*= true*/)
{
    __BNN_ASSERT(m_DataIO != nullptr);
    const String fileName = m_DataIO->getFilePath(fileID);
    return read(fileName, readAttributes, bStopIfFailed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::read(const String& fileName, const Vector<String>& readAttributes /*= {}*/, bool bStopIfFailed /*= true*/)
{
    std::ifstream ipf(fileName, std::ios::binary | std::ios::in);
    if(!ipf.is_open()) {
        if(m_Logger != nullptr) {
            m_Logger->printError("Cannot read file: " + fileName);
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_ByteRead = 0;
    m_ReadAttributeDataSizeMap.clear();
    m_bReadAttributeMap.clear();
    if(!readHeader(ipf)) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(readAttributes.size() > 0) {
        // set all to false
        for(auto& kv : m_bReadAttributeMap) {
            kv.second = false;
        }

        // only given attributes will be true
        for(auto& attrName : readAttributes) {
            m_bReadAttributeMap[attrName] = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    size_t cursor = ipf.tellg();
    for(auto& kv : m_FixedAttributes) {
        if(m_bReadAttributeMap[kv.second->name]) {
            bool success = readAttribute(kv.second, ipf, cursor);
            if(!success && bStopIfFailed) { return false; }
            cursor = ipf.tellg();
        } else {
            size_t attrDataSize = m_ReadAttributeDataSizeMap[kv.second->name];
            cursor += attrDataSize;
        }
    }

    for(auto& kv : m_ParticleAttributes) {
        if(m_bReadAttributeMap[kv.second->name]) {
            bool success = readAttribute(kv.second, ipf, cursor);
            if(!success && bStopIfFailed) { return false; }
            cursor = ipf.tellg();
        } else {
            size_t attrDataSize = m_ReadAttributeDataSizeMap[kv.second->name];
            cursor += attrDataSize;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    ipf.close();
    if(m_Logger != nullptr) {
        String str = String("Read file: "); str += fileName;
        str += String(" ("); str += NumberHelpers::formatWithCommas(static_cast<double>(m_ByteRead) / 1048576.0); str += String(" MBs)");
        m_Logger->printLog(str);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::readHeader(std::ifstream& ipf)
{
    String line;
    bool   gotMagic = false;

    auto getType = [&](const String& typeName) -> DataType
                   {
                       if(typeName == "int") {
                           return TypeInt;
                       }
                       if(typeName == "uint") {
                           return TypeUInt;
                       }
                       if(typeName == "real") {
                           return TypeReal;
                       }
                       if(typeName == "compressed_real") {
                           return TypeCompressedReal;
                       }
                       if(typeName == "vector_int") {
                           return TypeVectorInt;
                       }
                       if(typeName == "vector_uint") {
                           return TypeVectorUInt;
                       }
                       if(typeName == "vector_real") {
                           return TypeVectorReal;
                       }
                       if(typeName == "vector_compressed_real") {
                           return TypeVectorCompressedReal;
                       }
                       return TypeInt;
                   };

    while(std::getline(ipf, line)) {
        std::istringstream ls(line);
        String             token;
        ls >> token;

        if(token == "BananaParticleData") {
            gotMagic = true;
        } else if(token == "EndHeader.") {
            break;
        } else if(token == "NParticles") {
            ls >> m_nParticles;
        } else if(token == "FixedAttribute" || token == "ParticleAttribute") {
            String attrName, typeName;
            Int    typeSize;
            UInt   count;
            size_t dataSize;
            ls >> attrName >> typeName;
            ls >> typeSize >> count;
            ls >> dataSize;
            m_ReadAttributeDataSizeMap[attrName] = dataSize;
            m_bReadAttributeMap[attrName]        = true;

            if(token == "FixedAttribute") {
                m_FixedAttributes[attrName] = std::make_shared<Attribute>(attrName, getType(typeName), static_cast<ElementSize>(typeSize), count);
            } else {
                m_ParticleAttributes[attrName] = std::make_shared<Attribute>(attrName, getType(typeName), static_cast<ElementSize>(typeSize), count);
            }
        } else { return false; }
    }

    m_ByteRead = ipf.tellg();
    return gotMagic;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleSerialization::readAttribute(SharedPtr<Attribute>& attr, std::ifstream& ipf, size_t cursor)
{
    if(m_ReadAttributeDataSizeMap.find(attr->name) == m_ReadAttributeDataSizeMap.end()) {
        return false;
    }
    size_t dataSize = m_ReadAttributeDataSizeMap[attr->name];
    ipf.seekg(cursor);
    attr->buffer.resize(dataSize);
    ipf.read((char*)attr->buffer.data(), dataSize);
    m_ByteRead += dataSize;

    return ipf.good();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
