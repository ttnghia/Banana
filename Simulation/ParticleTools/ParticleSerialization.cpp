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
        case TypeInteger:
            return String("Integer");
        case TypeReal:
            return String("Real");
        case TypeCompressedReal:
            return String("CompressedReal");
        default:
            __BNN_DIE_UNKNOWN_ERROR
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t ParticleSerialization::Attribute::typeSize()
{
    switch(size) {
        case Size8:
            return 8;
        case Size16:
            return 16;
        case Size32:
            return 32;
        case Size64:
            return 64;
        default:
            return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::clear()
{
    for(auto& attr : m_FixedAttributes) {
        attr.buffer.clearBuffer();
        attr.bReady = false;
    }
    for(auto& attr : m_ParticleAttributes) {
        attr.buffer.clearBuffer();
        attr.bReady = false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::flush(int fileID)
{
    waitForBuffers();
    m_WriteFutureObj = std::async(std::launch::async, [&]()
                                  {
                                      const String fileName = m_DataIO.getFilePath(fileID);
                                      std::ofstream of(fileName, std::ios::binary | std::ios::out);
                                      if(!of.is_open()) {
                                          if(m_Logger != nullptr)
                                              m_Logger->printError("Cannot open file for writing!");
                                          return;
                                      }

                                      if(m_Logger != nullptr)
                                          m_Logger->printLog("Saving particle file: " + fileName);

                                      writeHeader(of);
                                      for(auto& attr : m_FixedAttributes) {
                                          __BNN_ASSERT(attr.bReady);
                                          of.write((char*)attr.buffer.data(), attr.buffer.size());
                                      }
                                      for(auto& attr : m_ParticleAttributes) {
                                          __BNN_ASSERT(attr.bReady);
                                          of.write((char*)attr.buffer.data(), attr.buffer.size());
                                      }
                                      of.close();
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSerialization::writeHeader(std::ofstream& of)
{
    const std::locale& fixLoc = std::locale("C");
    of.imbue(fixLoc);

    of << "BananaParticleData\n";

    of << "FixedAtributes: " << m_FixedAttributes.size() << "\n";
    for(auto& attr : m_FixedAttributes)
        of << "    " << attr.name << " " << attr.typeName() << " " << attr.typeSize() << " " << attr.count << "\n";

    of << "ParticleAttributes: " << m_ParticleAttributes.size() << "\n";
    for(auto& attr : m_ParticleAttributes)
        of << "    " << attr.name << " " << attr.typeName() << " " << attr.typeSize() << " " << attr.count << "\n";

    of << "NParticles: " << m_nParticles << "\n";
    of << "EndHeader\n";
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
