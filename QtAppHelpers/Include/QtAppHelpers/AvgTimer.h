//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <chrono>
#include <cassert>
#include <QObject>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AvgTimer : public QObject
{
    Q_OBJECT

public:
    using Clock = std::chrono::high_resolution_clock;

    AvgTimer(QObject *parent = nullptr,
             double updatePeriod = 2000) :
        QObject(parent),
        m_UpdatePeriod(updatePeriod),
        m_TickTockCount(0),
        m_TotalTime(0),
        m_AvgTime(0),
        m_isTimerStarted(false)
    {}

    void tick();
    void tock();

    double getAvgTime();
    double getTickTockDuration();

signals:
    void avgTimeChanged(double avgTime);

private:
    bool              m_isTimerStarted;
    int               m_TickTockCount;
    double            m_TotalTime;
    double            m_AvgTime;
    double            m_UpdatePeriod;
    Clock::time_point m_StartTime;
    Clock::time_point m_TickTime;
    Clock::time_point m_TockTime;
};
