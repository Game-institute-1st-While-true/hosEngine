#include "AsyncTcpRecv.h"

gzz::AsyncTcpRecv::AsyncTcpRecv()
	:RecvPosition(0)
{
	RecvBuf = new char[MAX_RECV_BUF_SIZE];
	ZeroMemory(RecvBuf, MAX_RECV_BUF_SIZE);

	Wsabuf.buf = nullptr;
	Wsabuf.len = 0;
}

gzz::AsyncTcpRecv::~AsyncTcpRecv()
{
	delete[] RecvBuf;
}

int gzz::AsyncTcpRecv::Receive(const SOCKET& sock, const LPOVERLAPPED& overraped)
{
	return RecvIO(sock, overraped);
}

//gzz::PACKET gzz::AsyncTcpRecv::GetPacket(const int remainBytes, const int readBytes)
//{
//	PACKET packet = { static_cast<int>(RECV_STATUS::OK), };
//
//	// ���� ����Ʈ���� ������ ���� ���� ���.
//	if (remainBytes < PACKET_HEADER_SIZE)
//	{
//		memcpy_s(RecvBuf, MAX_RECV_BUF_SIZE, RecvBuf + readBytes, remainBytes);
//		RecvPosition = remainBytes;
//		packet.head.size = static_cast<int>(RECV_STATUS::MORE);
//		return packet;
//	}
//
//	// ����� �д´�.
//	packet.head.size = *reinterpret_cast<unsigned short*>(RecvBuf + readBytes);
//
//	// ���� ����Ʈ���� �������� ������ ���� ���. (�� �޾ƾ� �ϴ� ���)
//	if (remainBytes < packet.head.size)
//	{
//		memcpy_s(RecvBuf, MAX_RECV_BUF_SIZE, RecvBuf + readBytes, remainBytes);
//		RecvPosition = remainBytes;
//		packet.head.size = static_cast<int>(RECV_STATUS::MORE);
//		return packet;
//	}
//
//	// �ٸ� ��� (�� ���� �� ��� or �� �Ѱ� �� ���), ��ü ��Ŷ�� ������.	
//	memcpy_s(&packet, packet.head.size, RecvBuf + readBytes, packet.head.size);
//	return packet;
//}

gzz::PACKET gzz::AsyncTcpRecv::GetPacket(const int remainBytes, const int readBytes)
{
	//PACKET packet = { static_cast<int>(RECV_STATUS::OK), };

	//if (RecvPosition != 0)
	//{
	//	// ����� �д´�.
	//	packet.head.size = *reinterpret_cast<unsigned short*>(RecvBuf + readBytes);

	//	// ���� ����Ʈ���� �������� ������ ���� ���. (�� �޾ƾ� �ϴ� ���)
	//	if (remainBytes < packet.head.size)
	//	{
	//		memcpy_s(RecvBuf + RecvPosition, MAX_RECV_BUF_SIZE - RecvPosition, RecvBuf + readBytes, remainBytes);
	//		RecvPosition += remainBytes;
	//		packet.head.size = static_cast<int>(RECV_STATUS::MORE);
	//		return packet;
	//	}

	//	// �ٸ� ��� (�� ���� �� ��� or �� �Ѱ� �� ���), ��ü ��Ŷ�� ������.	
	//	memcpy_s(&packet, packet.head.size, RecvBuf + readBytes, packet.head.size);
	//	//RecvPosition = 0;
	//	return packet;
	//}
	//else
	//{
	//	// ���� ����Ʈ���� ������ ���� ���� ���.
	//	if (remainBytes < PACKET_HEADER_SIZE)
	//	{
	//		memcpy_s(RecvBuf, MAX_RECV_BUF_SIZE, RecvBuf + readBytes, remainBytes);
	//		RecvPosition = remainBytes;
	//		packet.head.size = static_cast<int>(RECV_STATUS::MORE);
	//		return packet;
	//	}

	//	// ����� �д´�.
	//	packet.head.size = *reinterpret_cast<unsigned short*>(RecvBuf + readBytes);

	//	// ���� ����Ʈ���� �������� ������ ���� ���. (�� �޾ƾ� �ϴ� ���)
	//	if (remainBytes < packet.head.size)
	//	{
	//		memcpy_s(RecvBuf, MAX_RECV_BUF_SIZE, RecvBuf + readBytes, remainBytes);
	//		RecvPosition += remainBytes;
	//		packet.head.size = static_cast<int>(RECV_STATUS::MORE);
	//		return packet;
	//	}

	//	// �ٸ� ��� (�� ���� �� ��� or �� �Ѱ� �� ���), ��ü ��Ŷ�� ������.	
	//	memcpy_s(&packet, packet.head.size, RecvBuf + readBytes, packet.head.size);
	//	return packet;
	//}
	return PACKET();
}

void gzz::AsyncTcpRecv::Release()
{
	ZeroMemory(RecvBuf, sizeof(MAX_RECV_BUF_SIZE));
	RecvPosition = 0;
}

int gzz::AsyncTcpRecv::RecvIO(const SOCKET& sock, const LPOVERLAPPED& overraped)
{
	DWORD _bytes = 0;
	DWORD _flag = 0;

	ZeroMemory(overraped, sizeof(OVERLAPPED));
	//Wsabuf.buf = RecvBuf + RecvPosition;
	//Wsabuf.len = MAX_RECV_BUF_SIZE - RecvPosition;

	Wsabuf.buf = RecvBuf;
	Wsabuf.len = MAX_RECV_BUF_SIZE;

	int _result = WSARecv(sock,
		&Wsabuf,
		1,
		&_bytes,
		&_flag,
		overraped,
		nullptr);

	if (_result == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
	{
		return static_cast<int>(NET_ERR::WSA_RECV_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

