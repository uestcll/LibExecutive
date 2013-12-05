libexecutive.a : CLConditionVariable.o CLCriticalSection.o CLEvent.o CLExecutive.o CLExecutiveCommunication.o CLExecutiveCommunicationByNamedPipe.o CLExecutiveFunctionForMsgLoop.o CLExecutiveFunctionProvider.o CLExecutiveInitialFinishedNotifier.o CLExecutiveNameServer.o CLLibExecutiveInitializer.o CLLogger.o CLMessage.o CLMessageDeserializer.o CLMessageLoopManager.o CLMessageObserver.o CLMessageSerializer.o   CLMutex.o CLMutexByPThread.o CLMutexByRecordLocking.o CLMutexByRecordLockingAndPThread.o CLMutexBySharedPThread.o CLMutexInterface.o CLPrivateExecutiveCommunicationByNamedPipe.o CLProcess.o CLProcessFunctionForExec.o CLSharedConditionVariableImpl.o CLSharedEventImpl.o CLSharedMemory.o CLSharedMutexImpl.o CLSharedObjectsImpl.o CLStatus.o CLThread.o CLThreadInitialFinishedNotifier.o  CLBuffer.o CLDataReceiver.o CLDataReceiverBySTLQueue.o CLMessageReceiver.o CLIOVector.o CLSTLQueue.o CLProtoParser.o CLPointerMsgDeserializer.o CLDataReceiverByNamedPipe.o \
CLNamedPipe.o CLProtoParserForDefaultMsgFormat.o CLProtoEncapForDefaultMsgFormat.o CLMultiMsgDeserializer.o CLMultiMsgSerializer.o CLDataPoster.o CLDataPosterBySTLQueue.o CLDataPosterByNamedPipe.o CLDataPosterChannelMaintainer.o CLDataPosterChannelByNamedPipeMaintainer.o CLDataPosterChannelBySTLQueueMaintainer.o CLProtocolDataPoster.o CLProtocolEncapsulator.o CLMessagePoster.o CLPointerMsgSerializer.o CLMsgLoopManagerForSTLqueue.o CLProtoParserForPointerMsg.o CLMsgLoopManagerForPipeQueue.o CLDataPostResultNotifier.o CLEpoll.o CLEpollEvent.o CLMsgLoopManagerForEpoll.o CLTimerFd.o CLTimerApplyMsgDeserializer.o CLTimerApplyMsgSerializer.o CLTimerOutNotifyMsgDeserializer.o CLTimerOutNotifyMsgSerializer.o CLMessageReceiverByTimerFd.o
	ar -rc libexecutive.a CLConditionVariable.o CLCriticalSection.o CLEvent.o CLExecutive.o CLExecutiveCommunication.o CLExecutiveCommunicationByNamedPipe.o CLExecutiveFunctionForMsgLoop.o CLExecutiveFunctionProvider.o CLExecutiveInitialFinishedNotifier.o CLExecutiveNameServer.o CLLibExecutiveInitializer.o CLLogger.o CLMessage.o CLMessageDeserializer.o CLMessageLoopManager.o CLMessageObserver.o CLMessageSerializer.o  CLMutex.o CLMutexByPThread.o CLMutexByRecordLocking.o CLMutexByRecordLockingAndPThread.o CLMutexBySharedPThread.o CLMutexInterface.o  CLPrivateExecutiveCommunicationByNamedPipe.o  CLProcess.o CLProcessFunctionForExec.o CLSharedConditionVariableImpl.o CLSharedEventImpl.o CLSharedMemory.o CLSharedMutexImpl.o CLSharedObjectsImpl.o CLStatus.o CLThread.o CLThreadInitialFinishedNotifier.o CLBuffer.o CLDataReceiver.o CLDataReceiverBySTLQueue.o CLMessageReceiver.o CLIOVector.o CLSTLQueue.o CLProtoParser.o CLPointerMsgDeserializer.o CLDataReceiverByNamedPipe.o \
CLNamedPipe.o CLProtoParserForDefaultMsgFormat.o CLProtoEncapForDefaultMsgFormat.o CLMultiMsgDeserializer.o CLMultiMsgSerializer.o CLDataPoster.o CLDataPosterBySTLQueue.o CLDataPosterByNamedPipe.o CLDataPosterChannelMaintainer.o CLDataPosterChannelByNamedPipeMaintainer.o CLDataPosterChannelBySTLQueueMaintainer.o CLProtocolDataPoster.o CLProtocolEncapsulator.o CLMessagePoster.o CLPointerMsgSerializer.o CLMsgLoopManagerForSTLqueue.o CLProtoParserForPointerMsg.o CLMsgLoopManagerForPipeQueue.o CLDataPostResultNotifier.o CLEpoll.o CLEpollEvent.o CLMsgLoopManagerForEpoll.o CLTimerFd.o CLTimerApplyMsgDeserializer.o CLTimerApplyMsgSerializer.o CLTimerOutNotifyMsgDeserializer.o CLTimerOutNotifyMsgSerializer.o CLMessageReceiverByTimerFd.o
	rm *.o

##for timer
CLEpoll.o : ./src/CLEpoll.cpp
	g++ -o CLEpoll.o -c ./src/CLEpoll.cpp -I./include -g

CLEpollEvent.o : ./src/CLEpollEvent.cpp
	g++ -o CLEpollEvent.o -c ./src/CLEpollEvent.cpp -I./include -g

CLMsgLoopManagerForEpoll.o : ./src/CLMsgLoopManagerForEpoll.cpp
	g++ -o CLMsgLoopManagerForEpoll.o -c ./src/CLMsgLoopManagerForEpoll.cpp -I./include -g

CLTimerFd.o : ./src/CLTimerFd.cpp
	g++ -o CLTimerFd.o -c ./src/CLTimerFd.cpp -I./include -g

CLTimerApplyMsgDeserializer.o : ./src/CLTimerApplyMsgDeserializer.cpp
	g++ -o CLTimerApplyMsgDeserializer.o -c ./src/CLTimerApplyMsgDeserializer.cpp -I./include -g

CLTimerApplyMsgSerializer.o : ./src/CLTimerApplyMsgSerializer.cpp
	g++ -o CLTimerApplyMsgSerializer.o -c ./src/CLTimerApplyMsgSerializer.cpp -I./include -g

CLTimerOutNotifyMsgDeserializer.o : ./src/CLTimerOutNotifyMsgDeserializer.cpp
	g++ -o CLTimerOutNotifyMsgDeserializer.o -c ./src/CLTimerOutNotifyMsgDeserializer.cpp -I./include -g

CLTimerOutNotifyMsgSerializer.o : ./src/CLTimerOutNotifyMsgSerializer.cpp
	g++ -o CLTimerOutNotifyMsgSerializer.o -c ./src/CLTimerOutNotifyMsgSerializer.cpp -I./include -g

CLMessageReceiverByTimerFd.o : ./src/CLMessageReceiverByTimerFd.cpp
	g++ -o CLMessageReceiverByTimerFd.o -c ./src/CLMessageReceiverByTimerFd.cpp -I./include -g



CLDataPostResultNotifier.o : ./src/CLDataPostResultNotifier.cpp
	g++ -o CLDataPostResultNotifier.o -c ./src/CLDataPostResultNotifier.cpp -I./include -g

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

CLProtoParserForDefaultMsgFormat.o : ./src/CLProtoParserForDefaultMsgFormat.cpp
	g++ -o CLProtoParserForDefaultMsgFormat.o -c ./src/CLProtoParserForDefaultMsgFormat.cpp -I ./include -g

CLProtoEncapForDefaultMsgFormat.o : ./src/CLProtoEncapForDefaultMsgFormat.cpp
	g++ -o CLProtoEncapForDefaultMsgFormat.o -c ./src/CLProtoEncapForDefaultMsgFormat.cpp -I ./include -g

CLMultiMsgDeserializer.o : ./src/CLMultiMsgDeserializer.cpp
	g++ -o CLMultiMsgDeserializer.o -c ./src/CLMultiMsgDeserializer.cpp -I ./include -g

CLMultiMsgSerializer.o : ./src/CLMultiMsgSerializer.cpp
	g++ -o CLMultiMsgSerializer.o -c ./src/CLMultiMsgSerializer.cpp -I ./include -g

CLDataPoster.o : ./src/CLDataPoster.cpp
	g++ -o CLDataPoster.o -c ./src/CLDataPoster.cpp -I ./include -g

CLDataPosterBySTLQueue.o : ./src/CLDataPosterBySTLQueue.cpp
	g++ -o CLDataPosterBySTLQueue.o -c ./src/CLDataPosterBySTLQueue.cpp -I ./include -g

CLDataPosterByNamedPipe.o : ./src/CLDataPosterByNamedPipe.cpp
	g++ -o CLDataPosterByNamedPipe.o -c ./src/CLDataPosterByNamedPipe.cpp -I ./include -g

CLDataPosterChannelMaintainer.o : ./src/CLDataPosterChannelMaintainer.cpp
	g++ -o CLDataPosterChannelMaintainer.o -c ./src/CLDataPosterChannelMaintainer.cpp -I ./include -g

CLDataPosterChannelByNamedPipeMaintainer.o : ./src/CLDataPosterChannelByNamedPipeMaintainer.cpp
	g++ -o CLDataPosterChannelByNamedPipeMaintainer.o -c ./src/CLDataPosterChannelByNamedPipeMaintainer.cpp -I ./include -g

CLDataPosterChannelBySTLQueueMaintainer.o : ./src/CLDataPosterChannelBySTLQueueMaintainer.cpp
	g++ -o CLDataPosterChannelBySTLQueueMaintainer.o -c ./src/CLDataPosterChannelBySTLQueueMaintainer.cpp -I ./include -g	

CLProtocolDataPoster.o : ./src/CLProtocolDataPoster.cpp
	g++ -o CLProtocolDataPoster.o -c ./src/CLProtocolDataPoster.cpp -I ./include -g

CLProtocolEncapsulator.o : ./src/CLProtocolEncapsulator.cpp
	g++ -o CLProtocolEncapsulator.o -c ./src/CLProtocolEncapsulator.cpp -I ./include -g

CLMessagePoster.o : ./src/CLMessagePoster.cpp
	g++ -o CLMessagePoster.o -c ./src/CLMessagePoster.cpp -I ./include -g

CLPointerMsgSerializer.o : ./src/CLPointerMsgSerializer.cpp
	g++ -o CLPointerMsgSerializer.o -c ./src/CLPointerMsgSerializer.cpp -I ./include -g

CLProtoParserForPointerMsg.o : ./src/CLProtoParserForPointerMsg.cpp
	g++ -o CLProtoParserForPointerMsg.o -c ./src/CLProtoParserForPointerMsg.cpp -I ./include -g



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

CLMsgLoopManagerForPipeQueue.o : ./src/CLMsgLoopManagerForPipeQueue.cpp
	g++ -o CLMsgLoopManagerForPipeQueue.o -c ./src/CLMsgLoopManagerForPipeQueue.cpp -I./include -g

CLMsgLoopManagerForSTLqueue.o : ./src/CLMsgLoopManagerForSTLqueue.cpp
	g++ -o CLMsgLoopManagerForSTLqueue.o -c ./src/CLMsgLoopManagerForSTLqueue.cpp -I./include -g

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

CLSharedConditionVariableImpl.o : ./src/CLSharedConditionVariableImpl.cpp
	g++ -o CLSharedConditionVariableImpl.o -c ./src/CLSharedConditionVariableImpl.cpp -I./include -g

CLSharedEventImpl.o : ./src/CLSharedEventImpl.cpp
	g++ -o CLSharedEventImpl.o -c ./src/CLSharedEventImpl.cpp -I./include -g

CLSharedMemory.o : ./src/CLSharedMemory.cpp
	g++ -o CLSharedMemory.o -c ./src/CLSharedMemory.cpp -I./include -g

CLSharedMutexImpl.o : ./src/CLSharedMutexImpl.cpp
	g++ -o CLSharedMutexImpl.o -c ./src/CLSharedMutexImpl.cpp -I./include -g

CLSharedObjectsImpl.o : ./src/CLSharedObjectsImpl.cpp
	g++ -o CLSharedObjectsImpl.o -c ./src/CLSharedObjectsImpl.cpp -I./include -g

CLStatus.o : ./src/CLStatus.cpp
	g++ -o CLStatus.o -c ./src/CLStatus.cpp -I./include -g

CLThread.o : ./src/CLThread.cpp
	g++ -o CLThread.o -c ./src/CLThread.cpp -I./include -g

CLThreadInitialFinishedNotifier.o : ./src/CLThreadInitialFinishedNotifier.cpp
	g++ -o CLThreadInitialFinishedNotifier.o -c ./src/CLThreadInitialFinishedNotifier.cpp -I./include -g



