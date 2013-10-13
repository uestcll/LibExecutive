libexecutive.a : CLConditionVariable.o CLCriticalSection.o CLEvent.o CLExecutive.o CLExecutiveCommunication.o CLExecutiveCommunicationByNamedPipe.o CLExecutiveFunctionForMsgLoop.o CLExecutiveFunctionProvider.o CLExecutiveInitialFinishedNotifier.o CLExecutiveNameServer.o CLLibExecutiveInitializer.o CLLogger.o CLMessage.o CLMessageDeserializer.o CLMessageLoopManager.o CLMessageObserver.o CLMessageSerializer.o   CLMutex.o CLMutexByPThread.o CLMutexByRecordLocking.o CLMutexByRecordLockingAndPThread.o CLMutexBySharedPThread.o CLMutexInterface.o CLPrivateExecutiveCommunicationByNamedPipe.o CLProcess.o CLProcessFunctionForExec.o CLSharedConditionVariableAllocator.o CLSharedConditionVariableImpl.o CLSharedEventAllocator.o CLSharedEventImpl.o CLSharedExecutiveCommunicationByNamedPipe.o CLSharedMemory.o CLSharedMutexAllocator.o CLSharedMutexImpl.o CLSharedObjectsImpl.o CLStatus.o CLThread.o CLThreadInitialFinishedNotifier.o  CLBuffer.o CLDataReceiver.o CLDataReceiverBySTLQueue.o CLMessageReceiver.o CLIOVector.o CLSTLQueue.o CLProtoParser.o CLPointerMsgDeserializer.o
	ar -rc libexecutive.a CLConditionVariable.o CLCriticalSection.o CLEvent.o CLExecutive.o CLExecutiveCommunication.o CLExecutiveCommunicationByNamedPipe.o CLExecutiveFunctionForMsgLoop.o CLExecutiveFunctionProvider.o CLExecutiveInitialFinishedNotifier.o CLExecutiveNameServer.o CLLibExecutiveInitializer.o CLLogger.o CLMessage.o CLMessageDeserializer.o CLMessageLoopManager.o CLMessageObserver.o CLMessageSerializer.o  CLMutex.o CLMutexByPThread.o CLMutexByRecordLocking.o CLMutexByRecordLockingAndPThread.o CLMutexBySharedPThread.o CLMutexInterface.o  CLPrivateExecutiveCommunicationByNamedPipe.o  CLProcess.o CLProcessFunctionForExec.o CLSharedConditionVariableAllocator.o CLSharedConditionVariableImpl.o CLSharedEventAllocator.o CLSharedEventImpl.o CLSharedExecutiveCommunicationByNamedPipe.o CLSharedMemory.o CLSharedMutexAllocator.o CLSharedMutexImpl.o CLSharedObjectsImpl.o CLStatus.o CLThread.o CLThreadInitialFinishedNotifier.o CLBuffer.o CLDataReceiver.o CLDataReceiverBySTLQueue.o CLMessageReceiver.o CLIOVector.o CLSTLQueue.o CLProtoParser.o CLPointerMsgDeserializer.o 
	rm *.o


CLBuffer.o : ./src/CLBuffer.cpp
	g++ -o CLBuffer.o -c ./src/CLBuffer.cpp -I./include -g

CLDataReceiver.o : ./src/CLDataReceiver.cpp
	g++ -o CLDataReceiver.o -c ./src/CLDataReceiver.cpp -I./include -g

CLDataReceiverBySTLQueue.o : ./src/CLDataReceiverBySTLQueue.cpp
	g++ -o CLDataReceiverBySTLQueue.o -c ./src/CLDataReceiverBySTLQueue.cpp -I./include -g

CLIOVector.o : ./src/CLIOVector.cpp
	g++ -o CLIOVector.o -c ./src/CLIOVector.cpp -I./include -g

CLMessageReceiver.o : ./src/CLMessageReceiver.cpp
	g++ -o CLMessageReceiver.o -c ./src/CLMessageReceiver.cpp -I./include -g

CLSTLQueue.o : ./src/CLSTLQueue.cpp
	g++ -o CLSTLQueue.o -c ./src/CLSTLQueue.cpp -I./include -g

CLProtoParser.o : ./src/CLProtoParser.cpp
	g++ -o CLProtoParser.o -c ./src/CLProtoParser.cpp -I ./include -g

CLPointerMsgDeserializer.o : ./src/CLPointerMsgDeserializer.cpp
	g++ -o CLPointerMsgDeserializer.o -c ./src/CLPointerMsgDeserializer.cpp -I ./include -g

CLDataReceiverByNamedPipe.o : ./src/CLDataReceiverByNamedPipe.cpp
	g++ -o CLDataReceiverByNamedPipe.o -c ./src/CLDataReceiverByNamedPipe.cpp -I ./include -g

CLNamedPipe.o : ./src/CLNamedPipe.cpp
	g++ -o CLNamedPipe.o -c ./src/CLNamedPipe.cpp -I ./include -g

CLProtoParserForSharedPipe.o : ./src/CLProtoParserForSharedPipe.cpp
	g++ -o CLProtoParserForSharedPipe.o -c ./src/CLProtoParserForSharedPipe.cpp -I ./include -g

CLConditionVariable.o : ./src/CLConditionVariable.cpp
	g++ -o CLConditionVariable.o -c ./src/CLConditionVariable.cpp -I./include -g

CLCriticalSection.o : ./src/CLCriticalSection.cpp
	g++ -o CLCriticalSection.o -c ./src/CLCriticalSection.cpp -I./include -g

CLEvent.o : ./src/CLEvent.cpp
	g++ -o CLEvent.o -c ./src/CLEvent.cpp -I./include -g

CLExecutive.o : ./src/CLExecutive.cpp
	g++ -o CLExecutive.o -c ./src/CLExecutive.cpp -I./include -g

CLExecutiveCommunication.o : ./src/CLExecutiveCommunication.cpp
	g++ -o CLExecutiveCommunication.o -c ./src/CLExecutiveCommunication.cpp -I./include -g

CLExecutiveCommunicationByNamedPipe.o : ./src/CLExecutiveCommunicationByNamedPipe.cpp
	g++ -o CLExecutiveCommunicationByNamedPipe.o -c ./src/CLExecutiveCommunicationByNamedPipe.cpp -I./include -g

CLExecutiveFunctionForMsgLoop.o : ./src/CLExecutiveFunctionForMsgLoop.cpp
	g++ -o CLExecutiveFunctionForMsgLoop.o -c ./src/CLExecutiveFunctionForMsgLoop.cpp -I./include -g

CLExecutiveFunctionProvider.o : ./src/CLExecutiveFunctionProvider.cpp
	g++ -o CLExecutiveFunctionProvider.o -c ./src/CLExecutiveFunctionProvider.cpp -I./include -g

CLExecutiveInitialFinishedNotifier.o : ./src/CLExecutiveInitialFinishedNotifier.cpp
	g++ -o CLExecutiveInitialFinishedNotifier.o -c ./src/CLExecutiveInitialFinishedNotifier.cpp -I./include -g

CLExecutiveNameServer.o : ./src/CLExecutiveNameServer.cpp
	g++ -o CLExecutiveNameServer.o -c ./src/CLExecutiveNameServer.cpp -I./include -g

CLLibExecutiveInitializer.o : ./src/CLLibExecutiveInitializer.cpp
	g++ -o CLLibExecutiveInitializer.o -c ./src/CLLibExecutiveInitializer.cpp -I./include -g

CLLogger.o : ./src/CLLogger.cpp
	g++ -o CLLogger.o -c ./src/CLLogger.cpp -I./include -g

CLMessage.o : ./src/CLMessage.cpp
	g++ -o CLMessage.o -c ./src/CLMessage.cpp -I./include -g

CLMessageDeserializer.o : ./src/CLMessageDeserializer.cpp
	g++ -o CLMessageDeserializer.o -c ./src/CLMessageDeserializer.cpp -I./include -g

CLMessageLoopManager.o : ./src/CLMessageLoopManager.cpp
	g++ -o CLMessageLoopManager.o -c ./src/CLMessageLoopManager.cpp -I./include -g

CLMessageObserver.o : ./src/CLMessageObserver.cpp
	g++ -o CLMessageObserver.o -c ./src/CLMessageObserver.cpp -I./include -g

CLMessageSerializer.o : ./src/CLMessageSerializer.cpp
	g++ -o CLMessageSerializer.o -c ./src/CLMessageSerializer.cpp -I./include -g

# CLMsgLoopManagerForPipeQueue.o : ./src/CLMsgLoopManagerForPipeQueue.cpp
	# g++ -o CLMsgLoopManagerForPipeQueue.o -c ./src/CLMsgLoopManagerForPipeQueue.cpp -I./include -g

# CLMsgLoopManagerForSTLqueue.o : ./src/CLMsgLoopManagerForSTLqueue.cpp
	# g++ -o CLMsgLoopManagerForSTLqueue.o -c ./src/CLMsgLoopManagerForSTLqueue.cpp -I./include -g

CLMutex.o : ./src/CLMutex.cpp
	g++ -o CLMutex.o -c ./src/CLMutex.cpp -I./include -g

CLMutexByPThread.o : ./src/CLMutexByPThread.cpp
	g++ -o CLMutexByPThread.o -c ./src/CLMutexByPThread.cpp -I./include -g

CLMutexByRecordLocking.o : ./src/CLMutexByRecordLocking.cpp
	g++ -o CLMutexByRecordLocking.o -c ./src/CLMutexByRecordLocking.cpp -I./include -g

CLMutexByRecordLockingAndPThread.o : ./src/CLMutexByRecordLockingAndPThread.cpp
	g++ -o CLMutexByRecordLockingAndPThread.o -c ./src/CLMutexByRecordLockingAndPThread.cpp -I./include -g

CLMutexBySharedPThread.o : ./src/CLMutexBySharedPThread.cpp
	g++ -o CLMutexBySharedPThread.o -c ./src/CLMutexBySharedPThread.cpp -I./include -g

CLMutexInterface.o : ./src/CLMutexInterface.cpp
	g++ -o CLMutexInterface.o -c ./src/CLMutexInterface.cpp -I./include -g

# CLNonThreadForMsgLoop.o : ./src/CLNonThreadForMsgLoop.cpp
	# g++ -o CLNonThreadForMsgLoop.o -c ./src/CLNonThreadForMsgLoop.cpp -I./include -g

CLPrivateExecutiveCommunicationByNamedPipe.o : ./src/CLPrivateExecutiveCommunicationByNamedPipe.cpp
	g++ -o CLPrivateExecutiveCommunicationByNamedPipe.o -c ./src/CLPrivateExecutiveCommunicationByNamedPipe.cpp -I./include -g

CLProcess.o : ./src/CLProcess.cpp
	g++ -o CLProcess.o -c ./src/CLProcess.cpp -I./include -g

CLProcessFunctionForExec.o : ./src/CLProcessFunctionForExec.cpp
	g++ -o CLProcessFunctionForExec.o -c ./src/CLProcessFunctionForExec.cpp -I./include -g

CLSharedConditionVariableAllocator.o : ./src/CLSharedConditionVariableAllocator.cpp
	g++ -o CLSharedConditionVariableAllocator.o -c ./src/CLSharedConditionVariableAllocator.cpp -I./include -g

CLSharedConditionVariableImpl.o : ./src/CLSharedConditionVariableImpl.cpp
	g++ -o CLSharedConditionVariableImpl.o -c ./src/CLSharedConditionVariableImpl.cpp -I./include -g

CLSharedEventAllocator.o : ./src/CLSharedEventAllocator.cpp
	g++ -o CLSharedEventAllocator.o -c ./src/CLSharedEventAllocator.cpp -I./include -g

CLSharedEventImpl.o : ./src/CLSharedEventImpl.cpp
	g++ -o CLSharedEventImpl.o -c ./src/CLSharedEventImpl.cpp -I./include -g

CLSharedExecutiveCommunicationByNamedPipe.o : ./src/CLSharedExecutiveCommunicationByNamedPipe.cpp
	g++ -o CLSharedExecutiveCommunicationByNamedPipe.o -c ./src/CLSharedExecutiveCommunicationByNamedPipe.cpp -I./include -g

CLSharedMemory.o : ./src/CLSharedMemory.cpp
	g++ -o CLSharedMemory.o -c ./src/CLSharedMemory.cpp -I./include -g

CLSharedMutexAllocator.o : ./src/CLSharedMutexAllocator.cpp
	g++ -o CLSharedMutexAllocator.o -c ./src/CLSharedMutexAllocator.cpp -I./include -g

CLSharedMutexImpl.o : ./src/CLSharedMutexImpl.cpp
	g++ -o CLSharedMutexImpl.o -c ./src/CLSharedMutexImpl.cpp -I./include -g

CLSharedObjectsImpl.o : ./src/CLSharedObjectsImpl.cpp
	g++ -o CLSharedObjectsImpl.o -c ./src/CLSharedObjectsImpl.cpp -I./include -g

CLStatus.o : ./src/CLStatus.cpp
	g++ -o CLStatus.o -c ./src/CLStatus.cpp -I./include -g

CLThread.o : ./src/CLThread.cpp
	g++ -o CLThread.o -c ./src/CLThread.cpp -I./include -g

# CLThreadCommunicationBySTLqueue.o : ./src/CLThreadCommunicationBySTLqueue.cpp
	# g++ -o CLThreadCommunicationBySTLqueue.o -c ./src/CLThreadCommunicationBySTLqueue.cpp -I./include -g

# CLThreadForMsgLoop.o : ./src/CLThreadForMsgLoop.cpp
	# g++ -o CLThreadForMsgLoop.o -c ./src/CLThreadForMsgLoop.cpp -I./include -g

CLThreadInitialFinishedNotifier.o : ./src/CLThreadInitialFinishedNotifier.cpp
	g++ -o CLThreadInitialFinishedNotifier.o -c ./src/CLThreadInitialFinishedNotifier.cpp -I./include -g



