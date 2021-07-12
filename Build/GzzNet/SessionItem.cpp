#include "SessionItem.h"
#include "Note.h"

gzz::SessionItem::SessionItem(int id)
	:ICompletionItemEX(id), Socket(nullptr), ListenSock(INVALID_SOCKET), RecvItem(nullptr), SessionType(SESSION_TYPE::NONE)
{
}

gzz::SessionItem::SessionItem(int id, SESSION_TYPE type, SOCKET listener, LPOVERLAPPED recvItem, TcpSocketEX* sock)
	: ICompletionItemEX(id)
{
	SettingItem(type, listener, recvItem, sock);
}

gzz::SessionItem::~SessionItem()
{
}

void gzz::SessionItem::SettingItem(SESSION_TYPE type, SOCKET listener, LPOVERLAPPED recvItem, TcpSocketEX* sock)
{
	SessionType = type;
	ListenSock = listener;
	Socket = sock;
	SockId = Socket->GetId();
	RecvItem = recvItem;

	switch (type)
	{
	case SESSION_TYPE::ACCEPT:
		SessionFunc = &SessionItem::AcceptProcess;
		break;
	case SESSION_TYPE::CONNECT:
		SessionFunc = &SessionItem::ConnectProcess;
		break;
	}
}

int gzz::SessionItem::Process(int completionBytes, LPOVERLAPPED other)
{
	int _err = static_cast<int>(NET_ERR::SUCESS);
	_err = SessionFunc(*this, completionBytes);
	return _err;
}

int gzz::SessionItem::AcceptProcess(int completionBytes)
{
	DWORD second = 0;
	int second_len = sizeof(second);
	int _err = static_cast<int>(NET_ERR::SUCESS);

	TcpGuestEX* _guest = static_cast<TcpGuestEX*>(Socket);

	_err = ::getsockopt(Socket->GetSock(), SOL_SOCKET, SO_CONNECT_TIME, (char*)&second, &second_len);

	if (second == 0xffffffff)
	{
		return _err;
	}

	_guest->SetSocketOption(ListenSock);
	_guest->PostAcception();
	
	_err = _guest->Receive(_guest->GetSock(), RecvItem);

	if (_err == static_cast<int>(NET_ERR::WSA_RECV_FAIL))
	{
		SwitchingDisconnected();
		_guest->Close(static_cast<LPOVERLAPPED>(this));
	}

	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::ACCEPT);
	pack.id = _guest->GetId();

	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	ZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));

	Note::GetIns()->Write(L"SYSTEM : ", L"Session Accept Sucess ", L" ID : ", Socket->GetId(), L" IP : ", Socket->GetIP().c_str(), L" PORT : ", Socket->GetPort());
	SwitchingDisconnected();

	return _err;
}

int gzz::SessionItem::ConnectProcess(int completionBytes)
{
	int _err = static_cast<int>(NET_ERR::SUCESS);
	TcpClientEX* _client = static_cast<TcpClientEX*>(Socket);

	DWORD second = 0;
	int second_len = sizeof(second);

	_err = ::getsockopt(Socket->GetSock(), SOL_SOCKET, SO_CONNECT_TIME, (char*)&second, &second_len);

	if (second == 0xffffffff)
	{
		PACKAGE pack;
		pack.packet.head.size = SYSTEM_PACKET_SIZE;
		pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT_FAIL);
		pack.id = _client->GetId();
		static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
		Note::GetIns()->Write(L"SYSTEM : ", L"CONNECT FAIL... OTL");
		return _err;
	}

	SwitchingDisconnected();

	_client->SetSocketOption();
	_client->PostConnection();
	_client->Receive(_client->GetSock(), RecvItem);

	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT);
	pack.id = _client->GetId();

	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	ZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));

	Note::GetIns()->Write(L"SYSTEM : ", L"Session Connect Sucess", L" ID : ", Socket->GetId(), L" IP : ", Socket->GetIP().c_str(), L" PORT : ", Socket->GetPort());
	return _err;
}

void gzz::SessionItem::SwitchingConnected()
{
	switch (SessionType)
	{
	case SESSION_TYPE::ACCEPT:
		SessionFunc = &SessionItem::AcceptProcess;
		break;

	case SESSION_TYPE::CONNECT:
		SessionFunc = &SessionItem::ConnectProcess;
		break;
	}
}

void gzz::SessionItem::SwitchingDisconnected()
{
	switch (SessionType)
	{
	case SESSION_TYPE::ACCEPT:
		SessionFunc = &SessionItem::Disconnect_Accept;
		break;

	case SESSION_TYPE::CONNECT:
		SessionFunc = &SessionItem::Disconnect_Connect;
		break;
	}
}

int gzz::SessionItem::Disconnect_Accept(int completionBytes)
{
	SwitchingConnected();

	Socket->PostClose();
	Note::GetIns()->Write(L"SYSTEM : ", L"Session Disconnect", L" ID : ", Socket->GetId(), L" IP : ", Socket->GetIP().c_str(), L" PORT : ", Socket->GetPort());

	TcpGuestEX* _guest = static_cast<TcpGuestEX*>(Socket);
	int _err = static_cast<int>(NET_ERR::SUCESS);

	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::DISCONNECT_ACCEPT);
	pack.id = _guest->GetId();

	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));

	_err = _guest->Accept(ListenSock, static_cast<LPOVERLAPPED>(this));

	switch (static_cast<NET_ERR>(_err))
	{
	case NET_ERR::SET_SOCKET_UPDATE_ACCEPT_CONETEXT_FAIL:
		break;
	case NET_ERR::GET_ACCPEPT_EX_PTR_FAIL:
		break;
	case NET_ERR::SUCESS:
		break;
	}

	return _err;
}

int gzz::SessionItem::Disconnect_Connect(int completionBytes)
{
	SwitchingConnected();
	Socket->PostClose();

	Note::GetIns()->Write(L"SYSTEM : ", L"Session Disconnect", L" ID : ", Socket->GetId(), L" IP : ", Socket->GetIP().c_str(), L" PORT : ", Socket->GetPort());

	TcpClientEX* _client = static_cast<TcpClientEX*>(Socket);
	int _err = static_cast<int>(NET_ERR::SUCESS);

	PACKAGE pack;
	pack.packet.head.size = SYSTEM_PACKET_SIZE;
	pack.packet.head.protocol = static_cast<unsigned short>(SYSTEM_PROTOCOL::DISCONNECT_CONNECT);
	pack.id = _client->GetId();

	static_cast<RecvItemEX*>(RecvItem)->AddMessage(*reinterpret_cast<PACKAGE*>(&pack));
	return _err;
}