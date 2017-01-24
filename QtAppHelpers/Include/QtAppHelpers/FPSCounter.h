//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/23/2017
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
#include <QObject>

using Clock = std::chrono::high_resolution_clock;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FPSCounter : public QObject
{
    Q_OBJECT

public:
    FPSCounter(QObject * parent = nullptr,
               double updatePeriod = 2000) :
        QObject(parent),
        m_Counter(0),
        m_UpdatePeriod(updatePeriod)
    {
        m_StartTime = Clock::now();
    }

    void countFrame()
    {
        Clock::time_point countTime = Clock::now();
        ++m_Counter;

        std::chrono::duration<double, std::milli> totalDuration =
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (countTime - m_StartTime);


        if(totalDuration.count() >= m_UpdatePeriod)
        {
            double totalTimeInSec = totalDuration.count() / 1000.0;
            double fps = static_cast<double>(m_Counter) / totalTimeInSec;

            m_Counter = 0;
            m_StartTime = countTime;

            emit fpsChanged(fps);
        }
    }

signals:
    void fpsChanged(double fps);

private:
    int               m_Counter;
    double            m_UpdatePeriod;
    Clock::time_point m_StartTime;
};
