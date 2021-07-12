#include "Networkcomponent.h"
#include "GameObject.h"

using namespace hos;
hos::com::Networkcomponent::Networkcomponent()
	:Component(L"Networkcomponent"), Token(nullptr)
{
}

hos::com::Networkcomponent::Networkcomponent(NetworkManager* network)
	: Component(L"Networkcomponent"), Token(nullptr)
{
}

hos::com::Networkcomponent::Networkcomponent(const Networkcomponent& dest)
	: Component(dest), Token(nullptr)
{
}

hos::com::Networkcomponent::~Networkcomponent()
{
	Token = nullptr;
}

bool hos::com::Networkcomponent::SendMsg(PACKET& packet)
{
	if (Token)
	{
		Token->SendMsg(packet);
		return true;
	}
	else
	{
		return false;
	}
}

void hos::com::Networkcomponent::Call(int index, PACKAGE& packet)
{
	FuncMap[index](packet);
}

void hos::com::Networkcomponent::Disconnect()
{
	ut::Logger::GetIns()->LogConsole(L"net", L"연결이 끊어졌습니다.");
}

void hos::com::Networkcomponent::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->network)
	{
		throw std::invalid_argument("this Object already has Network");
	}

	m_GameObject = gameObject;
	gameObject->network = this;
}

void hos::com::Networkcomponent::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->network = nullptr;
		m_GameObject = nullptr;
	}
}

bool hos::com::Networkcomponent::IsConnection()
{
	if (Token)
	{
		return Token->Connection;
	}
	else
	{
		return false;
	}
}


hos::com::Networkcomponent* hos::com::Networkcomponent::Clone() const
{
	return new Networkcomponent(*this);
}

const std::vector<U8> hos::com::Networkcomponent::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;
	builder.Map([&]
		{
			builder.String("Name", name);
		});
	builder.Finish();
	return builder.GetBuffer();
}


bool hos::com::Networkcomponent::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}
	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);

	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"NetworkComponent try Deserialize to" + name);
		return false;
	}
	return true;
}

void hos::com::Networkcomponent::RequestToken(int index)
{
	// 자신과 연결할 토큰(소켓)을 가져온다.
	Token = Network->GetToken(index);

	Networkcomponent* old = Token->m_Networkcomponent;

	if (Token != nullptr)
	{
		Token->m_Networkcomponent = this;
	}
}

void hos::com::Networkcomponent::Connect(string ip, unsigned short port)
{
	if (Token == nullptr) return;
	Token->Connect(ip, port);
}

void hos::com::Networkcomponent::UnExConnect(string ip, unsigned short port)
{
	if (Token == nullptr)
	{
		Debug->LogConsole(L"DataManager", L"No Token Please Request Token");
	}
	else
	{
		if (IsConnection() == false)
			Token->UnExConnect(ip, port);
	}
}

void hos::com::Networkcomponent::OnEnable()
{
}

void hos::com::Networkcomponent::OnDisable()
{
}

void hos::com::Networkcomponent::Reset()
{
	Token = nullptr;
}
