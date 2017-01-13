//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/12/2017
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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <string>
#include <cassert>

#include <Banana/NumberHelpers.h>
//------------------------------------------------------------------------------------------
class Timer
{
public:
    Timer() : ticked(false)
    {}

    void tick()
    {
        assert(!ticked);

        start_time = std::chrono::high_resolution_clock::now();
        ticked = true;
    }

    double tock()
    {
        assert(ticked);

        end_time = std::chrono::high_resolution_clock::now();
        ticked = false;

        elapsed_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        return elapsed_time;
    }

    std::string get_run_time()
    {
        if(ticked)
        {
            tock();
        }

        return NumberHelpers::format_with_commas(elapsed_time) + "ms";
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    double elapsed_time;

    bool ticked;
};

#endif // __TIMER_H__
