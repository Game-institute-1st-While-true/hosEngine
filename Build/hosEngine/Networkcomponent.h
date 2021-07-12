#pragma once
#include "NetDefine.h"
#include "NetworkManager.h"
#include "NetworkToken.h"
#include "Component.h"


namespace hos::com
{
	class Networkcomponent : public Component
	{
		friend class NetworkToken;
	private:
		NetworkToken* Token;
		RpcMap FuncMap;

	public:
		Networkcomponent();
		Networkcomponent(NetworkManager* network);
		virtual ~Networkcomponent();


	protected:
		Networkcomponent(const Networkcomponent& dest);

	public:
		void RequestToken(int index);													// ��Ʈ��ũ �Ŵ����� ��ū�� ��û�մϴ�.
		void RemoveToken() { Token = nullptr; }
		NetworkToken* GetToken() { return Token; }
		void SetNetworkManager(NetworkManager* net) { if (!Network) { Network = net; } }

		void Connect(string ip, unsigned short port);
		void UnExConnect(string ip, unsigned short port);

		bool SendMsg(PACKET& packet);												// �޼��� send
		bool IsConnection();

	public:
		template<class C>
		void Mapping(C* this_class, int index, CLASS_MEM_PACKAGE_FUNC<C> func)
		{
			FuncMap.Mapping(func, index, this_class);
		}

		void Call(int index, PACKAGE& packet);										// ������Ʈ �� �Լ� ȣ��	
	private:
		void Disconnect();

		void Bind(GameObject* gameObject) override;
		void UnBind(int index) override;

		Networkcomponent* Clone()const override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;
		void OnEnable()override;
		void OnDisable()override;
		void Reset() override;
	};
}
