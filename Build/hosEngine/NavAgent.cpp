#include "NavAgent.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"

using namespace hos;

hos::com::NavAgent::NavAgent()
	:Component(L"NavAgent")
{
	 Query = new NavQuery();
}

hos::com::NavAgent::~NavAgent()
{
	SafeDelete(Query);
}

hos::com::NavAgent::NavAgent(const NavAgent& dest) :
	Component(dest)
{
	Query = new NavQuery();
}

void hos::com::NavAgent::SetPosition()
{
	if (m_GameObject && m_GameObject->transform)
	{
		Vector3 pos = m_GameObject->transform->GetPosition();
		Query->SetStart(pos);
	}
}

void hos::com::NavAgent::SetDestination(Vector3 dest)
{
	Query->SetEnd(dest);
}

void hos::com::NavAgent::SetNavMeshId(int id)
{
	Query->MeshId = id;
}

Vector3 hos::com::NavAgent::GetDir()
{
	Query->NearStraightPathPolys();
	return Query->Dir;
}

void hos::com::NavAgent::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}
	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}
	m_GameObject = gameObject;
}

const std::vector<U8> hos::com::NavAgent::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;
	builder.Map([&]
		{
			builder.String("Name", name);
			builder.Bool("IsActive", GetActive());
			//builder.Int("TryCount", GetTryCount());
			//builder.Int("MaxPathCount", GetMaxPathCount());
		});
	builder.Finish();
	return builder.GetBuffer();
}

bool hos::com::NavAgent::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"NavAgent try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	//Query.TryCount = m["TryCount"].AsInt64();
	//Query.MaxPathCount = m["MaxPathCount"].AsInt64();
	return true;
}

hos::com::NavAgent* hos::com::NavAgent::Clone() const
{
	return new hos::com::NavAgent(*this);
}

void hos::com::NavAgent::Reset()
{
}

void hos::com::NavAgent::Awake()
{
	Vector3 pos = m_GameObject->transform->GetPosition();

	int meshIndex = m_GameObject->m_Scene->GetBuildIndex();

	Query->MeshId = meshIndex;
	Query->SetStart(pos);

	Component::Awake();
}

void hos::com::NavAgent::OnEnable()
{
}

void hos::com::NavAgent::OnDisable()
{
	ReleasePathFind();
}

void hos::com::NavAgent::OnClose()
{
}

bool hos::com::NavAgent::RequestPathFind()
{
	if (g_Navigation)
	{
		if (!Query->Del.load())
		{
			if (g_Navigation->AddQuery(Query))
			{

			}
			else
				ut::Logger::GetIns()->LogConsole(L"navi", L"navigation pool is full request fail");
			return false;
		}
	}
	ut::Logger::GetIns()->LogConsole(L"navi", L"this agent is close");
	return false;
}

bool hos::com::NavAgent::ReleasePathFind()
{
	Query->Del.store(true);
	Query->MaxPathCount = 0;
	Query->CurrentPathCount = 0;
	ut::Logger::GetIns()->LogConsole(L"navi", L"query release ...");
	return true;
}

