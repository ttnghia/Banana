//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/24/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/AvgTimer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AvgTimer::tick()
{
    assert(!m_isTimerStarted);
    m_isTimerStarted = true;
    m_TickTime = Clock::now();

    if(m_TickTockCount == 0)
    {
        m_StartTime = Clock::now();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AvgTimer::tock()
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
double AvgTimer::getAvgTime()
{
    return m_TotalTime / static_cast<double>(m_TickTockCount);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
double AvgTimer::getTickTockDuration()
{
    return m_TotalTime;
}
