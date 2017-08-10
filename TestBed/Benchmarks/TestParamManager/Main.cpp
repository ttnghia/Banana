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
#include <Banana/Utils/ParameterManager.h>
#include <Banana/Utils/Timer.h>

#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <string>

using Real = double;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Parameters
{
public:
    Real param1 = 0;
    Real param2 = 0;
    Real param3 = 0;
    Real param4 = 0;
    Real param5 = 0;
    Real param6 = 0;
    Real param7 = 0;
    Real param8 = 0;

    void set(int idx)
    {
        switch(idx)
        {
            case 1:
                param1 += Real(rand()) / Real(RAND_MAX);
                break;
            case 2:
                param2 += Real(rand()) / Real(RAND_MAX);
                break;
            case 3:
                param3 += Real(rand()) / Real(RAND_MAX);
                break;
            case 4:
                param4 += Real(rand()) / Real(RAND_MAX);
                break;
            case 5:
                param5 += Real(rand()) / Real(RAND_MAX);
                break;
            case 6:
                param6 += Real(rand()) / Real(RAND_MAX);
                break;
            case 7:
                param7 += Real(rand()) / Real(RAND_MAX);
                break;
            case 8:
                param8 += Real(rand()) / Real(RAND_MAX);
                break;
        }
    }

    Real get(int idx)
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
//#define INT_KEY

enum Keys
{
    param1 = 0,
    param2,
    param3,
    param4,
    param5,
    param6,
    param7,
    param8,
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    srand(time(NULL));
    Parameters params;
#ifndef INT_KEY
    Banana::ParameterManager<std::string> paramManager;
    paramManager.setDouble("param1", 0);
    paramManager.setDouble("param2", 0);
    paramManager.setDouble("param3", 0);
    paramManager.setDouble("param4", 0);
    paramManager.setDouble("param5", 0);
    paramManager.setDouble("param6", 0);
    paramManager.setDouble("param7", 0);
    paramManager.setDouble("param8", 0);
#else
    Banana::ParameterManager<Keys> paramManager;
    paramManager.setDouble(param1,   0);
    paramManager.setDouble(param2,   0);
    paramManager.setDouble(param3,   0);
    paramManager.setDouble(param4,   0);
    paramManager.setDouble(param5,   0);
    paramManager.setDouble(param6,   0);
    paramManager.setDouble(param7,   0);
    paramManager.setDouble(param8,   0);
#endif

    {
        Real sum = 0;
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
        Real sum = 0;
        __BNN_PERORMANCE_TEST_BEGIN

        for(int i = 0; i < NUM_LOOP; ++i)
        {
            int randOp  = rand() % 2;
            int randNum = rand() % 8 + 1;

            if(randOp == 0)
            {
#ifndef INT_KEY
                switch(randNum)
                {
                    case 1:
                        paramManager.setDouble("param1", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 2:
                        paramManager.setDouble("param2", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 3:
                        paramManager.setDouble("param3", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 4:
                        paramManager.setDouble("param4", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 5:
                        paramManager.setDouble("param5", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 6:
                        paramManager.setDouble("param6", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 7:
                        paramManager.setDouble("param7", Real(rand()) / Real(RAND_MAX));
                        break;
                    case 8:
                        paramManager.setDouble("param8", Real(rand()) / Real(RAND_MAX));
                        break;
                }
#else
                switch(randNum)
                {
                    case 1:
                        paramManager.setDouble(param1, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 2:
                        paramManager.setDouble(param2, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 3:
                        paramManager.setDouble(param3, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 4:
                        paramManager.setDouble(param4, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 5:
                        paramManager.setDouble(param5, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 6:
                        paramManager.setDouble(param6, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 7:
                        paramManager.setDouble(param7, Real(rand()) / Real(RAND_MAX));
                        break;
                    case 8:
                        paramManager.setDouble(param8, Real(rand()) / Real(RAND_MAX));
                        break;
                }
#endif
            }
            else
            {
#ifndef INT_KEY
                switch(randNum)
                {
                    case 1:
                        sum += paramManager.getDouble("param1");
                        break;
                    case 2:
                        sum += paramManager.getDouble("param2");
                        break;
                    case 3:
                        sum += paramManager.getDouble("param3");
                        break;
                    case 4:
                        sum += paramManager.getDouble("param4");
                        break;
                    case 5:
                        sum += paramManager.getDouble("param5");
                        break;
                    case 6:
                        sum += paramManager.getDouble("param6");
                        break;
                    case 7:
                        sum += paramManager.getDouble("param7");
                        break;
                    case 8:
                        sum += paramManager.getDouble("param8");
                        break;
                }
#else
                switch(randNum)
                {
                    case 1:
                        sum += paramManager.getDouble(param1);
                        break;
                    case 2:
                        sum += paramManager.getDouble(param2);
                        break;
                    case 3:
                        sum += paramManager.getDouble(param3);
                        break;
                    case 4:
                        sum += paramManager.getDouble(param4);
                        break;
                    case 5:
                        sum += paramManager.getDouble(param5);
                        break;
                    case 6:
                        sum += paramManager.getDouble(param6);
                        break;
                    case 7:
                        sum += paramManager.getDouble(param7);
                        break;
                    case 8:
                        sum += paramManager.getDouble(param8);
                        break;
                }
#endif
            }
        }

        __BNN_PERORMANCE_TEST_END("stringbase")
    }

    return 0;
}