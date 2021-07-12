#include "NetworkToken.h"
#include "NetworkManager.h"
#include "Networkcomponent.h"

hos::com::NetworkToken::NetworkToken(const int session_id)
	:SessionId(session_id), Connection(false), m_Networkcomponent(nullptr), GroupId(0), TryCount(0)
{
}

hos::com::NetworkToken::~NetworkToken()
{
	m_Networkcomponent = nullptr;
}

bool hos::com::NetworkToken::SendMsg(PACKET& packet)
{
	if (Connection)
	{
		NetworkManager::SendPacket(SessionId, packet);
		return true;
	}
	else
	{
		return false;
	}
}

void hos::com::NetworkToken::Connect(std::wstring ip, unsigned short port)
{
}

void hos::com::NetworkToken::UnExConnect(std::wstring ip, unsigned short port)
{
	ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), "Connect Trying.....");

	if (TryCount > 0)
	{
		Network->UnExConnect(SessionId, ip, port);
	}
	else
	{
		int ret = Network->UnExConnect(ip, port);
		if (ret < 0)
		{
			ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), "소켓을 생성할 수 없습니다.");
			return;

		}
		if (ret != SessionId)
		{
			ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), "소켓 생성 순서를 확인하세요.");
			return;
		}
	}
	++TryCount;
}

void hos::com::NetworkToken::OnConnection()
{
	ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), L"id : ", SessionId, "  connect");
	Connection = true;
}

void hos::com::NetworkToken::OnAccept()
{
	ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), L"id : ", SessionId, "  accept");
	if (m_Networkcomponent)
	{
		Connection = true;
	}
	else
	{
		Disconnect();
	}
}

void hos::com::NetworkToken::OnDisconnect()
{
	ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), L"연결이 종료되었습니다. session id : ", SessionId);
	if (m_Networkcomponent)
	{
		m_Networkcomponent->Disconnect();
	}
	Connection = false;
}

void hos::com::NetworkToken::OnConnectFail()
{
	ut::Logger::GetIns()->Write(L"net", TEXT(__FILE__), std::to_wstring(__LINE__), L"연결이 실패했습니다. session id : ", SessionId);
	Connection = false;
}

void hos::com::NetworkToken::OnJob(PACKAGE& package)
{
	if (m_Networkcomponent)
		m_Networkcomponent->Call(package.packet.head.protocol, package);
	else
	{
		NetLogger::GetIns()->Write(L"net", L"abnormal approach");
	}
}

void hos::com::NetworkToken::Disconnect()
{
	Connection = false;
	NetworkManager::Disconnect(SessionId);
}
