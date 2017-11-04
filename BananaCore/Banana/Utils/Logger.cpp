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

#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/NumberHelpers.h>

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

#ifdef __BANANA_WINDOWS__
SharedPtr<spdlog::sinks::wincolor_stdout_sink_mt> Logger::s_ConsoleSink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
SharedPtr<spdlog::sinks::ansicolor_stdout_sink_mt> Logger::s_ConsoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
SharedPtr<spdlog::sinks::simple_file_sink_mt> Logger::s_LogFileSink = nullptr;
SharedPtr<Logger>                             Logger::s_MainLogger  = nullptr;

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

    finalStr.append(wrapper);
    finalStr.append(String(paddingSize, padding));
    finalStr.append(str);
    finalStr.append((finalLength == static_cast<size_t>(maxSize)) ? String(paddingSize, padding) : String(paddingSize + 1, padding));
    finalStr.append(wrapper);

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
void Logger::printWarning(const String& s, UInt maxSize)
{
    String str = s;
    str += String(" ");

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');
    printLog(str, spdlog::level::warn);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printError(const String& s, UInt maxSize)
{
    String str = s;
    str += String(" ");

    size_t paddingSize = (static_cast<size_t>(maxSize) - str.length());
    str += String(paddingSize, '*');
    printLog(str, spdlog::level::err);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const String& s)
{
    if(s_bPrint2Console) {
        m_ConsoleLogger->info(s);
    }

    if(s_bWriteLog2File) {
        m_FileLogger->info(s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLog(const String& s, spdlog::level::level_enum level)
{
    if(s_bPrint2Console) {
        m_ConsoleLogger->log(level, s);
    }

    if(s_bWriteLog2File) {
        m_FileLogger->log(level, s);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printLogIndent(const String& s, UInt indentLevel /*= 1*/, char trailing /*= ' '*/)
{
    String str;
    str.reserve(256);
    str.append(String(INDENT_SIZE * indentLevel, trailing));
    str.append(s);

    printLog(str);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::printMemoryUsage()
{
    String str;
    str.reserve(256);
    str += String("Memory usage: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getCurrentRSS()) / 1048576.0);
    str += String(" MB(s). Peak: ");
    str += NumberHelpers::formatWithCommas(static_cast<double>(getPeakRSS()) / 1048576.0) + " MB(s).";

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
            file = s_DataPath + "/Log/log_" + std::to_string(i) + ".txt";

            if(!FileHelpers::fileExisted(file)) {
                s_LogFileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>(file);
                s_TimeLogFile = s_DataPath + "/Log/time_" + std::to_string(i) + ".txt";
                break;
            }

            ++i;

            __BNN_ASSERT(i < 1000);
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
    s_MainLogger   = Logger::create("Main");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Logger::shutdown()
{
    if(s_bShutdown) {
        return;
    }
    s_bShutdown = true;

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
        strBuilder << "/" << (ltime.tm_year + 1900) << ", " << ltime.tm_hour << ":" << ltime.tm_min << ":" << ltime.tm_sec << std::endl;
        strBuilder << totalRunTime;

        FileHelpers::appendToFile(strBuilder.str(), s_TimeLogFile);
    }

    ////////////////////////////////////////////////////////////////////////////////
    s_MainLogger->printMemoryUsage();
    s_MainLogger->printLog(totalRunTime);
    s_MainLogger->printSeparator();
    s_MainLogger->newLine();
    spdlog::drop_all();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename Rep, typename Period>
void getDuration(std::chrono::duration<Rep, Period> t, UInt& n_days, UInt& n_hours, UInt& n_mins, UInt& n_secs)
{
    assert(0 <= t.count());

    // approximate because a day doesn't have a fixed length
    typedef std::chrono::duration<int, std::ratio<60 * 60 * 24> > days_t;

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