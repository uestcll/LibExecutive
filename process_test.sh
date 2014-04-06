#!/bin/bash
make
cd test_for_exec
cd test_for_CLConditionVariable
make
cd ..
cd test_for_CLEvent_Auto
make
cd ..
cd test_for_CLEvent_Semaphore
make
cd ..
cd test_for_CLMutex_RecordingLock
make
cd ..
cd test_for_CLMutex_RecordingLockAndPThread
make
cd ..
cd test_for_CLMutex_SharePThread
make
cd ..
cd test_for_CLNonThreadForMsgLoop_Stress_Testing
make
cd ..
cd test_for_CLProcess
make
cd ..
cd test_for_CLSharedConditionVariableAllocator
make
cd ..
cd test_for_CLSharedMemory
make
cd ..
cd test_for_CLSharedMutexAllocator
make
cd ../..
cd test
make
