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

#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/NumberHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Logger::LogLevel Logger::s_LogLevel        = LogLevel::NormalLevel;
bool             Logger::m_bPrintStdOut    = true;
bool             Logger::m_bWriteLogToFile = true;
bool             Logger::m_bDataPathReady  = false;
unsigned int     Logger::m_NumSources      = 0;

std::string                           Logger::m_DataPath;
std::string                           Logger::m_LogFile;
std::string                           Logger::m_LogTimeFile;
std::chrono::system_clock::time_point Logger::m_StartupTime;
std::chrono::system_clock::time_point Logger::m_ShutdownTime;
std::string                           Logger::m_TotalRunTime;

std::map<int, std::string>                    Logger::m_SourceNames = { { Logger::MainProgram, "Main" }, { Logger::LoggerClass, "Logger" }, { Logger::Debugger, "Debugger" } };
std::vector<std::shared_ptr<spdlog::logger> > Logger::m_ConsoleLogger;    // = spdlog::stdout_color_mt("console");;
std::vector<std::shared_ptr<spdlog::logger> > Logger::m_FileLogger;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::setDataPath(const std::string& dataPath)
{
    m_DataPath = dataPath;

    int         i = 1;
    std::string file;

    do
    {
        file = m_DataPath + "/Log/log_" + std::to_string(i) + ".txt";

        if(!FileHelpers::fileExisted(file))
        {
            m_LogFile     = file;
            m_LogTimeFile = m_DataPath + "/Log/time_" + std::to_string(i) + ".txt";
            break;
        }

        ++i;

        assert(i < 1000);
    } while(true);

    m_bDataPathReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::newLine()
{
    printLog("");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printSeparator()
{
    Logger::printSeparator(m_LogSourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printAligned(const std::string& s, char padding, const std::string& wrapper, int maxSize)
{
    Logger::printAligned(m_LogSourceID, s, padding, wrapper, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printGreeting(const std::string& s)
{
    Logger::printGreeting(m_LogSourceID, s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(const std::string& s, int maxSize)
{
    Logger::printWarning(m_LogSourceID, s, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::Logger::printTime(const char* caption, Timer& timer)
{
    Logger::printLog(m_LogSourceID, timer.getRunTime(caption));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Function>
void Banana::Logger::printTimeIndent(const char* caption, const Function& function)
{
    Logger::printLogIndent(m_LogSourceID, Timer::getRunTime(caption, function()));
}

void Banana::Logger::printTimeIndent(const char* caption, Timer& timer)
{
    Logger::printLogIndent(m_LogSourceID, timer.getRunTime(caption));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const std::string& s)
{
    Logger::printLog(m_LogSourceID, s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const std::string& s, int indentLevel /*= 1*/)
{
    Logger::printLogIndent(m_LogSourceID, s, indentLevel);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetail(const std::string& s)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        printLog(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetailIndent(const std::string& s, int indentLevel /*= 1*/)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        printLogIndent(s, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printAligned(int sourceID, const std::string& s, char padding, const std::string& wrapper, int maxSize)
{
    size_t            length = s.length();
    const std::string str    = length == 0 ? s : std::string(" " + s + " ");
    length = str.length();

    size_t paddingSize = ((size_t)maxSize - length - 2 * wrapper.length()) / 2;
    size_t finalLength = wrapper.length() * 2 + length + paddingSize * 2;

    std::string finalStr;
    finalStr.reserve(finalLength + 1);

    finalStr.append(wrapper);
    finalStr.append(std::string(paddingSize, padding));
    finalStr.append(str);
    finalStr.append((finalLength == static_cast<size_t>(maxSize)) ? std::string(paddingSize, padding) : std::string(paddingSize + 1, padding));
    finalStr.append(wrapper);

    Logger::printLog(sourceID, finalStr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printGreeting(int sourceID, const std::string& s)
{
    Logger::printSeparator(sourceID);
    Logger::printAligned(sourceID, "", ' ');
    Logger::printAligned(sourceID, s);
    Logger::printAligned(sourceID, "", ' ');
    Logger::printSeparator(sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(int sourceID, const std::string& s, int maxSize)
{
    const std::string str         = "Warning: " + s + " ";
    size_t            paddingSize = (static_cast<size_t>(maxSize) - str.length());

    Logger::printLog(sourceID, str + std::string(paddingSize, '*'));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::newLine(int sourceID)
{
    Logger::printLog(sourceID, "");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printSeparator(int sourceID)
{
    Logger::printAligned(sourceID, "", '=');
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(int sourceID, const std::string& s)
{
    printToStdOut(sourceID, s);
    logToFile(sourceID, s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(int sourceID, const std::string& s, int indentLevel /*= 1*/)
{
    std::string logStr;
    logStr.reserve(1024);
    logStr.append(std::string(INDENT_SIZE * indentLevel, ' '));
    logStr.append(s);

    Logger::printLog(sourceID, logStr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetail(int sourceID, const std::string& s)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Logger::printLog(sourceID, s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetailIndent(int sourceID, const std::string& s, int indentLevel /*= 1*/)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Logger::printLogIndent(sourceID, s, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebug(const std::string& s)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Logger::printLog(static_cast<int>(Source::Debugger), s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebugIndent(const std::string& s, int indentLevel /*= 1*/)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Logger::printLogIndent(static_cast<int>(Source::Debugger), s, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printMemoryUsage()
{
    Logger::printMemoryUsage(m_LogSourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printMemoryUsage(int sourceID)
{
    std::string str;
    str.reserve(1024);
    str += std::string("Memory usage: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getCurrentRSS()) / 1048576.0);
    str += std::string(" MB(s). Peak: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getPeakRSS()) / 1048576.0) + " MB(s).";

    Logger::printLog(sourceID, str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize(const std::string& dataPath)
{
    Logger::setDataPath(dataPath);
    Logger::initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize()
{
    m_StartupTime = std::chrono::system_clock::now();

    if(m_bWriteLogToFile)
    {
        FileHelpers::createFolder(std::string(m_DataPath + "/Log"));
        time_t currentTime = std::chrono::system_clock::to_time_t(m_StartupTime - std::chrono::hours(24));
        assert(m_bDataPathReady);
#ifdef __BANANA_WINDOWS__
        struct tm ltime;
        localtime_s(&ltime, &currentTime);
#else
        struct tm ltime = *localtime(&currentTime);
#endif
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Log created at: " << ltime.tm_mon << "/" << ltime.tm_mday;
        strBuilder << "/" << (ltime.tm_year + 1900) << ", " << ltime.tm_hour << ":" << ltime.tm_min << ":" << ltime.tm_sec << std::endl;

        FileHelpers::writeFile(strBuilder.str(), m_LogTimeFile);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::shutdown()
{
    m_ShutdownTime = std::chrono::system_clock::now();
    computeTotalRunTime();

    if(m_bWriteLogToFile)
    {
        assert(m_bDataPathReady);

        time_t currentTime = std::chrono::system_clock::to_time_t(m_ShutdownTime - std::chrono::hours(24));;
#ifdef __BANANA_WINDOWS__
        struct tm ltime;
        localtime_s(&ltime, &currentTime);
#else
        struct tm ltime = *localtime(&currentTime);
#endif
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Log finished at: " << ltime.tm_mon << "/" << ltime.tm_mday;
        strBuilder << "/" << (ltime.tm_year + 1900) << ", " << ltime.tm_hour << ":" << ltime.tm_min << ":" << ltime.tm_sec << std::endl;
        strBuilder << m_TotalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), m_LogTimeFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    Logger::printLog(static_cast<int>(Source::LoggerClass), m_TotalRunTime);
    Logger::printSeparator(static_cast<int>(Source::LoggerClass));
    Logger::newLine(static_cast<int>(Source::LoggerClass));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string Logger::getTotalRunTime()
{
    assert(m_TotalRunTime != "");
    return m_TotalRunTime;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename Rep, typename Period>
void getDuration(std::chrono::duration<Rep, Period> t, int& n_days, int& n_hours, int& n_mins, int& n_secs)
{
    assert(0 <= t.count());

    // approximate because a day doesn't have a fixed length
    typedef std::chrono::duration<int, std::ratio<60* 60* 24> > days_t;

    auto days  = std::chrono::duration_cast<days_t>(t);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(t - days);
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(t - days - hours);
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(t - days - hours - mins);

    n_days  = static_cast<int>(days.count());
    n_hours = static_cast<int>(hours.count());
    n_mins  = static_cast<int>(mins.count());
    n_secs  = static_cast<int>(secs.count());
}

void Logger::computeTotalRunTime()
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
std::string Logger::getSourceName(int sourceID)
{
    if(m_SourceNames.find(sourceID) != m_SourceNames.end())
    {
        return m_SourceNames[sourceID];
    }
    else
    {
        return std::string("UnknownSource");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
IntType Logger::getNumSources()
{
    return static_cast<IntType>(m_SourceNames.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::setSourceName(int sourceID, const std::string& sourceName)
{
    m_SourceNames[sourceID] = sourceName;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printToStdOut(int sourceID, const std::string& s)
{
    if(m_bPrintStdOut)
    {
        m_ConsoleLogger[sourceID]->info(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::logToFile(int sourceID, const std::string& s)
{
    if(!m_bWriteLogToFile)
    {
        return;
    }

    assert(m_bDataPathReady);

    m_FileLogger[sourceID]->info(s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana