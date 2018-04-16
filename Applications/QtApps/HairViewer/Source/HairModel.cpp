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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Data/DataBuffer.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/NumberHelpers.h>

#include "cyHairFile.h"
#include "HairModel.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairModel::loadHairModel(const String& hairFile, bool bScale /*= true*/, float scale /*= 0.8f*/)
{
    const String ext    = hairFile.substr(hairFile.rfind('.') + 1);
    bool         result = false;
    if(ext == "bnn") {
        result = loadBNNHairModel(hairFile);
    } else if(ext == "data") {
        result = loadUSCHairModel(hairFile);
    } else if(ext == "hair") {
        result = loadCYHairModel(hairFile);
    }

    if(result) {
        computeBoundingBox();
        if(bScale) {
            scaleModel(scale);
            m_BBoxMin = Vec3f(-scale);
            m_BBoxMax = Vec3f(scale);
        }
    }

    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairModel::saveBNNHairModel(const String& hairFile) const
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairModel::loadBNNHairModel(const String& hairFile)
{
    m_ModelType = ModelType::BananaType;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairModel::loadCYHairModel(const String& hairFile)
{
    cyHairFile cyHair;
    if(cyHair.LoadFromFile(hairFile.c_str()) < 0) {
        return false;
    }
    m_nStrands       = cyHair.GetHeader().hair_count;
    m_nTotalVertices = cyHair.GetHeader().point_count;

    m_Vertices.resize(m_nTotalVertices);
    m_Tangents.resize(m_nTotalVertices);
    m_nStrandVertices.resize(m_nStrands, static_cast<UInt16>(cyHair.GetHeader().d_segments) + 1);
    ////////////////////////////////////////////////////////////////////////////////
    // populate data
    std::memcpy((void*)m_Vertices.data(), (void*)cyHair.GetPointsArray(), m_nTotalVertices * sizeof(Vec3f));

    // swap y-z
    for(auto& vertex : m_Vertices) {
        std::swap(vertex.y, vertex.z);
    }

    const unsigned short* segments = cyHair.GetSegmentsArray();
    if(segments != nullptr) {
        std::memcpy(m_nStrandVertices.data(), segments, sizeof(UInt16) * m_nStrands);
        for(auto& nverts : m_nStrandVertices) {
            nverts += 1;
        }
    }

    cyHair.FillDirectionArray(reinterpret_cast<float*>(m_Tangents.data()));
    ////////////////////////////////////////////////////////////////////////////////
    m_ModelType = ModelType::CYType;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairModel::loadUSCHairModel(const String& hairFile)
{
    static DataBuffer buffer;
    if(!FileHelpers::readFile(buffer.buffer(), hairFile)) {
        return false;
    }
    __BNN_REQUIRE(buffer.size() > 4);

    ////////////////////////////////////////////////////////////////////////////////
    int nstrands = -1;
    buffer.getData(nstrands);
    __BNN_REQUIRE(nstrands > 0);

    m_nStrands = static_cast<UInt>(nstrands);
    m_nStrandVertices.resize(m_nStrands, 0);
    m_nTotalVertices = 0;
    size_t offset = 4;
    for(UInt i = 0; i < m_nStrands; ++i) {
        int nverts = -1;
        buffer.getData(nverts, offset);
        __BNN_REQUIRE(nverts > 0);

        m_nStrandVertices[i] = static_cast<UInt>(nverts);
        m_nTotalVertices    += m_nStrandVertices[i];
        offset              += static_cast<size_t>(4 + nverts * 12);
    }
    __BNN_REQUIRE(offset == buffer.size());
    m_Vertices.resize(m_nTotalVertices);
    m_Tangents.resize(m_nTotalVertices);
    ////////////////////////////////////////////////////////////////////////////////
    // populate data
    UInt vcount = 0;
    offset = 8;
    for(UInt i = 0; i < m_nStrands; ++i) {
        UInt nverts = m_nStrandVertices[i];
        std::memcpy((void*)&m_Vertices[vcount], (void*)buffer.data(offset), nverts * 12);
        offset += static_cast<size_t>(4 + nverts * 12);
        vcount += nverts;
    }

    cyHairFile cyHair;
    cyHair.header.hair_count  = m_nStrands;
    cyHair.header.point_count = m_nTotalVertices;
    cyHair.points             = reinterpret_cast<float*>(m_Vertices.data());
    cyHair.segments           = new unsigned short[m_nStrands];
    for(UInt i = 0; i < m_nStrands; ++i) {
        cyHair.segments[i] = m_nStrandVertices[i] - 1;
    }
    cyHair.FillDirectionArray(reinterpret_cast<float*>(m_Tangents.data()));
    cyHair.points = nullptr;
    ////////////////////////////////////////////////////////////////////////////////
    m_ModelType = ModelType::USCHairType;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairModel::computeBoundingBox()
{
    m_BBoxMin = Vec3f(std::numeric_limits<float>::max());
    m_BBoxMax = Vec3f(std::numeric_limits<float>::min());
    for(const auto& v : m_Vertices) {
        m_BBoxMin = MathHelpers::min(m_BBoxMin, v);
        m_BBoxMax = MathHelpers::max(m_BBoxMax, v);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairModel::scaleModel(float scale)
{
    Vec3f diff = m_BBoxMax - m_BBoxMin;
    m_Scale       = Vec3f(2.0f * scale / MathHelpers::max(diff[0], diff[1], diff[2]));
    m_Translation = (m_BBoxMax + m_BBoxMin) * 0.5f;

    m_Translation = -m_Translation * m_Scale;
    NumberHelpers::transform(m_Vertices, m_Translation, m_Scale);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana