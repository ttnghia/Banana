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

#include <Banana/Setup.h>
#include <Banana/System/MemoryUsage.h>
#include <Banana/Utils/Timer.h>

#include <spdlog/spdlog.h>

#include <chrono>
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <csignal>

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
    using Clock = std::chrono::system_clock;
public:
    Logger(const String& instanceName)
    {
        __BNN_ASSERT(s_bInitialized);
        m_ConsoleLogger = std::make_shared<spdlog::logger>(instanceName, s_ConsoleSink);

        if(s_LogFileSink != nullptr) {
            m_FileLogger = std::make_shared<spdlog::async_logger>(instanceName, s_LogFileSink, 1024);
        }
    }

    static auto create(const String& instanceName) { return std::make_shared<Logger>(instanceName); }

    ////////////////////////////////////////////////////////////////////////////////
    void setLoglevel(spdlog::level::level_enum level) { m_ConsoleLogger->set_level(level); if(m_FileLogger != nullptr) { m_FileLogger->set_level(level); } }

    void newLine() { printLog(""); }
    void newLineIf(bool bCondition) { if(bCondition) { printLog(""); } }
    void printSeparator();
    void printAligned(const String& s, char padding = PADDING, const String& wrapper = WRAPPER, UInt maxSize = 100);
    void printTextBox(const String& s);
    void printWarning(const String& s, UInt maxSize = 100);
    void printError(const String& s, UInt maxSize = 100);

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
    void printLog(const String& s);
    void printLog(const String& s, spdlog::level::level_enum level);
    void printLogIndent(const String& s, UInt indentLevel = 1, char trailing = ' ');

    void printLogIf(bool bCondition, const String& s) { if(bCondition) { printLog(s); } }
    void printLogIf(bool bCondition, const String& s, spdlog::level::level_enum level) { if(bCondition) { printLog(s, level); } }
    void printLogIndentIf(bool bCondition, const String& s, UInt indentLevel = 1, char trailing = ' ') { if(bCondition) { printLogIndent(s, indentLevel, trailing); } }

    void printMemoryUsage();

    ////////////////////////////////////////////////////////////////////////////////
    static void  initialize(bool bPrint2Console = true, bool bWriteLog2File = false);
    static void  initialize(const String& dataPath, bool bPrint2Console = true, bool bWriteLog2File = false);
    static void  shutdown();
    static void  setDataPath(const String& dataPath) { s_DataPath = dataPath; }
    static auto& mainLogger() noexcept { assert(s_MainLogger != nullptr); return *s_MainLogger; }
private:
    static String getTotalRunTime();
    static void   signalHandler(int signum);

    ////////////////////////////////////////////////////////////////////////////////
    static bool s_bPrint2Console;
    static bool s_bWriteLog2File;
    static bool s_bInitialized;
    static bool s_bShutdown;

    static String            s_DataPath;
    static String            s_TimeLogFile;
    static Clock::time_point s_StartupTime;
    static Clock::time_point s_ShutdownTime;

#ifdef __BANANA_WINDOWS__
    static SharedPtr<spdlog::sinks::wincolor_stdout_sink_mt> s_ConsoleSink;
#else
    static SharedPtr<spdlog::sinks::ansicolor_stdout_sink_mt> s_ConsoleSink;
#endif

    static SharedPtr<spdlog::sinks::simple_file_sink_mt> s_LogFileSink;
    static SharedPtr<Logger>                             s_MainLogger;

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<spdlog::logger>       m_ConsoleLogger = nullptr;
    SharedPtr<spdlog::async_logger> m_FileLogger    = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana