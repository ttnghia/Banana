//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <chrono>
#include <cassert>
#include <QObject>

using Clock = std::chrono::high_resolution_clock;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AvgTimer : public QObject
{
    Q_OBJECT

public:
    AvgTimer(QObject *parent = nullptr,
             double updatePeriod = 2000) :
        QObject(parent),
        m_UpdatePeriod(updatePeriod),
        m_TickTockCount(0),
        m_TotalTime(0),
        m_isTimerStarted(false)
    {}

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void tick()
    {
        assert(!m_isTimerStarted);
        m_isTimerStarted = true;
        m_TickTime = Clock::now();

        if(m_TickTockCount == 0)
        {
            m_StartTime = Clock::now();
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void tock()
    {
        assert(m_isTimerStarted);
        m_TockTime = Clock::now();
        m_isTimerStarted = false;
        ++m_TickTockCount;

        std::chrono::duration<double, std::milli> ticktockDuration =
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (m_TockTime - m_TickTime);

        std::chrono::duration<double, std::milli> totalDuration =
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (m_TockTime - m_StartTime);

        m_TotalTime += ticktockDuration.count();

        if(totalDuration.count() >= m_UpdatePeriod)
        {
            double avgTime = m_TotalTime / static_cast<double>(m_TickTockCount);

            emit avgTimeChanged(avgTime);

            m_TickTockCount = 0;
            m_TotalTime = 0;

        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    double getAvgTime()
    {
        return m_TotalTime / static_cast<double>(m_TickTockCount);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    double getTotalTime()
    {
        return m_TotalTime;
    }


signals:
    void avgTimeChanged(double avgTime);

private:
    bool              m_isTimerStarted;
    int               m_TickTockCount;
    double            m_TotalTime;
    double            m_UpdatePeriod;
    Clock::time_point m_StartTime;
    Clock::time_point m_TickTime;
    Clock::time_point m_TockTime;
};
