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

#include <BananaCore/Logger.h>
#include <BananaCore/FileHelpers.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int  Logger::s_LogLevel        = LogLevel::NormalLevel;
bool Logger::m_bPrintStdOut    = true;
bool Logger::m_bWriteLogToFile = true;
bool Logger::m_bDataPathReady  = false;

std::string                           Logger::m_DataPath;
std::string                           Logger::m_LogFile;
std::string                           Logger::m_LogTimeFile;
std::chrono::system_clock::time_point Logger::m_StartupTime;
std::chrono::system_clock::time_point Logger::m_ShutdownTime;
std::string                           Logger::m_TotalRunTime;

std::map<int, std::string>                    Logger::m_SourceNames;
std::vector<std::shared_ptr<spdlog::logger> > Logger::m_ConsoleLogger;// = spdlog::stdout_color_mt("console");;
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
    Logger::printAligned(s, m_LogSourceID, padding, wrapper, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printGreeting(const std::string& s)
{
    Logger::printGreeting(s, m_LogSourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(const std::string& s, int maxSize)
{
    Logger::printWarning(s, m_LogSourceID, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const std::string& s)
{
    Logger::printLog(s, m_LogSourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const std::string& s, int indentLevel)
{
    Logger::printLogIndent(s, m_LogSourceID, indentLevel);
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
void Logger::printDetailIndent(const std::string& s, int indentLevel)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        printLogIndent(s, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printAligned(const std::string& s, int sourceID, char padding, const std::string& wrapper, int maxSize)
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

    Logger::printLog(finalStr, sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printGreeting(const std::string& s, int sourceID)
{
    Logger::printSeparator(sourceID);
    Logger::printAligned("", sourceID, ' ');
    Logger::printAligned(s,  sourceID);
    Logger::printAligned("", sourceID, ' ');
    Logger::printSeparator(sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(const std::string& s, int sourceID, int maxSize)
{
    const std::string str         = "Warning: " + s + " ";
    size_t            paddingSize = (static_cast<size_t>(maxSize) - str.length());

    Logger::printLog(str + std::string(paddingSize, '*'), sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::newLine(int sourceID)
{
    Logger::printLog("", sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printSeparator(int sourceID)
{
    Logger::printAligned("", sourceID, '=');
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const std::string& s, int sourceID)
{
    printToStdOut(s, sourceID);
    logToFile(s, sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const std::string& s, int sourceID, int indentLevel)
{
    std::string logStr;
    logStr.reserve(1024);
    logStr.append(std::string(INDENT_SIZE * indentLevel, ' '));
    logStr.append(s);

    Logger::printLog(logStr, sourceID);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetail(const std::string& s, int sourceID)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Logger::printLog(s, sourceID);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDetailIndent(const std::string& s, int sourceID, int indentLevel)
{
    if(s_LogLevel == LogLevel::DetailLevel)
    {
        Logger::printLogIndent(s, sourceID, indentLevel);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebug(const std::string& s)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Logger::printLog(s, Source::Debugger);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebugIndent(const std::string& s, int indentLevel)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        Logger::printLogIndent(s, Source::Debugger, indentLevel);
    }
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
void Logger::shutdown()
{
    m_ShutdownTime = std::chrono::system_clock::now();
    computeTotalRunTime();

    if(m_bWriteLogToFile)
    {
        assert(m_bDataPathReady);

        time_t            currentTime = std::chrono::system_clock::to_time_t(m_ShutdownTime - std::chrono::hours(24));;
        struct tm         localTime   = *localtime(&currentTime);
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Log finished at: " << localTime.tm_mon << "/" << localTime.tm_mday;
        strBuilder << "/" << (localTime.tm_year + 1900) << ", " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;
        strBuilder << m_TotalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), m_LogTimeFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    Logger::printLog(m_TotalRunTime, Source::LoggerClass);
    Logger::printSeparator(Source::LoggerClass);
    Logger::newLine(Source::LoggerClass);
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
    typedef std::chrono::duration<int, std::ratio<60* 60* 24> >   days_t;

    auto days  = std::chrono::duration_cast<days_t>(t);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(t - days);
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(t - days - hours);
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(t - days - hours - mins);

    n_days  = days.count();
    n_hours = hours.count();
    n_mins  = mins.count();
    n_secs  = secs.count();
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
int Logger::getNumSources()
{
    return static_cast<int>(m_SourceNames.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::setSourceName(int sourceID, const std::string& sourceName)
{
    m_SourceNames[sourceID] = sourceName;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printToStdOut(const std::string& s, int sourceID)
{
    if(m_bPrintStdOut)
    {
        m_ConsoleLogger[sourceID]->info(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::logToFile(const std::string& s, int sourceID)
{
    if(!m_bWriteLogToFile)
    {
        return;
    }

    assert(m_bDataPathReady);

    m_FileLogger[sourceID]->info(s);
}
