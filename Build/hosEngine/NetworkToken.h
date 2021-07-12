#pragma once
#include "NetDefine.h"
#include "NetworkToken.h"

namespace hos::com
{
	class Networkcomponent;
	class GroupCallBackcomponent;
	class NetworkToken
	{
		friend class NetworkToken;
		friend class NetworkManager;
		friend class Networkcomponent;
		friend class Group;

	protected:
		int SessionId;
		int	GroupId; // ��ū�� � �׷쿡 �������� �� �ݿ�
		bool Connection;
		int TryCount;
		Networkcomponent* m_Networkcomponent;

	public:
		NetworkToken() = delete;
		NetworkToken(const int session_id);
		virtual ~NetworkToken();

	public:
		bool SendMsg(PACKET& packet);					  // �޼��� send

		const bool IsConnection()
		{
			return Connection;
		}

		const int GetSessionId()
		{
			return SessionId;
		}

		const int GetGroupId() { return GroupId; }
		void SetGroupId(const int group_id) { GroupId = group_id; }

	public:
		virtual void OnJob(PACKAGE& package);

	protected:
		virtual void Connect(std::wstring ip, unsigned short port);
		virtual void UnExConnect(std::wstring ip, unsigned short port);

		virtual void OnConnection();
		virtual void OnAccept();
		virtual void OnDisconnect();
		virtual void OnConnectFail();
		void Disconnect();
	};
}

