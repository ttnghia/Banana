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

#include <string>
#include <map>
#include <fstream>
#include <sstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AppConfigReader
{
public:
    AppConfigReader(const std::string& fileName) { loadConfig(fileName); }

    bool isFileLoaded() { return m_bFileLoaded; }
    bool hasParam(const std::string& paramName) { return (m_AppConfigs.find(paramName) != m_AppConfigs.end()); }

    int         getIntValue(const std::string& paramName) { return hasParam(paramName) ? std::stoi(m_AppConfigs[paramName]) : 0; }
    double      getDoubleValue(const std::string& paramName) { return hasParam(paramName) ? std::stod(m_AppConfigs[paramName]) : 0; }
    std::string getStringValue(const std::string& paramName) { return hasParam(paramName) ? m_AppConfigs[paramName] : std::string(""); }

private:
    void loadConfig(const std::string& fileName)
    {
        std::ifstream inFile(fileName);

        if(!inFile.is_open()) {
            if(m_bPrintError) {
                fprintf(stderr, "Warning: Cannot open file %s for reading. No config info was loaded.\n", fileName.c_str());
                fflush(stderr);
            }
            return;
        }

        std::string line;
        std::string paramName, paramValue;

        while(std::getline(inFile, line)) {
            line.erase(line.find_last_not_of(" \n\r\t") + 1);

            if(line == "") {
                continue;
            }

            if(line.find("//") != std::string::npos) {
                continue;
            }

            std::istringstream iss(line);
            iss >> paramName >> paramValue;

            m_AppConfigs[paramName] = paramValue;
        }

        inFile.close();
        m_bFileLoaded = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool m_bFileLoaded = false;
    bool m_bPrintError = false;

    std::map<std::string, std::string> m_AppConfigs;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana