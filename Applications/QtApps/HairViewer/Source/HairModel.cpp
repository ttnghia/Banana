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

#include <Banana/Utils/MathHelpers.h>

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
            m_BBoxMin = Vec3f(-1);
            m_BBoxMax = Vec3f(1);
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
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairModel::loadUSCHairModel(const String& hairFile)
{ return true; }

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
    scale /= MathHelpers::max(diff[0], diff[1], diff[2]);;
    auto center = (m_BBoxMax + m_BBoxMin) * scale * 0.5f;

    for(auto& vertex : m_Vertices) {
        vertex *= scale;
        vertex -= center;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana