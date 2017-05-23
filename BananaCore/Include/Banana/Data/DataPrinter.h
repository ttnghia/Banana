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

#include <Banana/Array/Array2.h>
#include <Banana/Array/Array3.h>

#include <cstdio>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace DataPrinter
{
extern class Monitor;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const std::vector<T>& array, const std::string& arrayName, size_t maxPrint, int precision = 5)
{
    size_t numPrint = maxPrint > 0 ? maxPrint : array.size();

    Monitor::printAligned(arrayName, EventSource::Debugger);

    for(size_t p = 0; p < numPrint; ++p)
    {
        Monitor::printAligned(NumberHelpers::formatWithCommas(p, precision) + ": " + NumberHelpers::toString(array[p]), EventSource::Debugger);
    }

    Monitor::newLine(EventSource::Debugger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array2<T>& array, const std::string& arrayName, Array2::size_type maxPrint_d0, Array2::size_type maxPrint_d1, int precision = 5)
{
    Array2::size_type numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.sizeX();
    Array2::size_type numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.sizeY();

    Monitor::printAligned(arrayName, EventSource::Debugger);

    std::stringstream ss;

    for(Array2::size_type j = 0; j < numPrint_d1; ++j)
    {
        Monitor::printAligned(NumberHelpers::formatWithCommas(i) + ": ", EventSource::Debugger);

        ss.str("");

        for(Array2::size_type i = 0; i < numPrint_d0 - 1; ++i)
        {
            ss << NumberHelpers::formatWithCommas(array(i, j), precision) << ", ";
        }

        ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j), precision);

        Monitor::printLog(ss.str(), EventSource::Debugger);
    }

    Monitor::newLine(EventSource::Debugger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array3<T>& array, const std::string& arrayName, Array3<T>::size_type maxPrint_d0, Array3<T>::size_type maxPrint_d1, Array3<T>::size_type maxPrint_d2, int precision = 5)
{
    Array3<T>::size_type numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.sizeX();
    Array3<T>::size_type numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.sizeY();
    Array3<T>::size_type numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d0 : array.sizeZ();

    Monitor::printAligned(arrayName, EventSource::Debugger);

    std::stringstream ss;

    for(Array3<T>::size_type k = 0; k < numPrint_d2; ++k)
    {
        Monitor::printAligned(NumberHelpers::formatWithCommas(i) + ": ", EventSource::Debugger);

        for(Array3<T>::size_type j = 0; j < numPrint_d1; ++j)
        {
            ss.str("");

            for(Array3<T>::size_type i = 0; i < numPrint_d0 - 1; ++i)
            {
                ss << NumberHelpers::formatWithCommas(array(i, j, k), precision) << ", ";
            }

            ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j, k), precision);

            Monitor::printLog(ss.str(), EventSource::Debugger);
        }

        Monitor::newLine(EventSource::Debugger);
    }

    Monitor::newLine(EventSource::Debugger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array2<std::vector<T> >& array, const std::string& arrayName, Array2<T>::size_type maxPrint_d0, Array2<T>::size_type maxPrint_d1, int precision = 5)
{
    Array3<T>::size_type numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.sizeX();
    Array3<T>::size_type numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.sizeY();

    Monitor::printAligned(arrayName, EventSource::Debugger);

    std::stringstream ss;

    for(Array3<T>::size_type j = 0; j < numPrint_d1; ++j)
    {
        for(Array3<T>::size_type i = 0; i < numPrint_d0; ++i)
        {
            const std::vector<T>& cell = array(i, j);

            if(cell.size() == 0)
                continue;

            ss.str("");
            ss << "(";
            ss << NumberHelpers::formatWithCommas(i) << ",";
            ss << NumberHelpers::formatWithCommas(j) << "), size = ";
            ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

            for(size_t p = 0; p < cell.size() - 1; ++p)
            {
                ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
            }

            ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);

            Monitor::printLog(ss.str(), EventSource::Debugger);
        }
    }


    Monitor::newLine(EventSource::Debugger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array3<std::vector<T> >& array, const std::string& arrayName,
           Array3<T>::size_type maxPrint_d0, Array3<T>::size_type maxPrint_d1, Array3<T>::size_type maxPrint_d2,
           int precision = 5)
{
    Array3<T>::size_type numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.sizeX();
    Array3<T>::size_type numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.sizeY();
    Array3<T>::size_type numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d0 : array.sizeZ();

    Monitor::printAligned(arrayName, EventSource::Debugger);

    std::stringstream ss;

    for(Array3<T>::size_type k = 0; k < numPrint_d2; ++k)
    {
        for(Array3<T>::size_type j = 0; j < numPrint_d1; ++j)
        {
            for(Array3<T>::size_type i = 0; i < numPrint_d0; ++i)
            {
                const std::vector<T>& cell = array(i, j, k);

                if(cell.size() == 0)
                    continue;

                ss.str("");
                ss << "(";
                ss << NumberHelpers::formatWithCommas(i) << ",";
                ss << NumberHelpers::formatWithCommas(j) << ",";
                ss << NumberHelpers::formatWithCommas(k) << "), size = ";
                ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

                for(size_t p = 0; p < cell.size() - 1; ++p)
                {
                    ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
                }

                ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);

                Monitor::printLog(ss.str(), EventSource::Debugger);
            }
        }
    }

    Monitor::newLine(EventSource::Debugger);
}
} // end namespace DataPrinter

