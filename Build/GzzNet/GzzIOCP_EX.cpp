#include "GzzIOCP_EX.h"


gzz::GzzIOCP_EX::GzzIOCP_EX(const int maxIoThreadCount, const int maxSessionCount)
	: IocpHandle(NULL), CloseCount(0), IsOnOff(true), MaxIoThreadCount(maxIoThreadCount), MaxSessionCount(maxSessionCount), Host(nullptr)
{
	int size = MaxSessionCount;
	SessionPool.reserve(size);
	SendItemPool.reserve(size);
	RecvItemPool.reserve(size);
	SessionItemPool.reserve(size);
	CreateCompletionItem(size);
}

gzz::GzzIOCP_EX::~GzzIOCP_EX()
{
	Stop();
	size_t size = SendItemPool.size();
	for (int i = 0; i < size; i++)
	{
		if (static_cast<ICompletionItemEX*>(SendItemPool[i]) != nullptr)
		{
			delete static_cast<ICompletionItemEX*>(SendItemPool[i]);
		}
	}
	SendItemPool.clear();

	size = RecvItemPool.size();
	for (int i = 0; i < size; i++)
	{
		if (static_cast<ICompletionItemEX*>(RecvItemPool[i]) != nullptr)
		{
			delete static_cast<ICompletionItemEX*>(RecvItemPool[i]);
		}
	}
	RecvItemPool.clear();

	size = SessionItemPool.size();
	for (int i = 0; i < size; i++)
	{
		if (static_cast<ICompletionItemEX*>(SessionItemPool[i]) != nullptr)
		{
			delete static_cast<ICompletionItemEX*>(SessionItemPool[i]);
		}
	}
	RecvItemPool.clear();

	size = SessionPool.size();
	for (int i = 0; i < size; i++)
	{
		if (SessionPool[i] != nullptr)
		{
			delete SessionPool[i];
		}
	}
	SessionPool.clear();

	if (Host != nullptr)
	{
		delete Host;
	}

	delete SendItemIdPool;
	WSACleanup();
}

int gzz::GzzIOCP_EX::Init()
{
	WSADATA _wsa;
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
	{
		return static_cast<int>(NET_ERR::START_UP_FAIL);
	}

	int _err = static_cast<int>(GZZ_ERR::SUCESS);
	_err = CreateCompletionPort();
	return _err;
}

int gzz::GzzIOCP_EX::Run()
{
	IsOnOff = true;
	CreateIoWoker();
	return static_cast<int>(GZZ_ERR::SUCESS);
}

int gzz::GzzIOCP_EX::Stop()
{
	DestroyThread();
	CloseSession();
	return static_cast<int>(GZZ_ERR::SUCESS);
}

int gzz::GzzIOCP_EX::Open(unsigned short maxConnection, unsigned short port)
{
	if (Host != nullptr)
	{
		return static_cast<int>(GZZ_ERR::FAIL);
	}
	Host = new TcpHostEX(maxConnection, port, IocpHandle);
	for (int index = 0; index < maxConnection; index++)
	{
		TcpGuestEX* _guest = new TcpGuestEX(index, Host->GetSock(), IocpHandle, SessionItemPool[index]);
		SessionPool.push_back(_guest);
		ReservationSession(SESSION_TYPE::ACCEPT, index, static_cast<IReceive*>(_guest), static_cast<ISend*>(_guest));
	}
	return static_cast<int>(GZZ_ERR::SUCESS);
}

int gzz::GzzIOCP_EX::Connect(const char* dst, const short port)
{
	int _id = static_cast<int>(SessionPool.size());

	if (_id + 1 > MaxSessionCount)
	{
		return -1;
	}

	TcpClientEX* _client = new TcpClientEX(_id, IocpHandle);
	SessionPool.push_back(_client);
	ReservationSession(SESSION_TYPE::CONNECT, _id, static_cast<IReceive*>(_client), static_cast<ISend*>(_client));
	_client->Connect(dst, port, SessionItemPool[_id]);
	return _id;
}

bool gzz::GzzIOCP_EX::Connect(int id, const char* dst, const short port)
{
	if (SessionPool.size() < id || id < 0)
	{
		return false;
	}
	static_cast<TcpClientEX*>(SessionPool[id])->Connect(dst, port, SessionItemPool[id]);
	return true;
}

int gzz::GzzIOCP_EX::UnExConnect(const char* dst, const short port)
{
	int _id = static_cast<int>(SessionPool.size());

	if (_id + 1 > MaxSessionCount)
	{
		return -1;
	}

	TcpClient* _client = new TcpClient(_id, IocpHandle);
	SessionPool.push_back(_client);
	ReservationSession(SESSION_TYPE::CONNECT, _id, static_cast<IReceive*>(_client), static_cast<ISend*>(_client));
	_client->Connect(dst, port, RecvItemPool[_id]);
	return _id;
}

bool gzz::GzzIOCP_EX::UnExConnect(int id, const char* dst, const short port)
{
	if (SessionPool.size() < id || id < 0)
	{
		return false;
	}
	static_cast<TcpClient*>(SessionPool[id])->Connect(dst, port, RecvItemPool[id]);
	return true;
}

int gzz::GzzIOCP_EX::Disconnect(const int Id)
{
	if (SessionPool[Id]->IsConnection() == true)
	{
		SessionPool[Id]->Close(SessionItemPool[Id]);
		return static_cast<int>(GZZ_ERR::SUCESS);
	}
	else
	{
		return static_cast<int>(GZZ_ERR::FAIL);
	}
}

void gzz::GzzIOCP_EX::FourceEnquePackage(PACKAGE& package)
{
	MessageQueue.Enqueue(package);
}

bool gzz::GzzIOCP_EX::GetPackage(PACKAGE& package)
{
	return 	MessageQueue.Dequeue(package);
}

int gzz::GzzIOCP_EX::SendMsg(const int id, PACKET packet)
{
	int result = static_cast<SendItemEX*>(SendItemPool[id])->SendMsg(packet);
	return result;
}

int gzz::GzzIOCP_EX::CreateCompletionPort()
{
	IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MaxIoThreadCount);
	if (IocpHandle == NULL)
	{
		return static_cast<int>(GZZ_ERR::CREATE_COMPLETION_PORT_FAIL);
	}
	return static_cast<int>(GZZ_ERR::SUCESS);
}

void gzz::GzzIOCP_EX::CreateIoWoker()
{
	for (int i = 0; i < MaxIoThreadCount; i++)
	{
		Note::GetIns()->Write(L"SYSTEM : ", L"WokerThread Start");
		IoWorkers.emplace_back([this]() { IoFunc(); });
	}
}

int gzz::GzzIOCP_EX::IoFunc()
{
	BOOL _ok = TRUE;
	DWORD _completionBytes;
	LPOVERLAPPED _overlapped = nullptr;
	TcpSocketEX* _completionKey = nullptr;
	while (true)
	{
		_ok = GetQueuedCompletionStatus
		(
			IocpHandle,
			&_completionBytes,
			(PULONG_PTR)&_completionKey,
			&_overlapped,
			INFINITE
		);

		// 서버 종료 
		if (_ok == TRUE && _completionKey == nullptr && _overlapped == nullptr)
		{
			IoCloseFlag();
			break;
		}
		ICompletionItemEX* _Item = static_cast<ICompletionItemEX*>(_overlapped);
		_Item->Process(_completionBytes, SessionItemPool[_Item->GetSockId()]);
	}
	return 0;
}

int gzz::GzzIOCP_EX::DestroyThread()
{
	IoCloseFlag();
	for (auto& woker : IoWorkers)
	{
		if (woker.joinable() == true)
		{
			woker.join();
		}
	}
	return static_cast<int>(GZZ_ERR::SUCESS);
}

void gzz::GzzIOCP_EX::IoCloseFlag()
{
	PostQueuedCompletionStatus(IocpHandle, 0, 0, nullptr);
}

void gzz::GzzIOCP_EX::CloseSession()
{
	for (auto sock : SessionPool)
	{
		sock->Close(SessionItemPool[sock->GetId()]);
		sock->ForceClose();
	}

	if (Host != nullptr)
	{
		Host->ForceClose();
	}
}


void gzz::GzzIOCP_EX::CreateCompletionItem(const int size)
{
	for (int i = 0; i < size; i++)
	{
		SendItemPool.push_back(new SendItemEX(i));
		RecvItemPool.push_back(new RecvItemEX(i));
		SessionItemPool.push_back(new SessionItem(i));
	}
}

void gzz::GzzIOCP_EX::ReservationSession(SESSION_TYPE type, const int id, IReceive* recv, ISend* send)
{
	//static_cast<RecvItemEX*>(RecvItemPool[id])->SettingItem(SessionPool[id], recv, &MessageQueue);
	//switch (type)
	//{
	//case SESSION_TYPE::ACCEPT:
	//	static_cast<SessionItem*>(SessionItemPool[id])->SettingItem(type, Host->GetSock(), RecvItemPool[id], SessionPool[id]);
	//	break;
	//case SESSION_TYPE::CONNECT:
	//	static_cast<SessionItem*>(SessionItemPool[id])->SettingItem(type, INVALID_SOCKET, RecvItemPool[id], SessionPool[id]);
	//	break;
	//}
	static_cast<SendItemEX*>(SendItemPool[id])->SettingItem(SessionPool[id], send);
	static_cast<RecvItemEX*>(RecvItemPool[id])->SettingItem(SessionPool[id], recv, &MessageQueue);
	switch (type)
	{
	case SESSION_TYPE::ACCEPT:
		static_cast<SessionItem*>(SessionItemPool[id])->SettingItem(type, Host->GetSock(), RecvItemPool[id], SessionPool[id]);
		break;
	case SESSION_TYPE::CONNECT:
		static_cast<SessionItem*>(SessionItemPool[id])->SettingItem(type, INVALID_SOCKET, RecvItemPool[id], SessionPool[id]);
		break;
	}
}

//void gzz::GzzIOCP_EX::CreateCompletionItemPool(const int size)
//{
//	for (int i = 0; i < size; i++)
//	{
//		RecvItemPool.push_back(new RecvItemEX(i));
//		SessionItemPool.push_back(new SessionItem(i));
//	}
//
//	SendItemIdPool = new LockedCQ<int>(size * 2 + 1);
//
//	for (int i = 0; i < size * 2; i++)
//	{
//		SendItemPool.push_back(new SendItemEX(i, SendItemIdPool));
//		SendItemIdPool->Enqueue(i);
//	}
//}
