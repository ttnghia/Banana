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

#include <string>
#include <map>
#include <fstream>
#include <sstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AppConfigReader
{
public:
    AppConfigReader(const std::string& fileName) : m_bFileLoaded(false) { loadConfig(fileName); }

    bool isFileLoaded() { return m_bFileLoaded; }
    bool hasParam(const std::string& paramName) { return (m_AppConfigs.find(paramName) != m_AppConfigs.end()); }

    int getIntValue(const std::string& paramName) { return hasParam(paramName) ? std::stoi(m_AppConfigs[paramName]) : 0; }
    double getDoubleValue(const std::string& paramName) { return hasParam(paramName) ? std::stod(m_AppConfigs[paramName]) : 0; }
    std::string getStringValue(const std::string& paramName)  { return hasParam(paramName) ? m_AppConfigs[paramName] : std::string(""); }

private:
    void loadConfig(const std::string& fileName)
    {
        std::ifstream inFile(fileName);

        if(!inFile.is_open())
        {
            fprintf(stderr, "Warning: Cannot open file %s for reading. No config info was loaded.\n", fileName.c_str());
            fflush(stderr);
            return;
        }

        std::string line;
        std::string paramName, paramValue;

        while(std::getline(inFile, line))
        {
            line.erase(line.find_last_not_of(" \n\r\t") + 1);

            if(line == "")
            {
                continue;
            }

            if(line.find("//") != std::string::npos)
            {
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
    bool m_bFileLoaded;

    std::map<std::string, std::string> m_AppConfigs;
};