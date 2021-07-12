#include "TcpClient.h"
#include "Note.h"
#include "RecvItem.h"

gzz::TcpClient::TcpClient(int id, const char* dst, const short port, HANDLE iocpHandle, LPOVERLAPPED overLapped)
	:TcpSocketEX(id), IocpHandle(iocpHandle), RecvItem(overLapped)
{
}

gzz::TcpClient::TcpClient(int id, HANDLE iocpHandle)
	: TcpSocketEX(id), IocpHandle(iocpHandle)
{

}

gzz::TcpClient::~TcpClient()
{
}

int gzz::TcpClient::Open()
{
	int _err = TcpSocketEX::Open();
	if (_err != static_cast<int>(NET_ERR::SUCESS))
	{
		return _err;
	}
	return _err;
}

int gzz::TcpClient::Bind(SOCKET sock, HANDLE iocpHandle)
{
	SOCKADDR_IN _addr;
	ZeroMemory(&_addr, sizeof(SOCKADDR_IN));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = 0;

	int _err = bind(Sock, (SOCKADDR*)&_addr, sizeof(_addr));

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

void gzz::TcpClient::Release()
{
	AsyncTcpRecv::Release();
	AsyncTcpSend::Release();
}

gzz::ISend* gzz::TcpClient::GetInterfaceSend()
{
	return static_cast<AsyncTcpSend*>(this);
}

gzz::IReceive* gzz::TcpClient::GetInterfaceRecv()
{
	return static_cast<AsyncTcpRecv*>(this);
}

int gzz::TcpClient::Close(LPOVERLAPPED overlapped)
{
	if (ConnFlag)
	{
		PostClose();
		Release();
		ForceClose();
		OnDisconnect();
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpClient::PostClose()
{
	ConnFlag = false;
	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpClient::ForceClose()
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

int gzz::TcpClient::Connect(const char* dst, const short port, LPOVERLAPPED overLapped)
{
	Open();
	SetSocketOption();
	Bind(INVALID_SOCKET, IocpHandle);

	SetIP(dst);
	SetPort(port);
	RecvItem = overLapped;

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family = AF_INET;
	inet_pton(AF_INET, dst, &Addr.sin_addr);
	Addr.sin_port = htons(port);

	int _err = connect(Sock, reinterpret_cast<SOCKADDR*>(&Addr), sizeof(Addr));

	if (_err != 0)
	{
		OnConnectFail();
		return static_cast<int>(NET_ERR::CONNECT_FAIL);
	}
	else
	{
		OnConnect();
		Note::GetIns()->Write(L"SYSTEM : ", L"Session Connect Sucess", L" ID : ", GetId(), L" IP : ", GetIP().c_str(), L" PORT : ", GetPort());
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

void gzz::TcpClient::OnConnect()
{
	PostConnection();
	SetSocketOption();
	Receive(Sock, RecvItem);

	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT);
	pack.id = GetId();

	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	int _err = static_cast<int>(NET_ERR::SUCESS);
}

void gzz::TcpClient::OnDisconnect()
{
	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::DISCONNECT_CONNECT);
	pack.id = GetId();
	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	Note::GetIns()->Write(L"SYSTEM : ", L"Session Disconnect", L" ID : ", GetId(), L" IP : ", GetIP().c_str(), L" PORT : ", GetPort());
}

void gzz::TcpClient::OnConnectFail()
{
	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT_FAIL);
	pack.id = GetId();
	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	Note::GetIns()->Write(L"SYSTEM : ", L"CONNECT FAIL... OTL");
}

int gzz::TcpClient::PostConnection()
{
	ConnFlag = true;

	SetNagle();
	SetKeepAlived();

	return static_cast<int>(NET_ERR::SUCESS);
}

int gzz::TcpClient::SetSocketOption()
{
	return static_cast<int>(NET_ERR::SUCESS);
}
