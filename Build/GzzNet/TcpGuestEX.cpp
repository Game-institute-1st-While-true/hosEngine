#include "TcpGuestEX.h"
#include <iostream>
gzz::TcpGuestEX::TcpGuestEX(int id, SOCKET listen, HANDLE iocpHandle, LPOVERLAPPED overLapped)
	:TcpSocketEX(id)
{
	Open();
	Bind(listen, iocpHandle);
	Accept(listen, overLapped);
}

gzz::TcpGuestEX::~TcpGuestEX()
{
	ForceClose();
}


int gzz::TcpGuestEX::Open()
{
	int _err = TcpSocketEX::Open();
	if (_err != static_cast<int>(NET_ERR::SUCESS))
	{
		return _err;
	}
	return _err;
}

int gzz::TcpGuestEX::Bind(SOCKET sock, HANDLE iocpHandle)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)Sock, iocpHandle, reinterpret_cast<ULONG_PTR>(this), 0);
	if (handle == NULL)
	{
		return static_cast<int>(NET_ERR::COMPLETIONPORT_BINDING_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

void gzz::TcpGuestEX::Release()
{
	AsyncTcpRecv::Release();
	AsyncTcpSend::Release();
}

gzz::ISend* gzz::TcpGuestEX::GetInterfaceSend()
{
	return static_cast<AsyncTcpSend*>(this);
}

gzz::IReceive* gzz::TcpGuestEX::GetInterfaceRecv()
{
	return static_cast<AsyncTcpRecv*>(this);
}

int gzz::TcpGuestEX::SetSocketOption(SOCKET listen)
{
	int _err = 0;
	_err = setsockopt(Sock,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT,
		(char*)&listen,
		sizeof(listen));

	if (_err == SOCKET_ERROR)
	{
		return static_cast<int>(NET_ERR::SET_SOCKET_UPDATE_ACCEPT_CONETEXT_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpGuestEX::Accept(SOCKET listen, LPOVERLAPPED overLapped)
{
	LPFN_ACCEPTEX _accept_ex_ptr = nullptr;
	GUID		  _guidAcceptEX = WSAID_ACCEPTEX;
	DWORD		  _bytes = 0;

	ZeroMemory(ConnectionBuffer, MAX_PACKET_SIZE);

	int _err = WSAIoctl(
		listen, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&_guidAcceptEX, sizeof(_guidAcceptEX),
		&_accept_ex_ptr, sizeof(_accept_ex_ptr),
		&_bytes, NULL, NULL);

	if (_err == SOCKET_ERROR)
	{
		return static_cast<int>(NET_ERR::GET_ACCPEPT_EX_PTR_FAIL);
	}

	_err = _accept_ex_ptr(
		listen,
		Sock,
		ConnectionBuffer,
		0,
		sizeof(Addr) + 16,
		sizeof(Addr) + 16,
		&_bytes,
		overLapped
	);

	if (_err != TRUE)
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
		{
			return static_cast<int>(NET_ERR::SUCESS);
		}

		if (WSAGetLastError() == WSAECONNRESET)
		{
			return static_cast<int>(NET_ERR::ACCEPT_FAIL);
		}
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpGuestEX::PostAcception()
{
	int		  _bytes = 0;
	_bytes = sizeof(Addr);
	getpeername(Sock, (SOCKADDR*)&Addr, &_bytes);

	char Ip[32];
	inet_ntop(AF_INET, &Addr.sin_addr, Ip, sizeof(Ip));

	SetIP(Ip);
	SetPort(ntohs(Addr.sin_port));

	SetNagle();
	SetKeepAlived();

	ConnFlag = true;

	return static_cast<int>(NET_ERR::SUCESS);
}
