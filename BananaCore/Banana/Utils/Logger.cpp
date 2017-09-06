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
Logger::LogLevel Logger::s_LogLevel       = LogLevel::NormalLevel;
bool             Logger::s_bPrint2Console = true;
bool             Logger::s_bWriteLog2File = false;
bool             Logger::s_bInitialized   = false;

std::string                           Logger::s_DataPath    = std::string(".");
std::string                           Logger::s_TimeLogFile = std::string("time.txt");
std::chrono::system_clock::time_point Logger::s_StartupTime;
std::chrono::system_clock::time_point Logger::s_ShutdownTime;

std::shared_ptr<spdlog::sinks::stdout_sink_mt>      Logger::s_ConsoleSink    = spdlog::sinks::stdout_sink_mt::instance();
std::shared_ptr<spdlog::sinks::simple_file_sink_mt> Logger::s_LogFileSink    = nullptr;
std::shared_ptr<Logger>                             Logger::s_MainFuncLogger = nullptr;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// instance functions
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printSeparator()
{
    printAligned("", '=');
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printAligned(const std::string& s, char padding, const std::string& wrapper, unsigned int maxSize /*= 100*/)
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

    printLog(finalStr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printTextBox(const std::string& s)
{
    printSeparator();
    printAligned("", ' ');
    printAligned(s);
    printAligned("", ' ');
    printSeparator();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(const std::string& s, unsigned int maxSize)
{
    std::string str = "Warning: ";
    str += s;
    str += " ";

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += std::string(paddingSize, '*');
    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printError(const std::string& s, unsigned int maxSize)
{
    std::string str = "Error: ";
    str += s;
    str += " ";

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += std::string(paddingSize, '*');
    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const std::string& s)
{
    if(s_bPrint2Console)
        m_ConsoleLogger->info(s);

    if(s_bWriteLog2File)
        m_FileLogger->info(s);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const std::string& s, unsigned int indentLevel /*= 1*/)
{
    std::string str;
    str.reserve(256);
    str.append(std::string(INDENT_SIZE * indentLevel, ' '));
    str.append(s);

    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebug(const std::string& s)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        if(s_bPrint2Console)
        {
            m_ConsoleLogger->debug(s);
        }

        if(s_bWriteLog2File)
        {
            m_FileLogger->debug(s);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printDebugIndent(const std::string& s, unsigned int indentLevel /*= 1*/)
{
    if(s_LogLevel == LogLevel::DebugLevel)
    {
        std::string str;
        str.reserve(256);
        str.append(std::string(INDENT_SIZE * indentLevel, ' '));
        str.append(s);

        if(s_bPrint2Console)
        {
            m_ConsoleLogger->debug(str);
        }

        if(s_bWriteLog2File)
        {
            m_FileLogger->debug(str);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printMemoryUsage()
{
    std::string str;
    str.reserve(256);
    str += std::string("Memory usage: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getCurrentRSS()) / 1048576.0);
    str += std::string(" MB(s). Peak: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getPeakRSS()) / 1048576.0) + " MB(s).";

    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// static functions
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize(const std::string& dataPath)
{
    Logger::setDataPath(dataPath);
    Logger::initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize()
{
    s_StartupTime = std::chrono::system_clock::now();

    if(s_bWriteLog2File)
    {
        FileHelpers::createFolder(std::string(s_DataPath + "/Log"));
        unsigned int i = 1;
        std::string  file;

        do
        {
            file = s_DataPath + "/Log/log_" + std::to_string(i) + ".txt";

            if(!FileHelpers::fileExisted(file))
            {
                s_LogFileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>(file);
                s_TimeLogFile = s_DataPath + "/Log/time_" + std::to_string(i) + ".txt";
                break;
            }

            ++i;

            __BNN_ASSERT(i < 1000);
        } while(true);

        ////////////////////////////////////////////////////////////////////////////////
        time_t currentTime = std::chrono::system_clock::to_time_t(s_StartupTime - std::chrono::hours(24));

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

        FileHelpers::writeFile(strBuilder.str(), s_TimeLogFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    signal(SIGFPE,  Logger::signalHandler);
    signal(SIGINT,  Logger::signalHandler);
    signal(SIGSEGV, Logger::signalHandler);
    signal(SIGTERM, Logger::signalHandler);

    ////////////////////////////////////////////////////////////////////////////////
    s_bInitialized   = true;
    s_MainFuncLogger = Logger::create("Main");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::shutdown()
{
    s_ShutdownTime = std::chrono::system_clock::now();
    std::string totalRunTime = getTotalRunTime();

    if(s_bWriteLog2File)
    {
        time_t currentTime = std::chrono::system_clock::to_time_t(s_ShutdownTime - std::chrono::hours(24));;
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
        strBuilder << totalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), s_TimeLogFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    s_MainFuncLogger->printMemoryUsage();
    s_MainFuncLogger->printLog(totalRunTime);
    s_MainFuncLogger->printSeparator();
    s_MainFuncLogger->newLine();
    spdlog::drop_all();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename Rep, typename Period>
void getDuration(std::chrono::duration<Rep, Period> t, unsigned int& n_days, unsigned int& n_hours, unsigned int& n_mins, unsigned int& n_secs)
{
    assert(0 <= t.count());

    // approximate because a day doesn't have a fixed length
    typedef std::chrono::duration<int, std::ratio<60* 60* 24> > days_t;

    auto days  = std::chrono::duration_cast<days_t>(t);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(t - days);
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(t - days - hours);
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(t - days - hours - mins);

    n_days  = static_cast<unsigned int>(days.count());
    n_hours = static_cast<unsigned int>(hours.count());
    n_mins  = static_cast<unsigned int>(mins.count());
    n_secs  = static_cast<unsigned int>(secs.count());
}

std::string Logger::getTotalRunTime()
{
    unsigned int days, hours, mins, secs;
    getDuration(s_ShutdownTime - s_StartupTime, days, hours, mins, secs);

    std::stringstream strBuilder;
    strBuilder.str("");
    strBuilder << "Total time: "
               << days << "(days), "
               << hours << "(hours), "
               << mins << "(mins), "
               << secs << "(secs).";

    return strBuilder.str();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::signalHandler(int signum)
{
    s_MainFuncLogger->newLine();

    switch(signum)
    {
        case SIGFPE:
            s_MainFuncLogger->printWarning("Signal Floating-Point Exception");
            break;

        case SIGINT:
            s_MainFuncLogger->printWarning("Signal Interrupt(User pressed Ctrl+C)");
            break;

        case SIGSEGV:
            s_MainFuncLogger->printWarning("Signal Segmentation Violation");
            break;

        case SIGTERM:
            s_MainFuncLogger->printWarning("Signal Terminate");
            break;

        default:
            s_MainFuncLogger->printWarning("Unknown signal caught: " + std::to_string(signum));
    }

    s_MainFuncLogger->newLine();
    s_MainFuncLogger->printLog("Cleanup and exit program..........");
    shutdown();

    // Raise sig abort, which is not handled by this function
    // This will guarantee to terminate the application
    raise(SIGABRT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana