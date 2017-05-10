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

#include <BananaCore/TypeNames.h>
#include <Simulation/Array2.h>
#include <Simulation/Array3.h>

#include <cstdio>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace DataPrinter
{
template<class T>
void print(const Vec_Vec3<T>& array, std::string arrayName, size_t maxPrint, int precision)
{
    size_t numPrint = maxPrint > 0 ? maxPrint : array.size();

    Monitor::printAligned(arrayName, MonitorSource::NoodleDebugger);

    for(size_t p = 0; p < numPrint; ++p)
    {
        Monitor::printAligned(NumberHelpers::formatWithCommas(p, precision) + ": " + NumberHelpers::toString(array[p]), MonitorSource::NoodleDebugger);
    }

    Monitor::newLine(MonitorSource::NoodleDebugger);
}

void print(const Array3_Real& array, std::string arrayName, UInt32 max_d0, UInt32 max_d1, UInt32 max_d2, int precision)
{
    UInt32 numPrint_d0 = max_d0 > 0 ? max_d0 : array.m_SizeX;
    UInt32 numPrint_d1 = max_d1 > 0 ? max_d0 : array.m_SizeY;
    UInt32 numPrint_d2 = max_d2 > 0 ? max_d0 : array.m_SizeZ;

    Monitor::printAligned(arrayName, MonitorSource::NoodleDebugger);

    std::string str;

    for(UInt32 i = 0; i < numPrint_d0; ++i)
    {
        Monitor::printAligned(NumberHelpers::formatWithCommas(i) + ": ", MonitorSource::NoodleDebugger);

        for(UInt32 j = 0; j < numPrint_d1; ++j)
        {
            str.clear();

            for(UInt32 k = 0; k < numPrint_d2 - 1; ++k)
            {
                str = str + NumberHelpers::formatWithCommas(array(i, j, k), precision) + ", ";
            }

            str += NumberHelpers::formatWithCommas(array(i, j, numPrint_d2 - 1), precision);

            Monitor::printLog(str, MonitorSource::NoodleDebugger);
        }

        Monitor::newLine(MonitorSource::NoodleDebugger);
    }

    Monitor::newLine(MonitorSource::NoodleDebugger);
}


void print(const Array3_VecUInt& array, std::string arrayName, UInt32 max_d0, UInt32 max_d1, UInt32 max_d2)
{
    UInt32 numPrint_d0 = max_d0 > 0 ? max_d0 : array.m_SizeX;
    UInt32 numPrint_d1 = max_d1 > 0 ? max_d0 : array.m_SizeY;
    UInt32 numPrint_d2 = max_d2 > 0 ? max_d0 : array.m_SizeZ;

    Monitor::printAligned(arrayName, MonitorSource::NoodleDebugger);

    std::string str;

    for(UInt32 i = 0; i < numPrint_d0; ++i)
    {
        for(UInt32 j = 0; j < numPrint_d1; ++j)
        {
            for(UInt32 k = 0; k < numPrint_d2; ++k)
            {
                const Vec_UInt& cell = array(i, j, k);

                if(cell.size() > 0)
                {
                    str.clear();
                    str = "(" + NumberHelpers::formatWithCommas(i) + "," + NumberHelpers::formatWithCommas(j) + "," + NumberHelpers::formatWithCommas(k) +
                          "), size = " + NumberHelpers::formatWithCommas(cell.size()) + ", data = ";

                    for(UInt32 p = 0; p < cell.size() - 1; ++p)
                    {
                        str = str + NumberHelpers::formatWithCommas(cell[p]) + ", ";
                    }

                    str += NumberHelpers::formatWithCommas(cell[cell.size() - 1]);

                    Monitor::printLog(str, MonitorSource::NoodleDebugger);
                }
            }
        }
    }

    Monitor::newLine(MonitorSource::NoodleDebugger);
}
} // end namespace DataDebug

