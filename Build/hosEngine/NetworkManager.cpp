#include "NetworkManager.h"
#include "NetworkComponent.h"
#include "../GzzNet/PFile.h"

IOCP* hos::com::NetworkManager::Net = nullptr;
hos::com::NetworkManager::NetworkManager()
	:CurrentSessionCount(0), MaxSessionCount(0), ExtraSessionCount(0), IsWorking(false), Port(0), CallBack(nullptr), RunCount(0)
{
}

hos::com::NetworkManager::~NetworkManager()
{
	Finalize();
	SafeDelete(CallBack);
}

void hos::com::NetworkManager::CreateSession()
{
	for (int i = 0; i < MaxSessionCount; i++)
	{
		Tokens.push_back(new NetworkToken(i));
	}
	for (int i = MaxSessionCount; i < MaxSessionCount + ExtraSessionCount; i++)
	{
		Tokens.push_back(nullptr);
		ExtraIdQueue.push_back(i);
	}
}

void hos::com::NetworkManager::CreateFuncTable()
{
	for (int i = 0; i < static_cast<unsigned short>(SYSTEM_PROTOCOL::JOB); i++)
	{
		NetworkFuncTable.push_back(nullptr);
		NetworkFuncTable[i] = &NetworkManager::NotImplemented;
	}
	NetworkFuncTable[static_cast<unsigned short>(SYSTEM_PROTOCOL::ACCEPT)] = &NetworkManager::OnAccept;
	NetworkFuncTable[static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT)] = &NetworkManager::OnConnect;
	NetworkFuncTable[static_cast<unsigned short>(SYSTEM_PROTOCOL::CONNECT_FAIL)] = &NetworkManager::OnConnectFail;
	NetworkFuncTable[static_cast<unsigned short>(SYSTEM_PROTOCOL::DISCONNECT_ACCEPT)] = &NetworkManager::OnDisconnect_Accept;
	NetworkFuncTable[static_cast<unsigned short>(SYSTEM_PROTOCOL::DISCONNECT_CONNECT)] = &NetworkManager::OnDisconnect_Connect;
}

void hos::com::NetworkManager::Start()
{
	Net->Run();
}

void hos::com::NetworkManager::WorkProc()
{
	ut::Logger::GetIns()->LogConsole(L"net", L"network process thread start");
	while (IsWorking)
	{
		Run();
	}
}

bool hos::com::NetworkManager::ReadPackage()
{
	bool result = false;
	PACKAGE pack = { 0, {0} };
	if (Net->GetPackage(pack) == true)
	{
		if (pack.packet.head.protocol < static_cast<unsigned char>(SYSTEM_PROTOCOL::JOB))
		{
			NetworkFuncTable[static_cast<unsigned short>(pack.packet.head.protocol)](*this, pack);
		}
		else
		{
			OnJob(pack);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void hos::com::NetworkManager::CreateWorkerThread()
{
	IsWorking = true;
	WokerThread = std::thread([this]() {WorkProc(); });
}

void hos::com::NetworkManager::DestroyThread()
{
	IsWorking = false;
	if (WokerThread.joinable() == true)
	{
		WokerThread.join();
	}
}

void hos::com::NetworkManager::Initialize(int max_thread, int max_session, int extra_session, IPaper* paper, int run_cnt)
{
	NetLogger::GetIns()->AddPage(paper);
	RunCount = run_cnt;
#if defined(_DEBUG)
	NetLogger::GetIns()->AddPage(new gzz::PFile());
#else
	NetLogger::GetIns()->AddPage(new gzz::PFile());
#endif // DEBUG

	MaxSessionCount = max_session;
	ExtraSessionCount = extra_session;

	NetLogger::GetIns()->StartStamp();
	CreateNet(max_thread, max_session + extra_session);
	CreateSession();
	CreateFuncTable();

	if (CallBack == nullptr)
	{
		CallBack = new NetworkCallBack();
	}
}

void hos::com::NetworkManager::Initialize(int max_thread, int max_session, int extra_session, NetworkCallBack* callback, IPaper* paper, int run_cnt)
{
	NetLogger::GetIns()->AddPage(paper);

#ifdef _DEBUG
	NetLogger::GetIns()->AddPage(new gzz::PFile());
#endif // DEBUG
	MaxSessionCount = max_session;
	ExtraSessionCount = extra_session;
	CallBack = callback;
	RunCount = run_cnt;

	NetLogger::GetIns()->StartStamp();
	CreateNet(max_thread, max_session + extra_session);
	CreateSession();
	CreateFuncTable();

	if (CallBack == nullptr)
	{
		CallBack = new NetworkCallBack();
	}
}

void hos::com::NetworkManager::Listen(unsigned short port)
{
	Port = port;
	Net->Open(MaxSessionCount, Port);
}

void hos::com::NetworkManager::Run()
{
	for (int i = 0; i < RunCount; i++)
	{
		ReadPackage();
	}
}

int hos::com::NetworkManager::AddExtraToken(NetworkToken* token)
{
	if (token)
	{
		token->SessionId = ExtraIdQueue.front();
		ExtraIdQueue.pop_front();
		Tokens[token->SessionId] = token;

		return token->SessionId;
	}
	return -1;
}

void hos::com::NetworkManager::DelExtraToken(NetworkToken* token)
{
	if (token)
	{
		ExtraIdQueue.push_back(token->SessionId);
		SafeDelete(Tokens[token->SessionId]);
	}
}

void hos::com::NetworkManager::Finalize()
{
	Net->Stop();
	DestroyThread();
	for (int i = 0; i < Tokens.size(); i++)
	{
		if (Tokens[i] != nullptr)
		{
			delete Tokens[i];
			Tokens[i] = nullptr;
		}
	}
	Tokens.clear();
	if (Net != nullptr)
	{
		delete Net;
	}
}

void hos::com::NetworkManager::CreateNet(int thread_count, int max_session)
{
	int threadCount = thread_count;
	if (threadCount == 0)
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		threadCount = sysInfo.dwNumberOfProcessors * 2;
	}
	Net = new IOCP(thread_count, max_session);
	Net->Init();
}

void hos::com::NetworkManager::Connect(std::wstring ip, unsigned short port)
{
	std::string st = ut::UTF16ToUTF8(ip);
	Net->Connect(st.c_str(), port);
}

void hos::com::NetworkManager::Connect(int sessionid, std::wstring ip, unsigned short port)
{
	std::string st = ut::UTF16ToUTF8(ip);
	Net->Connect(sessionid, st.c_str(), port);
}

int hos::com::NetworkManager::UnExConnect(std::wstring ip, unsigned short port)
{
	std::string st = ut::UTF16ToUTF8(ip);
	return Net->UnExConnect(st.c_str(), port);
}

bool hos::com::NetworkManager::UnExConnect(int sessionid, std::wstring ip, unsigned short port)
{
	std::string st = ut::UTF16ToUTF8(ip);
	return Net->UnExConnect(sessionid, st.c_str(), port);
}

void hos::com::NetworkManager::Disconnect(int session_id)
{
	Net->Disconnect(session_id);
}

void hos::com::NetworkManager::SendPacket(int dst, PACKET& packet)
{
	Net->SendMsg(dst, packet);
}

void hos::com::NetworkManager::FourceEnque(PACKAGE& package)
{
	Net->FourceEnquePackage(package);
}

void hos::com::NetworkManager::OnAccept(PACKAGE& package)
{
	Tokens[package.id]->OnAccept();
	CallBack->OnAccept(package);
}

void hos::com::NetworkManager::OnConnect(PACKAGE& package)
{
	Tokens[package.id]->OnConnection();
	Tokens[package.id]->OnJob(package);

	CallBack->OnConnect(package);
}

void hos::com::NetworkManager::OnConnectFail(PACKAGE& package)
{
	Tokens[package.id]->OnConnectFail();
	CallBack->OnConnectFail(package);
}

void hos::com::NetworkManager::OnDisconnect_Accept(PACKAGE& package)
{
	Tokens[package.id]->OnDisconnect();
	CallBack->OnDisconnect_Accept(package);
}

void hos::com::NetworkManager::OnDisconnect_Connect(PACKAGE& package)
{
	Tokens[package.id]->OnDisconnect();
	/*Tokens[package.id]->OnJob(package);*/

	CallBack->OnDisconnect_Connect(package);
}

void hos::com::NetworkManager::OnJob(PACKAGE& package)
{
	Tokens[package.id]->OnJob(package);

	CallBack->OnJob(package);
}

void hos::com::NetworkManager::NotImplemented(PACKAGE& package)
{
	NetLogger::GetIns()->Write(L"net", L"id : ", package.id, L" 미구현 함수 비정상 호출  PROTOCOL : ", package.packet.head.protocol, L"SIZE : ", package.packet.head.size);
}