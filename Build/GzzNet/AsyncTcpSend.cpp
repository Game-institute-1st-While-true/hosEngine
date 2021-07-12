#include "AsyncTcpSend.h"


#pragma region OldCode
gzz::AsyncTcpSend::AsyncTcpSend()
	:SendBuf(nullptr), WriteBuf(nullptr), RemainSize(0), WriteSize(0), BufferPosition(0)
{
	SendBuf = new char[MAX_SEND_BUF_SIZE];
	WriteBuf = new char[MAX_SEND_BUF_SIZE];

	Wsabuf.buf = nullptr;
	Wsabuf.len = 0;
}

gzz::AsyncTcpSend::~AsyncTcpSend()
{
	delete[] SendBuf;
	delete[] WriteBuf;
}

int gzz::AsyncTcpSend::SendMsg(const SOCKET& sock, const LPOVERLAPPED& overraped, PACKET& packet)
{
	// SendBuffer�� ���� �����Ͱ� �ִ°�.
	std::lock_guard lock(WriteLock);

	if (Write(packet))
	{
		if (RemainSize == 0)
		{
			SwapBuffer();
			SendIO(sock, overraped);
		}
		return static_cast<int>(NET_ERR::SUCESS);
	}
	else
	{
		return static_cast<int>(NET_ERR::FAIL);
	}
}

int gzz::AsyncTcpSend::SendProcess(const int completionBytes)
{
	// �� ���� ���.
	if (RemainSize == completionBytes)
	{
		RemainSize = 0;
		BufferPosition = 0;
		return static_cast<int>(NET_ERR::SUCESS);
	}

	// �� ���� ���.
	if (RemainSize > completionBytes)
	{
		RemainSize -= completionBytes;
		BufferPosition += completionBytes;
		return static_cast<int>(NET_ERR::SEND_ERROR_NEED_MORE_DATA);
	}

	// �����͸� �� ���� ���.
	if (RemainSize - completionBytes > 0)
	{
		RemainSize = 0;
		BufferPosition = 0;
		// �ʱ�ȭ �� Ŭ���� ���� ��.
		return static_cast<int>(NET_ERR::SEND_ERROR_MORE_DATA);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::AsyncTcpSend::RemainingMsg(const SOCKET& sock, const LPOVERLAPPED& overraped)
{
	std::lock_guard lock(WriteLock);

	// SendBuffer�� ���� �����Ͱ� �ִ°�.
	if (RemainSize != 0)
	{
		SendIO(sock, overraped);
		return static_cast<int>(NET_ERR::SUCESS);
	}

	if (WriteSize != 0)
	{
		SwapBuffer();
		SendIO(sock, overraped);

		return static_cast<int>(NET_ERR::SUCESS);
	}
	return static_cast<int>(NET_ERR::FAIL);
}

void gzz::AsyncTcpSend::Release()
{
	RemainSize = 0;
	WriteSize = 0;
	BufferPosition = 0;
}

int gzz::AsyncTcpSend::SendIO(const SOCKET& sock, const LPOVERLAPPED& overraped)
{
	int _result;
	DWORD _bytes;

	ZeroMemory(overraped, sizeof(OVERLAPPED));

	Wsabuf.buf = SendBuf + BufferPosition;
	Wsabuf.len = RemainSize;

	_result = WSASend(
		sock,					// ���� �� ����	
		&Wsabuf,				// WsaBuf
		1,						// WsaBuf Count
		&_bytes,				// ���� ����Ʈ ��
		0,						// flag
		overraped,				// OVERLAPPED ����ü
		nullptr);				// IOCP ��  nullptr, callback �Լ�

	if (_result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		Release();
		return static_cast<int>(NET_ERR::WSA_SEND_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

bool gzz::AsyncTcpSend::Write(PACKET packet)
{
	if (packet.head.size + WriteSize > MAX_SEND_BUF_SIZE)
	{
		return false;
	}

	memcpy_s(WriteBuf + WriteSize, MAX_SEND_BUF_SIZE - WriteSize, packet.memory, packet.head.size);
	WriteSize += packet.head.size;

	return true;
}

void gzz::AsyncTcpSend::SwapBuffer()
{
	char* tmp = SendBuf;
	SendBuf = WriteBuf;
	WriteBuf = tmp;

	RemainSize = WriteSize;
	WriteSize = 0;
}
#pragma endregion

//gzz::AsyncTcpSend::AsyncTcpSend()
//{
//	Release();
//}
//
//gzz::AsyncTcpSend::~AsyncTcpSend()
//{
//	Release();
//}
//
//int gzz::AsyncTcpSend::SendMsg(const SOCKET& sock, const LPOVERLAPPED& overraped, PACKET& packet)
//{
//	Wsabuf.buf = packet.memory;
//	Wsabuf.len = packet.head.size;
//
//	return SendIO(sock, overraped);
//}
//
//int gzz::AsyncTcpSend::SendProcess(const int completionBytes)
//{
//	return static_cast<int>(NET_ERR::SUCESS);
//	/*if (Wsabuf.len == completionBytes)
//	{
//		return static_cast<int>(NET_ERR::SUCESS);
//	}
//	return static_cast<int>(NET_ERR::FAIL);*/
//}
//
//void gzz::AsyncTcpSend::Release()
//{
//	Wsabuf.buf = nullptr;
//	Wsabuf.len = 0;
//}
//
//int gzz::AsyncTcpSend::SendIO(const SOCKET& sock, const LPOVERLAPPED& overraped)
//{
//	int _result;
//	DWORD _bytes;
//
//	ZeroMemory(overraped, sizeof(OVERLAPPED));
//
//	_result = WSASend(
//		sock,					// ���� �� ����	
//		&Wsabuf,				// WsaBuf
//		1,						// WsaBuf Count
//		&_bytes,				// ���� ����Ʈ ��
//		0,						// flag
//		overraped,				// OVERLAPPED ����ü
//		nullptr);				// IOCP ��  nullptr, callback �Լ�
//
//	if (_result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
//	{
//		Release();
//		return static_cast<int>(NET_ERR::WSA_SEND_FAIL);
//	}
//	return static_cast<int>(NET_ERR::SUCESS);
//}