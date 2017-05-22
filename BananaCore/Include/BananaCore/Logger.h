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
        MainProgram = 0,
        LoggerClass,
        Debugger
    };

    Logger() : m_LogSourceID(0) {}

    Logger(int sourceID) : m_LogSourceID(sourceID) {}

    static void setDataPath(const std::string& dataPath);

    static void enableStdOut()
    {
        m_bPrintStdOut = true;
    }

    static void disableStdOut()
    {
        m_bPrintStdOut = false;
    }

    static void enableLogFile()
    {
        m_bWriteLogToFile = true;
    }

    static void disableLogFile()
    {
        m_bWriteLogToFile = false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void newLine();
    void printSeparator();
    void printAligned(const std::string& s, char padding = PADDING, const std::string& wrapper = WRAPPER, int maxSize = 100);
    void printGreeting(const std::string& s);
    void printWarning(const std::string& s, int maxSize = 100);

    void printLog(const std::string& s);
    void printLogIndent(const std::string& s, int indentLevel);

    void printDetail(const std::string& s);
    void printDetailIndent(const std::string& s, int indentLevel);

    ////////////////////////////////////////////////////////////////////////////////
    static void newLine(int sourceID);
    static void printSeparator(int sourceID);
    static void printAligned(const std::string& s, int sourceID, char padding = PADDING, const std::string& wrapper = WRAPPER, int maxSize = 100);
    static void printGreeting(const std::string& s, int sourceID);
    static void printWarning(const std::string& s, int sourceID, int maxSize = 100);

    static void printLog(const std::string& s, int sourceID);
    static void printLogIndent(const std::string& s, int sourceID, int indentLevel);

    static void printDetail(const std::string& s, int sourceID);
    static void printDetailIndent(const std::string& s, int sourceID, int indentLevel);

    static void printDebug(const std::string& s);
    static void printDebugIndent(const std::string& s, int indentLevel);

    ////////////////////////////////////////////////////////////////////////////////
    static void initialize();
    static void initialize(const std::string& dataPath);
    static void shutdown();

    static void        computeTotalRunTime();
    static std::string getTotalRunTime();

    static std::string getSourceName(int sourceID);
    static void        setSourceName(int sourceID, const std::string& sourceName);
    static int         getNumSources();

    void setLogSource(int sourceID)
    {
        m_LogSourceID = sourceID;
    }

    static int s_LogLevel;

private:
    static void printToStdOut(const std::string& s, int sourceID);
    static void logToFile(const std::string& s, int sourceID);

    static bool m_bPrintStdOut;
    static bool m_bWriteLogToFile;
    static bool m_bDataPathReady;

    static std::string                           m_DataPath;
    static std::string                           m_LogFile;
    static std::string                           m_LogTimeFile;
    static std::chrono::system_clock::time_point m_StartupTime;
    static std::chrono::system_clock::time_point m_ShutdownTime;
    static std::string                           m_TotalRunTime;

    static std::map < int, std::string > m_SourceNames;
    static std::vector < std::shared_ptr < spdlog::logger >> m_FileLogger;
    static std::vector < std::shared_ptr < spdlog::logger >> m_ConsoleLogger;

    int m_LogSourceID;
};
