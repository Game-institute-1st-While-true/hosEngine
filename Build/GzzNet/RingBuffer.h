#pragma once

class RingBuffer
{
public:;

private:
	char* Buffer;

	int Head;
	int Tail;
	
	int MaxSize;

public:
	RingBuffer() = delete;
	RingBuffer(int size);
	~RingBuffer();

private:
	bool IsEmpty();										// ���۰� ����ִ��� Ȯ��
	bool IsFull();										// ���۰� ����á���� Ȯ��

	int NextIndex(const int current, const int offset);	// �ڽ��� ���� index Ȯ��

public:
	int Push(char* src, int size);
	int Pop(char* dst, int size);
	int Get(char* dst, int size);

	char* Front();
	char* End();

	int CurrentSize();
	int GetMax() { return MaxSize - 1; }
};

