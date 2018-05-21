//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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
#include <Banana/ParallelHelpers/AtomicOperations.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/Utils/NumberHelpers.h>

#define NUM_ELEMENTS (1 << 24)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testSpinLockFloat(Vector<float>& values, const Vector<int>& index)
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

void testAtomicOpsFloat(Vector<float>& values, const Vector<int>& index)
{
    srand(0);
    __BNN_PERORMANCE_TEST_BEGIN
    Banana::Scheduler::parallel_for(NUM_ELEMENTS, [&](int i)
                                    {
                                        int idx = index[i];
                                        float x = float(idx) / float(NUM_ELEMENTS);
                                        Banana::AtomicOperations::atomicAdd(values[idx], x);
                                    });
    __BNN_PERORMANCE_TEST_END("AtomicOps");
}

void testFloat()
{
    Vector<float> values1(NUM_ELEMENTS, 0);
    Vector<float> values2(NUM_ELEMENTS, 0);
    Vector<int>   index(NUM_ELEMENTS);
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        index[i] = rand() % NUM_ELEMENTS;
    }
    testSpinLockFloat(values1, index);
    testAtomicOpsFloat(values2, index);

    auto sum1 = Banana::ParallelSTL::sum(values1);
    auto sum2 = Banana::ParallelSTL::sum(values2);

    printf("\nTest float finished. Sum vectors: %10.5e/%10.5e\n\n", sum1, sum2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void testSpinLockVec3f(Vector<Vec3f>& values, const Vector<int>& index)
{
    Vector<Banana::ParallelObjects::SpinLock> locks(NUM_ELEMENTS);

    __BNN_PERORMANCE_TEST_BEGIN
    Banana::Scheduler::parallel_for(NUM_ELEMENTS, [&](int i)
                                    {
                                        int idx = index[i];
                                        float x = float(idx) / float(NUM_ELEMENTS);
                                        locks[idx].lock();
                                        values[idx] += Vec3f(x);
                                        locks[idx].unlock();
                                    });
    __BNN_PERORMANCE_TEST_END("SpinLock");
}

void testAtomicOpsVec3f(Vector<Vec3f>& values, const Vector<int>& index)
{
    srand(0);
    __BNN_PERORMANCE_TEST_BEGIN
    Banana::Scheduler::parallel_for(NUM_ELEMENTS, [&](int i)
                                    {
                                        int idx = index[i];
                                        float x = float(idx) / float(NUM_ELEMENTS);
                                        Banana::AtomicOperations::atomicAdd(values[idx].x, x);
                                        Banana::AtomicOperations::atomicAdd(values[idx].y, x);
                                        Banana::AtomicOperations::atomicAdd(values[idx].z, x);
                                    });
    __BNN_PERORMANCE_TEST_END("AtomicOps");
}

void testVec3f()
{
    Vector<Vec3f> values1(NUM_ELEMENTS, Vec3f(0));
    Vector<Vec3f> values2(NUM_ELEMENTS, Vec3f(0));
    Vector<int>   index(NUM_ELEMENTS);
    for(int i = 0; i < NUM_ELEMENTS; ++i) {
        index[i] = rand() % NUM_ELEMENTS;
    }
    testSpinLockVec3f(values1, index);
    testAtomicOpsVec3f(values2, index);

    auto sum1 = Banana::ParallelSTL::sum<3, float>(values1);
    auto sum2 = Banana::ParallelSTL::sum<3, float>(values2);

    printf("\nTest Vec3f finished. Sum vectors: %s/%s\n\n", Banana::NumberHelpers::toString(sum1).c_str(), Banana::NumberHelpers::toString(sum2).c_str());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main()
{
    testFloat();
    printf("\n\n\n");
    testVec3f();
    return 0;
}