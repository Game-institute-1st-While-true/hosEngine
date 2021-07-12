#pragma once
#include <iostream>
#include <mutex>

template <typename Data>
class CircularQueue
{
private:
	unsigned int MaxSize;

	unsigned int Tail;
	unsigned int Head;

	Data* DataBuffer;

public:
	CircularQueue() = delete;
	CircularQueue(const int bufsize);
	~CircularQueue();

public:
	bool Enqueue(Data& d);
	bool Dequeue(Data& d);

private:
	bool IsFull();
	bool IsEmpty();
	int NextIndex(const int current_index);

public:
	const int GetTail();
	const int GetHead();
	const int GetCurrentCount();
	const int GetMaxSize();

	const Data* GetData();
};

// Lock�� �ɸ��� ������
// ������ Safe, ������ �̱۽����庸�� ������.

template <typename Data>
class LockedCQ
{
private:
	unsigned int MaxSize;

	unsigned int Tail;
	unsigned int Head;

	Data* DataBuffer;

	std::mutex RingLock;

public:
	LockedCQ() = delete;
	LockedCQ(const int bufsize);
	~LockedCQ();

public:
	bool Enqueue(Data& d);
	bool Dequeue(Data& d);

private:
	bool IsFull();
	bool IsEmpty();
	int NextIndex(const int current_index);

public:
	const int GetTail();
	const int GetHead();
	const int GetCurrentCount();
	const int GetMaxSize();
};

// Lock �� ���� ������ --> Atomic
// Lock Free ������� ���� �Ͽ���.

// ����� �̱��� ����.

template <typename Data>
class LockFreeCQ
{
private:
	unsigned int MaxSize;

	unsigned int Tail;
	unsigned int Head;

	Data* DataBuffer;

public:
	LockFreeCQ() = delete;
	LockFreeCQ(const int bufsize);
	~LockFreeCQ();

public:
	bool Enqueue(Data& d);
	bool Dequeue(Data& d);

private:
	bool IsFull();
	bool IsEmpty();
	int NextIndex(const int current_index);

public:
	const int GetTail();
	const int GetHead();
	const int GetCurrentCount();
	const int GetMaxSize();

	const Data* GetData();
};