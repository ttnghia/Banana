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
class Logger
{
public:
    Logger();
    ~Logger();
};



#pragma once

#include <chrono>
#include <map>
#include <string>
#include <iostream>
#include <chrono>

#include <tbb/tbb.h>

#include <Noodle/Core/Global/TypeNames.h>
#include <Noodle/Core/Global/Enums.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define WRAPPER           "||"
#define PADDING           ' '
#define INDENT_SIZE       4
#define MAX_BUFFER_LENGTH 128

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename Rep, typename Period>
void getDuration(std::chrono::duration<Rep, Period> t, int& n_days, int& n_hours, int& n_mins, int& n_secs)
{
    assert(0 <= t.count());

    // approximate because a day doesn't have a fixed length
    typedef std::chrono::duration<int, std::ratio<60 * 60 * 24> >   days_t;

    auto days = std::chrono::duration_cast<days_t>(t);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(t - days);
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(t - days - hours);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(t - days - hours - mins);

    n_days = days.count();
    n_hours = hours.count();
    n_mins = mins.count();
    n_secs = secs.count();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Monitor
{
public:
    Monitor() : m_LogSource(MonitorSource::NoodleMain)
    {}

    Monitor(MonitorSource source) : m_LogSource(source)
    {}

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
    void printLogIndent(const std::string& s, int indentLevel = 1);

    void printDetail(const std::string& s);
    void printDetailIndent(const std::string& s, int indentLevel = 1);

    ////////////////////////////////////////////////////////////////////////////////
    static void newLine(MonitorSource source);
    static void printSeparator(MonitorSource source);
    static void printAligned(const std::string& s, MonitorSource source, char padding = PADDING, const std::string& wrapper = WRAPPER, int maxSize = 100);
    static void printGreeting(const std::string& s, MonitorSource source);
    static void printWarning(const std::string& s, MonitorSource source, int maxSize = 100);

    static void printLog(const std::string& s, MonitorSource source);
    static void printLogIndent(const std::string& s, MonitorSource source, int indentLevel = 1);

    static void printDetail(const std::string& s, MonitorSource source);
    static void printDetailIndent(const std::string& s, MonitorSource source, int indentLevel = 1);

    static void printDebug(const std::string& s);
    static void printDebugIndent(const std::string& s, int indentLevel = 1);

    ////////////////////////////////////////////////////////////////////////////////
    static void initialize();
    static void initialize(const std::string& dataPath);
    static void shutdown();
    static void checkCriticalCondition(bool condition, const std::string& err);
    static void criticalError(const std::string err, int maxSize = 100);

    static void        computeTotalRunTime();
    static std::string getTotalRunTime();

    static void        flushToFile();
    static std::string getSourceName(MonitorSource source);
    static void        getSourceName(MonitorSource source, const std::string& source_name);
    static int         getNumSources();

    void setLogSource(MonitorSource source)
    {
        m_LogSource = source;
    }

    static LogLevel s_LogLevel;

private:
    static void printToStdOut(const std::string& s);
    static void addToBuffer(const std::string& str);
    static void saveBufferToFile();

    static bool m_bPrintStdOut;
    static bool m_bWriteLogToFile;
    static bool m_bFileOpenFirstTime;
    static bool m_bDataPathReady;

    static std::string                           m_DataPath;
    static std::string                           m_LogFile;
    static std::string                           m_LogTimeFile;
    static std::chrono::system_clock::time_point m_StartupTime;
    static std::chrono::system_clock::time_point m_ShutdownTime;
    static std::string                           m_TotalRunTime;

    static std::map<MonitorSource, std::string> m_SourceNames;
    static tbb::concurrent_vector<std::string>  m_LogBuffer;

    MonitorSource m_LogSource;
};
