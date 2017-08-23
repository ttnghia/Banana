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

#include <chrono>
#include <map>
#include <string>
#include <iostream>
#include <chrono>
#include <memory>

#include <spdlog/spdlog.h>

#include <Banana/Utils/Timer.h>
#include <Banana/System/MemoryUsage.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define WRAPPER           "||"
#define PADDING           ' '
#define INDENT_SIZE       4
#define MAX_BUFFER_LENGTH 128

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Logger
{
public:
    enum LogLevel
    {
        NormalLevel = 0,
        DetailLevel,
        DebugLevel,
    };

    enum Source
    {
        MainProgram = 9990,
        LoggerClass = 9991,
        Debugger    = 9992
    };

    Logger() : m_LogSourceID(0) {}
    Logger(unsigned int sourceID) : m_LogSourceID(sourceID) {}
    Logger(unsigned int sourceID, const std::string& sourceName) : m_LogSourceID(sourceID) { Logger::setSourceName(sourceID, sourceName); }
    Logger(const std::string& sourceName) : m_LogSourceID(++m_NumSources) { Logger::setSourceName(m_LogSourceID, sourceName); }

    static void setDataPath(const std::string& dataPath);
    static void enableStdOut(bool bEnable = true) { m_bPrintStdOut = bEnable; }
    static void enableLogFile(bool bEnable = true) { m_bWriteLogToFile = bEnable; }


    ////////////////////////////////////////////////////////////////////////////////
    void newLine();
    void printSeparator();
    void printAligned(const std::string& s, char padding = PADDING, const std::string& wrapper = WRAPPER, int maxSize = 100);
    void printGreeting(const std::string& s);
    void printWarning(const std::string& s, int maxSize = 100);

    template<class Function>
    void printTime(const char* caption, const Function& function)
    {
        Logger::printLog(m_LogSourceID, Timer::getRunTime<Function>(caption, function));
    }

    void printTime(const char* caption, Timer& timer);

    template<class Function>
    void printTimeIndent(const char* caption, const Function& function);
    void printTimeIndent(const char* caption, Timer& timer);

    void printLog(const std::string& s);
    void printLogIndent(const std::string& s, int indentLevel = 1);

    void printDetail(const std::string& s);
    void printDetailIndent(const std::string& s, int indentLevel = 1);

    void printMemoryUsage();

    ////////////////////////////////////////////////////////////////////////////////
    static void newLine(unsigned int sourceID);
    static void printSeparator(unsigned int sourceID);
    static void printAligned(unsigned int sourceID, const std::string& s, char padding = PADDING, const std::string& wrapper = WRAPPER, int maxSize = 100);
    static void printGreeting(unsigned int sourceID, const std::string& s);
    static void printWarning(unsigned int sourceID, const std::string& s, int maxSize = 100);

    static void printLog(unsigned int sourceID, const std::string& s);
    static void printLogIndent(unsigned int sourceID, const std::string& s, int indentLevel = 1);

    static void printDetail(unsigned int sourceID, const std::string& s);
    static void printDetailIndent(unsigned int sourceID, const std::string& s, int indentLevel = 1);

    static void printDebug(const std::string& s);
    static void printDebugIndent(const std::string& s, int indentLevel = 1);

    static void printMemoryUsage(unsigned int sourceID);

    ////////////////////////////////////////////////////////////////////////////////
    static void initialize();
    static void initialize(const std::string& dataPath);
    static void shutdown();

    static void        computeTotalRunTime();
    static std::string getTotalRunTime();

    static std::string getSourceName(unsigned int sourceID);
    static void        setSourceName(unsigned int sourceID, const std::string& sourceName);

    template<class IntType>
    static IntType getNumSources();

    void setLogSource(unsigned int sourceID)
    {
        m_LogSourceID = sourceID;
    }

    static LogLevel s_LogLevel;

    static void signalHandler(unsigned int signum);

private:
    static void printToStdOut(unsigned int sourceID, const std::string& s);
    static void logToFile(unsigned int sourceID, const std::string& s);

    static bool m_bPrintStdOut;
    static bool m_bWriteLogToFile;
    static bool m_bDataPathReady;

    static std::string                           m_DataPath;
    static std::string                           m_LogFile;
    static std::string                           m_LogTimeFile;
    static std::chrono::system_clock::time_point m_StartupTime;
    static std::chrono::system_clock::time_point m_ShutdownTime;
    static std::string                           m_TotalRunTime;

    static unsigned int                    m_NumSources;
    static std::vector<std::string>        m_SourceNames;
    static std::shared_ptr<spdlog::logger> m_FileLogger;
    static std::shared_ptr<spdlog::logger> m_ConsoleLogger;

    int m_LogSourceID;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana