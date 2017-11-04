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

#include <Banana/Array/Array.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>

#include <cstdio>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace DataPrinter
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Vector<T>& array, const String& arrayName, size_t maxPrint = 0, int precision = 5)
{
    size_t numPrint = maxPrint > 0 ? maxPrint : array.size();

    Logger logger("DataPrinter");
    logger.printAligned(arrayName);

    for(size_t p = 0; p < numPrint; ++p) {
        logger.printAligned(NumberHelpers::formatWithCommas(p, precision) + ": " + NumberHelpers::toString(array[p]));
    }

    logger.newLine();
}

template<class T>
void printToFile(const String& fileName, const Vector<T>& array, const String& arrayName, size_t maxPrint = 0, int precision = 5)
{
    size_t numPrint = maxPrint > 0 ? maxPrint : array.size();

    Vector<String> fileContent;
    fileContent.reserve(numPrint + 1);
    fileContent.push_back(arrayName);

    for(size_t p = 0; p < numPrint; ++p) {
        fileContent.push_back(NumberHelpers::formatWithCommas(p, precision) + ": " + NumberHelpers::toString(array[p]));
    }

    FileHelpers::writeFile(fileContent, fileName);
}

template<class T>
void printToFile(const String& fileName, const Vector<Vector<T> >& array, const String& arrayName, size_t maxPrint = 0, int precision = 5)
{
    size_t numPrint = maxPrint > 0 ? maxPrint : array.size();

    size_t numRows = 1;
    for(size_t p = 0; p < numPrint; ++p) {
        if(array[p].size() != 0) {
            numRows += (array[p].size() + 2);
        }
    }

    Vector<String> fileContent;
    fileContent.reserve(numRows);
    fileContent.push_back(arrayName);

    for(size_t p = 0; p < numPrint; ++p) {
        if(array[p].size() == 0) {
            continue;
        }
        fileContent.push_back("================================================================================");
        fileContent.push_back("Element " + std::to_string(p) + ":");
        for(size_t i = 0; i < array[p].size(); ++i) {
            fileContent.push_back(NumberHelpers::formatWithCommas(p, precision) + ": " + NumberHelpers::toString(array[p][i]));
        }
    }

    FileHelpers::writeFile(fileContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array<2, T>& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d1 : array.size()[1];

    Logger logger("DataPrinter");
    logger.printAligned(arrayName);

    std::stringstream ss;

    for(size_t j = 0; j < numPrint_d1; ++j) {
        logger.printAligned(NumberHelpers::formatWithCommas(j) + ": ");

        ss.str("");
        for(size_t i = 0; i < numPrint_d0 - 1; ++i) {
            ss << NumberHelpers::formatWithCommas(array(i, j), precision) << ", ";
        }
        ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j), precision);
        ////////////////////////////////////////////////////////////////////////////////
        logger.printLog(ss.str());
    }

    logger.newLine();
}

template<class T>
void printToFile(const String& fileName, const Array<2, T>& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d1 : array.size()[1];

    Vector<String> fileContent;
    fileContent.reserve(numPrint_d1 + 1);
    fileContent.push_back(arrayName);

    std::stringstream ss;

    for(size_t j = 0; j < numPrint_d1; ++j) {
        logger.printAligned(NumberHelpers::formatWithCommas(j) + ": ");

        ss.str("");
        for(size_t i = 0; i < numPrint_d0 - 1; ++i) {
            ss << NumberHelpers::formatWithCommas(array(i, j), precision) << ", ";
        }
        ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j), precision);

        ////////////////////////////////////////////////////////////////////////////////
        fileContent.push_back(ss.str());
    }

    FileHelpers::writeFile(fileContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array<3, T>& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, size_t maxPrint_d2 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d1 : array.size()[1];
    size_t numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d2 : array.size()[2];

    Logger logger("DataPrinter");
    logger.printAligned(arrayName);

    std::stringstream ss;

    for(size_t k = 0; k < numPrint_d2; ++k) {
        logger.printAligned(NumberHelpers::formatWithCommas(k) + ": ");

        for(size_t j = 0; j < numPrint_d1; ++j) {
            ss.str("");
            for(size_t i = 0; i < numPrint_d0 - 1; ++i) {
                ss << NumberHelpers::formatWithCommas(array(i, j, k), precision) << ", ";
            }
            ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j, k), precision);
            ////////////////////////////////////////////////////////////////////////////////
            logger.printLog(ss.str());
        }

        logger.newLine();
    }

    logger.newLine();
}

template<class T>
void printToFile(const String& fileName, const Array<3, T>& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, size_t maxPrint_d2 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d1 : array.size()[1];
    size_t numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d2 : array.size()[2];

    Vector<String> fileContent;
    fileContent.reserve((numPrint_d1 + 1) * numPrint_d2 + 1);
    fileContent.push_back(arrayName);

    std::stringstream ss;

    for(size_t k = 0; k < numPrint_d2; ++k) {
        fileContent.push_back("Line: " + NumberHelpers::formatWithCommas(k) + ": ");

        for(size_t j = 0; j < numPrint_d1; ++j) {
            ss.str("");
            for(size_t i = 0; i < numPrint_d0 - 1; ++i) {
                ss << NumberHelpers::formatWithCommas(array(i, j, k), precision) << ", ";
            }
            ss << NumberHelpers::formatWithCommas(array(numPrint_d0 - 1, j, k), precision);
            ////////////////////////////////////////////////////////////////////////////////
            fileContent.push_back(ss.str());
        }

        fileContent.push_back("");
    }

    FileHelpers::writeFile(fileContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array<2, Vector<T> >& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d1 : array.size()[1];

    Logger logger("DataPrinter");
    logger.printAligned(arrayName);

    std::stringstream ss;

    for(size_t j = 0; j < numPrint_d1; ++j) {
        for(size_t i = 0; i < numPrint_d0; ++i) {
            const Vector<T>& cell = array(i, j);

            if(cell.size() == 0) {
                continue;
            }

            ss.str("");
            ss << "(";
            ss << NumberHelpers::formatWithCommas(i) << ",";
            ss << NumberHelpers::formatWithCommas(j) << "), size = ";
            ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

            for(size_t p = 0; p < cell.size() - 1; ++p) {
                ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
            }

            ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);

            logger.printLog(ss.str());
        }
    }


    logger.newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void printToFile(const String& fileName, const Array<2, Vector<T> >& array, const String& arrayName, size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.size()[1];

    Vector<String> fileContent;
    fileContent.reserve(numPrint_d0 * numPrint_d1 + 1);
    fileContent.push_back(arrayName);

    std::stringstream ss;

    for(size_t j = 0; j < numPrint_d1; ++j) {
        for(size_t i = 0; i < numPrint_d0; ++i) {
            const Vector<T>& cell = array(i, j);
            if(cell.size() == 0) {
                continue;
            }

            ss.str("");
            ss << "(";
            ss << NumberHelpers::formatWithCommas(i) << ",";
            ss << NumberHelpers::formatWithCommas(j) << "), size = ";
            ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

            for(size_t p = 0; p < cell.size() - 1; ++p) {
                ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
            }
            ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);
            ////////////////////////////////////////////////////////////////////////////////
            fileContent.push_back(ss.str());
        }
    }


    FileHelpers::writeFile(fileContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void print(const Array<3, Vector<T> >& array, const String& arrayName,
           size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, size_t maxPrint_d2 = 0,
           int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.size()[1];
    size_t numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d0 : array.sizeZ();

    Logger logger("DataPrinter");
    logger.printAligned(arrayName);

    std::stringstream ss;

    for(size_t k = 0; k < numPrint_d2; ++k) {
        for(size_t j = 0; j < numPrint_d1; ++j) {
            for(size_t i = 0; i < numPrint_d0; ++i) {
                const Vector<T>& cell = array(i, j, k);

                if(cell.size() == 0) {
                    continue;
                }

                ss.str("");
                ss << "(";
                ss << NumberHelpers::formatWithCommas(i) << ",";
                ss << NumberHelpers::formatWithCommas(j) << ",";
                ss << NumberHelpers::formatWithCommas(k) << "), size = ";
                ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

                for(size_t p = 0; p < cell.size() - 1; ++p) {
                    ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
                }
                ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);
                ////////////////////////////////////////////////////////////////////////////////
                logger.printLog(ss.str());
            }
        }
    }

    logger.newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void printToFile(const String& fileName, const Array<3, Vector<T> >& array, const String& arrayName,
                 size_t maxPrint_d0 = 0, size_t maxPrint_d1 = 0, size_t maxPrint_d2 = 0,
                 int precision = 5)
{
    size_t numPrint_d0 = maxPrint_d0 > 0 ? maxPrint_d0 : array.size()[0];
    size_t numPrint_d1 = maxPrint_d1 > 0 ? maxPrint_d0 : array.size()[1];
    size_t numPrint_d2 = maxPrint_d2 > 0 ? maxPrint_d0 : array.size()[2];

    Vector<String> fileContent;
    fileContent.reserve(numPrint_d0 * numPrint_d1 * numPrint_d2 + 1);
    fileContent.push_back(arrayName);

    std::stringstream ss;

    for(size_t k = 0; k < numPrint_d2; ++k) {
        for(size_t j = 0; j < numPrint_d1; ++j) {
            for(size_t i = 0; i < numPrint_d0; ++i) {
                const Vector<T>& cell = array(i, j, k);

                if(cell.size() == 0) {
                    continue;
                }

                ss.str("");
                ss << "(";
                ss << NumberHelpers::formatWithCommas(i) << ",";
                ss << NumberHelpers::formatWithCommas(j) << ",";
                ss << NumberHelpers::formatWithCommas(k) << "), size = ";
                ss << NumberHelpers::formatWithCommas(cell.size()) << ", data = ";

                for(size_t p = 0; p < cell.size() - 1; ++p) {
                    ss << NumberHelpers::formatWithCommas(cell[p]) << ", ";
                }
                ss << NumberHelpers::formatWithCommas(cell[cell.size() - 1]);
                ////////////////////////////////////////////////////////////////////////////////
                fileContent.push_back(ss.str());
            }
        }
    }

    FileHelpers::writeFile(fileContent, fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace DataPrinter

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana