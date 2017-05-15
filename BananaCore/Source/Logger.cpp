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

#include "..\Include\BananaCore\Logger.h"



Logger::Logger()
{}


Logger::~Logger()
{}


#include <iostream>

#include <Noodle/Core/Monitor/Monitor.h>
#include <Noodle/Core/Global/Macros.h>
#include <Noodle/Core/File/FileHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

LogLevel                              Monitor::s_LogLevel = LogLevel::NormalLevel;
bool                                  Monitor::m_bPrintStdOut = true;
bool                                  Monitor::m_bWriteLogToFile = true;
bool                                  Monitor::m_bFileOpenFirstTime = true;
bool                                  Monitor::m_bDataPathReady = false;

std::string                           Monitor::m_DataPath;
std::string                           Monitor::m_LogFile;
std::string                           Monitor::m_LogTimeFile;
std::chrono::system_clock::time_point Monitor::m_StartupTime;
std::chrono::system_clock::time_point Monitor::m_ShutdownTime;
std::string                           Monitor::m_TotalRunTime = "";

std::map<MonitorSource, std::string>  Monitor::m_SourceNames;
tbb::concurrent_vector<std::string>   Monitor::m_LogBuffer;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::setDataPath(const std::string& dataPath)
{
    m_DataPath = dataPath;

    int i = 1;

    do
    {
        std::string file = m_DataPath + "/Log/log_" + std::to_string(i) + ".txt";

        if(!FileHelpers::fileExisted(file))
        {
            m_LogFile = file;
            m_LogTimeFile = m_DataPath + "/Log/time_" + std::to_string(i) + ".txt";
            break;
        }

        ++i;

        assert(i < 1000);
    } while(true);

    m_LogBuffer.reserve(MAX_BUFFER_LENGTH);
    m_bDataPathReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::newLine()
{
    printLog("");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printSeparator()
{
    Monitor::printSeparator(m_LogSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printAligned(const std::string& s, char padding, const std::string& wrapper, int maxSize)
{
    Monitor::printAligned(s, m_LogSource, padding, wrapper, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printGreeting(const std::string& s)
{
    Monitor::printGreeting(s, m_LogSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printWarning(const std::string& s, int maxSize)
{
    Monitor::printWarning(s, m_LogSource, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printLog(const std::string& s)
{
    Monitor::printLog(s, m_LogSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printLogIndent(const std::string& s, int indentLevel)
{
    Monitor::printLogIndent(s, m_LogSource, indentLevel);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDetail(const std::string& s)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        printLog(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDetailIndent(const std::string& s, int indentLevel)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        printLogIndent(s, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printAligned(const std::string& s, MonitorSource source, char padding, const std::string& wrapper, int maxSize)
{
    size_t            length = s.length();
    const std::string str = length == 0 ? s : std::string(" " + s + " ");
    length = str.length();

    size_t      paddingSize = ((size_t)maxSize - length - 2 * wrapper.length()) / 2;
    size_t      finalLength = wrapper.length() * 2 + length + paddingSize * 2;

    std::string finalStr;
    finalStr.reserve(finalLength + 1);

    finalStr.append(wrapper);
    finalStr.append(std::string(paddingSize, padding));
    finalStr.append(str);
    finalStr.append((finalLength == static_cast<size_t>(maxSize)) ? std::string(paddingSize, padding) : std::string(paddingSize + 1, padding));
    finalStr.append(wrapper);

    Monitor::printLog(finalStr, source);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printGreeting(const std::string& s, MonitorSource source)
{
    Monitor::printSeparator(source);
    Monitor::printAligned("", source, ' ');
    Monitor::printAligned(s, source);
    Monitor::printAligned("", source, ' ');
    Monitor::printSeparator(source);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printWarning(const std::string& s, MonitorSource source, int maxSize)
{
    const std::string str = "Warning: " + s + " ";
    size_t            paddingSize = ((size_t)maxSize - str.length());

    Monitor::printLog(str + std::string(paddingSize, '*'), source);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::newLine(MonitorSource source)
{
    Monitor::printLog("", source);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printSeparator(MonitorSource source)
{
    Monitor::printAligned("", source, '=');
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printLog(const std::string& s, MonitorSource source)
{
    time_t currentTime = time(nullptr);

    // Convert the current time to the local time
    struct tm*  localTime = localtime(&currentTime);
    std::string logStr;
    logStr.reserve(1024);

    logStr.append("[");
    logStr.append(getSourceName(source));
    logStr.append("::");
    logStr.append(std::to_string(localTime->tm_hour));
    logStr.append(":");
    logStr.append(std::to_string(localTime->tm_min));
    logStr.append(":");
    logStr.append(std::to_string(localTime->tm_sec));
    logStr.append("] ");
    logStr.append(s);

    printToStdOut(logStr);
    addToBuffer(logStr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printLogIndent(const std::string& s, MonitorSource source, int indentLevel)
{
    std::string logStr;
    logStr.reserve(1024);
    logStr.append(std::string(INDENT_SIZE * indentLevel, ' '));
    logStr.append(s);

    Monitor::printLog(logStr, source);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDetail(const std::string& s, MonitorSource source)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Monitor::printLog(s, source);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDetailIndent(const std::string& s, MonitorSource source, int indentLevel)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Monitor::printLogIndent(s, source, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDebug(const std::string& s)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Monitor::printLog(s, MonitorSource::NoodleDebugger);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printDebugIndent(const std::string& s, int indentLevel)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Monitor::printLogIndent(s, MonitorSource::NoodleDebugger, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::initialize(const std::string& dataPath)
{
    Monitor::setDataPath(dataPath);
    Monitor::initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::initialize()
{
    m_StartupTime = std::chrono::system_clock::now();

    if(m_bWriteLogToFile)
    {
        FileHelpers::createFolder(std::string(m_DataPath + "/Log"));
        time_t currentTime = std::chrono::system_clock::to_time_t(m_StartupTime -
                                                                  std::chrono::hours(24));
        assert(m_bDataPathReady);
        struct tm         localTime = *localtime(&currentTime);
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Log created at: " << localTime.tm_mon << "/" << localTime.tm_mday;
        strBuilder << "/" << (localTime.tm_year + 1900) << ", " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;

        FileHelpers::writeFile(strBuilder.str(), m_LogTimeFile);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::shutdown()
{
    m_ShutdownTime = std::chrono::system_clock::now();
    computeTotalRunTime();

    if(m_bWriteLogToFile)
    {
        assert(m_bDataPathReady);

        time_t            currentTime = std::chrono::system_clock::to_time_t(m_ShutdownTime - std::chrono::hours(24));;
        struct tm         localTime = *localtime(&currentTime);
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Log finished at: " << localTime.tm_mon << "/" << localTime.tm_mday;
        strBuilder << "/" << (localTime.tm_year + 1900) << ", " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;
        strBuilder << m_TotalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), m_LogTimeFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    Monitor::printLog(m_TotalRunTime, MonitorSource::NoodleMonitor);
    Monitor::printSeparator(MonitorSource::NoodleMonitor);
    Monitor::newLine(MonitorSource::NoodleMonitor);
    flushToFile();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::checkCriticalCondition(bool condition, const std::string& err)
{
    if(!condition)
    {
        Monitor::criticalError(err);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::criticalError(const std::string err, int maxSize)
{
    const std::string str = "ERROR: " + err + " ";
    size_t            paddingSize = ((size_t)maxSize - str.length());

    Monitor::printLog(str + std::string(paddingSize, '*'), MonitorSource::NoodleMonitor);
    Monitor::shutdown();

    exit(EXIT_FAILURE);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string Monitor::getTotalRunTime()
{
    assert(m_TotalRunTime != "");
    return m_TotalRunTime;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::computeTotalRunTime()
{
    int days, hours, mins, secs;
    getDuration(m_ShutdownTime - m_StartupTime, days, hours, mins, secs);

    std::stringstream strBuilder;
    strBuilder.str("");
    strBuilder << "Total time: "
        << days << "(days), "
        << hours << "(hours), "
        << mins << "(mins), "
        << secs << "(secs).";

    m_TotalRunTime = strBuilder.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::flushToFile()
{
    saveBufferToFile();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string Monitor::getSourceName(MonitorSource source)
{
    if(m_SourceNames.find(source) != m_SourceNames.end())
    {
        return m_SourceNames[source];
    }
    else
    {
        return std::string("UnknownSource");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int Monitor::getNumSources()
{
    return static_cast<int>(m_SourceNames.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::getSourceName(MonitorSource source, const std::string& sourceName)
{
    m_SourceNames[source] = sourceName;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::printToStdOut(const std::string& s)
{
    if(m_bPrintStdOut)
    {
        printf("%s\n", s.c_str());
        fflush(stdout);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::addToBuffer(const std::string& str)
{
    m_LogBuffer.emplace_back(std::string(str));

    if(m_LogBuffer.size() >= MAX_BUFFER_LENGTH)
    {
        saveBufferToFile();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Monitor::saveBufferToFile()
{
    if(!m_bWriteLogToFile)
    {
        return;
    }

    assert(m_bDataPathReady);

    if(m_bFileOpenFirstTime)
    {
        m_bFileOpenFirstTime = false;

        FileHelpers::writeFile(m_LogBuffer, m_LogFile);
    }
    else
    {
        FileHelpers::appendToFile(m_LogBuffer, m_LogFile);
    }

    m_LogBuffer.clear();
}
