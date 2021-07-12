#include "PackageQueue.h"

gzz::PackageQueue::PackageQueue()
	:ReadQueue(nullptr), WriteQueue(nullptr)
{
	ReadQueue = new std::deque<PACKAGE>;
	WriteQueue = new std::deque<PACKAGE>;
}

gzz::PackageQueue::~PackageQueue()
{
	if (ReadQueue != nullptr)
	{
		delete ReadQueue;
	}

	if (WriteQueue != nullptr)
	{
		delete WriteQueue;
	}
}

bool gzz::PackageQueue::Safe_Enqueue(PACKAGE& pack)
{
	std::scoped_lock<std::recursive_mutex> lock(Lock);
	WriteQueue->push_back(pack);
	return true;
}

bool gzz::PackageQueue::Safe_Dequeue(PACKAGE& pack)
{
	std::scoped_lock<std::recursive_mutex> lock(Lock);
	if (ReadQueue->empty() == true)
	{
		if (WriteQueue->empty() == true)
		{
			return false;
		}
		Swap();
	}
	pack = ReadQueue->front();
	ReadQueue->pop_front();
	return true;
}

bool gzz::PackageQueue::Enqueue(PACKAGE& pack)
{
	WriteQueue->push_back(pack);
	return true;
}

bool gzz::PackageQueue::Dequeue(PACKAGE& pack)
{
	pack = ReadQueue->front();
	ReadQueue->pop_front();
	return true;
}

void gzz::PackageQueue::Swap()
{
	std::scoped_lock<std::recursive_mutex> lock(Lock);
	std::deque<PACKAGE>* tempQueue = ReadQueue;
	ReadQueue = WriteQueue;
	WriteQueue = tempQueue;
}
