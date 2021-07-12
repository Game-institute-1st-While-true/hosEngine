#include "TcpClientEX.h"

gzz::TcpClientEX::TcpClientEX(int id, const char* dst, const short port, HANDLE iocpHandle, LPOVERLAPPED overLapped)
	:TcpSocketEX(id)
{
	Open();
	SetSocketOption();
	Bind(INVALID_SOCKET, iocpHandle);
	//Connect(dst, port, overLapped);
}

gzz::TcpClientEX::TcpClientEX(int id, HANDLE iocpHandle)
	:TcpSocketEX(id)
{
	Open();
	SetSocketOption();
	Bind(INVALID_SOCKET, iocpHandle);
}

gzz::TcpClientEX::~TcpClientEX()
{
	ForceClose();
}

int gzz::TcpClientEX::Open()
{
	int _err = TcpSocketEX::Open();
	if (_err != static_cast<int>(NET_ERR::SUCESS))
	{
		return _err;
	}
	return _err;
}

int gzz::TcpClientEX::Bind(SOCKET sock, HANDLE iocpHandle)
{
	SOCKADDR_IN _addr;
	ZeroMemory(&_addr, sizeof(SOCKADDR_IN));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = 0;

	int _err = bind(Sock, reinterpret_cast<SOCKADDR*>(&_addr), sizeof(_addr));

	if (_err == SOCKET_ERROR)
	{
		return static_cast<int>(NET_ERR::BIND_FAIL);
	}

	HANDLE handle = CreateIoCompletionPort((HANDLE)Sock, iocpHandle, reinterpret_cast<ULONG_PTR>(this), 0);
	if (handle == NULL)
	{
		return static_cast<int>(NET_ERR::COMPLETIONPORT_BINDING_FAIL);
	}

	return static_cast<int>(NET_ERR::SUCESS);
}

void gzz::TcpClientEX::Release()
{
	AsyncTcpRecv::Release();
	AsyncTcpSend::Release();
}

gzz::ISend* gzz::TcpClientEX::GetInterfaceSend()
{
	return static_cast<AsyncTcpSend*>(this);
}

gzz::IReceive* gzz::TcpClientEX::GetInterfaceRecv()
{
	return static_cast<AsyncTcpRecv*>(this);
}

int gzz::TcpClientEX::Connect(const char* dst, const short port, LPOVERLAPPED overLapped)
{
	SetIP(dst);
	SetPort(port);

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family = AF_INET;
	inet_pton(AF_INET, dst, &Addr.sin_addr);
	Addr.sin_port = htons(port);

	LPFN_CONNECTEX _connect_ex_ptr = nullptr;
	GUID		   _guidConnectEX = WSAID_CONNECTEX;
	DWORD _bytes = 0;

	int _err = WSAIoctl(
		Sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&_guidConnectEX, sizeof(_guidConnectEX),
		&_connect_ex_ptr, sizeof(_connect_ex_ptr),
		&_bytes, NULL, NULL);

	char _firstSendBuf[4] = "khg";
	//const int _firstSendSize = 3;
	const int _firstSendSize = 0;

	_err = _connect_ex_ptr(
		Sock,
		(SOCKADDR*)&Addr,
		sizeof(Addr),
		_firstSendBuf,
		_firstSendSize,
		&_bytes,
		overLapped
	);

	if (_err != TRUE)
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
		{
			return static_cast<int>(NET_ERR::SUCESS);
		}
		else
		{
			return static_cast<int>(NET_ERR::CONNECT_FAIL);
		}
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpClientEX::PostConnection()
{
	ConnFlag = true;

	SetKeepAlived();
	SetNagle();

	return static_cast<int>(NET_ERR::SUCESS);
}


int gzz::TcpClientEX::SetSocketOption()
{
	int _err = 0;
	_err = setsockopt(
		Sock,
		SOL_SOCKET,
		SO_UPDATE_CONNECT_CONTEXT,
		NULL, 0);

	if (_err == SOCKET_ERROR)
	{
		return static_cast<int>(NET_ERR::SET_SOCKET_UPDATE_CONNECT_CONETEXT_FAIL);
	}
	return static_cast<int>(NET_ERR::SUCESS);
}
