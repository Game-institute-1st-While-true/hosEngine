#include "AudioListener.h"
#include "GameObject.h"
#include "Transform.h"

using namespace hos;

hos::com::AudioListener::AudioListener() :
	Component(L"AudioListener"),
	Listener(DirectX::AudioListener())
{
	Reset();
}

hos::com::AudioListener::~AudioListener()
{
}

hos::com::AudioListener::AudioListener(const AudioListener& dest) :
	Component(dest),
	Listener(DirectX::AudioListener())
{
	if (dest.m_GameObject)
	{
		Listener.SetPosition(dest.m_GameObject->transform->GetPosition());
	}
}

com::AudioListener* hos::com::AudioListener::Clone() const
{
	return new AudioListener(*this);
}

void hos::com::AudioListener::Reset()
{

}

void hos::com::AudioListener::Awake()
{
	Component::Awake();
}

void hos::com::AudioListener::Update()
{
	Listener.Update(m_GameObject->transform->GetPosition(), m_GameObject->transform->GetUp(), (float)Time->DeltaTime());
}

void hos::com::AudioListener::OnEnable()
{
}

void hos::com::AudioListener::OnDisable()
{
}

DirectX::AudioListener& hos::com::AudioListener::Get()
{
	return Listener;
}

void hos::com::AudioListener::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->listener)
	{
		throw std::invalid_argument("this Object already has AudioListener");
	}

	m_GameObject = gameObject;
	gameObject->listener = this;

	if (m_GameObject->m_Scene)
	{
		if (m_GameObject->m_Scene->GetAudioListener() == nullptr)
		{
			m_GameObject->m_Scene->SetAudioListener(this);
		}
	}
}

void hos::com::AudioListener::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->listener = nullptr;
		m_GameObject = nullptr;
	}
}

const std::vector<U8> hos::com::AudioListener::Serialize()
{
	string t = GetName();
	mbstring name = ut::UTF16ToAnsi(t);

	flexbuffers::Builder builder;

	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::AudioListener::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"AudioListener try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	return true;
}
