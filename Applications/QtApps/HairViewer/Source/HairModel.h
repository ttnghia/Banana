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

#pragma once

#include <Banana/Setup.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class HairModel
{
public:
    HairModel() = default;

    /***
     * \brief Get the dimension of the model. Must be either 2 or 3
     */
    UInt getDimension() const { return m_Dimension; }

    /***
     * \brief Get the number of strands in the model
     */
    UInt getNStrands() const { return m_nStrands; }

    /***
     * \brief Get the number of total vertices of all strands
     */
    UInt getNTotalVertices() const { return m_nTotalVertices; }

    /***
     * \brief Get array of number of vertices for each strand
     */
    const Vec_UInt16& getNStrandVertices() const { return m_nStrandVertices; }

    auto getVertices() const { return m_Vertices; }
    auto getTangents() const { return m_Tangents; }
    auto getBMin() const { return m_BBoxMin; }
    auto getBMax() const { return m_BBoxMax; }

    /***
     * \brief Load a hair model, which must be either Cem Yuksel's hair format, USC-HairSalon hair format, or Banana's binary hair format
     * \param bScale == true will scale the model to fit the bounding box of [-scale..scale]^N
     * \return true if file has been succesfully loaded, false otherwise
     */
    bool loadHairModel(const String& hairFile, bool bScale = true, float scale = 0.8f);

    /***
     * \brief Save a hair model to Banana's binary hair format
     */
    void saveBNNHairModel(const String& hairFile) const;

private:
    /***
     * \brief Load Banana's binary hair format
     * \return true if file has been succesfully loaded, false otherwise
     */
    bool loadBNNHairModel(const String& hairFile);

    /***
     * \brief Load Cem Yuksel's hair model format: http://cemyuksel.com/research/hairmodels/
     * \return true if file has been succesfully loaded, false otherwise
     */
    bool loadCYHairModel(const String& hairFile);

    /***
     * \brief Load USC-HairSalon hair model format: http://www-scf.usc.edu/~liwenhu/SHM/database.html
     * \return true if file has been succesfully loaded, false otherwise
     */
    bool loadUSCHairModel(const String& hairFile);

    /***
     * \brief Compute the axis-aligned bounding box of the model
     */
    void computeBoundingBox();

    /***
     * \brief Scale the model so that it fit to the bounding box [-scale..scale]^N
     */
    void scaleModel(float scale);

    ////////////////////////////////////////////////////////////////////////////////
    UInt       m_Dimension      = 3;
    UInt       m_nStrands       = 0;
    UInt       m_nTotalVertices = 0;
    Vec_UInt16 m_nStrandVertices;

    Vec_Vec3f m_Vertices;
    Vec_Vec3f m_Tangents;
    Vec3f     m_BBoxMin = Vec3f(-1);
    Vec3f     m_BBoxMax = Vec3f(1);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana