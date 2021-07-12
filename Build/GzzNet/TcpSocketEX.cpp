#include "TcpSocketEX.h"

gzz::TcpSocketEX::TcpSocketEX(int id)
	:Id(id),
	ConnectionBufSize(MAX_PACKET_SIZE), ConnFlag(false)
{
	ZeroMemory(ConnectionBuffer, MAX_PACKET_SIZE);
}

gzz::TcpSocketEX::~TcpSocketEX()
{
}

int gzz::TcpSocketEX::Open()
{
	if (Sock == INVALID_SOCKET)
	{
		Sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
		if (Sock == INVALID_SOCKET)
		{
			return static_cast<int>(NET_ERR::SOCKET_CREATE_FAIL);
		}
	}
	else
	{
		return static_cast<int>(NET_ERR::SOCKET_ALREADY_ASSIGNED);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpSocketEX::Close(LPOVERLAPPED overlapped)
{
	Release();
	LPFN_DISCONNECTEX disconnect_ptr = nullptr;
	GUID guidDisconnect = WSAID_DISCONNECTEX;
	DWORD bytes;

	int err = WSAIoctl(Sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisconnect, sizeof(guidDisconnect),
		&disconnect_ptr, sizeof(disconnect_ptr),
		&bytes, NULL, NULL);

	if (err != 0)
	{
		return static_cast<int>(NET_ERR::DISCONNECT_ALLOC_FAIL);
	}

	err = disconnect_ptr(Sock, overlapped, TF_REUSE_SOCKET, 0);

	if (err != TRUE &&
		WSAGetLastError() != ERROR_IO_PENDING)
	{
		return static_cast<int>(NET_ERR::DISCONNECT_EX_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpSocketEX::PostClose()
{
	ConnFlag = false;
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpSocketEX::ForceClose()
{
	Release();

	LINGER  ling = { 0, };
	ling.l_onoff = 1;
	ling.l_linger = 0;

	setsockopt(Sock, SOL_SOCKET, SO_LINGER, (CHAR*)&ling, sizeof(ling));
	closesocket(Sock);
	ConnFlag = false;
	Sock = INVALID_SOCKET;
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpSocketEX::SetKeepAlived()
{
	tcp_keepalive keepalived;
	keepalived.onoff = 1;
	keepalived.keepalivetime = DEFAULT_KEEPALIVED_TIME;
	keepalived.keepaliveinterval = DEFAULT_KEEPALIVED_INTERVAL_TIME;
	DWORD bytes;

	int _err;

	_err = WSAIoctl
	(
		Sock,
		SIO_KEEPALIVE_VALS,
		&keepalived, sizeof(keepalived),
		0, 0,
		&bytes, NULL, NULL
	);

	return static_cast<int>(NET_ERR::SUCESS);
}

void gzz::TcpSocketEX::SetNagle()
{
	int option = TRUE;
	setsockopt(Sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
}
