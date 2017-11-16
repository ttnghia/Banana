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
#include <cassert>
#include <QObject>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AvgTimer : public QObject
{
    Q_OBJECT

    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double, std::milli>;

public:
    AvgTimer(QObject* parent = nullptr, double updatePeriod = 2000) : QObject(parent), m_UpdatePeriod(updatePeriod) {}

    void tick()
    {
        assert(!m_isTimerStarted);
        m_isTimerStarted = true;
        m_TickTime       = Clock::now();

        if(m_TickTockCount == 0) {
            m_StartTime = Clock::now();
        }
    }

    void tock()
    {
        assert(m_isTimerStarted);
        m_TockTime       = Clock::now();
        m_isTimerStarted = false;
        ++m_TickTockCount;

        auto ticktockDuration = std::chrono::duration_cast<Duration>(m_TockTime - m_TickTime);
        auto totalDuration    = std::chrono::duration_cast<Duration>(m_TockTime - m_StartTime);

        m_TotalTime += ticktockDuration.count();

        if(totalDuration.count() >= m_UpdatePeriod) {
            m_AvgTime = m_TotalTime / static_cast<double>(m_TickTockCount);
            emit avgTimeChanged(m_AvgTime);

            m_TickTockCount = 0;
            m_TotalTime     = 0;
        }
    }

    auto getAvgTime() const { return m_AvgTime; }
    auto getTickTockDuration() const { return m_TotalTime; }
    void setUpdatePeriod(double updatePeriod) { m_UpdatePeriod = updatePeriod; }

signals:
    void avgTimeChanged(double avgTime);

private:
    bool   m_isTimerStarted = false;
    int    m_TickTockCount  = 0;
    double m_TotalTime      = 0;
    double m_AvgTime        = 0;

    double            m_UpdatePeriod;
    Clock::time_point m_StartTime;
    Clock::time_point m_TickTime;
    Clock::time_point m_TockTime;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana