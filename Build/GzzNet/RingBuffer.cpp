#include "RingBuffer.h"
#include <cstdio>
#include <memory>

RingBuffer::RingBuffer(int size)
	:MaxSize(size), Head(0), Tail(0)
{
	Buffer = new char[size];
	memset(Buffer, 0, size);
}

RingBuffer::~RingBuffer()
{
	if (Buffer)
	{
		delete[] Buffer;
	}
}

bool RingBuffer::IsEmpty()
{
	return (Head == Tail);
}

bool RingBuffer::IsFull()
{
	return (NextIndex(Tail, 1) == Head);
}

int RingBuffer::NextIndex(const int current, const int offset)
{
	return (current + offset) % MaxSize;
}

int RingBuffer::CurrentSize()
{
	return (Tail - Head + MaxSize) % MaxSize;
}

int RingBuffer::Push(char* src, int size)
{
	if (IsFull()) return 0;
	if (CurrentSize() + size >= MaxSize)
	{
		return (CurrentSize() + size) - MaxSize;
	}

	if (Tail + size > MaxSize)
	{
		// 버퍼 크기 정하기
		int pre = MaxSize - Tail;
		int end = size - pre;

		memcpy_s(Buffer + Tail, pre, src, pre);
		memcpy_s(Buffer, end, src + pre, end);
	}
	else
	{
		memcpy_s(Buffer + Tail, size, src, size);
	}
	Tail = NextIndex(Tail, size);

	return -1;
}

int RingBuffer::Pop(char* dst, int size)
{
	if (IsEmpty()) return -2;
	if (size > CurrentSize()) return -2;

	if (Head + size > MaxSize)
	{
		// 버퍼 크기 정하기.
		int pre = MaxSize - Head;
		int end = size - pre;

		memcpy_s(dst, pre, Buffer + Head, pre);
		memcpy_s(dst + pre, end, Buffer, end);
	}
	else
	{
		memcpy_s(dst, size, Buffer + Head, size);
	}
	Head = NextIndex(Head, size);

	return -1;
}

int RingBuffer::Get(char* dst, int size)
{
	if (IsEmpty()) return -2;
	if (size > CurrentSize()) return -2;

	if (Head + size > MaxSize)
	{
		// 버퍼 크기 정하기.
		int pre = MaxSize - Head;
		int end = size - pre;

		memcpy_s(dst, pre, Buffer + Head, pre);
		memcpy_s(dst + pre, end, Buffer, end);
	}
	else
	{
		memcpy_s(dst, size, Buffer + Head, size);
	}
	return -1;
}

char* RingBuffer::Front()
{
	return &Buffer[Head];
}

char* RingBuffer::End()
{
	return &Buffer[Tail];
}
