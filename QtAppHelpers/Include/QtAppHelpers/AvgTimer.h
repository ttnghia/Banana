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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AvgTimer : public QObject
{
    Q_OBJECT

public:
    AvgTimer(double _updateTime = 2000) :
        update_time(_updateTime),
        ticktock_count(0),
        timer_started(false)
    {}

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void tick()
    {
        assert(!timer_started);
        tick_time = std::chrono::high_resolution_clock::now();
        timer_started = true;

        if(ticktock_count == 0)
        {
            start_time = std::chrono::high_resolution_clock::now();
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void tock()
    {
        assert(timer_started);
        tock_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> ticktock_duration =
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (tock_time - tick_time);

        std::chrono::duration<double, std::milli> timer_duration =
            std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
            (tock_time - start_time);

        total_time += ticktock_duration.count();
        timer_started = false;
        ++ticktock_count;

        if(timer_duration.count() >= update_time)
        {
            double avg_time = total_time / (double)ticktock_count;

            //        qDebug() << "FPS: " << 1000.0 / renderTime << ", t = " << renderTime;
            emit avgTimeChanged(avg_time);

            ticktock_count = 0;
            total_time = 0;

        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    double getAvgTime()
    {
        return total_time / (double)ticktock_count;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    double getTotalTime()
    {
        return total_time;
    }


signals:
    void avgTimeChanged(double average_time);

private:
    bool timer_started;
    double total_time;
    double update_time;
    int ticktock_count;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point tick_time;
    std::chrono::high_resolution_clock::time_point tock_time;
};
