//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/Formatters.h>
#include <Banana/System/MemoryUsage.h>

#include <csignal>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool Logger::s_bPrint2Console = true;
bool Logger::s_bWriteLog2File = false;
bool Logger::s_bInitialized   = false;
bool Logger::s_bShutdown      = false;

String                    Logger::s_DataPath    = String(".");
String                    Logger::s_TimeLogFile = String("time.txt");
Logger::Clock::time_point Logger::s_StartupTime;
Logger::Clock::time_point Logger::s_ShutdownTime;

Vector<String>                                                  Logger::s_DataLogFiles;
std::map<String, SharedPtr<spdlog::sinks::simple_file_sink_mt>> Logger::s_DataLogSinks;

#ifdef __BANANA_WINDOWS__
SharedPtr<spdlog::sinks::wincolor_stdout_sink_mt> Logger::s_ConsoleSink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
SharedPtr<spdlog::sinks::ansicolor_stdout_sink_mt> Logger::s_ConsoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
SharedPtr<spdlog::sinks::simple_file_sink_mt> Logger::s_SystemLogFileSink = nullptr;
SharedPtr<Logger>                             Logger::s_MainLogger        = nullptr;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Logger::Logger(const String& loggerName, bool bDefaultLogPolicy, bool bPrint2Console, bool bWriteLog2File, bool bDataLogger)
{
    __BNN_REQUIRE(s_bInitialized);

    if(bDefaultLogPolicy) {
        m_bPrint2Console = s_bPrint2Console;
        m_bWriteLog2File = s_bWriteLog2File;
    } else {
        m_bPrint2Console = bPrint2Console;
        m_bWriteLog2File = bWriteLog2File;
    }

    if(m_bPrint2Console) {
        m_ConsoleLogger = std::make_shared<spdlog::logger>(loggerName, s_ConsoleSink);
    }

    if(m_bWriteLog2File) {
        if(!bDataLogger) {
            __BNN_REQUIRE(s_SystemLogFileSink != nullptr);
            m_FileLogger = std::make_shared<spdlog::async_logger>(loggerName, s_SystemLogFileSink, 1024);
        } else {
            __BNN_REQUIRE(s_DataLogSinks.find(loggerName) != s_DataLogSinks.end() && s_DataLogSinks[loggerName] != nullptr);
            m_FileLogger = std::make_shared<spdlog::async_logger>(loggerName, s_DataLogSinks[loggerName], 1024);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::setLoglevel(spdlog::level::level_enum level)
{
    if(m_ConsoleLogger != nullptr) {
        m_ConsoleLogger->set_level(level);
    }

    if(m_FileLogger != nullptr) {
        m_FileLogger->set_level(level);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printSeparator()
{
    printAligned("", '=');
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printAligned(const String& s, char padding, const String& wrapper, UInt maxSize /*= 100*/)
{
    size_t       length = s.length();
    const String str    = length == 0 ? s : String(" " + s + " ");
    length = str.length();

    size_t paddingSize = ((size_t)maxSize - length - 2 * wrapper.length()) / 2;
    size_t finalLength = wrapper.length() * 2 + length + paddingSize * 2;

    String finalStr;
    finalStr.reserve(finalLength + 1);

    finalStr += wrapper;
    finalStr += String(paddingSize, padding);
    finalStr += str;
    finalStr += (finalLength == static_cast<size_t>(maxSize)) ? String(paddingSize, padding) : String(paddingSize + 1, padding);
    finalStr += wrapper;

    printLog(finalStr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printTextBox(const String& s)
{
    printSeparator();
    printAligned("", ' ');
    printAligned(s);
    printAligned("", ' ');
    printSeparator();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printTextBox(const Vector<String>& strs)
{
    printSeparator();
    printAligned("", ' ');
    for(const auto& s: strs) {
        printAligned(s);
    }
    printAligned("", ' ');
    printSeparator();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printWarning(const String& s, UInt maxSize)
{
    printLogPadding(s, spdlog::level::warn, maxSize);
}

void Logger::printWarningIndent(const String& s, UInt indentLevel /* = 1 */, char trailing /* = ' ' */, UInt maxSize /* = 100 */)
{
    printLogPaddingIndent(s, spdlog::level::warn, indentLevel, trailing, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printError(const String& s, UInt maxSize)
{
    printLogPadding(s, spdlog::level::err, maxSize);
}

void Logger::printErrorIndent(const String& s, UInt indentLevel /* = 1 */, char trailing /* = ' ' */, UInt maxSize /* = 100 */)
{
    printLogPaddingIndent(s, spdlog::level::err, indentLevel, trailing, maxSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const String& s)
{
    if(m_bPrint2Console) {
        m_ConsoleLogger->info(s);
    }

    if(m_bWriteLog2File) {
        m_FileLogger->info(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const String& s, spdlog::level::level_enum level)
{
    if(m_bPrint2Console) {
        m_ConsoleLogger->log(level, s);
    }

    if(m_bWriteLog2File) {
        m_FileLogger->log(level, s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const String& s, UInt indentLevel /*= 1*/, char trailing /*= ' '*/)
{
    String str;
    str.reserve(256);
    str += String(INDENT_SIZE * indentLevel, trailing);
    str += s;

    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogPadding(const String& s, UInt maxSize /*= 100*/)
{
    auto str = s;
    str += String(" ");
    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');

    if(m_bPrint2Console) {
        m_ConsoleLogger->info(str);
    }

    if(m_bWriteLog2File) {
        m_FileLogger->info(str);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogPadding(const String& s, spdlog::level::level_enum level, UInt maxSize /*= 100*/)
{
    auto str = s;
    str += String(" ");
    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');

    if(m_bPrint2Console) {
        m_ConsoleLogger->log(level, str);
    }

    if(m_bWriteLog2File) {
        m_FileLogger->log(level, str);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogPaddingIndent(const String& s, UInt indentLevel /*= 1*/, char trailing /*= ' '*/, UInt maxSize /* = 100 */)
{
    String str;
    str.reserve(256);
    str += String(INDENT_SIZE * indentLevel, trailing);
    str += s;
    str += String(" ");

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');

    printLog(str);
}

void Logger::printLogPaddingIndent(const String& s, spdlog::level::level_enum level, UInt indentLevel /*= 1*/, char trailing /*= ' '*/, UInt maxSize /* = 100 */)
{
    String str;
    str.reserve(256);
    str += String(INDENT_SIZE * indentLevel, trailing);
    str += s;
    str += String(" ");

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');

    if(m_bPrint2Console) {
        m_ConsoleLogger->log(level, str);
    }

    if(m_bWriteLog2File) {
        m_FileLogger->log(level, str);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIf(bool bCondition, const String& s)
{
    if(bCondition) {
        printLog(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIf(bool bCondition, const String& s, spdlog::level::level_enum level)
{
    if(bCondition) {
        printLog(s, level);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndentIf(bool bCondition, const String& s, UInt indentLevel /*= 1*/, char trailing /*= ' '*/)
{
    if(bCondition) {
        printLogIndent(s, indentLevel, trailing);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printMemoryUsage()
{
    String str;
    str.reserve(256);
    str += String("Memory usage: ");
    str += Formatters::toString(static_cast<double>(getCurrentRSS()) / 1048576.0);
    str += String(" MB(s). Peak: ");
    str += Formatters::toString(static_cast<double>(getPeakRSS()) / 1048576.0) + " MB(s).";

    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// static functions
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize(const String& dataPath, bool bPrint2Console /*= true*/, bool bWriteLog2File /*= false*/)
{
    Logger::setDataPath(dataPath);
    Logger::initialize(bPrint2Console, bWriteLog2File);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::initialize(bool bPrint2Console /*= true*/, bool bWriteLog2File /*= false*/)
{
    s_bPrint2Console = bPrint2Console;
    s_bWriteLog2File = bWriteLog2File;
    s_StartupTime    = Clock::now();

    if(s_bWriteLog2File) {
        FileHelpers::createFolder(String(s_DataPath + "/Log"));
        UInt   i = 1;
        String file;

        do {
            file = s_DataPath + String("/Log/log_") + std::to_string(i) + String(".txt");

            if(!FileHelpers::fileExisted(file)) {
                s_SystemLogFileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>(file);
                s_TimeLogFile       = s_DataPath + String("/Log/time_") + std::to_string(i) + String(".txt");

                // create loggers for data, if any
                for(const auto& dataFile: s_DataLogFiles) {
                    file                     = s_DataPath + "/Log/" + dataFile + String("_") + std::to_string(i) + String(".txt");
                    s_DataLogSinks[dataFile] = std::make_shared<spdlog::sinks::simple_file_sink_mt>(file);
                }
                break;
            }

            ++i;

            __BNN_REQUIRE(i < 1000);
        } while(true);

        ////////////////////////////////////////////////////////////////////////////////
        time_t currentTime = Clock::to_time_t(s_StartupTime - std::chrono::hours(24));

#ifdef __BANANA_WINDOWS__
        struct tm ltime;
        localtime_s(&ltime, &currentTime);
#else
        struct tm ltime = *localtime(&currentTime);
#endif

        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Logger initialized at: " << ltime.tm_mon << "/" << ltime.tm_mday;
        strBuilder << "/" << (ltime.tm_year + 1900) << ", " << ltime.tm_hour << ":" << ltime.tm_min << ":" << ltime.tm_sec << std::endl;

        FileHelpers::writeFile(strBuilder.str(), s_TimeLogFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    signal(SIGFPE,  Logger::signalHandler);
    signal(SIGINT,  Logger::signalHandler);
    signal(SIGSEGV, Logger::signalHandler);
    signal(SIGTERM, Logger::signalHandler);

    ////////////////////////////////////////////////////////////////////////////////
    s_bInitialized = true;
    s_bShutdown    = false;
    s_MainLogger   = Logger::createLogger("Main");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::shutdown()
{
    if(s_bShutdown) {
        return;
    }
    s_bInitialized = false;
    s_bShutdown    = true;
    ////////////////////////////////////////////////////////////////////////////////
    s_ShutdownTime = Clock::now();
    String totalRunTime = getTotalRunTime();

    if(s_bWriteLog2File) {
        time_t currentTime = Clock::to_time_t(s_ShutdownTime - std::chrono::hours(24));;
#ifdef __BANANA_WINDOWS__
        struct tm ltime;
        localtime_s(&ltime, &currentTime);
#else
        struct tm ltime = *localtime(&currentTime);
#endif
        std::stringstream strBuilder;
        strBuilder.str("");
        strBuilder << "Logger shut down at: " << ltime.tm_mon << "/" << ltime.tm_mday;
        strBuilder << "/" << (ltime.tm_year + 1900) << ", " << ltime.tm_hour << ":" << ltime.tm_min << ":" << ltime.tm_sec << "\n";
        strBuilder << totalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), s_TimeLogFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    s_MainLogger->printMemoryUsage();
    s_MainLogger->printLogPadding(totalRunTime, 120);
    s_MainLogger->printLog("\n");
    spdlog::drop_all();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename Rep, typename Period>
void getDuration(std::chrono::duration<Rep, Period> t, UInt& n_days, UInt& n_hours, UInt& n_mins, UInt& n_secs)
{
    assert(0 <= t.count());

    // approximate because a day doesn't have a fixed length
    typedef std::chrono::duration<int, std::ratio<60* 60* 24>> days_t;

    auto days  = std::chrono::duration_cast<days_t>(t);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(t - days);
    auto mins  = std::chrono::duration_cast<std::chrono::minutes>(t - days - hours);
    auto secs  = std::chrono::duration_cast<std::chrono::seconds>(t - days - hours - mins);

    n_days  = static_cast<UInt>(days.count());
    n_hours = static_cast<UInt>(hours.count());
    n_mins  = static_cast<UInt>(mins.count());
    n_secs  = static_cast<UInt>(secs.count());
}

String Logger::getTotalRunTime()
{
    UInt days, hours, mins, secs;
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
    s_MainLogger->newLine();

    switch(signum) {
        case SIGFPE:
            s_MainLogger->printWarning("Signal Floating-Point Exception");
            break;
        case SIGINT:
            s_MainLogger->printWarning("Signal Interrupt(User pressed Ctrl+C)");
            break;
        case SIGSEGV:
            s_MainLogger->printWarning("Signal Segmentation Violation");
            break;
        case SIGTERM:
            s_MainLogger->printWarning("Signal Terminate");
            break;
        default:
            s_MainLogger->printWarning("Unknown signal caught: " + std::to_string(signum));
    }

    s_MainLogger->newLine();
    s_MainLogger->printLog("Cleanup and exit program..........");
    shutdown();

    // Raise sig abort, which is not handled by this function
    // This will guarantee to terminate the application
    raise(SIGABRT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana