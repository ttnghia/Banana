//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Macros.h>
#include <Banana/ParameterManager.h>
#include <Banana/Timer.h>

#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <string>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Parameters
{
public:
    float param1 = 0;
    float param2 = 0;
    float param3 = 0;
    float param4 = 0;
    float param5 = 0;
    float param6 = 0;
    float param7 = 0;
    float param8 = 0;

    void set(int idx)
    {
        switch(idx)
        {
            case 1:
                param1 += float(rand()) / float(RAND_MAX);
                break;
            case 2:
                param2 += float(rand()) / float(RAND_MAX);
                break;
            case 3:
                param3 += float(rand()) / float(RAND_MAX);
                break;
            case 4:
                param4 += float(rand()) / float(RAND_MAX);
                break;
            case 5:
                param5 += float(rand()) / float(RAND_MAX);
                break;
            case 6:
                param6 += float(rand()) / float(RAND_MAX);
                break;
            case 7:
                param7 += float(rand()) / float(RAND_MAX);
                break;
            case 8:
                param8 += float(rand()) / float(RAND_MAX);
                break;
        }
    }

    float get(int idx)
    {
        switch(idx)
        {
            case 1:
                return param1;
            case 2:
                return param2;
            case 3:
                return param3;
            case 4:
                return param4;
            case 5:
                return param5;
            case 6:
                return param6;
            case 7:
                return param7;
            case 8:
                return param8;
        }
    }
};

#define  NUM_LOOP 1e8
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    srand(time(NULL));
    Parameters               params;
    Banana::ParameterManager paramManager;
    paramManager.setParam("param1", "0");
    paramManager.setParam("param2", "0");
    paramManager.setParam("param3", "0");
    paramManager.setParam("param4", "0");
    paramManager.setParam("param5", "0");
    paramManager.setParam("param6", "0");
    paramManager.setParam("param7", "0");
    paramManager.setParam("param8", "0");

    {
        float sum = 0;
        __BNN_PERORMANCE_TEST_BEGIN

        for(int i = 0; i < NUM_LOOP; ++i)
        {
            int randOp  = rand() % 2;
            int randNum = rand() % 8 + 1;

            if(randOp == 0)
                params.set(randNum);
            else
                sum += params.get(randNum);
        }
        __BNN_PERORMANCE_TEST_END("Native")
    }


    {
        float sum = 0;
        __BNN_PERORMANCE_TEST_BEGIN

        for(int i = 0; i < NUM_LOOP; ++i)
        {
            int randOp  = rand() % 2;
            int randNum = rand() % 8 + 1;

            if(randOp == 0)
            {
                switch(randNum)
                {
                    case 1:
                        paramManager.setParam("param1", "0");
                        break;
                    case 2:
                        paramManager.setParam("param2", "0");
                        break;
                    case 3:
                        paramManager.setParam("param3", "0");
                        break;
                    case 4:
                        paramManager.setParam("param4", "0");
                        break;
                    case 5:
                        paramManager.setParam("param5", "0");
                        break;
                    case 6:
                        paramManager.setParam("param6", "0");
                        break;
                    case 7:
                        paramManager.setParam("param7", "0");
                        break;
                    case 8:
                        paramManager.setParam("param8", "0");
                        break;
                }
            }
            else
            {
                switch(randNum)
                {
                    case 1:
                        sum += paramManager.getReal<float>("param1");
                        break;
                    case 2:
                        sum += paramManager.getReal<float>("param2");
                        break;
                    case 3:
                        sum += paramManager.getReal<float>("param3");
                        break;
                    case 4:
                        sum += paramManager.getReal<float>("param4");
                        break;
                    case 5:
                        sum += paramManager.getReal<float>("param5");
                        break;
                    case 6:
                        sum += paramManager.getReal<float>("param6");
                        break;
                    case 7:
                        sum += paramManager.getReal<float>("param7");
                        break;
                    case 8:
                        sum += paramManager.getReal<float>("param8");
                        break;
                }
            }
        }

        __BNN_PERORMANCE_TEST_END("stringbase")
    }

    return 0;
}