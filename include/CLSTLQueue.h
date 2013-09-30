#ifndef CLSTLQUEUE_H
#define CLSTLQUEUE_H

#include <iostream>
#include <queue>
#include "CLStatus.h"

using namespace std;

template <typename T>
class CLSTLQueue
{
public:
	CLSTLQueue();
	virtual ~CLSTLQueue();

	T PopItem();
	CLStatus PushItem(T item);
	 
private:
	queue<T> m_DataQueue; 
};

#endif