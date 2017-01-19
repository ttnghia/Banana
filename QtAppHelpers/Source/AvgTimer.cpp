//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 9/28/2016
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
//------------------------------------------------------------------------------------------

#include <QtAppHelpers/AvgTimer.h>
#include <cassert>

//------------------------------------------------------------------------------------------
void AvgTimer::tick()
{
    assert(!timer_started);
    tick_time = std::chrono::high_resolution_clock::now();
    timer_started = true;

    if(ticktock_count == 0)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }
}

//------------------------------------------------------------------------------------------
void AvgTimer::tock()
{
    assert(timer_started);
    tock_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> ticktock_duration =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli> >
        (tock_time - tick_time);

    std::chrono::duration<double, std::milli> timer_duration =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli> >
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

//------------------------------------------------------------------------------------------
double AvgTimer::getAvgTime()
{
    return total_time / (double)ticktock_count;
}

//------------------------------------------------------------------------------------------
double AvgTimer::getTotalTime()
{
    return total_time;
}
