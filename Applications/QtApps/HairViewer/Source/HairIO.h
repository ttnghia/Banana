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
class HairIO
{
public:
    HairIO() = default;

    /***
     * \brief get the number of strands in the model
     */
    UInt getNStrands() const;

    /***
     * \brief Load a hair model, which must be either Cem Yuksel's hair format, USC-HairSalon hair format, or Banana's binary hair format
     * \return true if file has been succesfully loaded, false otherwise
     */
    bool loadHairModel(const String& hairFile);

    /***
     * \brief Save a hair model to Banana's binary hair format
     */
    void saveBNNHairModel(const String& hairFile);

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
};
