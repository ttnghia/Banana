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

#include <chrono>
#include <string>
#include <cassert>
#include <sstream>
#include <iomanip>

#include <Banana/Utils/NumberHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Timer
{
    using Clock = std::chrono::high_resolution_clock;
public:
    Timer() : m_TimerTicked(false) {}

    void tick()
    {
        assert(!m_TimerTicked);
        m_StartTime   = Clock::now();
        m_TimerTicked = true;
    }

    double tock()
    {
        assert(m_TimerTicked);
        m_EndTime     = Clock::now();
        m_TimerTicked = false;
        m_ElapsedTime = std::chrono::duration<double, std::milli>(m_EndTime - m_StartTime).count();

        return m_ElapsedTime;
    }

    String getRunTime()
    {
        if(m_TimerTicked) {
            tock();
        }

        m_StrBuilder.str("");
        m_StrBuilder << NumberHelpers::formatWithCommas(m_ElapsedTime);
        m_StrBuilder << "ms";

        return m_StrBuilder.str();
    }

    String getRunTime(const char* caption)
    {
        if(m_TimerTicked) {
            tock();
        }

        m_StrBuilder.str("");
        m_StrBuilder << caption;
        m_StrBuilder << NumberHelpers::formatWithCommas(m_ElapsedTime);
        m_StrBuilder << "ms";

        return m_StrBuilder.str();
    }

    template<class Function>
    static String getRunTime(const char* caption, const Function& function)
    {
        Timer timer;

        ////////////////////////////////////////////////////////////////////////////////
        timer.tick();
        function();
        timer.tock();
        ////////////////////////////////////////////////////////////////////////////////

        return timer.getRunTime(caption);
    }

private:
    Clock::time_point m_StartTime;
    Clock::time_point m_EndTime;
    std::stringstream m_StrBuilder;
    double            m_ElapsedTime;
    bool              m_TimerTicked;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana