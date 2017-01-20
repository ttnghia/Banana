#ifndef __AVG_TIMER_H__
#define __AVG_TIMER_H__

#include <chrono>
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

    void tick();
    void tock();
    double getAvgTime();
    double getTotalTime();

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

#endif // __AVG_TIMER_H__
