//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Utils/Timer.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/Scheduler.h>

#include <atomic>

#define NUM_ELEMENTS (1 << 20)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<typename T>
void atomicAdd(T& target, const T& operand)
{
    std::atomic<T>& tgt = *((std::atomic<T>*) & target);

    T cur_val = target;
    T new_val;
    do {
        new_val = cur_val + operand;
    } while(!tgt.compare_exchange_weak(cur_val, new_val));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testSpinLock(Vector<float>& values, const Vector<int>& index)
{
    Vector<Banana::ParallelObjects::SpinLock> locks(NUM_ELEMENTS);

    __BNN_PERORMANCE_TEST_BEGIN
    Banana::Scheduler::parallel_for(NUM_ELEMENTS, [&](int i)
                                    {
                                        int idx = index[i];
                                        float x = float(idx) / float(NUM_ELEMENTS);
                                        locks[idx].lock();
                                        values[idx] += x;
                                        locks[idx].unlock();
                                    });
    __BNN_PERORMANCE_TEST_END("SpinLock");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testAtomicOps(Vector<float>& values, const Vector<int>& index)
{
    srand(0);
    __BNN_PERORMANCE_TEST_BEGIN
    Banana::Scheduler::parallel_for(NUM_ELEMENTS, [&](int i)
                                    {
                                        int idx = index[i];
                                        float x = float(idx) / float(NUM_ELEMENTS);
                                        atomicAdd(values[idx], x);
                                    });
    __BNN_PERORMANCE_TEST_END("AtomicOps");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    Vector<float> values1(NUM_ELEMENTS, 0);
    Vector<float> values2(NUM_ELEMENTS, 0);
    Vector<int>   index(NUM_ELEMENTS);
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        index[i] = rand() % NUM_ELEMENTS;
    }
    testSpinLock(values1, index);
    testAtomicOps(values2, index);

    auto sum1 = Banana::ParallelSTL::sum(values1);
    auto sum2 = Banana::ParallelSTL::sum(values2);

    printf("\nSum vectors: %10.5e/%10.5e\n\n", sum1, sum2);

    return 0;
}