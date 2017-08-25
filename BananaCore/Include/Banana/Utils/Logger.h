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
#include <memory>
#include <csignal>

#include <spdlog/spdlog.h>

#include <Banana/System/MemoryUsage.h>
#include <Banana/Utils/Timer.h>
#include <Banana/Macros.h>

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
        DebugLevel,
    };

    Logger(const std::string& instanceName)
    {
        __BNN_ASSERT(s_bInitialized);
        m_ConsoleLogger = std::make_shared<spdlog::logger>(instanceName, s_ConsoleSink);

        if(s_LogFileSink != nullptr)
            m_FileLogger = std::make_shared<spdlog::async_logger>(instanceName, s_LogFileSink, 1024);
    }

    static std::shared_ptr<Logger> create(const std::string& instanceName)
    {
        return std::make_shared<Logger>(instanceName);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void newLine() { printLog(""); }
    void printSeparator();
    void printAligned(const std::string& s, char padding = PADDING, const std::string& wrapper = WRAPPER, unsigned int maxSize = 100);
    void printGreeting(const std::string& s);
    void printWarning(const std::string& s, unsigned int maxSize = 100);
    void printError(const std::string& s, unsigned int maxSize = 100);

    ////////////////////////////////////////////////////////////////////////////////
    template<class Function>
    void printRunTime(const char* caption, const Function& function) { printLog(Timer::getRunTime<Function>(caption, function)); }
    template<class Function>
    void printRunTimeIndent(const char* caption, const Function& function) { printLogIndent(Timer::getRunTime<Function>(caption, function)); }

    template<class Function>
    void printRunTime(const char* caption, Timer& timer, const Function& function) { timer.tick(); function(); timer.tock(); printLog(timer.getRunTime(caption)); }
    template<class Function>
    void printRunTimeIndent(const char* caption, Timer& timer, const Function& function) { timer.tick(); function(); timer.tock(); printLogIndent(timer.getRunTime(caption)); }

    void printRunTime(const char* caption, Timer& timer) { printLog(timer.getRunTime(caption)); }
    void printRunTimeIndent(const char* caption, Timer& timer) { printLogIndent(timer.getRunTime(caption)); }

    ////////////////////////////////////////////////////////////////////////////////
    void printLog(const std::string& s);
    void printLogIndent(const std::string& s, unsigned int indentLevel = 1);

    void printDebug(const std::string& s);
    void printDebugIndent(const std::string& s, unsigned int indentLevel = 1);

    void printMemoryUsage();

    ////////////////////////////////////////////////////////////////////////////////
    static void initialize();
    static void initialize(const std::string& dataPath);
    static void shutdown();
    static void setLoglevel(LogLevel logLevel) { s_LogLevel = logLevel; }

    static void setDataPath(const std::string& dataPath) { s_DataPath = dataPath; }
    static void enableLog2Console(bool bEnable = true) { s_bPrint2Console = bEnable; }
    static void enableLog2File(bool bEnable = true) { s_bWriteLog2File = bEnable; }

    static std::string getTotalRunTime();

private:
    static void signalHandler(int signum);

    ////////////////////////////////////////////////////////////////////////////////
    static LogLevel s_LogLevel;
    static bool     s_bPrint2Console;
    static bool     s_bWriteLog2File;
    static bool     s_bInitialized;

    static std::string                           s_DataPath;
    static std::string                           s_TimeLogFile;
    static std::chrono::system_clock::time_point s_StartupTime;
    static std::chrono::system_clock::time_point s_ShutdownTime;

    static std::shared_ptr<spdlog::sinks::stdout_sink_mt>      s_ConsoleSink;
    static std::shared_ptr<spdlog::sinks::simple_file_sink_mt> s_LogFileSink;

    static std::shared_ptr<Logger> s_MainFuncLogger;

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<spdlog::logger>       m_ConsoleLogger = nullptr;
    std::shared_ptr<spdlog::async_logger> m_FileLogger    = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana